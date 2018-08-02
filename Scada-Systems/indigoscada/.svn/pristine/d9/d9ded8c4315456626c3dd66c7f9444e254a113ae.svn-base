#ifndef _XVH
#define _XVH

/* bits and pieces extracted from the true xv.h file */

/* MONO returns total intensity of r,g,b triple (i = .33R + .5G + .17B) */
#define MONO(rd,gn,bl) ( ((int)(rd)*11 + (int)(gn)*16 + (int)(bl)*5) >> 5)


/* indicies into conv24MB */
#define CONV24_8BIT  0
#define CONV24_24BIT 1

/* values 'picType' can take */
#define PIC8  CONV24_8BIT
#define PIC24 CONV24_24BIT

/*#define byte unsigned char*/

/*#define REVDATE   "Version 3.10a  Rev: 12/29/94"*/



/* Add in the EiC stuff */
#define xvGrey(data,w,h)             _xvImage(1,data,NULL,NULL,w,h)
#define xvRgb(red,green,blue,w,h)    _xvImage(2,red,green,blue,w,h)
#define xvBin(bin,w,h)               _xvImage(3,bin,NULL,NULL,w,h)

void _xvImage(int type,
              unsigned char * red,
              unsigned char * green,
              unsigned char * blue,
              int w, int h);

void xvFloat(float *f, int w, int h);
void xvInt(int *f, int w, int h);
#endif
 
