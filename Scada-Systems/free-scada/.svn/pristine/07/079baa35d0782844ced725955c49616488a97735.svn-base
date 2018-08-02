// ==========================================================
// PNG Loader and Writer
//
// Design and implementation by
// - Floris van den Berg (flvdberg@wxs.nl)
// - Herve Drolon (drolon@infonie.fr)
// - Detlev Vendt (detlev.vendt@brillit.de)
// - Aaron Shumate (trek@startreker.com)
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

#pragma warning (disable : 4786) // identifier was truncated to 'number' characters

#include "FreeImage.h"
#include "Utilities.h"

#include "../Metadata/FreeImageTag.h"

// ----------------------------------------------------------

#define PNG_ASSEMBLER_CODE_SUPPORTED
#define PNG_BYTES_TO_CHECK 8

// ----------------------------------------------------------

#include "../LibPNG/png.h"

// ----------------------------------------------------------

static FreeImageIO *s_io;
static fi_handle s_handle;

/////////////////////////////////////////////////////////////////////////////
// libpng interface 
// 

static void
_ReadProc(struct png_struct_def *, unsigned char *data, png_size_t size) {
	s_io->read_proc(data, size, 1, s_handle);
}

static void
_WriteProc(struct png_struct_def *, unsigned char *data, png_size_t size) {
	s_io->write_proc(data, size, 1, s_handle);
}

static void
_FlushProc(png_structp png_ptr) {
	// empty flush implementation
}

static void
error_handler(struct png_struct_def *, const char *error) {
	throw error;
}

// in FreeImage warnings disabled

static void
warning_handler(struct png_struct_def *, const char *warning) {
}

// ==========================================================
// Metadata routines
// ==========================================================

static BOOL 
ReadMetadata(png_structp png_ptr, png_infop info_ptr, FIBITMAP *dib) {
	// XMP keyword
	char *g_png_xmp_keyword = "XML:com.adobe.xmp";

	png_textp text_ptr = NULL;
	int num_text = 0;

	// iTXt/tEXt/zTXt chuncks
	if(png_get_text(png_ptr, info_ptr, &text_ptr, &num_text) > 0) {
		for(int i = 0; i < num_text; i++) {
			FITAG tag;
			memset(&tag, 0, sizeof(FITAG));

			tag.length = MAX(text_ptr[i].text_length, text_ptr[i].itxt_length);
			tag.count = tag.length;
			tag.type = FIDT_ASCII;
			tag.value = (char*)text_ptr[i].text;

			if(strcmp(text_ptr[i].key, g_png_xmp_keyword) == 0) {
				// store the tag as XMP
				tag.key = g_TagLib_XMPFieldName;
				FreeImage_SetMetadata(FIMD_XMP, dib, tag.key, &tag);
			} else {
				// store the tag as a comment
				tag.key = text_ptr[i].key;
				FreeImage_SetMetadata(FIMD_COMMENTS, dib, tag.key, &tag);
			}
		}
	}

	return TRUE;
}

