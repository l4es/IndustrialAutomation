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

#include "convert.h"
#include "oapc_libio.h"

void yuv_to_rgb(unsigned char y,unsigned char u,unsigned char v,unsigned char *r, unsigned char *g, unsigned char *b)
{
   *r=CLIP(y+ 1.402f  *(v-128));
   *g=CLIP(y- 0.34414f*(u-128) - 0.71414f*(v-128));
   *b=CLIP(y+ 1.772f  *(u-128));
}


void yuv422_to_rgb24(struct instData *data,unsigned char *data_in)
{
   unsigned char y1, y2, u, v,r,g,b;
   int size;
   unsigned char *src;
   unsigned char *d;
   int i;

   size=data->height *data->width / 2;
   src =data_in;
   d=(unsigned char*)&data->rawBinBuf->data;

   for(i = 0 ; i < size ; i++)
   {
      y1=*src; src++;
      u= *src; src++;
      y2=*src; src++;
      v= *src; src++;

      yuv_to_rgb(y1,u,v,&r,&g,&b);
      *d=r; d++;
      *d=g; d++;
      *d=b; d++;
      yuv_to_rgb(y2,u,v,&r,&g,&b);
      *d=r; d++;
      *d=g; d++;
      *d=b; d++;
   }
}


void yuv422p_to_rgb24(struct instData *data,unsigned char *data_in)
{
  	unsigned char y1, y2, u, v;
  	int size;
  	unsigned char *src;
  	unsigned char *d;
  	int i;	

  	size=data->height *data->width / 2;
	src =data_in;
	d=(unsigned char*)&data->rawBinBuf->data;
	
	for(i = 0 ; i < size ; i++)
	{
   	y1 = src[0];
   	u = src[1];
   	y2 = src[2];
		v = src[3];
		
		*d++ = y1;
		*d++ = u;
		*d++ = y2;
		*d++ = v;
		
		src += 4;
	}	
}

