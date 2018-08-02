// ==========================================================
// GIF Loader
//
// Design and implementation by
// - Rui Godinho Lopes <ruiglopes@yahoo.com>
// - Detlev Vendt (detlev.vendt@brillit.de)
//
// This file is part of FreeImage 3
//
// COVERED CODE IS PROVIDED UNDER THIS LICENSE ON AN "AS IS" BASIS, WITHOUT WARRANTY
// OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, WITHOUT LIMITATION, WARRANTIES
// THAT THE COVERED CODE IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
// OR NON-INFRINGING. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE COVERED
// CODE IS WITH YOU. SHOULD ANY COVERED CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT
// THE INITIAL DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY NECESSARY
// SERVICING, REPAIR OR CORRECTION. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL
// PART OF THIS LICENSE. NO USE OF ANY COVERED CODE IS AUTHORIZED HEREUNDER EXCEPT UNDER
// THIS DISCLAIMER.
//
// Use at your own risk!
// ==========================================================

#define GIF_PLUGIN_SAVE_SUPPORT	// enable write support only with UNISYS license ....

extern "C" {
#include "../LibGIF/gif_lib.h"
}
#include "FreeImage.h"
#include "Utilities.h"

// interlace pattern constants for loading/saving interlaced gif files
static const int InterlacedOffset[]= { 0, 4, 2, 1 }; // The way Interlaced image should
static const int InterlacedJumps[]= { 8, 8, 4, 2 };  // be read - offsets and jumps...

#ifdef WIN32
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

struct TGIFGraphicControlExtensionBlock
{
	BYTE nFlags;
	WORD nDelay;
	BYTE nTransparentColorIndex;
};

#ifdef WIN32
#pragma pack(pop)
#else
#pragma pack(4)
#endif

// data structure created at GifPlugin_Open and destroyed at GifPlugin_Close
struct TGifPluginData {
	FreeImageIO		*m_pIO;
	fi_handle		 m_Handle;
	GifFileType		*m_pGifFileType;
	int				 m_nTransparentColorIndex;
#ifdef GIF_PLUGIN_SAVE_SUPPORT
	BOOL			 m_bRead;
#endif
};

// adapter function for use by the gif-lib input logic
int InputFuncAdapt(GifFileType *pGifFileType, GifByteType *pBuf, int nBytesToRead) {
	return ((TGifPluginData *)pGifFileType->UserData)->m_pIO->read_proc(
		pBuf, 1, nBytesToRead, ((TGifPluginData *)pGifFileType->UserData)->m_Handle);
}

// adapter function for use by the gif-lib output logic
#ifdef GIF_PLUGIN_SAVE_SUPPORT
int OutputFuncAdapt(GifFileType *pGifFileType, const GifByteType *pBuf, int nBytesToWrite) {
	return ((TGifPluginData *)pGifFileType->UserData)->m_pIO->write_proc(
		(unsigned char *)pBuf, 1, nBytesToWrite, ((TGifPluginData *)pGifFileType->UserData)->m_Handle);
}
#endif

// ==========================================================
// Plugin Interface
// ==========================================================

static int gs_format_id;

// ==========================================================
// Plugin Implementation
// ==========================================================

static const char * DLL_CALLCONV 
GifPlugin_Format() {
	return "GIF";
}

static const char * DLL_CALLCONV 
GifPlugin_MimeType() {
	return "image/gif";
}

static const char * DLL_CALLCONV 
GifPlugin_Description() {
	return "Graphics Interchange Format";
}

static const char * DLL_CALLCONV 
GifPlugin_Extension() {
	return "gif";
}

static const char * DLL_CALLCONV 
GifPlugin_RegExpr() {
	return "^GIF";
}

static BOOL DLL_CALLCONV 
GifPlugin_SupportsExportDepth(int depth) {
#ifdef GIF_PLUGIN_SAVE_SUPPORT
	return (depth == 8);
#else
	return FALSE;
#endif
}

static BOOL DLL_CALLCONV 
GifPlugin_SupportsExportType(FREE_IMAGE_TYPE type) {
	return (type == FIT_BITMAP) ? TRUE : FALSE;
}

static BOOL DLL_CALLCONV 
GifPlugin_Validate(FreeImageIO *io, fi_handle handle) {
	char buf[GIF_STAMP_LEN];
	int  nBytesReaded = io->read_proc(buf, 1, GIF_STAMP_LEN, handle);

	//NOTE: The actual gif file version is not checked, this because
	//the GIF file structure is designed not to change significantly...	
	BOOL bResult = (nBytesReaded != GIF_STAMP_LEN ? FALSE : !strncmp(GIF_STAMP, buf, GIF_VERSION_POS));

	io->seek_proc(handle, -nBytesReaded, SEEK_CUR);

	return bResult;
}

