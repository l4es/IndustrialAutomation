// ==========================================================
// Bitmap rotation by means of 3 shears.
//
// Design and implementation by
// - Hervé Drolon (drolon@infonie.fr)
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

/* 
 ============================================================
 References : 
 [1] Paeth A., A Fast Algorithm for General Raster Rotation. 
 Graphics Gems, p. 179, Andrew Glassner editor, Academic Press, 1990. 
 [2] Yariv E., High quality image rotation (rotate by shear). 
 [Online] http://www.codeproject.com/bitmap/rotatebyshear.asp
 ============================================================
*/

#include "Utilities.h"
#include "FreeImage.h"

#define ROTATE_PI  double (3.1415926535897932384626433832795)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Prototypes definition

static void HorizSkew(FIBITMAP *src, FIBITMAP *dst, int row, int iOffset, double dWeight);
static void VertSkew(FIBITMAP *src, FIBITMAP *dst, int col, int iOffset, double dWeight);
static FIBITMAP* Rotate90(FIBITMAP *src);
static FIBITMAP* Rotate180(FIBITMAP *src);
static FIBITMAP* Rotate270(FIBITMAP *src);
static FIBITMAP* Rotate45(FIBITMAP *src, double dAngle);
static FIBITMAP* Rotate8Bit(FIBITMAP *src, double dAngle);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Skews a row horizontally (with filtered weights). 
Limited to 45 degree skewing only. Filters two adjacent pixels.
@param src Pointer to source image to rotate
@param dst Pointer to destination image
@param row Row index
@param iOffset Skew offset
@param dWeight Relative weight of right pixel
*/
static void 
HorizSkew(FIBITMAP *src, FIBITMAP *dst, int row, int iOffset, double dWeight) {
	int i = 0;
	BYTE *pSrc = NULL, *pDst = NULL;
	int src_width  = FreeImage_GetWidth(src);
	int src_height = FreeImage_GetHeight(src);
	int dst_width  = FreeImage_GetWidth(dst);
	int dst_height = FreeImage_GetHeight(dst);

	// Fill gap left of skew with background
	pDst = FreeImage_GetScanLine(dst, row);
	for(i = 0; i < iOffset; i++) {
        pDst[i] = 0;
	}

	pSrc = FreeImage_GetScanLine(src, row);
	pDst = FreeImage_GetScanLine(dst, row);

	BYTE pxlOldLeft = 0;
	for(i = 0; i < src_width; i++) {
		// Loop through row pixels
		BYTE pxlSrc = pSrc[i];
		// Calculate weights
		BYTE pxlLeft = BYTE((double)pxlSrc * dWeight);
		// Update left over on source
		pxlSrc = pxlSrc - (pxlLeft - pxlOldLeft);
		// Check boundaries 
		if((i + iOffset >= 0) && (i + iOffset < dst_width)) {
			pDst[i + iOffset] = pxlSrc;
		}
		// Save leftover for next pixel in scan
		pxlOldLeft = pxlLeft;
	}
	// Go to rightmost point of skew
	i += iOffset;  
	if(i < dst_width) {
		// If still in image bounds, put leftovers there
		pDst[i] = pxlOldLeft;
	}
	while(++i < dst_width) {
		// Clear to the right of the skewed line with background
		pDst[i] = 0;
	}
}
/**
Skews a column vertically (with filtered weights). 
Limited to 45 degree skewing only. Filters two adjacent pixels.
@param src Pointer to source image to rotate
@param dst Pointer to destination image
@param col Column index
@param iOffset Skew offset
@param dWeight Relative weight of upper pixel
*/
static void 
VertSkew(FIBITMAP *src, FIBITMAP *dst, int col, int iOffset, double dWeight) {
	int i = 0;
	BYTE *pSrc = NULL, *pDst = NULL;
	int src_width  = FreeImage_GetWidth(src);
	int src_height = FreeImage_GetHeight(src);
	int dst_width  = FreeImage_GetWidth(dst);
	int dst_height = FreeImage_GetHeight(dst);

	int iYPos = 0;

	// Fill gap above skew with background
	for(i = 0; i < iOffset; i++) {
		pDst = FreeImage_GetScanLine(dst, i);
		pDst[col] = 0;
	}

	BYTE pxlOldLeft = 0;

	for(i = 0; i < src_height; i++) {
		pSrc = FreeImage_GetScanLine(src, i);
		// Loop through column pixels
		BYTE pxlSrc = pSrc[col];
		iYPos = i + iOffset;
		// Calculate weights
		BYTE pxlLeft = BYTE((double)pxlSrc * dWeight);
		// Update left over on source
		pxlSrc = pxlSrc - (pxlLeft - pxlOldLeft);
		// Check boundaries
		if((iYPos >= 0) && (iYPos < dst_height)) {
			pDst = FreeImage_GetScanLine(dst, iYPos);
			pDst[col] = pxlSrc;
		}
		// Save leftover for next pixel in scan
		pxlOldLeft = pxlLeft;
	}
	// Go to bottom point of skew
	i = iYPos;  
	if(i < dst_height) {
		// If still in image bounds, put leftovers there
		pDst = FreeImage_GetScanLine(dst, i);
		pDst[col] = pxlOldLeft;
	}
	while(++i < dst_height) {
		// Clear below skewed line with background
		pDst = FreeImage_GetScanLine(dst, i);
		pDst[col] = 0;
	}
} 

