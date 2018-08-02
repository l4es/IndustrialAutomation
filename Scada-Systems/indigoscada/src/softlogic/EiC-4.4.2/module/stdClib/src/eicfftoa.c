/* eicfftoa.c
 *
 *	(C) Copyright Dec 29 1995, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */



/*#define DBL_MIN_EXP -100 */

#include <math.h>
#include <limits.h>
#include <float.h>

#include "stdliblocal.h"

char *fftoa(double d, char *str, int p, char type, int trunc)
{
    /*
     * Convert a floating-point number into a
     * formatted 'f','e' or 'E' type string.
     * (c) Edmond J.Breen, March 1995.
     * where:  'p'  represents the precision
     *                i.e. the number of digits
     *                that will be placed after
     *                the decimal point.
     *                A precision of 0 suppresses
     *                the decimal point.
     *         'type' is either 'f', 'e' or 'E'.
     *                'f' ->  [-]mmm.dddd
     *                'e' ->  [-]m.dddde+xx
     *                      | [-]m.dddde-xx
     *                'E' ->  [-]m.ddddE+xx
     *                      | [-]m.ddddE-xx
     *         'trunc' if trunc != 0 then trailing
     *                 zeros will be removed.
     *                 i.e. 'g' or 'G' format.
     */
    double M;
    int i, j, width, spt, dec, prec;
    static char *s;

    prec = p;
    i = 0;
    if (d < 0)
	str[0] = '-', s = &str[1], d = -d;
    else
	s = str;
    if (d >= 1) {		/* collect integer part */
	int k, c;
	double D = floor(d);
	d -= D;
	do {
	    j = fmod(D,10.0);
	    s[i++] = j + '0';
	    D -= j;
	} while ((D /= 10) >= 1);
	/* now reverse the string */
	for (k = i, j = 0, k--; j < k; j++, k--) {
	    c = s[j];
	    s[j] = s[k];
	    s[k] = c;
	}
    }
    dec = i;
    if (!dec) {			/* check for numbers less than 1 */
	if (type != 'f') {	/* assume 'e' or 'E' format */
	    if (d != 0) {	/* d == d might be a safety check? */
		while ((d *= 10) < 1 && dec > DBL_MIN_EXP)
		    --dec;
		--dec;
	    }
	    if (d >= 1)
		prec++, d /= 10;
	} else
	    s[i++] = '0';
    }
    if (dec < 0)
	width = prec;
    else if (type == 'f')
	width = prec + (dec > 0 ? dec : 1);
    else
	width = prec + 1;

    M = DBL_EPSILON/(FLT_RADIX*2.0); /* precision value */
    while(1) {	/* now collect fraction */
	d *= 10;
	j = (int) d;
	d -= j;
	M *=10;
	if((i < width) && (d >= M && d <= (1-M)))
	    s[i++] = (char) j + '0';
	else
	    break;
    } 
    if(d>=0.5)
	j++;
    s[i++] = (char) j + '0';
    while(i<=width)
	s[i++] = '0';
    
    /* Have to round see ANSI176,13.9.5.2 */
    if (!strround(s, width + 1)) /* watch for over flow */
	s[0] = '1', width++, dec++;

    if (p != 0) {		/* add in decimal point */
	if (type == 'f') {
	    if (dec > 0)
		spt = dec;
	    else
		spt = 1;
	} else
	    spt = 1;
	for (i = width; i > spt; i--)
	    s[i] = s[i - 1];
	s[spt] = '.';
    } else
	width--;

    if (trunc) {		/* remove trailing zeros */
	while (width && s[width] == '0')
	    width--;
	if (s[width] == '.')
	    width--;
    }
    if (type != 'f') {		/* add in exponent */
	s[++width] = type;
	if (dec >= 0) {
	    s[++width] = '+';
	    if (dec != 0)
		dec = dec - 1;
	    if (dec < 10)
		s[++width] = '0';
	} else if (dec < 0) {
	    s[++width] = '-';
	    if (dec > -10)
		s[++width] = '0';
	    dec = -dec;
	}
	itoa(dec, &s[++width], 10);
    } else
	s[width + 1] = 0;
    return str;
}








