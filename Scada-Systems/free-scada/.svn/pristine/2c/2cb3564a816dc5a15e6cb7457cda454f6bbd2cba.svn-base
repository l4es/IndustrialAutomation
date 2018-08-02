// ==========================================================
// Metadata functions implementation
// Exif metadata model
//
// Design and implementation by
// - Hervé Drolon (drolon@infonie.fr)
//
// Based on the following implementations:
// - metadata-extractor : http://www.drewnoakes.com/code/exif/
// - jhead : http://www.sentex.net/~mwandel/jhead/
// - ImageMagick : http://www.imagemagick.org/
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
#include "FreeImageTag.h"

// ==========================================================
// Exif JPEG routines
// ==========================================================

#define EXIF_NUM_FORMATS  12

#define TAG_EXIF_OFFSET			0x8769	// Exif IFD Pointer
#define TAG_GPS_OFFSET			0x8825	// GPS Info IFD Pointer
#define TAG_INTEROP_OFFSET		0xA005	// Interoperability IFD Pointer
#define TAG_THUMBNAIL_OFFSET	0x0201	// Thumbnail Pointer
#define TAG_MAKER_NOTE			0x927C	// Maker note

// CANON cameras have some funny bespoke fields that need further processing...
#define TAG_CANON_CAMERA_STATE_1 0x0001
#define TAG_CANON_CAMERA_STATE_2 0x0004


// =====================================================================
// Reimplementation of strnicmp (it is not supported on some systems)
// =====================================================================

int 
FreeImage_strnicmp(const char *s1, const char *s2, size_t len) {
	unsigned char c1, c2;

	c1 = 0;	c2 = 0;
	if(len) {
		do {
			c1 = *s1; c2 = *s2;
			s1++; s2++;
			if (!c1)
				break;
			if (!c2)
				break;
			if (c1 == c2)
				continue;
			c1 = tolower(c1);
			c2 = tolower(c2);
			if (c1 != c2)
				break;
		} while (--len);
	}
	return (int)c1 - (int)c2;
}


// ----------------------------------------------------------
//   Little Endian / Big Endian io routines
// ----------------------------------------------------------

static short 
ReadInt16(BOOL msb_order, void *buffer) {
	short value;

	if(msb_order) {
		value = (short)((((BYTE*) buffer)[0] << 8) | ((BYTE*) buffer)[1]);
		return value;
    }
	value = (short)((((BYTE*) buffer)[1] << 8) | ((BYTE*) buffer)[0]);
	return value;
}

static long 
ReadInt32(BOOL msb_order, void *buffer) {
	long value;

	if(msb_order) {
		value = (long)((((BYTE*) buffer)[0] << 24) | (((BYTE*) buffer)[1] << 16) | (((BYTE*) buffer)[2] << 8) | (((BYTE*) buffer)[3]));
		return value;
    }
	value = (long)((((BYTE*) buffer)[3] << 24) | (((BYTE*) buffer)[2] << 16) | (((BYTE*) buffer)[1] << 8 ) | (((BYTE*) buffer)[0]));
	return value;
}

static unsigned short 
ReadUint16(BOOL msb_order, void *buffer) {
	unsigned short value;
	
	if(msb_order) {
		value = (unsigned short) ((((BYTE*) buffer)[0] << 8) | ((BYTE*) buffer)[1]);
		return value;
    }
	value = (unsigned short) ((((BYTE*) buffer)[1] << 8) | ((BYTE*) buffer)[0]);
	return value;
}

static unsigned long 
ReadUint32(BOOL msb_order, void *buffer) {
	return ((unsigned long) ReadInt32(msb_order, buffer) & 0xFFFFFFFF);
}

// ----------------------------------------------------------
//   Exif JPEG markers routines
// ----------------------------------------------------------

/**
Process a IFD offset
*/
static void 
processIFDOffset(FIBITMAP *dib, FITAG & tag, char *pval, BOOL msb_order, DWORD *subdirOffset, TagLib::MDMODEL *md_model) {
	// get the IFD offset
	*subdirOffset = (DWORD) ReadUint32(msb_order, pval);

	// select a tag info table
	switch(tag.id) {
		case TAG_EXIF_OFFSET:
			*md_model = TagLib::EXIF_EXIF;
			break;
		case TAG_GPS_OFFSET:
			*md_model = TagLib::EXIF_GPS;
			break;
		case TAG_INTEROP_OFFSET:
			*md_model = TagLib::EXIF_INTEROP;
			break;
	}

}

