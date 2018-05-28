void etl_raise(void *data, int offset, int state);
const char *raise_catch_start(void);
const char *etl_register_raise(
        const char *id, 
        const char *msg, 
        unsigned int auto_reset,
        unsigned int width,
        void **priv_data);
void raise_catch_end(void);
