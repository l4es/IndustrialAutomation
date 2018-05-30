#ifndef  __EC_PROCESS_DATA_H__
#define  __EC_PROCESS_DATA_H__


#ifdef __cplusplus
extern "C" {
#endif



struct __ecat_slave__ ;

int set_process_data(struct __ecat_slave__*, uint8_t * data, uint16_t offset, uint16_t datalen);
int get_process_data(struct __ecat_slave__*, uint8_t * data, uint16_t offset, uint16_t datalen);
int init_process_data(struct __ecat_slave__ *);
void normalize_sizes(struct __ecat_slave__ *ecs, uint32_t *offset,uint16_t *datalen);


#ifdef __cplusplus
}
#endif

#endif
