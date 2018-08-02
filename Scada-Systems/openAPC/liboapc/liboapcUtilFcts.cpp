/********************************************************************************************

This program and source file is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>.

*********************************************************************************************/

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>

#ifdef ENV_WINDOWS
 #undef _UNICODE
 #include <windows.h>
 #ifndef ENV_WINDOWSCE
#endif
 #define MSG_NOSIGNAL 0
 #define strdup _strdup
#else
 #include <sys/types.h>
 #include <arpa/inet.h>
 #include <unistd.h>
#endif

#ifndef ENV_WINDOWSCE
 #include <errno.h>
#else
 #include <Winsock.h>
#endif

#ifdef ENV_QNX
 #define MSG_NOSIGNAL 0
#endif
 
#include "liboapc.h"



/**
 * This is a small helper function that converts a standard double to its representation in
 * format of the structure oapc_num_value_block; this structure is completely platform- and
 * architecture independent and therefore can be used for saving data in a portable way or
 * for transmitting them via network
 * @param[in] inValue the double value that has to be converted
 * @param[out] outBlock pointer to a structure of type oapc_num_value_block where the converted
 *             float value has to be stored into
 */
OAPC_EXT_API void oapc_util_dbl_to_block(double inValue,struct oapc_num_value_block *outBlock)
{
   if ((fabs(inValue)<200000000.0) && ((fabs(inValue)-abs((int)inValue))!=0))
   {
      outBlock->numDivider=0;
      while ((fabs(inValue)<200000000.0) && ((fabs(inValue)-abs((int)inValue))!=0) && (outBlock->numDivider<20)) // slow but necessary to avoid problems with incorrect floating point calculations
      {
         outBlock->numDivider++;
         inValue*=10;
      }
      outBlock->numValue=(int)OAPC_ROUND(inValue,0);
   }
   else
   {
      outBlock->numDivider=0;
      outBlock->numValue=(int)inValue;
   }
   outBlock->numDivider=htons(outBlock->numDivider);
   outBlock->numValue=htonl(outBlock->numValue);
   outBlock->reserved=0;
}



/**
Converts a floating point number from the portable structure back to the platform and
architecture dependent format
@param[in] inBlock the data block to be converted
@return the floating point number that can be used directly
*/
OAPC_EXT_API double oapc_util_block_to_dbl(struct oapc_num_value_block *inBlock)
{
   struct oapc_num_value_block numBlock;
   
   numBlock.numValue=ntohl(inBlock->numValue);
   numBlock.numDivider=ntohs(inBlock->numDivider);
   
   if ((numBlock.numDivider==0) || (numBlock.numDivider==1)) return numBlock.numValue;
   return numBlock.numValue/pow(10.0,numBlock.numDivider);
}



/**
Converts an ASCII-string to Unicode
@param[in] str ASCII input string
@param[out] out converted Unicode string
@param[in] outSize size of the output string that doesn't has to be exceeded
*/
OAPC_EXT_API bool oapc_util_to_unicode(char* str,wchar_t* out,int outSize) 
{ 
   size_t i,length;

   if ((!str) || (!out)) return false; 
   length=strlen(str); 
   if (length>=(size_t)outSize) length=outSize-1;

   for (i=0; i<length; i++)  out[i]=(wchar_t)str[i]; 
   out[length]=0; 
   return true; 
}

/**
 * Convert a 64 bit integer from host to network byteorder; this function
 * automagically checks the endianness of the current platform (small disadvantage:
 * this check is done every time)
 * @param[in] host_longlong the 64 bit integer that has to be converted
 * @return the 64 bit integer in network byteorder
 */
OAPC_EXT_API int64_t oapc_util_htonll(int64_t host_longlong)
{
   int x = 1;
 
   /* little endian */
   if(*(char *)&x == 1) return ((((int64_t)htonl((unsigned long)host_longlong)) << 32) + htonl((unsigned long)(host_longlong >> 32)));
   /* big endian */
   else return host_longlong;
}

