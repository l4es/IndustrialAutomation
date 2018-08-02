// ==========================================================
// Metadata functions implementation
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

#pragma warning (disable : 4786) // identifier was truncated to 'number' characters

#include "FreeImage.h"
#include "Utilities.h"
#include "FreeImageTag.h"

// ----------------------------------------------------------
//   IPTC JPEG / TIFF markers routines
// ----------------------------------------------------------

/**
	Read IPTC binary data
*/
BOOL 
read_iptc_profile(FIBITMAP *dib, const BYTE *dataptr, unsigned int datalen) {
	size_t length = datalen;
	BYTE *profile = (BYTE*)dataptr;

	std::string Keywords;
	std::string SupplementalCategory;

	FITAG tag;

	TagLib& tag_lib = TagLib::instance();

    // find start of the BIM portion of the binary data
    int offset = 0;
	while(offset < length - 1) {
		if((profile[offset] == 0x1C) && (profile[offset+1] == 0x02))
			break;
		offset++;
	}

    // for each tag
    while (offset < length) {
		memset(&tag, 0, sizeof(FITAG));

        // identifies start of a tag
        if (profile[offset] != 0x1c) {
            break;
        }
        // we need at least five bytes left to read a tag
        if ((offset + 5) >= length) {
            break;
        }

        offset++;

		int directoryType	= profile[offset++];
        int tagType			= profile[offset++];;
        int tagByteCount	= ((profile[offset] & 0xFF) << 8) | (profile[offset + 1] & 0xFF);
        offset += 2;

        if ((offset + tagByteCount) > length) {
            // data for tag extends beyond end of iptc segment
            break;
        }

		// process the tag

		tag.id = tagType | (directoryType << 8);
		tag.length = tagByteCount;
		tag.count = tag.length;

		// allocate a buffer to store the tag value
		BYTE *iptc_value = (BYTE*)malloc((tag.length + 1) * sizeof(BYTE));
		memset(iptc_value, 0, (tag.length + 1) * sizeof(BYTE));

		// get the tag value

		switch (tag.id) {
			case TAG_RECORD_VERSION:
			{
				// short
				tag.type = FIDT_SSHORT;
				short *pvalue = (short*)&iptc_value[0];
				*pvalue = (short)((profile[offset] << 8) | profile[offset + 1]);
				tag.value = pvalue;
				break;
			}

			case TAG_RELEASE_DATE:
			case TAG_DATE_CREATED:
				// Date object
			case TAG_RELEASE_TIME:
			case TAG_TIME_CREATED:
				// time
			default:
			{
				// string
				tag.type = FIDT_ASCII;
				for(int i = 0; i < tag.length; i++) {
					iptc_value[i] = profile[offset + i];
				}
				iptc_value[tag.length] = '\0';
				tag.value = (char*)&iptc_value[0];
				break;
			}
		}

		if(tag.id == TAG_SUPPLEMENTAL_CATEGORIES) {
			// concatenate the categories
			if(SupplementalCategory.length() == 0) {
				SupplementalCategory.append((char*)iptc_value);
			} else {
				SupplementalCategory.append(",");
				SupplementalCategory.append((char*)iptc_value);
			}
		}
		else if(tag.id == TAG_KEYWORDS) {
			// concatenate the keywords
			if(Keywords.length() == 0) {
				Keywords.append((char*)iptc_value);
			} else {
				Keywords.append(",");
				Keywords.append((char*)iptc_value);
			}
		}
		else {
			// get the tag key and description
			tag.key = (char*)tag_lib.getTagFieldName(TagLib::IPTC, tag.id);
			tag.description = (char*)tag_lib.getTagDescription(TagLib::IPTC, tag.id);

			// store the tag
			if(tag.key) {
				FreeImage_SetMetadata(FIMD_IPTC, dib, tag.key, &tag);
			}
		}

		free(iptc_value);

        // next tag
		offset += tagByteCount;

    }

	// store the 'keywords' tag
	if(Keywords.length()) {
		memset(&tag, 0, sizeof(FITAG));
		tag.type = FIDT_ASCII;
		tag.id = TAG_KEYWORDS;
		tag.key = (char*)tag_lib.getTagFieldName(TagLib::IPTC, tag.id);
		tag.description = (char*)tag_lib.getTagDescription(TagLib::IPTC, tag.id);
		tag.length = Keywords.length();
		tag.count = tag.length;
		tag.value = (char*)Keywords.c_str();
		FreeImage_SetMetadata(FIMD_IPTC, dib, tag.key, &tag);
	}

	// store the 'supplemental category' tag
	if(SupplementalCategory.length()) {
		memset(&tag, 0, sizeof(FITAG));
		tag.type = FIDT_ASCII;
		tag.id = TAG_SUPPLEMENTAL_CATEGORIES;
		tag.key = (char*)tag_lib.getTagFieldName(TagLib::IPTC, tag.id);
		tag.length = SupplementalCategory.length();
		tag.count = tag.length;
		tag.value = (char*)SupplementalCategory.c_str();
		FreeImage_SetMetadata(FIMD_IPTC, dib, tag.key, &tag);
	}

	return TRUE;
}