static BOOL 
WriteMetadata(png_structp png_ptr, png_infop info_ptr, FIBITMAP *dib) {
	// XMP keyword
	char *g_png_xmp_keyword = "XML:com.adobe.xmp";

	FITAG *tag = NULL;
	FIMETADATA *mdhandle = NULL;
	BOOL bResult = TRUE;

	png_text text_metadata;
	int num_text = 0;

	// set the 'Comments' metadata as iTXt chuncks

	mdhandle = FreeImage_FindFirstMetadata(FIMD_COMMENTS, dib, &tag);

	if(mdhandle) {
		do {
			memset(&text_metadata, 0, sizeof(png_text));
			text_metadata.compression = 1;			 // iTXt, none
			text_metadata.key = tag->key;			 // keyword, 1-79 character description of "text"
			text_metadata.text = (char*)tag->value;	 // comment, may be an empty string (ie "")
			text_metadata.text_length = tag->length; // length of the text string
			text_metadata.itxt_length = tag->length; // length of the itxt string
			text_metadata.lang = 0;					 // language code, 0-79 characters or a NULL pointer
			text_metadata.lang_key = 0;				 // keyword translated UTF-8 string, 0 or more chars or a NULL pointer

			// set the tag 
			png_set_text(png_ptr, info_ptr, &text_metadata, 1);

		} while(FreeImage_FindNextMetadata(mdhandle, &tag));

		FreeImage_FindCloseMetadata(mdhandle);
		bResult &= TRUE;
	}

	// set the 'XMP' metadata as iTXt chuncks
	tag = NULL;
	FreeImage_GetMetadata(FIMD_XMP, dib, g_TagLib_XMPFieldName, &tag);
	if(tag && tag->length) {
		memset(&text_metadata, 0, sizeof(png_text));
		text_metadata.compression = 1;			 // iTXt, none
		text_metadata.key = g_png_xmp_keyword;	 // keyword, 1-79 character description of "text"
		text_metadata.text = (char*)tag->value;	 // comment, may be an empty string (ie "")
		text_metadata.text_length = tag->length; // length of the text string
		text_metadata.itxt_length = tag->length; // length of the itxt string
		text_metadata.lang = 0;					 // language code, 0-79 characters or a NULL pointer
		text_metadata.lang_key = 0;				 // keyword translated UTF-8 string, 0 or more chars or a NULL pointer

		// set the tag 
		png_set_text(png_ptr, info_ptr, &text_metadata, 1);
		bResult &= TRUE;
	}

	return bResult;
}

// ==========================================================
// Plugin Interface
// ==========================================================

static int s_format_id;

// ==========================================================
// Plugin Implementation
// ==========================================================

static const char * DLL_CALLCONV
Format() {
	return "PNG";
}

static const char * DLL_CALLCONV
Description() {
	return "Portable Network Graphics";
}

static const char * DLL_CALLCONV
Extension() {
	return "png";
}

static const char * DLL_CALLCONV
RegExpr() {
	return "^.PNG\r";
}

static const char * DLL_CALLCONV
MimeType() {
	return "image/png";
}