/**
 * Convert a 64 bit integer from network to host byteorder; this function
 * automagically checks the endianness of the current platform (small disadvantage:
 * this check is done every time)
 * @param[in] host_longlong the 64 bit integer that has to be converted
 * @return the 64 bit integer in host byteorder
 */
OAPC_EXT_API int64_t oapc_util_ntohll(int64_t net_longlong)
{
   int x = 1;
 
   /* little endian */
   if(*(char *)&x == 1) return ((((int64_t)ntohl((unsigned long)net_longlong)) << 32) + ntohl((unsigned long)(net_longlong >> 32)));
    /* big endian */
   else return net_longlong;
}

/**
 * Creates a gray value out of a RGB colour
 * @param[in] colour input RGB colour value
 * @return the converted gray value
 */
OAPC_EXT_API unsigned int oapc_util_colour2gray(unsigned int colour)
{
   unsigned int col;

   col=(colour & 0xFF) + ((colour & 0xFF00)>>8) + ((colour & 0xFF0000)>>16);
   col=col/3;
   return (col<<16)|(col<<8)|col;
}


#ifdef ENV_WINDOWSCE
static double _atof(const char *str) 
{
   double number,p10;
   int    exponent,negative;
   char  *p = (char *) str;
   int    n,num_digits,num_decimals;

   // Skip leading whitespace
   while (isspace(*p)) p++;

   // Handle optional sign
   negative = 0;
   switch (*p) 
   {
      case '-': negative = 1; // Fall through to increment position
      case '+': p++;
   }
   number = 0.0;
   exponent = 0;
   num_digits = 0;
   num_decimals = 0;

   // Process string of digits
   while (isdigit(*p)) 
   {
      number = number * 10. + (*p - '0');
      p++;
      num_digits++;
   }

   // Process decimal part
   if (*p == '.') 
   {
      p++;

      while (isdigit(*p)) 
      {
         number = number * 10. + (*p - '0');
         p++;
         num_digits++;
         num_decimals++;
      }
      exponent -= num_decimals;
   }

   if (num_digits == 0) return 0.0;

   // Correct for sign
   if (negative) number = -number;

   // Process an exponent string
   if (*p == 'e' || *p == 'E') 
   {
      // Handle optional sign
      negative = 0;
      switch (*++p) 
      {
         case '-': negative = 1;   // Fall through to increment pos
         case '+': p++;
      }
   }

   // Process string of digits
   n = 0;
   while (isdigit(*p)) 
   {
      n = n * 10 + (*p - '0');
      p++;
   }

   if (negative) exponent -= n;
   else exponent += n; 

   if (exponent < DBL_MIN_EXP  || exponent > DBL_MAX_EXP) return HUGE_VAL;

   // Scale the result
   p10 = 10.;
   n = exponent;
   if (n < 0) n = -n;
   while (n) 
   {
      if (n & 1) 
      {
         if (exponent < 0) number /= p10;
         else number *= p10;
      }
      n >>= 1;
      p10 *= p10;
   }
   return number;
}
#endif

/**
 * Converts a string with a dot as decimal delimiter to a floating point number;
 * locales that would expect e.g. a comma are ignored
 * @param[in] c the string representing a floating point number with a dot (.) 
 *            as separator
 * @return the converted floating point number
 */
OAPC_EXT_API double oapc_util_atof_dot(const char *c)
{
#ifdef ENV_WINDOWSCE
   return _atof(c);
#else
   char  *p,*oldLoc=NULL;
   double result;

   p=setlocale(LC_NUMERIC,NULL);
   if (p) 
   {
      oldLoc=strdup(p);
      setlocale(LC_NUMERIC,"C");
   }
   result=atof(c);
   if (p) 
   {
      setlocale(LC_NUMERIC,oldLoc);
      free(oldLoc);
   }
   return result;
#endif
}

