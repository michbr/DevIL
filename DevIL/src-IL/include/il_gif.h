//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2002 by Denton Woods
// Last modified: 05/18/2002 <--Y2K Compliant! =]
//
// Filename: il/il_gif.h
//
// Description: Reads from a Graphics Interchange Format (.gif) file.
//
//-----------------------------------------------------------------------------


#ifndef GIF_H
#define GIF_H

#include "il_internal.h"

#define GIF87A 87
#define GIF89A 89

#ifdef _WIN32
	#pragma pack(push, gif_struct, 1)
#endif
typedef struct GIFHEAD
{
	char		Sig[6];
	ILushort	Width;
	ILushort	Height;
	ILubyte		ColourInfo;
	ILubyte		Background;
	ILubyte		Aspect;
} IL_PACKSTRUCT GIFHEAD;

typedef struct IMAGEDESC
{
	ILubyte		Separator;
	ILushort	OffX;
	ILushort	OffY;
	ILushort	Width;
	ILushort	Height;
	ILubyte		ImageInfo;
} IL_PACKSTRUCT IMAGEDESC;
#ifdef _WIN32
	#pragma pack(pop, gif_struct)
#endif

// Internal functions
ILboolean iLoadGifInternal(ILvoid);
ILboolean ilLoadGifF(ILHANDLE File);
ILboolean iIsValidGif(ILvoid);
ILboolean GetPalette(ILubyte Info, ILpal *Pal);
ILboolean GetImages(ILpal *GlobalPal);
ILboolean SkipExtensions(ILvoid);
ILboolean GifGetData(ILvoid);
ILboolean RemoveInterlace(ILvoid);
ILboolean CopyPalette(ILpal *Dest, ILpal *Src);

#endif//GIF_H