// this plugin open function
static void *DLL_CALLCONV 
GifPlugin_Open(FreeImageIO *io, fi_handle handle, BOOL read) {

	TGifPluginData *pData= new TGifPluginData;
	if (!pData)
		return NULL;

	pData->m_Handle = handle;
	pData->m_pIO    = io;
	pData->m_nTransparentColorIndex = -1; // set to no transparent color
#ifdef GIF_PLUGIN_SAVE_SUPPORT
	pData->m_bRead  = read;
#endif

	if (read)
	{
		pData->m_pGifFileType = ::DGifOpen(pData, InputFuncAdapt);
	}
	else
	{
#ifdef GIF_PLUGIN_SAVE_SUPPORT
		pData->m_pGifFileType = ::EGifOpen(pData, OutputFuncAdapt);
#else
		pData->m_pGifFileType = NULL;
#endif
	}

	if (!pData->m_pGifFileType)
	{
		delete pData;
		return NULL;
	}

	return pData;
}


// this plugin close function
static void DLL_CALLCONV 
GifPlugin_Close(FreeImageIO *io, fi_handle handle, void *data) {

	TGifPluginData *pData = (TGifPluginData *)data;
	if (!pData)
		return;

#ifdef GIF_PLUGIN_SAVE_SUPPORT
	if (pData->m_bRead)
		::DGifCloseFile(pData->m_pGifFileType);
	else
		::EGifCloseFile(pData->m_pGifFileType);
#else
	::DGifCloseFile(pData->m_pGifFileType);
#endif

	delete pData;
}

