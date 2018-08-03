#ifndef _HARDWARE_H_
#define _HARDWARE_H_
/**
 *@file hardware.h
 *@brief hardware access functions
*/
#include <inttypes.h>

/**
 * @brief apply a configuration
 * @param the configuration
 */
void hw_config(const config_t conf);

/**
 * @brief Enable bus communication
 * @return error code
 */
int enable_bus();

/**
 * @brief Disable bus communication
 * @return error code
 */
int disable_bus();

/**
 * @brief fetch all input bytes if necessary
 * @return error code
 */
int io_fetch();

/**
 * @brief flush all output bytes if necessary
 * @return error code
 */
int io_flush();

/**
 * @brief read digital input
 * @param index
 * @param value
 */
void dio_read(unsigned int index ,unsigned char* value);

/**
 * @brief write bit to digital output
 * @param value
 * @param n index
 * @param bit
 */
void dio_write(const unsigned char *value, int n, int bit);

/**
 * @brief read / write binary mask to digital input / output
 * @param mask
 * @param bits
 */
void dio_bitfield(const unsigned char* mask, unsigned char *bits);

/**
 * @brief read analog sample
 * @param the index
 * @param the raw value 
 */
void data_read(unsigned int index, uint64_t* value);

/**
 * @brief write analog sample
 * @param the index
 * @param the raw value 
 */
void data_write(unsigned int index, uint64_t value);


#endif //_HARDWARE_H_
