#include <stdint.h>
#include <ecrt.h>

#ifdef __cplusplus
extern "C" {
#endif

struct pdo_map {
    uint16_t pdo_entry_index;
    uint8_t pdo_entry_subindex;
    unsigned int datatype;
    uint8_t bigendian; 
    unsigned int idx;
    void *address;

    /* The next values are used by the support layer */
    struct ecat_domain *domain;
    int offset;
    unsigned int bit_pos;
};

/* Structure to temporarily store SDO objects prior to registration */
struct soe_config {
    /* SoE values. Used by EtherCAT functions */
    uint16_t idn;
    const char *data;
    size_t data_len;
};

/* Structure to temporarily store SDO objects prior to registration */
struct sdo_config {
    /* The data type of the sdo: 
     * only si_uin8_t, si_uint16_t, si_uint32_t are allowed */
    unsigned int datatype;

    /* SDO values. Used by EtherCAT functions */
    uint16_t sdo_index;
    int16_t sdo_subindex; /* May be -1 to indicate complete access */
    uint32_t value;     /* Scalar value or nelem in case of byte_array */
    const char *byte_array;
};

struct ec_slave {
    const struct ec_slave *next;
    unsigned int tid;
    unsigned int master;
    unsigned int domain;
    unsigned int alias;
    unsigned int position;
    unsigned int vendor;
    unsigned int product;

    size_t sdo_config_count;
    const struct sdo_config *sdo_config;

    size_t soe_config_count;
    const struct soe_config *soe_config;

    const ec_sync_info_t *ec_sync_info;
    struct {
        uint16_t assign_activate;
        uint8_t input_based;
        uint32_t cycle_time0;
        uint32_t cycle_time1;
        int32_t shift_time;
    } dc_config;

    size_t rxpdo_count;
    size_t txpdo_count;
    struct pdo_map *pdo_map;
};

void ecs_send(size_t tid);
void ecs_receive(size_t tid);

void ecs_end(size_t nst);

const char *ecs_start( 
        const struct ec_slave *slave_head,
        unsigned int *st,       /* List of sample times in nanoseconds */
        size_t nst,             /* Number of sample times */
        unsigned int single_tasking     /* Set if the model is single tasking,
                                         * even though there are more than one
                                         * sample time */
        );

const char *ecs_setup_master(
        unsigned int master_id, 
        unsigned int refclk_sync_dec,
        void **master);

ec_domain_t *ecs_get_domain_ptr(
        unsigned int master_id, 
        unsigned int domain_id, 
        char input,             /* Domain is  input domain (for TxPdo) */
        char output,            /* Domain is output domain (for RxPdo) */
        unsigned int tid,       /* Task Id of domain */
        const char **errmsg);

int ecs_sdo_handler(
        unsigned int master_id, 
        unsigned int alias, 
        unsigned int position, 
        unsigned int len, 
        void **addr);

#ifdef __cplusplus
}
#endif