// this plugin load function
static FIBITMAP * DLL_CALLCONV 
GifPlugin_Load(FreeImageIO *io, fi_handle handle, int page, int flags, void *data) {

	TGifPluginData *pData = (TGifPluginData *)data;
	if (!pData)
		return NULL;

	FIBITMAP *pBitmap= NULL; // the freeimage bitmap. NOTE: This is needed here because of the exception logic (if we throw an exeception we will free the bitmap in the catch block).
	GifFileType *pGifFile = pData->m_pGifFileType;
	GifRecordType RecordType;

	try {
		do {
			if (::DGifGetRecordType(pGifFile, &RecordType) == GIF_ERROR)
				throw "io error or invalid gif format";

			switch (RecordType) {

				case IMAGE_DESC_RECORD_TYPE:
				{
					/*if (8 != pGifFile->SColorResolution)
						throw "only 8 bit color resolution gif are supported";*/

					if (::DGifGetImageDesc(pGifFile) == GIF_ERROR)
						throw "io error or invalid gif format";

					//
					// Read the image line by line
					// NOTE: The global size of the image is given by,
					// GifFile->SWidth and GifFile->SHeight
					//
					// The following sizes/positions are for the current frame only!
					//
					int nImagePosX   = pGifFile->Image.Left;
					int nImagePosY   = pGifFile->Image.Top;
					int nImageWidth  = pGifFile->Image.Width;
					int nImageHeight = pGifFile->Image.Height;

					if (nImagePosX < 0 || 
						nImagePosX > pGifFile->SWidth || 
						nImagePosX + nImageWidth > pGifFile->SWidth	|| 
						nImagePosY < 0 || 
						nImagePosY > pGifFile->SHeight || 
						nImagePosY + nImageHeight > pGifFile->SHeight)
						throw "invalid gif dimensions";


					//
					// 1. Allocate a freeimage bitmap
					//

					pBitmap = FreeImage_Allocate(pGifFile->SWidth, pGifFile->SHeight, 8);
					if (!pBitmap)
						throw "DIB allocation failed";

					// Set's the transparent color of the gif
					if (pData->m_nTransparentColorIndex >= 0)
					{
						BYTE TransparencyTable[256];

						memset(TransparencyTable, 0xff, pData->m_nTransparentColorIndex);
						TransparencyTable[pData->m_nTransparentColorIndex] = 0;

						FreeImage_SetTransparencyTable(pBitmap, 
							TransparencyTable, pData->m_nTransparentColorIndex+1);
					}

					// clear dib with GIF background color
					{
						//TODO: memset the all the bib at once and not line by line.
						for (int n = 0; n < pGifFile->SHeight; ++n)
						{
							BYTE *pScanLine = FreeImage_GetScanLine(pBitmap, n);
							memset(pScanLine, pGifFile->SBackGroundColor, pGifFile->SWidth);
						}
					}


					//
					// 2. copy the palette from gif to freeimage palette
					//

					// get the current image color map (the image map can be local to a frame or global to all the file)
					ColorMapObject *pImageColorMap = 
						pGifFile->Image.ColorMap? pGifFile->Image.ColorMap : pGifFile->SColorMap;

					if (pImageColorMap->ColorCount > 256)
						throw "invalid gif color count";

					GifColorType *pGifPalette = pImageColorMap->Colors;
					RGBQUAD *pPalette = FreeImage_GetPalette(pBitmap);

					for (int nColorIndex = 0; nColorIndex < pImageColorMap->ColorCount; ++nColorIndex)
					{
						pPalette->rgbRed   = pGifPalette->Red;
						pPalette->rgbGreen = pGifPalette->Green;
						pPalette->rgbBlue  = pGifPalette->Blue;

						++pGifPalette;
						++pPalette;
					}

					//
					// 3. copy gif scanlines to freeimage bitmap
					//

					if (pGifFile->Image.Interlace)
					{
						// Need to perform 4 passes on the image
						for (int nPass= 0; nPass < 4; ++nPass)
							for (int n = InterlacedOffset[nPass]; n < nImageHeight; n += InterlacedJumps[nPass])
							{
								//NOTE: The gif is an top-down image BUT freeimage dib are bottom-up
								BYTE *pScanLine = FreeImage_GetScanLine(pBitmap, nImageHeight-n-1+nImagePosY);

								if (::DGifGetLine(pGifFile, pScanLine, nImageWidth) == GIF_ERROR)
									throw "io error or invalid gif format";
							}
					}
					else
						for (int n = nImageHeight; n--; )
						{
							//NOTE: The gif is an top-down image BUT freeimage dib are bottom-up
							BYTE *pScanLine = FreeImage_GetScanLine(pBitmap, n+nImagePosY);

							if (::DGifGetLine(pGifFile, pScanLine+nImagePosX, nImageWidth) == GIF_ERROR)
								throw "io error or invalid gif format";
						}


					// free this image since we don't need it in the gif structure...
					// NOTE: If we don't free it and the gif as more than one image
					// that images all get appended to the SavedImages array.
					::FreeSavedImages(pGifFile);
					pGifFile->SavedImages = NULL;
					pGifFile->ImageCount  = 0;

					return pBitmap;
				}
				break;

				case EXTENSION_RECORD_TYPE:
				{
					int ExtCode;
					GifByteType *Extension;

					//
					// Skip any extension blocks in file
					//
					if (::DGifGetExtension(pGifFile, &ExtCode, &Extension) == GIF_ERROR)
						throw "io error or invalid gif format";

					if (ExtCode == GRAPHICS_EXT_FUNC_CODE	// is this a Graphic Control Extension block?
						&& Extension[0] >= 4)				// this block must have at least 4 bytes!
					{
						TGIFGraphicControlExtensionBlock *pExt = 
							(TGIFGraphicControlExtensionBlock *)&Extension[1];

						if (pExt->nFlags&0x01)  // this image has an transparent color?
							pData->m_nTransparentColorIndex = pExt->nTransparentColorIndex;

						//in pExt->nDelay is the delay time in 1/100 of a second that the next frame is expected to wait
						//unsigned nDelay= (Extension[2] | (Extension[3]<<8)) * 10; // nDelay converted to ms
					}

					while (Extension)
					{
						if (::DGifGetExtensionNext(pGifFile, &Extension) == GIF_ERROR)
							throw "io error or invalid gif format";
					}
				}
				break;
			} // switch

		} while (RecordType != TERMINATE_RECORD_TYPE);

	} catch (const char *pErrorMsg)	{
		FreeImage_OutputMessageProc(gs_format_id, pErrorMsg);
	}
	FreeImage_Unload(pBitmap);
	return NULL;
}