/**
Rotates an image by 90 degrees (counter clockwise). 
Precise rotation, no filters required.
@param src Pointer to source image to rotate
@return Returns a pointer to a newly allocated rotated image if successful, returns NULL otherwise
*/
static FIBITMAP* 
Rotate90(FIBITMAP *src) {
	int src_width  = FreeImage_GetWidth(src);
	int src_height = FreeImage_GetHeight(src);
	int dst_width  = src_height;
	int dst_height = src_width;

	FIBITMAP *dst = FreeImage_Allocate(dst_width, dst_height, 8);
	if(NULL == dst) return NULL;

	for(int y = 0; y < src_height; y++) {
		BYTE *pSrc = FreeImage_GetScanLine(src, y);
		for(int x = 0; x < src_width; x++) {
			BYTE *pDst = FreeImage_GetScanLine(dst, dst_height - x - 1);
			pDst[y] = pSrc[x];
		}
	}
	return dst;
}

/**
Rotates an image by 180 degrees (counter clockwise). 
Precise rotation, no filters required.
@param src Pointer to source image to rotate
@return Returns a pointer to a newly allocated rotated image if successful, returns NULL otherwise
*/
static FIBITMAP* 
Rotate180(FIBITMAP *src) {
	int src_width  = FreeImage_GetWidth(src);
	int src_height = FreeImage_GetHeight(src);
	int dst_width  = src_width;
	int dst_height = src_height;

	FIBITMAP *dst = FreeImage_Allocate(dst_width, dst_height, 8);
	if(NULL == dst) return NULL;

	for(int y = 0; y < src_height; y++) {
		BYTE *pSrc = FreeImage_GetScanLine(src, y);
		for(int x = 0; x < src_width; x++) {
			BYTE *pDst = FreeImage_GetScanLine(dst, dst_height - y - 1);
			pDst[dst_width - x - 1] = pSrc[x];
		}
	}
	return dst;
}

/**
Rotates an image by 270 degrees (counter clockwise). 
Precise rotation, no filters required.
@param src Pointer to source image to rotate
@return Returns a pointer to a newly allocated rotated image if successful, returns NULL otherwise
*/
static FIBITMAP* 
Rotate270(FIBITMAP *src) {
	int src_width  = FreeImage_GetWidth(src);
	int src_height = FreeImage_GetHeight(src);
	int dst_width  = src_height;
	int dst_height = src_width;

	FIBITMAP *dst = FreeImage_Allocate(dst_width, dst_height, 8);
	if(NULL == dst) return NULL;

	for(int y = 0; y < src_height; y++) {
		BYTE *pSrc = FreeImage_GetScanLine(src, y);
		for(int x = 0; x < src_width; x++) {
			BYTE *pDst = FreeImage_GetScanLine(dst, x);
			pDst[dst_width - y - 1] = pSrc[x];
		}
	}
	return dst;
}

