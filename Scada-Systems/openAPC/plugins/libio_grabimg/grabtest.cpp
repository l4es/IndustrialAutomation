#include <string.h>
#include <stdio.h>

#include "libio_grabimg.h"
#include "oapc_libio.h"
#ifdef ENV_LINUX
#else
 #include "dshow_capture.h"
#endif

struct bmp_head
{
   char id[2];
   int  filesize,reserved;
   int  dataOffset;
};

struct bmp_infoheader 
{
   unsigned short size;
   unsigned int   width,height;
   unsigned short planes,bitCount;
   unsigned int   compression,imageSize;
   int            xPelsPerMeter,yPelsPerMeter;
   unsigned int   clrUsed;
   int            clrImportant;
};


int main(int argc, char *argv[])
{
   struct instData data;

   memset(&data,0,sizeof(struct instData));
   data.config.flags=LIBIO_GRABIMG_FLAG_AUTO_SIZE; // use full available size
   if (openVideoDevice(&data,0)==OAPC_OK)
   {
      if (captureImage(&data)==OAPC_OK)
      {
         struct bmp_head bmpHead={{'B','M'},0,0,sizeof(struct bmp_head)+sizeof(struct bmp_infoheader)};
         struct bmp_infoheader bmpInfo={sizeof(struct bmp_infoheader),0,0,0,24,0,0,0,0,0,0};
         FILE  *FH;

         FH=fopen("grabbed.bmp","wb");
         if (FH)
         {
            bmpHead.filesize=sizeof(struct bmp_head)+sizeof(struct bmp_infoheader)+(data.width*data.height*3);
            fwrite(&bmpHead,sizeof(struct bmp_head),1,FH);

            bmpInfo.width=data.width;
            bmpInfo.height=data.height;
            bmpInfo.imageSize=(data.width*data.height*3);
            fwrite(&bmpInfo,sizeof(struct bmp_infoheader),1,FH);

            fwrite(data.rawBuf,bmpInfo.imageSize,1,FH);

            fclose(FH);
         }
         else printf("Error: could not open file for writing!\n");
      }
      else printf("Error: could not grab image!\n");
      closeVideoDevice(&data);
   }
   else printf("Error: could not open video device!\n");
   return 0;
}

