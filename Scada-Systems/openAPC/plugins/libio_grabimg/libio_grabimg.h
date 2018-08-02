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


#ifndef LIBIO_GRABIMG_H
#define LIBIO_GRABIMG_H

#define MAX_XML_SIZE       6500

#ifdef ENV_WINDOWS
 #include "dshow_capture.h"
#else
 #include "v4l2_capture.h"
 #include "convert.h"
 #include <sys/types.h>

struct buffer
{
   void  *start;
   size_t length;
};
#endif


#define LIBIO_GRABIMG_FLAG_AUTO_SIZE       0x0001
//#define LIBIO_GRABIMG_FLAG_AUTO_BRIGHT     0x0002
//#define LIBIO_GRABIMG_FLAG_AUTO_CONTRAST   0x0004
//#define LIBIO_GRABIMG_FLAG_AUTO_SATURATION 0x0008
//#define LIBIO_GRABIMG_FLAG_AUTO_HUE        0x0010
#define LIBIO_GRABIMG_FLAG_GREYSCALE       0x0020
#define LIBIO_GRABIMG_FLAG_MIRRORX         0x0040
#define LIBIO_GRABIMG_FLAG_MIRRORY         0x0080
#define LIBIO_GRABIMG_FLAG_TESTPATTERN     0x0100
#define LIBIO_GRABIMG_FLAG_ROT90           0x0200
#define LIBIO_GRABIMG_FLAG_ROT180          0x0400
#define LIBIO_GRABIMG_FLAG_ROT270          0x0800
#define LIBIO_GRABIMG_FLAG_NOISE           0x1000

#define MAX_FILENAMESIZE    255

struct libio_config
{
   unsigned short version,length;
   int            width,height,cycleTime,device,flags;
   int            mBright,mContrast,mGamma,mR,mG,mB;
   // version 2
   char           m_testimage[MAX_FILENAMESIZE+4];
};


struct instData
{
   struct libio_config   config;
   struct oapc_bin_head *m_bin;
   bool                  running;
   void*                 threadHandle;
   void*                 m_signal;
   void                 *imgMutex;
   struct oapc_bin_head *rawBinBuf;
   unsigned int          width,height;
#ifdef ENV_WINDOWS
   struct device_context devContext;
#else
   int                   fd;
   int                   pixelformat;
   struct buffer        *buffers;
   unsigned int          n_buffers;
#endif
};

#endif
