//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2001 by Denton Woods
// Last modified: 08/30/2001 <--Y2K Compliant! =]
//
// Filename: openil/convert.c
//
// Description: Converts between several image formats
//
//-----------------------------------------------------------------------------


#include "il_internal.h"
#include "il_manip.h"
#include <limits.h>


ILvoid* ILAPIENTRY iSwitchTypes(ILuint SizeOfData, ILenum SrcType, ILenum DestType, ILvoid *Buffer);

#define CHECK_ALLOC() 	if (NewData == NULL) { \
							if (Data != Buffer) \
								ifree(Data); \
							ilSetError(IL_OUT_OF_MEMORY); \
							return IL_FALSE; \
						}

ILAPI ILvoid* ILAPIENTRY ilConvertBuffer(ILuint SizeOfData, ILenum SrcFormat, ILenum DestFormat, ILenum SrcType, ILenum DestType, ILvoid *Buffer)
{
	//static const	ILfloat LumFactor[3] = { 0.299f, 0.587f, 0.114f };  // Used for conversion to luminance
	//static const	ILfloat LumFactor[3] = { 0.3086f, 0.6094f, 0.0820f };  // http://www.sgi.com/grafica/matrix/index.html
	static const	ILfloat LumFactor[3] = { 0.212671f, 0.715160f, 0.072169f };  // http://www.inforamp.net/~poynton/ and libpng's libpng.txt

	static ILubyte	*NewData;
	static ILuint	i, j, c, Size;
	static ILfloat	Resultf;
	static ILdouble	Resultd;
	static ILuint	NumPix;  // Really number of pixels * bpp.
	static ILuint	BpcDest;
	ILvoid *Data;

	if (SizeOfData == 0 || Buffer == NULL) {
		ilSetError(IL_INVALID_PARAM);
		return NULL;
	}

	Data = iSwitchTypes(SizeOfData, SrcType, DestType, Buffer);
	if (Data == NULL)
		return NULL;

	BpcDest = ilGetBppType(DestType);
	NumPix = SizeOfData / ilGetBppType(SrcType);

	if (DestFormat == SrcFormat) {
		NewData = (ILubyte*)ialloc(NumPix * BpcDest);
		if (NewData == NULL) {
			ilSetError(IL_OUT_OF_MEMORY);
			return IL_FALSE;
		}
		memcpy(NewData, Data, NumPix * BpcDest);
		if (Data != Buffer)
			ifree(Data);

		return NewData;
	}

	switch (SrcFormat)
	{
		case IL_RGB:
			switch (DestFormat)
			{
				case IL_BGR:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest);
					CHECK_ALLOC();
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0; i < NumPix; i += 3) {
								NewData[i] = ((ILubyte*)(Data))[i+2];
								NewData[i+1] = ((ILubyte*)(Data))[i+1];
								NewData[i+2] = ((ILubyte*)(Data))[i];
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
							for (i = 0; i < NumPix; i += 3) {
								((ILushort*)(NewData))[i] = ((ILushort*)(Data))[i+2];
								((ILushort*)(NewData))[i+1] = ((ILushort*)(Data))[i+1];
								((ILushort*)(NewData))[i+2] = ((ILushort*)(Data))[i];
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0; i < NumPix; i += 3) {
								((ILuint*)(NewData))[i] = ((ILuint*)(Data))[i+2];
								((ILuint*)(NewData))[i+1] = ((ILuint*)(Data))[i+1];
								((ILuint*)(NewData))[i+2] = ((ILuint*)(Data))[i];
							}
							break;
						case IL_FLOAT:
							for (i = 0; i < NumPix; i += 3) {
								((ILfloat*)(NewData))[i] = ((ILfloat*)(Data))[i+2];
								((ILfloat*)(NewData))[i+1] = ((ILfloat*)(Data))[i+1];
								((ILfloat*)(NewData))[i+2] = ((ILfloat*)(Data))[i];
							}
							break;

							break;
						case IL_DOUBLE:
							for (i = 0; i < NumPix; i += 3) {
								((ILdouble*)(NewData))[i] = ((ILdouble*)(Data))[i+2];
								((ILdouble*)(NewData))[i+1] = ((ILdouble*)(Data))[i+1];
								((ILdouble*)(NewData))[i+2] = ((ILdouble*)(Data))[i];
							}
							break;

							break;
					}
					break;

				case IL_RGBA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest * 4 / 3);
					CHECK_ALLOC();
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								NewData[j] = ((ILubyte*)(Data))[i];
								NewData[j+1] = ((ILubyte*)(Data))[i+1];
								NewData[j+2] = ((ILubyte*)(Data))[i+2];
								NewData[j+3] = UCHAR_MAX;
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILushort*)(NewData))[j] = ((ILushort*)(Data))[i];
								((ILushort*)(NewData))[j+1] = ((ILushort*)(Data))[i+1];
								((ILushort*)(NewData))[j+2] = ((ILushort*)(Data))[i+2];
								((ILushort*)(NewData))[j+3] = USHRT_MAX;
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILuint*)(NewData))[j] = ((ILuint*)(Data))[i];
								((ILuint*)(NewData))[j+1] = ((ILuint*)(Data))[i+1];
								((ILuint*)(NewData))[j+2] = ((ILuint*)(Data))[i+2];
								((ILuint*)(NewData))[j+3] = UINT_MAX;
							}
							break;
						case IL_FLOAT:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILfloat*)(NewData))[j] = ((ILfloat*)(Data))[i];
								((ILfloat*)(NewData))[j+1] = ((ILfloat*)(Data))[i+1];
								((ILfloat*)(NewData))[j+2] = ((ILfloat*)(Data))[i+2];
								((ILfloat*)(NewData))[j+3] = 1.0f;
							}
							break;
						case IL_DOUBLE:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILdouble*)(NewData))[j] = ((ILdouble*)(Data))[i];
								((ILdouble*)(NewData))[j+1] = ((ILdouble*)(Data))[i+1];
								((ILdouble*)(NewData))[j+2] = ((ILdouble*)(Data))[i+2];
								((ILdouble*)(NewData))[j+3] = 1.0;
							}
							break;
					}
					break;

				case IL_BGRA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest * 4 / 3);
					CHECK_ALLOC();
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								NewData[j] = ((ILubyte*)(Data))[i+2];
								NewData[j+1] = ((ILubyte*)(Data))[i+1];
								NewData[j+2] = ((ILubyte*)(Data))[i];
								NewData[j+3] = UCHAR_MAX;
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILushort*)(NewData))[j] = ((ILushort*)(Data))[i+2];
								((ILushort*)(NewData))[j+1] = ((ILushort*)(Data))[i+1];
								((ILushort*)(NewData))[j+2] = ((ILushort*)(Data))[i];
								((ILushort*)(NewData))[j+3] = USHRT_MAX;
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILuint*)(NewData))[j] = ((ILuint*)(Data))[i+2];
								((ILuint*)(NewData))[j+1] = ((ILuint*)(Data))[i+1];
								((ILuint*)(NewData))[j+2] = ((ILuint*)(Data))[i];
								((ILuint*)(NewData))[j+3] = UINT_MAX;
							}
							break;
						case IL_FLOAT:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILfloat*)(NewData))[j] = ((ILfloat*)(Data))[i+2];
								((ILfloat*)(NewData))[j+1] = ((ILfloat*)(Data))[i+1];
								((ILfloat*)(NewData))[j+2] = ((ILfloat*)(Data))[i];
								((ILfloat*)(NewData))[j+3] = 1.0f;
							}
							break;
						case IL_DOUBLE:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILdouble*)(NewData))[j] = ((ILdouble*)(Data))[i+2];
								((ILdouble*)(NewData))[j+1] = ((ILdouble*)(Data))[i+1];
								((ILdouble*)(NewData))[j+2] = ((ILdouble*)(Data))[i];
								((ILdouble*)(NewData))[j+3] = 1.0f;
							}
							break;
					}
					break;

				case IL_LUMINANCE:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest / 3);
					CHECK_ALLOC();
					Size = NumPix / 3;
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0; i < Size; i++) {
								Resultf = 0;
								for (c = 0; c < 3; c++) {
									Resultf += ((ILubyte*)(Data))[i * 3 + c] * LumFactor[c];
								}
								NewData[i] = (ILubyte)Resultf;
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
							for (i = 0; i < Size; i++) {
								Resultf = 0;
								for (c = 0; c < 3; c++) {
									Resultf += ((ILushort*)(Data))[i * 3 + c] * LumFactor[c];
								}
								((ILushort*)(NewData))[i] = (ILushort)Resultf;
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0; i < Size; i++) {
								Resultf = 0;
								for (c = 0; c < 3; c++) {
									Resultf += ((ILuint*)(Data))[i * 3 + c] * LumFactor[c];
								}
								((ILuint*)(NewData))[i] = (ILuint)Resultf;
							}
							break;
						case IL_FLOAT:
							for (i = 0; i < Size; i++) {
								Resultf = 0;
								for (c = 0; c < 3; c++) {
									Resultf += ((ILfloat*)(Data))[i * 3 + c] * LumFactor[c];
								}
								((ILfloat*)(NewData))[i] = Resultf;
							}
							break;
						case IL_DOUBLE:
							for (i = 0; i < Size; i++) {
								Resultd = 0;
								for (c = 0; c < 3; c++) {
									Resultd += ((ILdouble*)(Data))[i * 3 + c] * LumFactor[c];
								}
								((ILdouble*)(NewData))[i] = Resultd;
							}
							break;
					}
					break;

				default:
					ilSetError(IL_INVALID_CONVERSION);
					if (Data != Buffer)
						ifree(Data);
					return NULL;
			}
			break;

		case IL_RGBA:
			switch (DestFormat)
			{
				case IL_BGRA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest);
					CHECK_ALLOC();
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0; i < NumPix; i += 4) {
								NewData[i] = ((ILubyte*)(Data))[i+2];
								NewData[i+1] = ((ILubyte*)(Data))[i+1];
								NewData[i+2] = ((ILubyte*)(Data))[i];
								NewData[i+3] = ((ILubyte*)(Data))[i+3];
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
							for (i = 0; i < NumPix; i += 4) {
								((ILushort*)(NewData))[i] = ((ILushort*)(Data))[i+2];
								((ILushort*)(NewData))[i+1] = ((ILushort*)(Data))[i+1];
								((ILushort*)(NewData))[i+2] = ((ILushort*)(Data))[i];
								((ILushort*)(NewData))[i+3] = ((ILushort*)(Data))[i+3];
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0; i < NumPix; i += 4) {
								((ILuint*)(NewData))[i] = ((ILuint*)(Data))[i+2];
								((ILuint*)(NewData))[i+1] = ((ILuint*)(Data))[i+1];
								((ILuint*)(NewData))[i+2] = ((ILuint*)(Data))[i];
								((ILuint*)(NewData))[i+3] = ((ILuint*)(Data))[i+3];
							}
							break;
						case IL_FLOAT:
							for (i = 0; i < NumPix; i += 4) {
								((ILfloat*)(NewData))[i] = ((ILfloat*)(Data))[i+2];
								((ILfloat*)(NewData))[i+1] = ((ILfloat*)(Data))[i+1];
								((ILfloat*)(NewData))[i+2] = ((ILfloat*)(Data))[i];
								((ILfloat*)(NewData))[i+3] = ((ILfloat*)(Data))[i+3];
							}
							break;
						case IL_DOUBLE:
							for (i = 0; i < NumPix; i += 4) {
								((ILdouble*)(NewData))[i] = ((ILdouble*)(Data))[i+2];
								((ILdouble*)(NewData))[i+1] = ((ILdouble*)(Data))[i+1];
								((ILdouble*)(NewData))[i+2] = ((ILdouble*)(Data))[i];
								((ILdouble*)(NewData))[i+3] = ((ILdouble*)(Data))[i+3];
							}
							break;
					}
					break;

				case IL_RGB:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest * 3 / 4);
					CHECK_ALLOC();
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								NewData[j] = ((ILubyte*)(Data))[i];
								NewData[j+1] = ((ILubyte*)(Data))[i+1];
								NewData[j+2] = ((ILubyte*)(Data))[i+2];
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILushort*)(NewData))[j] = ((ILushort*)(Data))[i];
								((ILushort*)(NewData))[j+1] = ((ILushort*)(Data))[i+1];
								((ILushort*)(NewData))[j+2] = ((ILushort*)(Data))[i+2];
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILuint*)(NewData))[j] = ((ILuint*)(Data))[i];
								((ILuint*)(NewData))[j+1] = ((ILuint*)(Data))[i+1];
								((ILuint*)(NewData))[j+2] = ((ILuint*)(Data))[i+2];
							}
							break;
						case IL_FLOAT:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILfloat*)(NewData))[j] = ((ILfloat*)(Data))[i];
								((ILfloat*)(NewData))[j+1] = ((ILfloat*)(Data))[i+1];
								((ILfloat*)(NewData))[j+2] = ((ILfloat*)(Data))[i+2];
							}
							break;
						case IL_DOUBLE:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILdouble*)(NewData))[j] = ((ILdouble*)(Data))[i];
								((ILdouble*)(NewData))[j+1] = ((ILdouble*)(Data))[i+1];
								((ILdouble*)(NewData))[j+2] = ((ILdouble*)(Data))[i+2];
							}
							break;
					}
					break;

				case IL_BGR:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest * 3 / 4);
					CHECK_ALLOC();
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								NewData[j] = ((ILubyte*)(Data))[i+2];
								NewData[j+1] = ((ILubyte*)(Data))[i+1];
								NewData[j+2] = ((ILubyte*)(Data))[i];
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILushort*)(NewData))[j] = ((ILushort*)(Data))[i+2];
								((ILushort*)(NewData))[j+1] = ((ILushort*)(Data))[i+1];
								((ILushort*)(NewData))[j+2] = ((ILushort*)(Data))[i];
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILuint*)(NewData))[j] = ((ILuint*)(Data))[i+2];
								((ILuint*)(NewData))[j+1] = ((ILuint*)(Data))[i+1];
								((ILuint*)(NewData))[j+2] = ((ILuint*)(Data))[i];
							}
							break;
						case IL_FLOAT:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILfloat*)(NewData))[j] = ((ILfloat*)(Data))[i+2];
								((ILfloat*)(NewData))[j+1] = ((ILfloat*)(Data))[i+1];
								((ILfloat*)(NewData))[j+2] = ((ILfloat*)(Data))[i];
							}
							break;
						case IL_DOUBLE:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILdouble*)(NewData))[j] = ((ILdouble*)(Data))[i+2];
								((ILdouble*)(NewData))[j+1] = ((ILdouble*)(Data))[i+1];
								((ILdouble*)(NewData))[j+2] = ((ILdouble*)(Data))[i];
							}
							break;
					}
					break;

				case IL_LUMINANCE:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest / 4);
					CHECK_ALLOC();
					Size = NumPix / 4;
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0; i < Size; i++) {
								Resultf = 0.0f;
								for (c = 0; c < 3; c++) {
									Resultf += ((ILubyte*)(Data))[i * 4 + c] * LumFactor[c];
								}
								NewData[i] = (ILubyte)Resultf;
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
							for (i = 0; i < Size; i++) {
								Resultf = 0.0f;
								for (c = 0; c < 3; c++) {
									Resultf += ((ILushort*)(Data))[i * 4 + c] * LumFactor[c];
								}
								((ILushort*)(NewData))[i] = (ILushort)Resultf;
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0; i < Size; i++) {
								Resultd = 0.0;
								for (c = 0; c < 3; c++) {
									Resultd += ((ILuint*)(Data))[i * 4 + c] * LumFactor[c];
								}
								((ILuint*)(NewData))[i] = (ILuint)Resultd;
							}
							break;
						case IL_FLOAT:
							for (i = 0; i < Size; i++) {
								Resultd = 0.0;
								for (c = 0; c < 3; c++) {
									Resultd += ((ILfloat*)(Data))[i * 4 + c] * LumFactor[c];
								}
								((ILfloat*)(NewData))[i] = (ILfloat)Resultd;
							}
							break;
						case IL_DOUBLE:
							for (i = 0; i < Size; i++) {
								Resultd = 0.0;
								for (c = 0; c < 3; c++) {
									Resultd += ((ILdouble*)(Data))[i * 4 + c] * LumFactor[c];
								}
								((ILdouble*)(NewData))[i] = Resultd;
							}
							break;
					}
					break;

				default:
					ilSetError(IL_INVALID_CONVERSION);
					if (Data != Buffer)
						ifree(Data);
					return NULL;
			}
			break;

		case IL_BGR:
			switch (DestFormat)
			{
				case IL_RGB:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest);
					CHECK_ALLOC();
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0; i < NumPix; i += 3) {
								NewData[i] = ((ILubyte*)(Data))[i+2];
								NewData[i+1] = ((ILubyte*)(Data))[i+1];
								NewData[i+2] = ((ILubyte*)(Data))[i];
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
							for (i = 0; i < NumPix; i += 3) {
								((ILushort*)(NewData))[i] = ((ILushort*)(Data))[i+2];
								((ILushort*)(NewData))[i+1] = ((ILushort*)(Data))[i+1];
								((ILushort*)(NewData))[i+2] = ((ILushort*)(Data))[i];
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0; i < NumPix; i += 3) {
								((ILuint*)(NewData))[i] = ((ILuint*)(Data))[i+2];
								((ILuint*)(NewData))[i+1] = ((ILuint*)(Data))[i+1];
								((ILuint*)(NewData))[i+2] = ((ILuint*)(Data))[i];
							}
							break;
						case IL_FLOAT:
							for (i = 0; i < NumPix; i += 3) {
								((ILfloat*)(NewData))[i] = ((ILfloat*)(Data))[i+2];
								((ILfloat*)(NewData))[i+1] = ((ILfloat*)(Data))[i+1];
								((ILfloat*)(NewData))[i+2] = ((ILfloat*)(Data))[i];
							}
							break;
						case IL_DOUBLE:
							for (i = 0; i < NumPix; i += 3) {
								((ILdouble*)(NewData))[i] = ((ILdouble*)(Data))[i+2];
								((ILdouble*)(NewData))[i+1] = ((ILdouble*)(Data))[i+1];
								((ILdouble*)(NewData))[i+2] = ((ILdouble*)(Data))[i];
							}
							break;
					}
					break;

				case IL_BGRA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest * 4 / 3);
					CHECK_ALLOC();
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								NewData[j] = ((ILubyte*)(Data))[i];
								NewData[j+1] = ((ILubyte*)(Data))[i+1];
								NewData[j+2] = ((ILubyte*)(Data))[i+2];
								NewData[j+3] = UCHAR_MAX;
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILushort*)(NewData))[j] = ((ILushort*)(Data))[i];
								((ILushort*)(NewData))[j+1] = ((ILushort*)(Data))[i+1];
								((ILushort*)(NewData))[j+2] = ((ILushort*)(Data))[i+2];
								((ILushort*)(NewData))[j+3] = USHRT_MAX;
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILuint*)(NewData))[j] = ((ILuint*)(Data))[i];
								((ILuint*)(NewData))[j+1] = ((ILuint*)(Data))[i+1];
								((ILuint*)(NewData))[j+2] = ((ILuint*)(Data))[i+2];
								((ILuint*)(NewData))[j+3] = UINT_MAX;
							}
							break;
						case IL_FLOAT:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILfloat*)(NewData))[j] = ((ILfloat*)(Data))[i];
								((ILfloat*)(NewData))[j+1] = ((ILfloat*)(Data))[i+1];
								((ILfloat*)(NewData))[j+2] = ((ILfloat*)(Data))[i+2];
								((ILfloat*)(NewData))[j+3] = 1.0f;
							}
							break;
						case IL_DOUBLE:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILdouble*)(NewData))[j] = ((ILdouble*)(Data))[i];
								((ILdouble*)(NewData))[j+1] = ((ILdouble*)(Data))[i+1];
								((ILdouble*)(NewData))[j+2] = ((ILdouble*)(Data))[i+2];
								((ILdouble*)(NewData))[j+3] = 1.0;
							}
							break;
					}
					break;

				case IL_RGBA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest * 4 / 3);
					CHECK_ALLOC();
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								NewData[j] = ((ILubyte*)(Data))[i+2];
								NewData[j+1] = ((ILubyte*)(Data))[i+1];
								NewData[j+2] = ((ILubyte*)(Data))[i];
								NewData[j+3] = UCHAR_MAX;
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILushort*)(NewData))[j] = ((ILushort*)(Data))[i+2];
								((ILushort*)(NewData))[j+1] = ((ILushort*)(Data))[i+1];
								((ILushort*)(NewData))[j+2] = ((ILushort*)(Data))[i];
								((ILushort*)(NewData))[j+3] = USHRT_MAX;
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILuint*)(NewData))[j] = ((ILuint*)(Data))[i+2];
								((ILuint*)(NewData))[j+1] = ((ILuint*)(Data))[i+1];
								((ILuint*)(NewData))[j+2] = ((ILuint*)(Data))[i];
								((ILuint*)(NewData))[j+3] = UINT_MAX;
							}
							break;
						case IL_FLOAT:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILfloat*)(NewData))[j] = ((ILfloat*)(Data))[i+2];
								((ILfloat*)(NewData))[j+1] = ((ILfloat*)(Data))[i+1];
								((ILfloat*)(NewData))[j+2] = ((ILfloat*)(Data))[i];
								((ILfloat*)(NewData))[j+3] = 1.0f;
							}
							break;
						case IL_DOUBLE:
							for (i = 0, j = 0; i < NumPix; i += 3, j += 4) {
								((ILdouble*)(NewData))[j] = ((ILdouble*)(Data))[i+2];
								((ILdouble*)(NewData))[j+1] = ((ILdouble*)(Data))[i+1];
								((ILdouble*)(NewData))[j+2] = ((ILdouble*)(Data))[i];
								((ILdouble*)(NewData))[j+3] = 1.0;
							}
							break;
					}
					break;

				case IL_LUMINANCE:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest / 3);
					CHECK_ALLOC();
					Size = NumPix / 3;
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0; i < Size; i++) {
								Resultf = 0.0f;  j = 2;
								for (c = 0; c < 3; c++, j--) {
									Resultf += ((ILubyte*)(Data))[i * 3 + c] * LumFactor[j];
								}
								NewData[i] = (ILubyte)Resultf;
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
							for (i = 0; i < Size; i++) {
								Resultf = 0.0f;  j = 2;
								for (c = 0; c < 3; c++, j--) {
									Resultf += ((ILushort*)(Data))[i * 3 + c] * LumFactor[j];
								}
								((ILushort*)(NewData))[i] = (ILushort)Resultf;
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0; i < Size; i++) {
								Resultd = 0.0f;  j = 2;
								for (c = 0; c < 3; c++, j--) {
									Resultd += ((ILuint*)(Data))[i * 3 + c] * LumFactor[j];
								}
								((ILuint*)(NewData))[i] = (ILuint)Resultf;
							}
							break;
						case IL_FLOAT:
							for (i = 0; i < Size; i++) {
								Resultd = 0.0f;  j = 2;
								for (c = 0; c < 3; c++, j--) {
									Resultd += ((ILfloat*)(Data))[i * 3 + c] * LumFactor[j];
								}
								((ILfloat*)(NewData))[i] = (ILfloat)Resultd;
							}
							break;
						case IL_DOUBLE:
							for (i = 0; i < Size; i++) {
								Resultd = 0.0f;  j = 2;
								for (c = 0; c < 3; c++, j--) {
									Resultd += ((ILdouble*)(Data))[i * 3 + c] * LumFactor[j];
								}
								((ILdouble*)(NewData))[i] = Resultd;
							}
							break;
					}
					break;

				default:
					ilSetError(IL_INVALID_CONVERSION);
					if (Data != Buffer)
						ifree(Data);
					return NULL;
			}
			break;

		case IL_BGRA:
			switch (DestFormat)
			{
				case IL_RGBA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest);
					CHECK_ALLOC();
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0; i < NumPix; i += 4) {
								NewData[i] = ((ILubyte*)(Data))[i+2];
								NewData[i+1] = ((ILubyte*)(Data))[i+1];
								NewData[i+2] = ((ILubyte*)(Data))[i];
								NewData[i+3] = ((ILubyte*)(Data))[i+3];
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
							for (i = 0; i < NumPix; i += 4) {
								((ILushort*)(NewData))[i] = ((ILushort*)(Data))[i+2];
								((ILushort*)(NewData))[i+1] = ((ILushort*)(Data))[i+1];
								((ILushort*)(NewData))[i+2] = ((ILushort*)(Data))[i];
								((ILushort*)(NewData))[i+3] = ((ILushort*)(Data))[i+3];
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0; i < NumPix; i += 4) {
								((ILuint*)(NewData))[i] = ((ILuint*)(Data))[i+2];
								((ILuint*)(NewData))[i+1] = ((ILuint*)(Data))[i+1];
								((ILuint*)(NewData))[i+2] = ((ILuint*)(Data))[i];
								((ILuint*)(NewData))[i+3] = ((ILuint*)(Data))[i+3];
							}
							break;
						case IL_FLOAT:
							for (i = 0; i < NumPix; i += 4) {
								((ILfloat*)(NewData))[i] = ((ILfloat*)(Data))[i+2];
								((ILfloat*)(NewData))[i+1] = ((ILfloat*)(Data))[i+1];
								((ILfloat*)(NewData))[i+2] = ((ILfloat*)(Data))[i];
								((ILfloat*)(NewData))[i+3] = ((ILfloat*)(Data))[i+3];
							}
							break;
						case IL_DOUBLE:
							for (i = 0; i < NumPix; i += 4) {
								((ILdouble*)(NewData))[i] = ((ILdouble*)(Data))[i+2];
								((ILdouble*)(NewData))[i+1] = ((ILdouble*)(Data))[i+1];
								((ILdouble*)(NewData))[i+2] = ((ILdouble*)(Data))[i];
								((ILdouble*)(NewData))[i+3] = ((ILdouble*)(Data))[i+3];
							}
							break;
					}
					break;

				case IL_BGR:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest * 3 / 4);
					CHECK_ALLOC();
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								NewData[j] = ((ILubyte*)(Data))[i];
								NewData[j+1] = ((ILubyte*)(Data))[i+1];
								NewData[j+2] = ((ILubyte*)(Data))[i+2];
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILushort*)(NewData))[j] = ((ILushort*)(Data))[i];
								((ILushort*)(NewData))[j+1] = ((ILushort*)(Data))[i+1];
								((ILushort*)(NewData))[j+2] = ((ILushort*)(Data))[i+2];
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILuint*)(NewData))[j] = ((ILuint*)(Data))[i];
								((ILuint*)(NewData))[j+1] = ((ILuint*)(Data))[i+1];
								((ILuint*)(NewData))[j+2] = ((ILuint*)(Data))[i+2];
							}
							break;
						case IL_FLOAT:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILfloat*)(NewData))[j] = ((ILfloat*)(Data))[i];
								((ILfloat*)(NewData))[j+1] = ((ILfloat*)(Data))[i+1];
								((ILfloat*)(NewData))[j+2] = ((ILfloat*)(Data))[i+2];
							}
							break;
						case IL_DOUBLE:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILdouble*)(NewData))[j] = ((ILdouble*)(Data))[i];
								((ILdouble*)(NewData))[j+1] = ((ILdouble*)(Data))[i+1];
								((ILdouble*)(NewData))[j+2] = ((ILdouble*)(Data))[i+2];
							}
							break;
					}
					break;

				case IL_RGB:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest * 3 / 4);
					CHECK_ALLOC();
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								NewData[j] = ((ILubyte*)(Data))[i+2];
								NewData[j+1] = ((ILubyte*)(Data))[i+1];
								NewData[j+2] = ((ILubyte*)(Data))[i];
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILushort*)(NewData))[j] = ((ILushort*)(Data))[i+2];
								((ILushort*)(NewData))[j+1] = ((ILushort*)(Data))[i+1];
								((ILushort*)(NewData))[j+2] = ((ILushort*)(Data))[i];
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILuint*)(NewData))[j] = ((ILuint*)(Data))[i+2];
								((ILuint*)(NewData))[j+1] = ((ILuint*)(Data))[i+1];
								((ILuint*)(NewData))[j+2] = ((ILuint*)(Data))[i];
							}
							break;
						case IL_FLOAT:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILfloat*)(NewData))[j] = ((ILfloat*)(Data))[i+2];
								((ILfloat*)(NewData))[j+1] = ((ILfloat*)(Data))[i+1];
								((ILfloat*)(NewData))[j+2] = ((ILfloat*)(Data))[i];
							}
							break;
						case IL_DOUBLE:
							for (i = 0, j = 0; i < NumPix; i += 4, j += 3) {
								((ILdouble*)(NewData))[j] = ((ILdouble*)(Data))[i+2];
								((ILdouble*)(NewData))[j+1] = ((ILdouble*)(Data))[i+1];
								((ILdouble*)(NewData))[j+2] = ((ILdouble*)(Data))[i];
							}
							break;
					}
					break;

				case IL_LUMINANCE:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest / 4);
					CHECK_ALLOC();
					Size = NumPix / 4;
					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0; i < Size; i++) {
								Resultf = 0.0f;  j = 2;
								for (c = 0; c < 3; c++, j--) {
									Resultf += ((ILubyte*)(Data))[i * 4 + c] * LumFactor[j];
								}
								NewData[i] = (ILubyte)Resultf;
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
							for (i = 0; i < Size; i++) {
								Resultf = 0.0f;  j = 2;
								for (c = 0; c < 3; c++, j--) {
									Resultf += ((ILushort*)(Data))[i * 4 + c] * LumFactor[j];
								}
								((ILushort*)(NewData))[i] = (ILushort)Resultf;
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0; i < Size; i++) {
								Resultd = 0.0f;  j = 2;
								for (c = 0; c < 3; c++, j--) {
									Resultd += ((ILuint*)(Data))[i * 4 + c] * LumFactor[j];
								}
								((ILuint*)(NewData))[i] = (ILuint)Resultd;
							}
							break;
						case IL_FLOAT:
							for (i = 0; i < Size; i++) {
								Resultf = 0.0f;  j = 2;
								for (c = 0; c < 3; c++, j--) {
									Resultd += ((ILfloat*)(Data))[i * 4 + c] * LumFactor[j];
								}
								((ILfloat*)(NewData))[i] = (ILfloat)Resultd;
							}
							break;
						case IL_DOUBLE:
							for (i = 0; i < Size; i++) {
								Resultd = 0.0f;  j = 2;
								for (c = 0; c < 3; c++, j--) {
									Resultd += ((ILdouble*)(Data))[i * 4 + c] * LumFactor[j];
								}
								((ILdouble*)(NewData))[i] = Resultd;
							}
							break;
					}
					break;

				default:
					ilSetError(IL_INVALID_CONVERSION);
					if (Data != Buffer)
						ifree(Data);
					return NULL;
			}
			break;

		case IL_LUMINANCE:
			switch (DestFormat)
			{
				case IL_RGB:
				case IL_BGR:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest * 3);
					CHECK_ALLOC();

					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0, j = 0; i < NumPix; i++, j += 3) {
								for (c = 0; c < 3; c++) {
									NewData[j + c] = ((ILubyte*)(Data))[i];
								}
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
							for (i = 0, j = 0; i < NumPix; i++, j += 3) {
								for (c = 0; c < 3; c++) {
									((ILushort*)(NewData))[j + c] = ((ILushort*)(Data))[i];
								}
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0, j = 0; i < NumPix; i++, j += 3) {
								for (c = 0; c < 3; c++) {
									((ILuint*)(NewData))[j + c] = ((ILuint*)(Data))[i];
								}
							}
							break;
						case IL_FLOAT:
							for (i = 0, j = 0; i < NumPix; i++, j += 3) {
								for (c = 0; c < 3; c++) {
									((ILfloat*)(NewData))[j + c] = ((ILfloat*)(Data))[i];
								}
							}
							break;
						case IL_DOUBLE:
							for (i = 0, j = 0; i < NumPix; i++, j += 3) {
								for (c = 0; c < 3; c++) {
									((ILdouble*)(NewData))[j + c] = ((ILdouble*)(Data))[i];
								}
							}
							break;
					}
					break;

				case IL_RGBA:
				case IL_BGRA:
					NewData = (ILubyte*)ialloc(NumPix * BpcDest * 4);
					CHECK_ALLOC();

					switch (DestType)
					{
						case IL_UNSIGNED_BYTE:
						case IL_BYTE:
							for (i = 0, j = 0; i < NumPix; i++, j += 4) {
								for (c = 0; c < 3; c++) {
									NewData[j + c] = ((ILubyte*)(Data))[i];
								}
								NewData[j + 3] = UCHAR_MAX;  // Full opacity
							}
							break;
						case IL_UNSIGNED_SHORT:
						case IL_SHORT:
							for (i = 0, j = 0; i < NumPix; i++, j += 4) {
								for (c = 0; c < 3; c++) {
									((ILushort*)(NewData))[j + c] = ((ILushort*)(Data))[i];
								}
								((ILushort*)(NewData))[j + 3] = USHRT_MAX;  // Full opacity
							}
							break;
						case IL_UNSIGNED_INT:
						case IL_INT:
							for (i = 0, j = 0; i < NumPix; i++, j += 4) {
								for (c = 0; c < 3; c++) {
									((ILuint*)(NewData))[j + c] = ((ILuint*)(Data))[i];
								}
								((ILuint*)(NewData))[j + 3] = UINT_MAX;  // Full opacity
							}
							break;
						case IL_FLOAT:
							for (i = 0, j = 0; i < NumPix; i++, j += 4) {
								for (c = 0; c < 3; c++) {
									((ILfloat*)(NewData))[j + c] = ((ILfloat*)(Data))[i];
								}
								((ILfloat*)(NewData))[j + 3] = 1.0f;  // Full opacity
							}
							break;
						case IL_DOUBLE:
							for (i = 0, j = 0; i < NumPix; i++, j += 4) {
								for (c = 0; c < 3; c++) {
									((ILdouble*)(NewData))[j + c] = ((ILdouble*)(Data))[i];
								}
								((ILdouble*)(NewData))[j + 3] = 1.0;  // Full opacity
							}
							break;
					}
					break;

				/*case IL_COLOUR_INDEX:
					NewData = (ILubyte*)ialloc(iCurImage->SizeOfData);
					NewImage->Pal.Palette = (ILubyte*)ialloc(768);
					if (NewData == NULL || NewImage->Pal.Palette) {
						ilSetError(IL_OUT_OF_MEMORY);
						ifree(NewImage);
						return IL_FALSE;
					}

					// Fill the palette
					for (i = 0; i < 256; i++) {
						for (c = 0; c < 3; c++) {
							NewImage->Pal.Palette[i * 3 + c] = (ILubyte)i;
						}
					}
					// Copy the data
					for (i = 0; i < iCurImage->SizeOfData; i++) {
						NewData[i] = iCurImage->Data[i];
					}
					break;*/

				default:
					ilSetError(IL_INVALID_CONVERSION);
					if (Data != Buffer)
						ifree(Data);
					return NULL;
			}
			break;
	}

	if (Data != Buffer)
		ifree(Data);

	return NewData;
}