// this plugin save function
#ifdef GIF_PLUGIN_SAVE_SUPPORT
static BOOL DLL_CALLCONV 
GifPlugin_Save(FreeImageIO *io, FIBITMAP *pDib, fi_handle handle, int page, int flags, void *data) {

	TGifPluginData *pData = (TGifPluginData *)data;
	if (!pData)
		return FALSE;

	GifFileType *pGifFile = pData->m_pGifFileType;
	ColorMapObject *pGifColorMap = NULL;

	try {
		if (FreeImage_GetBPP(pDib) != 8)
			throw "Only 8 bit images are supported";

		//
		// if we have an transparency table make sure that the transparent
		// color is with 0 (full transparent alpha) value.
		// NOTE: This plugin sets the gif transparent color index to the
		// last transparency index of the freeimage dib.
		//
		int nTransparentColorIndex = FreeImage_GetTransparencyCount(pDib)-1;
		if (nTransparentColorIndex >= 0 && FreeImage_GetTransparencyTable(pDib)[nTransparentColorIndex])
			throw "invalid transparent color in dib";

		//
		// copy color table from dib into gif color map
		//
		if (!(pGifColorMap = ::MakeMapObject(256, NULL)))
			throw "No memory";

		RGBQUAD *pPalette = FreeImage_GetPalette(pDib);
		GifColorType *pGifPalette = pGifColorMap->Colors;

		for (int nColorIndex = 0; nColorIndex < 256; ++nColorIndex)
		{
			pGifPalette->Red   = pPalette->rgbRed;
			pGifPalette->Green = pPalette->rgbGreen;
			pGifPalette->Blue  = pPalette->rgbBlue;

			++pGifPalette;
			++pPalette;
		}

		//
		// write gif screen description
		//
		if (::EGifPutScreenDesc(pGifFile, 
				FreeImage_GetWidth(pDib), FreeImage_GetHeight(pDib), 8, 0, NULL) == GIF_ERROR)
			throw "io error";

		//
		// write transparent color information if the dib has one
		//
		if (nTransparentColorIndex >= 0)
		{
			TGIFGraphicControlExtensionBlock Extension = { 0x01, 0, nTransparentColorIndex };
			if (::EGifPutExtension(pGifFile, GRAPHICS_EXT_FUNC_CODE, 4, &Extension) == GIF_ERROR)
				throw "io error";
		}

		//
		// write image data
		//
		int nImageWidth  = pGifFile->SWidth;
		int nImageHeight = pGifFile->SHeight;

		//TODO: retreive the interlace option value from flags argument
		if (::EGifPutImageDesc(pGifFile, 0, 0, 
				nImageWidth, nImageHeight, FALSE, pGifColorMap) == GIF_ERROR)
			throw "io error";

		::FreeMapObject(pGifColorMap);
		pGifColorMap = NULL; //NOTE: need to place NULL here because the exception handling code frees it!

		for (int n= nImageHeight; n--; )
		{
			//NOTE: The gif is an top-down image BUT freeimage dib are bottom-up
			BYTE *pScanLine = FreeImage_GetScanLine(pDib, n);

			if (::EGifPutLine(pGifFile, pScanLine, nImageWidth) == GIF_ERROR)
				throw "io error";
		}
		return TRUE;

	} catch (const char *pErrorMsg)	{
		FreeImage_OutputMessageProc(gs_format_id, pErrorMsg);
	}

	if (pGifColorMap)
		::FreeMapObject(pGifColorMap);

	return FALSE;
}
#endif //#ifdef GIF_PLUGIN_SAVE_SUPPORT

// ==========================================================
//   Init
// ==========================================================

void DLL_CALLCONV 
InitGIF(Plugin *plugin, int format_id) {
	gs_format_id = format_id;

	plugin->format_proc = GifPlugin_Format;
	plugin->description_proc = GifPlugin_Description;
	plugin->extension_proc = GifPlugin_Extension;
	plugin->regexpr_proc = GifPlugin_RegExpr;
	plugin->open_proc = GifPlugin_Open;
	plugin->load_proc = GifPlugin_Load;

#ifdef GIF_PLUGIN_SAVE_SUPPORT
	plugin->save_proc = GifPlugin_Save;
#else
	plugin->save_proc = NULL;
#endif // #ifdef GIF_PLUGIN_SAVE_SUPPORT

	plugin->close_proc = GifPlugin_Close;
	plugin->validate_proc = GifPlugin_Validate;

	plugin->pagecount_proc = NULL;
	plugin->pagecapability_proc = NULL;
	plugin->mime_proc = GifPlugin_MimeType;
	plugin->supports_export_bpp_proc = GifPlugin_SupportsExportDepth;
	plugin->supports_export_type_proc = GifPlugin_SupportsExportType;
	plugin->supports_icc_profiles_proc = NULL;

#ifdef GIF_PLUGIN_SAVE_SUPPORT
	//NOTE: Since this plugin can make transparent gif
	//we will produce gif89a version gif's
	::EGifSetGifVersion("89a");
#endif // #ifdef GIF_PLUGIN_SAVE_SUPPORT

}

