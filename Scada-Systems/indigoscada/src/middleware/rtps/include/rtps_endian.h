#ifndef RTPS_ENDIAN_H
#define RTPS_ENDIAN_H 1

#define FLAG_BIG_ENDIAN 0
#define FLAG_LITTLE_ENDIAN 1

#if WORDS_BIGENDIAN

# define FLAG_ENDIANNESS FLAG_BIG_ENDIAN
# define conversion_needed(to_endianness) ((to_endianness)!=FLAG_BIG_ENDIAN)

#else

# define FLAG_ENDIANNESS FLAG_LITTLE_ENDIAN
# define conversion_needed(to_endianness) ((to_endianness)!=FLAG_LITTLE_ENDIAN)

#endif

static inline 
void rtps_byteswap(uint8_t *outdata,
			  const uint8_t *data,
			  uint32_t datalen)
{
  const uint8_t *source_ptr = data;
  uint8_t *dest_ptr = outdata + datalen - 1;
  while(dest_ptr >= outdata)
    *dest_ptr-- = *source_ptr++;
}


#endif /* FLAG_ENDIANNESS */
