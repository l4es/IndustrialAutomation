#include "../include/xgeneral.h"

void ec_printf(const char *str, ...)
{
}

/** Outputs frame contents for debugging purposes.
 * If the data block is larger than 256 bytes, only the first 128
 * and the last 128 bytes will be shown
 */
void ec_print_data(const uint8_t * data,/**< pointer to data */
		   size_t size /**< number of bytes to output */
    )
{
	unsigned int i;

	for (i = 0; i < size; i++) {
		printk("%02X ", data[i]);

		if ((i + 1) % 16 == 0 && i < size - 1) {
			printk("\n");
		}

		if (i + 1 == 128 && size > 256) {
			printk("dropped %zu bytes\n", size - 128 - i);
			i = size - 128;
		}
	}
	printk("\n");
}

/** Print a MAC address to a buffer.
 *
 * \return number of bytes written.
 */
ssize_t ec_mac_print(const uint8_t * mac,
			    /**< MAC address */
		     char *buffer
		     /**< target buffer */
    )
{
	off_t off = 0;
	unsigned int i;

	for (i = 0; i < ETH_ALEN; i++) {
		off += sprintf(buffer + off, "%02X", mac[i]);
		if (i < ETH_ALEN - 1)
			off += sprintf(buffer + off, ":");
	}

	return off;
}