/**
Process a maker note IFD offset
*/
static void 
processMakerNote(FIBITMAP *dib, FITAG & tag, char *pval, BOOL msb_order, DWORD *subdirOffset, TagLib::MDMODEL *md_model) {
	FITAG *tagMake = NULL;

	*subdirOffset = 0;
	*md_model = TagLib::UNKNOWN;

	// Determine the camera model and makernote format
	FreeImage_GetMetadata(FIMD_EXIF_MAIN, dib, "Make", &tagMake);

	if(strncmp("OLYMP", pval, 5) == 0) {
		// Olympus Makernote
		*md_model = TagLib::EXIF_MAKERNOTE_OLYMPUS;
		*subdirOffset = 8;
	} 
	else if(FreeImage_strnicmp("NIKON", (char*)tagMake->value, 5) == 0) {
		if(strncmp("Nikon", pval, 5) == 0) {
			/* There are two scenarios here:
			 * Type 1:
			 * :0000: 4E 69 6B 6F 6E 00 01 00-05 00 02 00 02 00 06 00 Nikon...........
			 * :0010: 00 00 EC 02 00 00 03 00-03 00 01 00 00 00 06 00 ................
			 * Type 3:
			 * :0000: 4E 69 6B 6F 6E 00 02 00-00 00 4D 4D 00 2A 00 00 Nikon....MM.*...
			 * :0010: 00 08 00 1E 00 01 00 07-00 00 00 04 30 32 30 30 ............0200
			 */
			if (pval[6] == 1) {
				// Nikon type 1 Makernote
				*md_model = TagLib::EXIF_MAKERNOTE_NIKONTYPE1;
				*subdirOffset = 8;
            } else if (pval[6] == 2) {
                // Nikon type 3 Makernote
				*md_model = TagLib::EXIF_MAKERNOTE_NIKONTYPE3;
				*subdirOffset = 18;
            } else {
				// Unsupported makernote data ignored
				*md_model = TagLib::UNKNOWN;
			}
		} else {
			// Nikon type 2 Makernote
			*md_model = TagLib::EXIF_MAKERNOTE_NIKONTYPE2;
			*subdirOffset = 0;
		}
    } else if(FreeImage_strnicmp("Canon", (char*)tagMake->value, 5) == 0) {
        // Canon Makernote
		*md_model = TagLib::EXIF_MAKERNOTE_CANON;
		*subdirOffset = 0;		
    } else if(FreeImage_strnicmp("Casio", (char*)tagMake->value, 5) == 0) {
        // Casio Makernote
		*md_model = TagLib::EXIF_MAKERNOTE_CASIO;
		*subdirOffset = 0;
	} else if ((strncmp("FUJIFILM", pval, 8) == 0) || (FreeImage_strnicmp("Fujifilm", (char*)tagMake->value, 8) == 0)) {
        // Fujifile Makernote
		*md_model = TagLib::EXIF_MAKERNOTE_FUJIFILM;
        DWORD ifdStart = (DWORD) ReadUint32(msb_order, pval + 8);
		*subdirOffset = ifdStart;
    }
	else if(memcmp("KYOCERA            \x00\x00\x00", pval, 22) == 0) {
		*md_model = TagLib::EXIF_MAKERNOTE_KYOCERA;
		*subdirOffset = 22;
	}
	else if(FreeImage_strnicmp("Minolta", (char*)tagMake->value, 7) == 0) {
		// Minolta maker note
		*md_model = TagLib::EXIF_MAKERNOTE_MINOLTA;
		*subdirOffset = 0;
	}
	else if(memcmp("Panasonic\x00\x00\x00", pval, 12) == 0) {
		// Panasonic maker note
		*md_model = TagLib::EXIF_MAKERNOTE_PANASONIC;
		*subdirOffset = 12;
	}
	else if((FreeImage_strnicmp("Pentax", (char*)tagMake->value, 6) == 0) || (FreeImage_strnicmp("Asahi", (char*)tagMake->value, 5) == 0)) {
		// Pentax maker note
		*md_model = TagLib::EXIF_MAKERNOTE_PENTAX;
		if(strncmp("AOC\x00", pval, 4) == 0) {
			// Type 2 Pentax Makernote
			*subdirOffset = 6;
		} else {
			// Type 1 Pentax Makernote
			*subdirOffset = 0;
		}
	}
}

