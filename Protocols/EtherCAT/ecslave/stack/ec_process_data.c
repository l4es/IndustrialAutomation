#include "../include/xgeneral.h"
#include "ethercattype.h"
#include "ec_process_data.h"
#include "ecs_slave.h"
#include "ec_sii.h"
#include "ec_regs.h"

int init_process_data(ecat_slave *esv)
{
	esv->pd.size = ec_sii_pdoes_sizes(esv);
	if (esv->pd.size <= 0 ){
		return -1;
	}
	esv->pd.data = xmalloc(esv->pd.size);
	if (esv->pd.data == 0)
		return -1;
	memset(esv->pd.data, 'x', esv->pd.size);
	esv->last_word_offset = -1;
	esv->sii_command = NULL;
	return 0;
}

/*
 * process data 
*/
int set_process_data(ecat_slave *esv,uint8_t *data, uint16_t offset, uint16_t datalen)
{
	memcpy(&esv->pd.data[offset % esv->pd.size], data, datalen);
	return 0;
}

int get_process_data(ecat_slave* esv,uint8_t *data, uint16_t offset, uint16_t datalen)
{
	memcpy(data, &esv->pd.data[offset % esv->pd.size], datalen);
	return 0;
}

void normalize_sizes(ecat_slave *esv, uint32_t *offset,uint16_t *datalen)
{
	int off  = (ec_station_address(esv) -1) * esv->pd.size + *offset;
	if (off < 0) {
		ec_printf("%s off=%d  offset=%d  stafr=%hu\n",
			__func__, off, *offset, ec_station_address(esv));
		return;
	}
	ec_printf("%s off=%d offset=%d statr=%d datalen=%hu\n",
			__func__, 
			off, 
			*offset, 
			ec_station_address(esv),
			*datalen);
	*offset = (uint32_t)off;
	*datalen = *datalen % esv->pd.size;
	if (*datalen == 0){
		*datalen = esv->pd.size;
	}
}