static BOOL DLL_CALLCONV
Validate(FreeImageIO *io, fi_handle handle) {
	BYTE png_signature[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
	BYTE signature[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	io->read_proc(&signature, 1, 8, handle);

	return (memcmp(png_signature, signature, 8) == 0);
}

static BOOL DLL_CALLCONV
SupportsExportDepth(int depth) {
	return (
			(depth == 1) ||
			(depth == 4) ||
			(depth == 8) ||
			(depth == 24) ||
			(depth == 32)
		);
}

static BOOL DLL_CALLCONV 
SupportsExportType(FREE_IMAGE_TYPE type) {
	return (
		(type == FIT_BITMAP) ||
		(type == FIT_UINT16)
	);
}

static BOOL DLL_CALLCONV
SupportsICCProfiles() {
	return TRUE;
}

// ----------------------------------------------------------

static FIBITMAP * DLL_CALLCONV
Load(FreeImageIO *io, fi_handle handle, int page, int flags, void *data) {
	png_structp png_ptr;
	png_infop info_ptr;
	png_uint_32 width, height;
	png_colorp png_palette;
	int color_type, palette_entries;
	int bit_depth, pixel_depth;		// pixel_depth = bit_depth * channels

	FIBITMAP *dib = NULL;
	RGBQUAD *palette = NULL;		// pointer to dib palette
	png_bytepp  row_pointers = NULL;
	int i;

	s_io = io;
	s_handle = handle;

	if (handle) {
		try {		
			// check to see if the file is in fact a PNG file

			unsigned char png_check[PNG_BYTES_TO_CHECK];

			io->read_proc(png_check, PNG_BYTES_TO_CHECK, 1, handle);

			if (png_sig_cmp(png_check, (png_size_t)0, PNG_BYTES_TO_CHECK) != 0)
				return NULL;	// Bad signature
			
			// create the chunk manage structure

			png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)error_handler, error_handler, warning_handler);

			if (!png_ptr)
				return NULL;			

			// create the info structure

		    info_ptr = png_create_info_struct(png_ptr);

			if (!info_ptr) {
				png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
				return NULL;
			}

			// init the IO

			png_set_read_fn(png_ptr, info_ptr, _ReadProc);

			if (setjmp(png_jmpbuf(png_ptr))) {
				png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
				return NULL;
			}

			// because we have already read the signature...

			png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK);

			// read the IHDR chunk

			png_read_info(png_ptr, info_ptr);
			png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);
			pixel_depth = info_ptr->pixel_depth;

			// get image data type (assume standard image type)

			FREE_IMAGE_TYPE image_type = FIT_BITMAP;
			if (bit_depth == 16) {
				if (color_type == PNG_COLOR_TYPE_GRAY) {
					image_type = FIT_UINT16;
#ifndef FREEIMAGE_BIGENDIAN
					// turn on 16 bit byte swapping
					png_set_swap(png_ptr);
#endif
				} else {
					// tell libpng to strip 16 bit/color files down to 8 bits/color
					png_set_strip_16(png_ptr);
					bit_depth = 8;
				}
			}


			// set some additional flags

			switch(color_type) {
				case PNG_COLOR_TYPE_RGB:
				case PNG_COLOR_TYPE_RGB_ALPHA:
#ifndef FREEIMAGE_BIGENDIAN
					// flip the RGB pixels to BGR (or RGBA to BGRA)

					png_set_bgr(png_ptr);
#endif
					break;

				case PNG_COLOR_TYPE_PALETTE:
					// expand palette images to the full 8 bits from 2 or 4 bits/pixel

					if ((pixel_depth == 2) || (pixel_depth == 4)) {
						png_set_packing(png_ptr);
						pixel_depth = 8;
					}					

					break;

				case PNG_COLOR_TYPE_GRAY:
					// expand grayscale images to the full 8 bits from 2 or 4 bits/pixel

					if ((pixel_depth == 2) || (pixel_depth == 4)) {
						png_set_expand(png_ptr);
						pixel_depth = 8;
					}

					break;

				case PNG_COLOR_TYPE_GRAY_ALPHA:
					// expand 8-bit greyscale + 8-bit alpha to 32-bit

					png_set_gray_to_rgb(png_ptr);
#ifndef FREEIMAGE_BIGENDIAN
					// flip the RGBA pixels to BGRA

					png_set_bgr(png_ptr);
#endif
					pixel_depth = 32;

					break;

				default:
					throw "PNG format not supported";
			}

			// Get the background color to draw transparent and alpha images over.
			// Note that even if the PNG file supplies a background, you are not required to
			// use it - you should use the (solid) application background if it has one.

			png_color_16p image_background = NULL;
			RGBQUAD rgbBkColor;

			if (png_get_bKGD(png_ptr, info_ptr, &image_background)) {
				rgbBkColor.rgbRed      = (BYTE)image_background->red;
				rgbBkColor.rgbGreen    = (BYTE)image_background->green;
				rgbBkColor.rgbBlue     = (BYTE)image_background->blue;
				rgbBkColor.rgbReserved = 0;
			}

			// if this image has transparency, store the trns values

			png_bytep trans               = NULL;
			int num_trans                 = 0;
			png_color_16p trans_values    = NULL;
			png_uint_32 transparent_value = png_get_tRNS(png_ptr, info_ptr, &trans, &num_trans, &trans_values);

			// unlike the example in the libpng documentation, we have *no* idea where
			// this file may have come from--so if it doesn't have a file gamma, don't
			// do any correction ("do no harm")

			double gamma = 0;
			double screen_gamma = 2.2;

			if (png_get_gAMA(png_ptr, info_ptr, &gamma) && ( flags & PNG_IGNOREGAMMA ) != PNG_IGNOREGAMMA)
				png_set_gamma(png_ptr, screen_gamma, gamma);

			// all transformations have been registered; now update info_ptr data

			png_read_update_info(png_ptr, info_ptr);

			// color type may have changed, due to our transformations

			color_type = png_get_color_type(png_ptr,info_ptr);

			// create a DIB and write the bitmap header
			// set up the DIB palette, if needed

			switch (color_type) {
				case PNG_COLOR_TYPE_RGB:
					png_set_invert_alpha(png_ptr);

					dib = FreeImage_Allocate(width, height, 24, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
					break;

				case PNG_COLOR_TYPE_RGB_ALPHA :
					dib = FreeImage_Allocate(width, height, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
					break;

				case PNG_COLOR_TYPE_PALETTE :
					dib = FreeImage_Allocate(width, height, pixel_depth);

					png_get_PLTE(png_ptr,info_ptr, &png_palette,&palette_entries);

					palette = FreeImage_GetPalette(dib);

					// store the palette

					for (i = 0; i < palette_entries; i++) {
						palette[i].rgbRed   = png_palette[i].red;
						palette[i].rgbGreen = png_palette[i].green;
						palette[i].rgbBlue  = png_palette[i].blue;
					}

					// store the transparency table

					if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
						FreeImage_SetTransparencyTable(dib, (BYTE *)trans, num_trans);					

					break;

				case PNG_COLOR_TYPE_GRAY:
					dib = FreeImage_AllocateT(image_type, width, height, pixel_depth);

					if(pixel_depth <= 8) {
						palette = FreeImage_GetPalette(dib);
						palette_entries = 1 << pixel_depth;

						for (i = 0; i < palette_entries; i++) {
							palette[i].rgbRed   =
							palette[i].rgbGreen =
							palette[i].rgbBlue  = (i * 255) / (palette_entries - 1);
						}
					}
					// store the transparency table

					if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
						FreeImage_SetTransparencyTable(dib, (BYTE *)trans, num_trans);					

					break;

				default:
					throw "PNG format not supported";
			}

			// store the background color 
			if(image_background) {
				FreeImage_SetBackgroundColor(dib, &rgbBkColor);
			}

			// get physical resolution

			if (png_get_valid(png_ptr, info_ptr, PNG_INFO_pHYs)) {
				png_uint_32 res_x, res_y;
				
				// we'll overload this var and use 0 to mean no phys data,
				// since if it's not in meters we can't use it anyway

				int res_unit_type = 0;

				png_get_pHYs(png_ptr,info_ptr,&res_x,&res_y,&res_unit_type);

				if (res_unit_type == 1) {
					BITMAPINFOHEADER *bih = FreeImage_GetInfoHeader(dib);

					bih->biXPelsPerMeter = res_x;
					bih->biYPelsPerMeter = res_y;
				}
			}

			// get possible ICC profile

			if (png_get_valid(png_ptr, info_ptr, PNG_INFO_iCCP)) {
				png_charp profile_name = NULL;
				png_charp profile_data = NULL;
				png_uint_32 profile_length = 0;
				int  compression_type;

				png_get_iCCP(png_ptr, info_ptr, &profile_name, &compression_type, &profile_data, &profile_length);

				// copy ICC profile data (must be done after FreeImage_Allocate)

				FreeImage_CreateICCProfile(dib, profile_data, profile_length);
			}


			// set the individual row_pointers to point at the correct offsets

			row_pointers = (png_bytepp)malloc(height * sizeof(png_bytep));

			if (!row_pointers) {
				if (palette)
					png_free(png_ptr, palette);				

				png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

				FreeImage_Unload(dib);
				return NULL;
			}

			// read in the bitmap bits via the pointer table

			for (png_uint_32 k = 0; k < height; k++)				
				row_pointers[height - 1 - k] = FreeImage_GetScanLine(dib, k);			

			png_read_image(png_ptr, row_pointers);

			// check if the bitmap contains transparency, if so enable it in the header

			if (FreeImage_GetBPP(dib) == 32)
				if (FreeImage_GetColorType(dib) == FIC_RGBALPHA)
					FreeImage_SetTransparent(dib, TRUE);
				else
					FreeImage_SetTransparent(dib, FALSE);
				
			// cleanup

			if (row_pointers) {
				free(row_pointers);
				row_pointers = NULL;
			}

			// read the rest of the file, getting any additional chunks in info_ptr

			png_read_end(png_ptr, info_ptr);

			// get possible metadata (it can be located both before and after the image data)

			ReadMetadata(png_ptr, info_ptr, dib);

			if (png_ptr) {
				// clean up after the read, and free any memory allocated - REQUIRED
				png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
			}

			return dib;
		} catch (const char *text) {
			if (png_ptr)
				png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
			
			if (row_pointers)
				free(row_pointers);

			if (dib)
				FreeImage_Unload(dib);			

			FreeImage_OutputMessageProc(s_format_id, text);
			
			return NULL;
		}
	}			

	return NULL;
}