/**
Process a Canon maker note tag. 
A single Canon tag may contain many other tags within.
*/
static void 
processCanonMakerNoteTag(FIBITMAP *dib, FITAG & tag) {
	TagLib& s = TagLib::instance();

	if (tag.id == TAG_CANON_CAMERA_STATE_1) {
        // this single tag has multiple values within

		FITAG canonTag;
		WORD *pvalue = (WORD*)tag.value;

        int subTagTypeBase = 0xC100;

        // we intentionally skip the first array member
        for (int i = 1; i < tag.count; i++) {
			memset(&canonTag, 0, sizeof(FITAG));

			canonTag.id = subTagTypeBase + i;
			canonTag.type = FIDT_SHORT;
			canonTag.count = 1;
			canonTag.length = 2;
			canonTag.value = &pvalue[i];

			// get the tag key and description
			canonTag.key = (char*)s.getTagFieldName(TagLib::EXIF_MAKERNOTE_CANON, canonTag.id);
			canonTag.description = (char*)s.getTagDescription(TagLib::EXIF_MAKERNOTE_CANON, canonTag.id);

			// store the tag
			if(canonTag.key) {
				FreeImage_SetMetadata(FIMD_EXIF_MAKERNOTE, dib, canonTag.key, &canonTag);
			}
        }
    } else if (tag.id == TAG_CANON_CAMERA_STATE_2) {
        // this single tag has multiple values within

		FITAG canonTag;
		WORD *pvalue = (WORD*)tag.value;

        int subTagTypeBase = 0xC400;

        // we intentionally skip the first array member
        for (int i = 1; i < tag.count; i++) {
			memset(&canonTag, 0, sizeof(FITAG));

			canonTag.id = subTagTypeBase + i;
			canonTag.type = FIDT_SHORT;
			canonTag.count = 1;
			canonTag.length = 2;
			canonTag.value = &pvalue[i];

			// get the tag key and description
			canonTag.key = (char*)s.getTagFieldName(TagLib::EXIF_MAKERNOTE_CANON, canonTag.id);
			canonTag.description = (char*)s.getTagDescription(TagLib::EXIF_MAKERNOTE_CANON, canonTag.id);

			// store the tag
			if(canonTag.key) {
				FreeImage_SetMetadata(FIMD_EXIF_MAKERNOTE, dib, canonTag.key, &canonTag);
			}
        }
	}
	else {
		// process as a normal tag

		// get the tag key and description
		tag.key = (char*)s.getTagFieldName(TagLib::EXIF_MAKERNOTE_CANON, tag.id);
		tag.description = (char*)s.getTagDescription(TagLib::EXIF_MAKERNOTE_CANON, tag.id);

		// store the tag
		if(tag.key) {
			FreeImage_SetMetadata(FIMD_EXIF_MAKERNOTE, dib, tag.key, &tag);
		}
	}
}

