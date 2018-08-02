/* xvimage.c
 *
 *      (C) Copyright Jan 28 1996, Edmond J. Breen.
 *                 ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */


/* FORMATS FOR PGM/PBM/PPM FILES
 * format:
 * <magic_number><WS><comment><WS><width><WS><height><WS><max_grey_value><WS><data>
 *
 *   <magic_number> P1 | P4 | P2 | P5 | P3 | P6
 *             <WS> spaces | newlines | tabs
 *          <width> number of columns in image
 *         <height> number of rows in image
 * <max_grey_value> maximum grey value, ie. 255. Used with PGM and PPM files only
 *           <data> pixel data in raster scan order. For PPM data the pixel
 *                  values are interlaced, i.e. r1g1b1 r2g2b2 ... etc.
 *        <comment> lines beginning with # are ignored
 *
 *   Traditional  ascii format is specified by P1, P2 and P3
 *   There  is  also a variant on each format, available by set-
 *   ting the RAWBITS option at compile time.
 *
 *
 *    P1 and P4 are for PBM  format: bit maps, 0 or 1, width * height  bits.
 *    in raw mode, P4, the bits are stored 8 per byte, i.e. 8 pixels.
 *
 *    P2 and P5 are for PGM format: 8 bits/pixel in raw mode
 *    P3 and P6 are for PPM format: 24 bits/pixel in raw mode.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char * makeBitImage(unsigned char * data,
                            int w, int h)
{

   unsigned char * byte;
   int  nbytes,i,j,k;

   nbytes = w/8;
   if(w%8)
       ++nbytes;

   nbytes *= h;
   /* create bit image */
   byte = (unsigned char *)calloc(nbytes,sizeof(char));
   if(!byte)
       return NULL;
   for(i=0;i<h;++i) {
       for(k=7,j=0;j<w;++j,--k) {
           if(*data++ != 0)
               *byte |= (1<<k);
           if(k==0) {
               k = 8;
               ++byte;
           }
        }
       if(w%8)
           ++byte;
   }
   return byte - nbytes;
}


/* write a 1 bit raw binary  image to disk */
int WritePBM(FILE *fp,
             int w,int h,
             unsigned char * data)
{
    unsigned char * bits;
    int w2;

    w2 = w/8 + (w%8 != 0);
    bits = makeBitImage(data,w,h);
    fprintf(fp,"P4 %d  %d\n",w,h);
    if(ferror(fp)) return -1;
    fwrite(bits,h*w2,1,fp);
    if (ferror(fp)) return -1;
    free(bits);
    return 0;
}

/* write an 8 bit raw grey scale image to disk */
int WritePGM(FILE *fp,
             int w,int h,
             unsigned char * data)
{
    fprintf(fp,"P5 %d  %d 255\n",w,h);
    if(ferror(fp)) return -1;
    fwrite(data,w*h,1,fp);
    if (ferror(fp)) return -1;
    return 0;
}


/* write a 24bit rgb image to disk */
int WritePPM(FILE *fp,
             int w,int h,
             unsigned char * red,
             unsigned char * green,
             unsigned char * blue)
{

    unsigned int i, sz = w * h;
    fprintf(fp,"P6 %d  %d 255\n",w,h);
    if(ferror(fp)) return -1;

    for(i=0;i<sz;++i,++red,++green,++blue) {
        fputc(*red,fp);
        fputc(*green,fp);
        fputc(*blue,fp);
    }
    if (ferror(fp)) return -1;
    return 0;
}


#define xvGrey(data,w,h)             _xvImage(1,data,NULL,NULL,w,h)
#define xvRgb(red,green,blue,w,h)    _xvImage(2,red,green,blue,w,h)
#define xvBin(bin,w,h)               _xvImage(3,bin,NULL,NULL,w,h)


void _xvImage(int type,
              unsigned char * red,
              unsigned char * green,
              unsigned char * blue,
              int w, int h)
{
#define mindim 300
    float mag;

    char tmpname[100];
    char command[200];

    FILE *fp;

    tmpnam(tmpname);

    fp = fopen(tmpname,"wb");
    if(!fp) {
        fprintf(stderr,"Failed to open temp file %s\n",tmpname);
        return;
    }

    switch(type) {
      case 1:WritePGM(fp,w,h,red); break;
      case 2:WritePPM(fp,w,h,red,green,blue); break;
      case 3:WritePBM(fp,w,h,red); break;
      default:
        fputs("Unknown type in _xvImage",stderr);
    }


    fclose(fp);

    if(w < mindim && h < mindim) {
        mag = w > h ? w : h;
        mag = mindim/mag;
    } else
        mag = 1.0;

    sprintf(command,"xv -RM -raw -expand %g  %s &",mag, tmpname);
    system(command);

#undef mindim
}

void xvFloat(float *f, int w, int h)
{
    float mx,mn;
    int i, sz;
    unsigned char * data = (unsigned char*)malloc(w*h*sizeof(char));
    if(!data)
        fputs("Error in xvFloat, couldn't create memory",stderr);
    sz = w * h;
    mx = mn = *f++;
    for(i=1;i<sz;++i,++f) {
        if(*f > mx)
            mx = *f;
        if(*f < mn)
            mn = *f;
    }
    if(mx != mn) {
        printf(" min = %g  max = %g\n",mn,mx);
        mx = 255/(mx - mn);
        f -= sz;
        for(i=0;i<sz;i++)
            *data++ = (*f++ - mn)*mx;
        data  -= sz;
    } else
        memset(data,(char)mx,sz);
    xvGrey(data,w,h);
    free(data);
}

void xvInt(int *f, int w, int h)
{
    float mx,mn;
    int i, sz;
    unsigned char * data = (unsigned char*)malloc(w*h*sizeof(char));
    if(!data)
        fputs("Error in xvFloat, couldn't create memory",stderr);
    sz = w * h;
    mx = mn = *f++;
    for(i=1;i<sz;++i,++f) {
        if(*f > mx)
            mx = *f;
        if(*f < mn)
            mn = *f;
    }
    if(mx != mn) {
        printf(" min = %g  max = %g\n",mn,mx);
        mx = 255/(mx - mn);
        f -= sz;
        for(i=0;i<sz;i++)
            *data++ = (*f++ - mn)*mx;
        data  -= sz;
    } else
        memset(data,(char)mx,sz);
    xvGrey(data,w,h);
    free(data);
}


    
