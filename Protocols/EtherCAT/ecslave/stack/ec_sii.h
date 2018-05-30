#ifndef __EC_SII__
#define __EC_SII__

#ifdef __cplusplus
extern "C" {
#endif

void init_sii(ecat_slave *);
void ec_sii_rw(ecat_slave* esv,uint8_t * data, int datalen);
int ec_sii_start_read(ecat_slave* esv,uint8_t * data, int datalen);
int ec_sii_pdoes_sizes(ecat_slave *);
void ec_sii_syncm(ecat_slave *,int reg, uint8_t* data, int datalen);


#ifdef __cplusplus
}
#endif

#endif