/**
Process a standard Exif tag
*/
static void 
processExifTag(FIBITMAP *dib, FITAG & tag, char *pval, BOOL msb_order, TagLib::MDMODEL md_model) {
	int n, i;

	// allocate a buffer to store the tag value
	BYTE *exif_value = (BYTE*)malloc(tag.length * sizeof(BYTE));
	memset(exif_value, 0, tag.length * sizeof(BYTE));

	// get the tag value
	switch(tag.type) {

		case FIDT_SHORT:
		{
			WORD *value = (WORD*)&exif_value[0];
			for(i = 0; i < tag.count; i++) {
				value[i] = ReadUint16(msb_order, pval + i * sizeof(WORD));
			}
			tag.value = value;
			break;
		}
		case FIDT_SSHORT:
		{
			short *value = (short*)&exif_value[0];
			for(i = 0; i < tag.count; i++) {
				value[i] = ReadInt16(msb_order, pval + i * sizeof(short));
			}
			tag.value = value;
			break;
		}
		case FIDT_LONG:
		{
			DWORD *value = (DWORD*)&exif_value[0];
			for(i = 0; i < tag.count; i++) {
				value[i] = ReadUint32(msb_order, pval + i * sizeof(DWORD));
			}
			tag.value = value;
			break;
		}
		case FIDT_SLONG:
		{
			LONG *value = (LONG*)&exif_value[0];
			for(i = 0; i < tag.count; i++) {
				value[i] = ReadInt32(msb_order, pval + i * sizeof(LONG));
			}
			tag.value = value;
			break;
		}
		case FIDT_RATIONAL:
		{
			n = sizeof(DWORD);

			DWORD *value = (DWORD*)&exif_value[0];						
			for(i = 0; i < 2 * tag.count; i++) {
				// read a sequence of (numerator, denominator)
				value[i] = ReadUint32(msb_order, n*i + (char*)pval);
			}
			tag.value = value;
			break;
		}
		case FIDT_SRATIONAL:
		{
			n = sizeof(LONG);

			LONG *value = (LONG*)&exif_value[0];
			for(i = 0; i < 2 * tag.count; i++) {
				// read a sequence of (numerator, denominator)
				value[i] = ReadInt32(msb_order, n*i + (char*)pval);
			}
			tag.value = value;
			break;
		}
		case FIDT_BYTE:
		case FIDT_ASCII:
		case FIDT_SBYTE:
		case FIDT_UNDEFINED:
		case FIDT_FLOAT:
		case FIDT_DOUBLE:
		default:
			tag.value = pval;
			break;
	}

	if(md_model == TagLib::EXIF_MAKERNOTE_CANON) {
		// A single Canon tag can have multiple values within
		processCanonMakerNoteTag(dib, tag);
	}
	else {
		TagLib& s = TagLib::instance();
		// get the tag key and description
		tag.key = (char*)s.getTagFieldName(md_model, tag.id);
		tag.description = (char*)s.getTagDescription(md_model, tag.id);

		// store the tag
		if(tag.key) {
			FreeImage_SetMetadata(s.getFreeImageModel(md_model), dib, tag.key, &tag);
		}
	}
	

	// free the temporary buffer
	free(exif_value);

}

/**
	Process Exif directory

	@param dib Input FIBITMAP
	@param tiffp Pointer to the TIFF header
	@param offset 0th IFD offset
	@param length Length of the datafile
	@param msb_order Endianess order of the datafile
	@return 
*/
BOOL 
jpeg_read_exif_dir(FIBITMAP *dib, const BYTE *tiffp, unsigned int offset, unsigned int length, BOOL msb_order) {
	WORD de, nde;

	std::stack<WORD>			destack;	// directory entries stack
	std::stack<BYTE*>			ifdstack;	// IFD stack
	std::stack<TagLib::MDMODEL>	modelstack; // metadata model stack

    #define DIR_ENTRY_ADDR(_start, _entry) (_start + 2 + (12 * _entry))


	// set the metadata model to Exif

	TagLib::MDMODEL md_model = TagLib::EXIF_MAIN;

	// set the pointer to the first IFD and follow it were it leads.

	BYTE *ifdp = (BYTE*)tiffp + offset;

	de = 0;

	do {
		// if there is anything on the stack then pop it off
		if(!destack.empty()) {
			ifdp		= ifdstack.top();	ifdstack.pop();
			de			= destack.top();	destack.pop();
			md_model	= modelstack.top();	modelstack.pop();
		}

		// determine how many entries there are in the current IFD
		nde = ReadUint16(msb_order, ifdp);

		for(; de < nde; de++) {
			char *pde = NULL;	// pointer to the directory entry
			char *pval = NULL;	// pointer to the tag value

			FITAG tag;
			memset(&tag, 0, sizeof(FITAG));

			// point to the directory entry
			pde = (char*) DIR_ENTRY_ADDR(ifdp, de);
			
			tag.id = (WORD)ReadUint16(msb_order, pde);			// get tag value
			tag.type = (WORD)ReadUint16(msb_order, pde + 2);	// get the format

			if((tag.type - 1) >= EXIF_NUM_FORMATS)
				break;

			tag.count = (DWORD)ReadUint32(msb_order, pde + 4);			// get number of components
			tag.length = tag.count * FreeImage_TagDataWidth(tag.type);	// get the size of the tag value in bytes
			if(tag.length <= 4) {
				// 4 bytes or less and value is in the dir entry itself
				pval = pde + 8;
			} else {
				DWORD offset_value;

				// if its bigger than 4 bytes, the directory entry contains an offset
				offset_value = ReadUint32(msb_order, pde + 8);
				if((size_t) (offset_value + tag.length) > length)
					continue;
				pval = (char*)(tiffp + offset_value);
			}

			// check for a IFD offset
			BOOL isIFDOffset = FALSE;
			switch(tag.id) {
				case TAG_EXIF_OFFSET:
				case TAG_GPS_OFFSET:
				case TAG_INTEROP_OFFSET:
				case TAG_MAKER_NOTE:
					isIFDOffset = TRUE;
					break;
			}
			if(isIFDOffset)	{
				DWORD sub_offset = 0;
				TagLib::MDMODEL next_mdmodel = md_model;
				BYTE *next_ifd = ifdp;
				
				// get offset and metadata model
				if (tag.id == TAG_MAKER_NOTE) {
					processMakerNote(dib, tag, pval, msb_order, &sub_offset, &next_mdmodel);
					next_ifd = (BYTE*)pval + sub_offset;
				} else {
					processIFDOffset(dib, tag, pval, msb_order, &sub_offset, &next_mdmodel);
					next_ifd = (BYTE*)tiffp + sub_offset;
				}

				if((sub_offset < (DWORD) length) && (next_mdmodel != TagLib::UNKNOWN)) {
					// push our current directory state onto the stack
					ifdstack.push(ifdp);
					// bump to the next entry
					de++;
					destack.push(de);

					// push our current metadata model
					modelstack.push(md_model);

					// push new state onto of stack to cause a jump
					ifdstack.push(next_ifd);
					destack.push(0);

					// select a new metadata model
					modelstack.push(next_mdmodel);
					
					break; // break out of the for loop
				}
				else {
					// unsupported camera model or something unknown
					// process as a standard tag
					processExifTag(dib, tag, pval, msb_order, md_model);
				}			

			} else {
				// process as a standard tag
				processExifTag(dib, tag, pval, msb_order, md_model);
			}


        } // for(nde)

		// additional thumbnail data is skipped

    } while (!destack.empty()); 

	return TRUE;
}