/**
Rotates an image by a given degree in range [-45 .. +45] (counter clockwise) 
using the 3-shear technique.
@param src Pointer to source image to rotate
@param dAngle Rotation angle
@return Returns a pointer to a newly allocated rotated image if successful, returns NULL otherwise
*/
static FIBITMAP* 
Rotate45(FIBITMAP *src, double dAngle) {
	int u;

	if(0 == dAngle) {
		// No rotation at all
		return FreeImage_Clone(src);
	}

	double dRadAngle = dAngle * ROTATE_PI / double(180); // Angle in radians
	double dSinE = sin(dRadAngle);
	double dTan = tan(dRadAngle / 2);

	int src_width  = FreeImage_GetWidth(src);
	int src_height = FreeImage_GetHeight(src);

	// Calc first shear (horizontal) destination image dimensions 
	int width_1  = src_width + int((double)src_height*fabs(dTan));
	int height_1 = src_height; 

	/******* Perform 1st shear (horizontal) ******/

	// Allocate image for 1st shear
	FIBITMAP *dst1 = FreeImage_Allocate(width_1, height_1, 8);
	if(NULL == dst1) {
		return NULL;
	}
	
	for(u = 0; u < height_1; u++) {  
		double dShear;

		if(dTan >= 0)	{
			// Positive angle
			dShear = (double(u) + 0.5) * dTan;
		}
		else {
			// Negative angle
			dShear = (double(int(u) - height_1) + 0.5) * dTan;
		}
		int iShear = int(floor(dShear));
		HorizSkew(src, dst1, u, iShear, dShear - double(iShear));
	}

	/******* Perform 2nd shear  (vertical) ******/

	// Calc 2nd shear (vertical) destination image dimensions
	int width_2  = width_1;
	int height_2 = int((double)src_width*fabs(dSinE) + (double)src_height*cos(dRadAngle)) + 1;

	// Allocate image for 2nd shear
	FIBITMAP *dst2 = FreeImage_Allocate(width_2, height_2, 8);
	if(NULL == dst2) {
		FreeImage_Unload(dst1);
		return NULL;
	}

	double dOffset;     // Variable skew offset
	if(dSinE > 0)	{   
		// Positive angle
		dOffset = double(src_width - 1) * dSinE;
	}
	else {
		// Negative angle
		dOffset = -dSinE * double (src_width - width_2);
	}

	for(u = 0; u < width_2; u++, dOffset -= dSinE) {
		int iShear = int(floor(dOffset));
		VertSkew(dst1, dst2, u, iShear, dOffset - double(iShear));
	}

	/******* Perform 3rd shear (horizontal) ******/

	// Free result of 1st shear
	FreeImage_Unload(dst1);

	// Calc 3rd shear (horizontal) destination image dimensions
	int width_3  = int(double(src_height)*fabs(dSinE) + double(src_width)*cos(dRadAngle)) + 1;
	int height_3 = height_2;

	// Allocate image for 3rd shear
	FIBITMAP *dst3 = FreeImage_Allocate(width_3, height_3, 8);
	if(NULL == dst3) {
		FreeImage_Unload(dst2);
		return NULL;
	}

	if(dSinE >= 0) {
		// Positive angle
		dOffset = double(src_width - 1) * dSinE * -dTan;
	}
	else {
		// Negative angle
		dOffset = dTan * (double(src_width - 1) * -dSinE + double(1 - height_3));
	}
	for(u = 0; u < height_3; u++, dOffset += dTan) {
		int iShear = int(floor(dOffset));
		HorizSkew(dst2, dst3, u, iShear, dOffset - double (iShear));
	}
	// Free result of 2nd shear    
	FreeImage_Unload(dst2);

	// Return result of 3rd shear
	return dst3;      
}

