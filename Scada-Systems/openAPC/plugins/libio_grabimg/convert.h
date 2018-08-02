/********************************************************************************************

These sources are distributed in the hope that they will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. All
the information given here, within the interface descriptions and within the specification
are subject to change without notice. Errors and omissions excepted.

These sources demonstrate the usage of the OpenAPC Plug-In programming interface. They can be
used freely according to the OpenAPC Dual License: As long as the sources and the resulting
applications/libraries/Plug-Ins are used together with the OpenAPC software, they are
licensed as freeware. When you use them outside the OpenAPC software they are licensed under
the terms of the GNU General Public License.

For more information please refer to http://www.openapc.com/oapc_license.php

*********************************************************************************************/


#ifndef CONVERT_H
#define CONVERT_H

#include "libio_grabimg.h"

#define CLIP(t) (((t)>255)?255:(((t)<0)?0:(t)))

/// Pixel convertion from YUV to RGB (in bytes)
void yuv_to_rgb(unsigned char y_, unsigned char u_, unsigned char v_, unsigned char *r_, unsigned char *g_, unsigned char *b_);

/// Pixmap convertion from YUV422 to RGB24 (in bytes)
void yuv422p_to_rgb24(struct instData *data,unsigned char *data_in);

/// Pixmap convertion from YUV422 to RGB24 (in bytes)
void yuv422_to_rgb24(struct instData *data,unsigned char *data_in);


#endif