/**
	Read JPEG_APP1 marker (Exif profile)
	@param dib Input FIBITMAP
	@param dataptr Pointer to the APP1 marker
	@param datalen APP1 marker length
	@return Returns TRUE if successful, FALSE otherwise
*/
BOOL  
jpeg_read_exif_profile(FIBITMAP *dib, const BYTE *dataptr, unsigned int datalen) {
    // marker identifying string for Exif = "Exif\0\0"
    BYTE exif_signature[6] = { 0x45, 0x78, 0x69, 0x66, 0x00, 0x00 };
	BYTE lsb_first[4] = { 0x49, 0x49, 0x2A, 0x00 };		// Intel order
	BYTE msb_first[4] = { 0x4D, 0x4D, 0x00, 0x2A };		// Motorola order

	size_t length = datalen;
	BYTE *profile = (BYTE*)dataptr;

	// verify the identifying string

	if(memcmp(exif_signature, profile, sizeof(exif_signature)) == 0) {
		// Exif profile

		profile += sizeof(exif_signature);
		length  -= sizeof(exif_signature);

		// check the endianess order
		
		BOOL bMotorolaOrder = TRUE;

		if(memcmp(profile, lsb_first, sizeof(lsb_first)) == 0) {
			// Exif section in Intel order
			bMotorolaOrder = FALSE;
		} else {
			if(memcmp(profile, msb_first, sizeof(msb_first)) == 0) {
				// Exif section in Motorola order
				bMotorolaOrder = TRUE;
			} else {
				// Invalid Exif alignment marker
				return FALSE;
			}
		}

		// this is the offset to the first IFD
		size_t first_offset = ReadUint32(bMotorolaOrder, profile + 4);

		if (first_offset < 8 || first_offset > 16) {
			// This is usually set to 8
			// but PENTAX Optio 230 has it set differently, and uses it as offset. 
			FreeImage_OutputMessageProc(FIF_JPEG, "Exif: Suspicious offset of first IFD value");
			return FALSE;
		}

		// process Exif directories
		return jpeg_read_exif_dir(dib, profile, first_offset, length, bMotorolaOrder);
	}

	return FALSE;
}


