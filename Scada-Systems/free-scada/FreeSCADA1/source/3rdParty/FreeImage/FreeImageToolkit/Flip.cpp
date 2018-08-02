// ==========================================================
// Flipping routines
//
// Design and implementation by
// - Floris van den Berg (flvdberg@wxs.nl)
// - Hervé Drolon (drolon@infonie.fr)
// - Jim Keir (jimkeir@users.sourceforge.net)
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

#include "FreeImage.h"
#include "Utilities.h"

/**
Flip the image horizontally along the vertical axis.
@param src Input image to be processed.
@return Returns TRUE if successful, FALSE otherwise.
*/
BOOL DLL_CALLCONV 
FreeImage_FlipHorizontal(FIBITMAP *src) {
	if (!src) return FALSE;

	unsigned line   = FreeImage_GetLine(src);
	unsigned height	= FreeImage_GetHeight(src);

	BYTE *new_bits = (BYTE*)malloc(line * sizeof(BYTE));

	// mirror the buffer

	for (unsigned y = 0; y < height; y++) {
		BYTE *bits = FreeImage_GetScanLine(src, y);
		memcpy(new_bits, bits, line);

		switch (FreeImage_GetBPP(src)) {
			case 1 :
			{
				unsigned width	= FreeImage_GetWidth(src);
				for(unsigned x = 0; x < width; x++) {
					// get pixel at (x, y)
					BOOL value = (new_bits[x >> 3] & (0x80 >> (x & 0x07))) != 0;
					// set pixel at (new_x, y)
					unsigned new_x = width - 1 - x;
					value ? bits[new_x >> 3] |= (0x80 >> (new_x & 0x7)) : bits[new_x >> 3] &= (0xff7f >> (new_x & 0x7));
				}

				break;
			}

			case 4 :
			{
				for(unsigned c = 0; c < line; c++) {
					bits[c] = new_bits[line - c - 1];

					BYTE nibble = (bits[c] & 0xF0) >> 4;

					bits[c] = bits[c] << 4;
					bits[c] |= nibble;
				}

				break;
			}

			case 8 :
			{
				for(unsigned c = 0; c < line; c += sizeof(BYTE)) {
					memcpy(bits + c, new_bits + line - c - sizeof(BYTE), sizeof(BYTE));
				}

				break;
			}

			case 16 :
			{
				for(unsigned c = 0; c < line; c += sizeof(WORD)) {
					memcpy(bits + c, new_bits + line - c - sizeof(WORD), sizeof(WORD));						
				}

				break;
			}

			case 24 :
			case 32 :
			{
				int bytespp = FreeImage_GetLine(src) / FreeImage_GetWidth(src);

				for(unsigned c = 0; c < line; c += bytespp) {
					memcpy(bits + c, new_bits + line - c - bytespp, bytespp);						
				}

				break;
			}

		}
	}

	free(new_bits);

	return TRUE;
}

/**
Flip the image vertically along the horizontal axis.
@param src Input image to be processed.
@return Returns TRUE if successful, FALSE otherwise.
*/

BOOL DLL_CALLCONV 
FreeImage_FlipVertical(FIBITMAP *src) {
	BYTE *From, *Mid;

	if (!src) return FALSE;

	// swap the buffer

	unsigned pitch  = FreeImage_GetPitch(src);
	unsigned height = FreeImage_GetHeight(src);

	Mid = (BYTE*)malloc(pitch * sizeof(BYTE));
	if (!Mid) return FALSE;

	From = FreeImage_GetBits(src);

	for(unsigned y = 0; y < height/2; y++) {

		unsigned line_s = y * pitch;
		unsigned line_t = (height-y-1) * pitch;

		memcpy(Mid, From + line_s, pitch);
		memcpy(From + line_s, From + line_t, pitch);
		memcpy(From + line_t, Mid, pitch);

	}

	free(Mid);

	return TRUE;
}

