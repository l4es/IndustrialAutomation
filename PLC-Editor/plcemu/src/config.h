#ifndef _CONFIG_H_
#define _CONFIG_H_

#define ADVANTECH_HISTORICAL_BASE 50176

typedef struct config {
    //FILE * ErrLog;
    
    //vm configuration    
    BYTE nt;
    BYTE ns;
    BYTE nm;
    BYTE nr;
    BYTE di;
    BYTE dq;
    BYTE ai;
    BYTE aq; 

    //ui
    int sigenable;
    BYTE page_width;
    BYTE page_len;
    
    
    //hw
    char hw[MAXSTR];
    //userland device
    unsigned int base;
    BYTE wr_offs;
    BYTE rd_offs;
    //comedi
    unsigned int comedi_file;    
    unsigned int comedi_subdev_i;
    unsigned int comedi_subdev_q;
    unsigned int comedi_subdev_ai;
    unsigned int comedi_subdev_aq;
    //simulation
    char sim_in_file[MAXSTR];
    char sim_out_file[MAXSTR];
    
    //sampling
    unsigned long step;
    char pipe[MAXSTR];
    //obsolete
    char response_file[MAXSTR];
} * config_t;

/**
 * @brief initialize configuration
 * @return a newly alloced config
 */
config_t init_config();

/**
 * @brief cleanup and free configuration
 * @param the configuration
 */
void clear_config(config_t *c);

/**
 * @brief load text line into configuration
 * @param the text
 * @param the configuration
 * @return OK or ERR
 */
int load_config_line(const char * line, config_t conf);

/**
 * @brief entry point: load text file into configuration
 * @param filename (full path)
 * @param the configuration
 * @return OK or ERR
 */
int load_config(const char * filename, config_t conf);

void configure(const config_t conf, plc_t plc);

#endif //_CONFIG_H_




































