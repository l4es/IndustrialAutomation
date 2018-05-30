
#ifndef __EC_CMD_H__
#define __EC_CMD_H__

#ifdef __cplusplus
extern "C" {
#endif


void ecs_process_cmd(ecat_slave *ecs, uint8_t *dgram_ec);
void ec_process_datagrams(ecat_slave *ecs,int len, uint8_t *dgram);

#ifdef __cplusplus
}
#endif

#endif
