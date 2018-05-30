#ifndef _EC_UTIL_H__
#define _EC_UTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

struct   ether_header *eth_hdr(uint8_t *buf);
uint16_t htons(uint16_t port);
uint32_t ecat_local_time();
void     ecat_wake_timer();
void    ecat_set_rx_time(void *dummy);
void	*xmalloc(int size);

#ifdef __cplusplus
}
#endif

#endif