/**
 * Converts a string to a floating point number independent from the used locale
 * and independent from the possibly unknown locale of the strings origin,
 * if necessary the string is converted to the correct format before
 * @param[in] c the string representing a floating point number with a dot (.) or
 *            comma (,) as separator
 * @return the converted floating point number
 */
OAPC_EXT_API double oapc_util_atof(const char *c)
{
   #define CONV_MODE_UNDEFINED 0 // behaviour of atof() not detected yet
   #define CONV_MODE_DOT       1 // atof() expects a dot for conversion
   #define CONV_MODE_COMMA     2 // atof() expects a comma for conversion

   static char  conv_mode=CONV_MODE_UNDEFINED;
          char  tmpC[20+4];
          char *separator;

   if (conv_mode==CONV_MODE_UNDEFINED)
   {
      if (atof("1.5")==1.5) conv_mode=CONV_MODE_DOT;
      else if (atof("1,5")==1.5) conv_mode=CONV_MODE_COMMA;
   }
   if (strstr(c,","))
   {
      if (conv_mode==CONV_MODE_COMMA) return atof(c);
      strncpy(tmpC,c,20);
      separator=strstr(tmpC,",");
      if (separator)
      {
         *separator='.';
         return atof(tmpC);
      }
      else return atof(c);
   }
   else // handle dot
   {
      if (conv_mode==CONV_MODE_DOT) return atof(c);
      strncpy(tmpC,c,20);
      separator=strstr(tmpC,".");
      if (separator)
      {
         *separator=',';
         return atof(tmpC);
      }
      else return atof(c);
   }
}


#ifdef ENV_WINDOWS

#define CLOCK_REALTIME 0

struct timespec
{
   long tv_sec;
   long tv_nsec;
};

static int clock_gettime(int /*X*/, struct timespec *tv)
{
   SYSTEMTIME systime;

   GetSystemTime(&systime);
   tv->tv_sec=systime.wSecond+(systime.wMinute*60)+(systime.wHour*60*60);
   tv->tv_nsec=systime.wMilliseconds*1000000;
   return 0;
}

#endif



/**
 * Evaluates the current time and returns the result within an anonymous data block
 * @return the current time; this data block is an allocated memory area that has to be released
 *         e.g. by calling oapc_util_diff_time()
 */
OAPC_EXT_API void* oapc_util_get_time()
{
   struct timespec *time;

   time=(struct timespec*)malloc(sizeof(struct timespec));
   if (time) clock_gettime(CLOCK_REALTIME,time);
   return (void*)time;
}


/**
 * Releases the memory that belongs to unused time data
 * @param[in] time1 the time data that have to be released
 */
OAPC_EXT_API void oapc_util_release_time(void *time1)
{
   if (time1) free(time1);
}


/**
 * Calculates the difference between two time information, the memory areas of these
 * time information are released by this function, they should not be used afterwards
 * @param[in] time1 start time
 * @param[in] time2 end time
 * @return the time difference between both in unit seconds and with up to nanosecond
 *         resolution, the maximum resolution and accuracy is platform dependent
 */
OAPC_EXT_API double oapc_util_diff_time(void *time1,void *time2)
{
   struct timespec *t1,*t2;
   double           diffSec=0,diffNSec=0;

   if ((time1) && (time2))
   {
      t1=(struct timespec*)time1;
      t2=(struct timespec*)time2;
      diffSec=t2->tv_sec-t1->tv_sec;
      diffNSec=t2->tv_nsec-t1->tv_nsec;
   }
   oapc_util_release_time(time1);
   oapc_util_release_time(time2);
   return diffSec+(diffNSec/1000000000.0);
}


/**
 * Evaluates the time of the current day in unit seconds
 * @param[in] time1 the time data that contain the current time
 * @return the time that has elapsed since midnight
 */
OAPC_EXT_API double oapc_util_get_timeofday(void *time1)
{
   struct timespec *t1;
   double           d=0;

   if (time1)
   {
      t1=(struct timespec*)time1;
      d=t1->tv_sec % 86400;
      d=d+(t1->tv_nsec/1000000000.0);
   }
   return d;
}