static BOOL DLL_CALLCONV
Save(FreeImageIO *io, FIBITMAP *dib, fi_handle handle, int page, int flags, void *data) {
	png_structp png_ptr;
	png_infop info_ptr;
	png_colorp palette = NULL;
	png_uint_32 width, height;
	BOOL has_alpha_channel = FALSE;

	RGBQUAD *pal;					// pointer to dib palette
	int bit_depth, pixel_depth;		// pixel_depth = bit_depth * channels
	int palette_entries;
	int	interlace_type;

	s_io = io;
	s_handle = handle;

	if ((dib) && (handle)) {
		try {
			// create the chunk manage structure

			png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, (png_voidp)error_handler, error_handler, warning_handler);

			if (!png_ptr)  {
				return FALSE;
			}

			// allocate/initialize the image information data.

			info_ptr = png_create_info_struct(png_ptr);

			if (!info_ptr)  {
				png_destroy_write_struct(&png_ptr,  (png_infopp)NULL);
				return FALSE;
			}

			// Set error handling.  REQUIRED if you aren't supplying your own
			// error handling functions in the png_create_write_struct() call.

			if (setjmp(png_jmpbuf(png_ptr)))  {
				// if we get here, we had a problem reading the file

				png_destroy_write_struct(&png_ptr, &info_ptr);

				return FALSE;
			}

			// init the IO

			png_set_write_fn(png_ptr, info_ptr, _WriteProc, _FlushProc);

			// set physical resolution

			BITMAPINFOHEADER *bih = FreeImage_GetInfoHeader(dib);
			png_uint_32 res_x = bih->biXPelsPerMeter;
			png_uint_32 res_y = bih->biYPelsPerMeter;

			if ((res_x > 0) && (res_y > 0))  {
				png_set_pHYs(png_ptr, info_ptr, res_x, res_y, 1);
			}
	
			// Set the image information here.  Width and height are up to 2^31,
			// bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
			// the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
			// PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
			// or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
			// PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
			// currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE. REQUIRED

			interlace_type = PNG_INTERLACE_NONE;	// Default value
			width = FreeImage_GetWidth(dib);
			height = FreeImage_GetHeight(dib);
			pixel_depth = FreeImage_GetBPP(dib);	
			if(FreeImage_GetImageType(dib) == FIT_BITMAP) {
				// standard image type
				bit_depth = (pixel_depth > 8) ? 8 : pixel_depth;
			} else {
				// 16-bit greyscale
				bit_depth = pixel_depth;
			}


			switch (FreeImage_GetColorType(dib)) {
				case FIC_MINISWHITE:
					// Invert monochrome files to have 0 as black and 1 as white (no break here)
					png_set_invert_mono(png_ptr);

				case FIC_MINISBLACK:
					png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, 
						PNG_COLOR_TYPE_GRAY, interlace_type, 
						PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

					break;

				case FIC_PALETTE:
				{
					png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, 
						PNG_COLOR_TYPE_PALETTE, interlace_type, 
						PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

					// set the palette

					palette_entries = 1 << bit_depth;
					palette = (png_colorp)png_malloc(png_ptr, palette_entries * sizeof (png_color));
					pal = FreeImage_GetPalette(dib);

					for (int i = 0; i < palette_entries; i++) {
						palette[i].red   = pal[i].rgbRed;
						palette[i].green = pal[i].rgbGreen;
						palette[i].blue  = pal[i].rgbBlue;
					}
					
					png_set_PLTE(png_ptr, info_ptr, palette, palette_entries);

					// You must not free palette here, because png_set_PLTE only makes a link to
					// the palette that you malloced.  Wait until you are about to destroy
					// the png structure.

					break;
				}

				case FIC_RGBALPHA :
					has_alpha_channel = TRUE;

					png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, 
						PNG_COLOR_TYPE_RGBA, interlace_type, 
						PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

#ifndef FREEIMAGE_BIGENDIAN
					// flip BGR pixels to RGB
					png_set_bgr(png_ptr);
#endif
					break;
	
				case FIC_RGB:
					png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, 
						PNG_COLOR_TYPE_RGB, interlace_type, 
						PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

#ifndef FREEIMAGE_BIGENDIAN
					// flip BGR pixels to RGB
					png_set_bgr(png_ptr);
#endif
					break;
					
				case FIC_CMYK:
					break;
			}

			// write possible ICC profile

			FIICCPROFILE *iccProfile = FreeImage_GetICCProfile(dib);
			if (iccProfile->size && iccProfile->data) {
				png_set_iCCP(png_ptr, info_ptr, "Embedded Profile", 0, (png_charp)iccProfile->data, iccProfile->size);
			}

			// write metadata

			WriteMetadata(png_ptr, info_ptr, dib);

			// Optional gamma chunk is strongly suggested if you have any guess
			// as to the correct gamma of the image.
			// png_set_gAMA(png_ptr, info_ptr, gamma);

			// set the transparency table

			if ((pixel_depth == 8) && (FreeImage_IsTransparent(dib)) && (FreeImage_GetTransparencyCount(dib) > 0))
				png_set_tRNS(png_ptr, info_ptr, FreeImage_GetTransparencyTable(dib), FreeImage_GetTransparencyCount(dib), NULL);

			// set the background color

			if(FreeImage_HasBackgroundColor(dib)) {
				png_color_16 image_background;
				RGBQUAD rgbBkColor;

				FreeImage_GetBackgroundColor(dib, &rgbBkColor);
				memset(&image_background, 0, sizeof(png_color_16));
				image_background.blue  = rgbBkColor.rgbBlue;
				image_background.green = rgbBkColor.rgbGreen;
				image_background.red   = rgbBkColor.rgbRed;
				image_background.index = rgbBkColor.rgbReserved;

				png_set_bKGD(png_ptr, info_ptr, &image_background);
			}
			
			// Write the file header information.

			png_write_info(png_ptr, info_ptr);

			// write out the image data

#ifndef FREEIMAGE_BIGENDIAN
			if (bit_depth == 16) {
				// turn on 16 bit byte swapping
				png_set_swap(png_ptr);
			}
#endif


			if ((pixel_depth == 32) && (!has_alpha_channel)) {
				BYTE *buffer = (BYTE *)malloc(width * 3);

				// transparent conversion to 24-bit
				for (png_uint_32 k = 0; k < height; k++) {
					FreeImage_ConvertLine32To24(buffer, FreeImage_GetScanLine(dib, height - k - 1), width);

					png_write_row(png_ptr, buffer);
				}

				free(buffer);
			} else {
				for (png_uint_32 k = 0; k < height; k++) {
					png_write_row(png_ptr, FreeImage_GetScanLine(dib, height - k - 1));
				}
			}

			// It is REQUIRED to call this to finish writing the rest of the file
			// Bug with png_flush

			png_write_end(png_ptr, info_ptr);

			// clean up after the write, and free any memory allocated
			if (palette)
				png_free(png_ptr, palette);

			png_destroy_write_struct(&png_ptr, &info_ptr);

			return TRUE;
		} catch (const char *text) {
			FreeImage_OutputMessageProc(s_format_id, text);
		}
	}

	return FALSE;
}

// ==========================================================
//   Init
// ==========================================================

void DLL_CALLCONV
InitPNG(Plugin *plugin, int format_id) {
	s_format_id = format_id;

	plugin->format_proc = Format;
	plugin->description_proc = Description;
	plugin->extension_proc = Extension;
	plugin->regexpr_proc = RegExpr;
	plugin->open_proc = NULL;
	plugin->close_proc = NULL;
	plugin->pagecount_proc = NULL;
	plugin->pagecapability_proc = NULL;
	plugin->load_proc = Load;
	plugin->save_proc = Save;
	plugin->validate_proc = Validate;
	plugin->mime_proc = MimeType;
	plugin->supports_export_bpp_proc = SupportsExportDepth;
	plugin->supports_export_type_proc = SupportsExportType;
	plugin->supports_icc_profiles_proc = SupportsICCProfiles;
}