/**
Rotates a 8-bit image by a given angle (given in degree). 
Angle is unlimited. 3-shears technique is used.
@param src Pointer to source image to rotate
@param dAngle Rotation angle
@return Returns a pointer to a newly allocated rotated image if successful, returns NULL otherwise
*/
static FIBITMAP* 
Rotate8Bit(FIBITMAP *src, double dAngle) {
	if(NULL == src) {
		return NULL;
	}

	int src_width  = FreeImage_GetWidth(src);
	int src_height = FreeImage_GetHeight(src);

	FIBITMAP *image = src;

	while(dAngle >= 360) {
		// Bring angle to range of (-INF .. 360)
		dAngle -= 360;
	}
	while(dAngle < 0) {
		// Bring angle to range of [0 .. 360) 
		dAngle += 360;
	}
	if((dAngle > 45) && (dAngle <= 135)) {
		// Angle in (45 .. 135] 
		// Rotate image by 90 degrees into temporary image,
		// so it requires only an extra rotation angle 
		// of -45 .. +45 to complete rotation.
		image = Rotate90(src);
		dAngle -= 90;
	}
	else if((dAngle > 135) && (dAngle <= 225)) { 
		// Angle in (135 .. 225] 
		// Rotate image by 180 degrees into temporary image,
		// so it requires only an extra rotation angle 
		// of -45 .. +45 to complete rotation.
		image = Rotate180(src);
		dAngle -= 180;
	}
	else if((dAngle > 225) && (dAngle <= 315)) { 
		// Angle in (225 .. 315] 
		// Rotate image by 270 degrees into temporary image,
		// so it requires only an extra rotation angle 
		// of -45 .. +45 to complete rotation.
		image = Rotate270(src);
		dAngle -= 270;
	}

	// If we got here, angle is in (-45 .. +45]

	if(NULL == image)	{
		// Failed to allocate middle image
		return NULL;
	}

	FIBITMAP *dst = Rotate45(image, dAngle);

	if(src != image) {
		// Middle image was required, free it now.
		FreeImage_Unload(image);
	}

	return dst;
}

// ==========================================================

FIBITMAP *DLL_CALLCONV 
FreeImage_RotateClassic(FIBITMAP *dib, double angle) {
	int x, y, bpp;
	int channel, nb_channels;
	BYTE *src_bits, *dst_bits;
	FIBITMAP *src8 = NULL, *dst8 = NULL, *dst = NULL;

	if(!dib) return NULL;	

	if(0 == angle) {
		return FreeImage_Clone(dib);
	}
	// DIB are stored upside down ...
	angle *= -1;

	try {

		bpp = FreeImage_GetBPP(dib);

		if(bpp == 8) {
			dst8 = Rotate8Bit(dib, angle);
			if(!dst8) throw(1);
			
			// buid a greyscale palette			
			RGBQUAD *dst_pal = FreeImage_GetPalette(dst8);
			for(int i = 0; i < 256; i++) {
				dst_pal[i].rgbRed = dst_pal[i].rgbGreen = dst_pal[i].rgbBlue = (BYTE)i;
			}

			return dst8;
		}
		if((bpp == 24) || (bpp == 32)) {
			// allocate a temporary 8-bit dib (no need to build a palette)
			int width  = FreeImage_GetWidth(dib);
			int height = FreeImage_GetHeight(dib);
			int dst_width  = 0;
			int dst_height = 0;

			src8 = FreeImage_Allocate(width, height, 8);
			if(!src8) throw(1);

			// process each channel separately
			// -------------------------------
			nb_channels = (bpp / 8);

			for(channel = 0; channel < nb_channels; channel++) {
				// extract channel from source dib
				for(y = 0; y < height; y++) {
					src_bits = FreeImage_GetScanLine(dib, y);
					dst_bits = FreeImage_GetScanLine(src8, y);
					for(x = 0; x < width; x++) {
						dst_bits[x] = src_bits[channel];
						src_bits += nb_channels;
					}
				}

				// process channel
				dst8 = Rotate8Bit(src8, angle);
				if(!dst8) throw(1);

				if(!dst) {
					// allocate dst image
					dst_width  = FreeImage_GetWidth(dst8);
					dst_height = FreeImage_GetHeight(dst8);
					if( bpp == 24 ) {
						dst = FreeImage_Allocate(dst_width, dst_height, bpp, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
					} else {
						dst = FreeImage_Allocate(dst_width, dst_height, bpp, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
					}
					if(!dst) throw(1);
				}
				// insert channel to destination dib
				for(y = 0; y < dst_height; y++) {
					src_bits = FreeImage_GetScanLine(dst8, y);
					dst_bits = FreeImage_GetScanLine(dst, y);
					for(x = 0; x < dst_width; x++) {
						dst_bits[channel] = src_bits[x];
						dst_bits += nb_channels;
					}
				}

				FreeImage_Unload(dst8);
			}

			FreeImage_Unload(src8);

			return dst;
		}
	} catch(int) {
		if(src8) FreeImage_Unload(src8);
		if(dst8) FreeImage_Unload(dst8);
		if(dst)  FreeImage_Unload(dst);
	}

	return NULL;
}