/**
 * creates and initialises a binary data structure
 * @param[in] type the type of the binary structure
 * @param[in] subType the sub-type that belongs to the given type
 * @param[in] compression the used compression mode
 * @param[in] sizeData the size of the appended data block
 * @return the allocated binary data structure or NULL in case of an error
 */
OAPC_EXT_API struct oapc_bin_head *oapc_util_alloc_bin_data(unsigned char type,unsigned char subType,unsigned char compression,int sizeData)
{
   struct oapc_bin_head *bin;

   bin=(struct oapc_bin_head*)malloc(sizeof(struct oapc_bin_head)+sizeData);
   if (!bin) return NULL;
   memset(bin,0,sizeof(struct oapc_bin_head)+sizeData);
   bin->type=type;
   bin->subType=subType;
   bin->compression=compression;
   bin->version=1;
   bin->sizeHead_donotuse=0xFFFFFFFF;
   bin->sizeData=sizeData;
   return bin;
}


OAPC_EXT_API struct oapc_bin_head *oapc_util_duplicate_bin_data(const struct oapc_bin_head *bin_in)
{
   struct oapc_bin_head *bin;
   unsigned int          sizeData;

   sizeData= bin_in->sizeData;
// to be enabled only on non-big-endian platforms
//   if ((sizeData & 0xFFF00000)>0) sizeData=ntohl(sizeData); // this is a workaround, sizeData should have been set to network byteorder in general but tthis was not done due to a bug, so we need to find out if this value may be byte-swapped or not
   bin=oapc_util_alloc_bin_data(bin_in->type,bin_in->subType,bin_in->compression,sizeData);
   if (!bin) return NULL;

   memcpy(bin,bin_in,sizeof(struct oapc_bin_head)+sizeData);
   return bin;
}


/**
 * release the memory that belongs to a binary data block created with oapc_util_alloc_bin_data()
 * @param[in] bin the memory are to be released
 */
OAPC_EXT_API void oapc_util_release_bin_data(struct oapc_bin_head *bin)
{
//#ifndef _DEBUG // temporary workaround for a problem in 64 bit windows build; yes, this leaves a memory leak!
   free(bin);
//#endif
}



/**
 * Allocates and initialises data for a new ring buffer.
 * @param[in] buffer ring buffer information structure, here a pointer to a variable of type
 *            struct oapc_util_rb_data has to be handed over. It does not have to be initialised,
 *            this is done by the function completely. The structure has to be left untouched
 *            and needs to be used for all other rungbuffer function calls
 * @param[in] elements the number of memory pointers the ring buffer is able to store at maximum;
 *            this value will be used during the whole lifetime of the ring buffer, resizing
 *            during operation is not possible
 * @return OAPC_ERROR_NO_MEMORY in case the ring buffer could not be initialised or OAPC_OK when
 *         everything was OK and the buffer handler "buffer" can be used for subsequent operations
 */
OAPC_EXT_API int oapc_util_rb_alloc(struct oapc_util_rb_data *buffer,int elements)
{
   buffer->elements=elements+1;
   buffer->buffer=(void**)malloc(buffer->elements*sizeof(void*));
   if (!buffer->buffer) return OAPC_ERROR_NO_MEMORY;
   buffer->head=0;
   buffer->tail=0;
   return OAPC_OK;
}



/**
 * Releases all ring buffer related resources but none of the data that still may be stored within
 * the buffer. It is recommended to fetch, release and remove all data of the buffer until it is
 * empty before this function is called.
 * @param[in] buffer the ring buffer handler that has to be released, after calling this function
 *            no other ring buffer functions can be used with this handler as long as no new call
 *            of oapc_util_rb_alloc() is done
 * @return OAPC_OK
 */
OAPC_EXT_API int oapc_util_rb_release(struct oapc_util_rb_data *buffer)
{
   if (buffer->buffer) free(buffer->buffer);
   buffer->buffer=NULL;
   return OAPC_OK;
}