// Really shouldn't have to check for default, as in above ilConvertBuffer().
ILvoid* ILAPIENTRY iSwitchTypes(ILuint SizeOfData, ILenum SrcType, ILenum DestType, ILvoid *Buffer)
{
	static ILuint		BpcSrc, BpcDest, Size, i;
	static ILubyte		*NewData, *BytePtr;
	static ILushort		*ShortPtr;
	static ILuint		*IntPtr;
	static ILfloat		*FloatPtr;
	static ILdouble		*DblPtr;

	BpcSrc = ilGetBppType(SrcType);
	BpcDest = ilGetBppType(DestType);
	Size = SizeOfData / BpcSrc;

	if (BpcSrc == BpcDest) {
		return Buffer;
	}

	NewData = (ILubyte*)ialloc(Size * BpcDest);
	if (NewData == NULL) {
		ilSetError(IL_OUT_OF_MEMORY);
		return IL_FALSE;
	}

	switch (DestType)
	{
		case IL_UNSIGNED_BYTE:
		case IL_BYTE:
			BytePtr = (ILubyte*)NewData;
			switch (SrcType)
			{
				case IL_UNSIGNED_SHORT:
				case IL_SHORT:
					for (i = 0; i < Size; i++) {
						BytePtr[i] = ((ILushort*)Buffer)[i] >> 8;
					}
					break;
				case IL_UNSIGNED_INT:
				case IL_INT:
					for (i = 0; i < Size; i++) {
						BytePtr[i] = ((ILuint*)Buffer)[i] >> 24;
					}
					break;
				case IL_FLOAT:
					for (i = 0; i < Size; i++) {
						BytePtr[i] = (ILubyte)( ((ILfloat*)Buffer)[i] * UCHAR_MAX);
					}
					break;
				case IL_DOUBLE:
					for (i = 0; i < Size; i++) {
						BytePtr[i] = (ILubyte)( ((ILdouble*)Buffer)[i] * UCHAR_MAX);
					}
					break;
			}
			break;

		case IL_UNSIGNED_SHORT:
		case IL_SHORT:
			ShortPtr = (ILushort*)NewData;
			switch (SrcType)
			{
				case IL_UNSIGNED_BYTE:
				case IL_BYTE:
					for (i = 0; i < Size; i++) {
						ShortPtr[i] = ((ILubyte*)Buffer)[i] << 8;
					}
					break;
				case IL_UNSIGNED_INT:
				case IL_INT:
					for (i = 0; i < Size; i++) {
						ShortPtr[i] = ((ILuint*)Buffer)[i] >> 16;
					}
					break;
				case IL_FLOAT:
					for (i = 0; i < Size; i++) {
						ShortPtr[i] = (ILushort)( ((ILfloat*)Buffer)[i] * USHRT_MAX);
					}
					break;
				case IL_DOUBLE:
					for (i = 0; i < Size; i++) {
						ShortPtr[i] = (ILushort)( ((ILdouble*)Buffer)[i] * USHRT_MAX);
					}
					break;
			}
			break;

		case IL_UNSIGNED_INT:
		case IL_INT:
			IntPtr = (ILuint*)NewData;
			switch (SrcType)
			{
				case IL_UNSIGNED_BYTE:
				case IL_BYTE:
					for (i = 0; i < Size; i++) {
						IntPtr[i] = ((ILubyte*)Buffer)[i] << 24;
					}
					break;
				case IL_UNSIGNED_SHORT:
				case IL_SHORT:
					for (i = 0; i < Size; i++) {
						IntPtr[i] = ((ILushort*)Buffer)[i] << 16;
					}
					break;
				case IL_FLOAT:
					for (i = 0; i < Size; i++) {
						IntPtr[i] = (ILuint)( ((ILfloat*)Buffer)[i] * UINT_MAX);
					}
					break;
				case IL_DOUBLE:
					for (i = 0; i < Size; i++) {
						IntPtr[i] = (ILuint)( ((ILdouble*)Buffer)[i] * UINT_MAX);
					}
					break;
			}
			break;

		case IL_FLOAT:
			FloatPtr = (ILfloat*)NewData;
			switch (SrcType)
			{
				case IL_UNSIGNED_BYTE:
					for (i = 0; i < Size; i++) {
						FloatPtr[i] = ((ILubyte*)Buffer)[i];
					}
					break;
				case IL_BYTE:
					for (i = 0; i < Size; i++) {
						FloatPtr[i] = ((ILbyte*)Buffer)[i];
					}
					break;
				case IL_UNSIGNED_SHORT:
					for (i = 0; i < Size; i++) {
						FloatPtr[i] = ((ILushort*)Buffer)[i];
					}
					break;
				case IL_SHORT:
					for (i = 0; i < Size; i++) {
						FloatPtr[i] = ((ILshort*)Buffer)[i];
					}
					break;
				case IL_UNSIGNED_INT:
					for (i = 0; i < Size; i++) {
						FloatPtr[i] = (ILfloat)((ILuint*)Buffer)[i];
					}
					break;
				case IL_INT:
					for (i = 0; i < Size; i++) {
						FloatPtr[i] = (ILfloat)((ILint*)Buffer)[i];
					}
					break;
				case IL_DOUBLE:
					for (i = 0; i < Size; i++) {
						FloatPtr[i] = (ILfloat)((ILdouble*)Buffer)[i];
					}
					break;
			}
			break;

		case IL_DOUBLE:
			DblPtr = (ILdouble*)NewData;
			switch (SrcType)
			{
				case IL_UNSIGNED_BYTE:
					for (i = 0; i < Size; i++) {
						DblPtr[i] = ((ILubyte*)Buffer)[i];
					}
					break;
				case IL_BYTE:
					for (i = 0; i < Size; i++) {
						DblPtr[i] = ((ILbyte*)Buffer)[i];
					}
					break;
				case IL_UNSIGNED_SHORT:
					for (i = 0; i < Size; i++) {
						DblPtr[i] = ((ILushort*)Buffer)[i];
					}
					break;
				case IL_SHORT:
					for (i = 0; i < Size; i++) {
						DblPtr[i] = ((ILshort*)Buffer)[i];
					}
					break;
				case IL_UNSIGNED_INT:
					for (i = 0; i < Size; i++) {
						DblPtr[i] = ((ILuint*)Buffer)[i];
					}
					break;
				case IL_INT:
					for (i = 0; i < Size; i++) {
						DblPtr[i] = ((ILint*)Buffer)[i];
					}
					break;
				case IL_FLOAT:
					for (i = 0; i < Size; i++) {
						DblPtr[i] = ((ILfloat*)Buffer)[i];
					}
					break;
			}
			break;
	}

	return NewData;
}