/**
 * Add a new pointer to a memory area to the ring buffer
 * @param[in] buffer the handler for the ring buffer that has to be used for this function call
 * @param[in] data the pointer that has to be added to the ring buffer
 * @return OAPC_ERROR_RESOURCE in case the ring buffer is full and no new data could be added
 *         or OAPC_OK
 */
OAPC_EXT_API int oapc_util_rb_push(struct oapc_util_rb_data *buffer,void *data)
{
   if (!buffer->buffer) return OAPC_ERROR_INVALID_INPUT;
   if (oapc_util_rb_full(buffer)) return OAPC_ERROR_RESOURCE;
   if (buffer->tail>=buffer->elements) buffer->tail-=buffer->elements;
   buffer->buffer[buffer->tail]=data;
   buffer->tail++;
   return OAPC_OK;
}



/**
 * Evaluates the free space in buffer
 * @param[in] buffer the handler for the ring buffer that has to be used for this function call
 * @return the number of elements that still can be stored within the buffer
 */
OAPC_EXT_API bool oapc_util_rb_empty(struct oapc_util_rb_data *buffer)
{
   return (buffer->head==buffer->tail);
}



/**
 * Evaluates the free space in buffer
 * @param[in] buffer the handler for the ring buffer that has to be used for this function call
 * @return the number of elements that still can be stored within the buffer
 */
OAPC_EXT_API bool oapc_util_rb_full(struct oapc_util_rb_data *buffer)
{
   int dist;

   dist=buffer->head-buffer->tail;
   if (dist<0) dist+=buffer->elements;
   return (dist==1);
}



/**
 * Get the next data from the ring buffer but don't remove them
 * @param[in] buffer the handler for the ring buffer that has to be used for this function call
 * @return NULL in case the ring buffer is empty and no more data are available or the next data
 */
OAPC_EXT_API void *oapc_util_rb_front(struct oapc_util_rb_data *buffer)
{
   if (!buffer->buffer) return NULL;
   if (buffer->tail>=buffer->elements) buffer->tail-=buffer->elements;
   if (buffer->head==buffer->tail) return NULL;
   return buffer->buffer[buffer->head];
}



/**
 * Remove the next available data from the ring buffer
 * @param[in] buffer the handler for the ring buffer that has to be used for this function call
 * @return OAPC_ERROR_RESOURCE in case the buffer was already empty and no more data can be removed
 *         or OAPC_OK in case the operation was successful
 */
OAPC_EXT_API int oapc_util_rb_pop(struct oapc_util_rb_data *buffer)
{
   if (!buffer->buffer) return OAPC_ERROR_INVALID_INPUT;
   if (oapc_util_rb_empty(buffer)) return OAPC_ERROR_RESOURCE;
   buffer->head++;
   if (buffer->head>=buffer->elements) buffer->head-=buffer->elements;
   return OAPC_OK;
}



/**
 * Checks if a mask bit within a black/white mask bitmap is set or not
 * @param[in] bin binary structure of type OAPC_BIN_TYPE_IMAGE/OAPC_BIN_SUBTYPE_IMAGE_BW1 where
 *            to check for the mask bit at a specific position
 * @param[in] x position in x direction to check for the mask bits state
 * @param[in] y position in y direction to check for the mask bits state
 * @return 1 when the mask bit is set, 0 otherwise
 */
OAPC_EXT_API int oapc_util_check_maskbit(struct oapc_bin_head *bin,int x,int y)
{
    unsigned char *d;
    unsigned char  mask;
    int            pos;

    if ((!bin) || (bin->type!=OAPC_BIN_TYPE_IMAGE) || (bin->subType!=OAPC_BIN_SUBTYPE_IMAGE_BW1)) return 1;

    pos=(y*bin->param1)+x;
    d=(unsigned char*)&bin->data;
    d=d+(pos/8);
    mask=(unsigned char)(0x80 >> (pos % 8));
    return (*d & mask);
}

