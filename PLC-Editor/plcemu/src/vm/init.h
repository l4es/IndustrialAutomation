#ifndef _INIT_H_
#define _INIT_H_


/**
 * @brief extract alphanumeric variable name from file line starting
 * at index start
 * @in line
 * @out name
 * @param start
 * @return index position, or error
 */
int extract_name(const char *line, char *name, int start );

/**
 * @brief extract numeric index from file line starting
 * at index start
 * @in line
 * @out idx
 * @param start
 * @return index position, or error
 */
int extract_index(const char *line, int *idx, int start);

/**
 * @brief extract configuration value from file line starting
 * at index start, drop comments
 * @in line
 * @out val
 * @param start
 * @return index position, or error
 */
int extract_value( const char *line, char *val, int start);
//tech debt
int declare_input(const char* name,  int idx, const char* val);

int declare_output(const char* name, int idx, const char* val);

int declare_analog_input(const char* name, int idx, const char* val);

int declare_analog_output(const char* name, int idx, const char* val);

int configure_input_min(const char* name, int idx, const char* val);

int configure_output_min(const char* name, int idx, const char* val);

int configure_input_max(const char* name, int idx, const char* val);

int configure_output_max(const char* name, int idx, const char* val);

int declare_register(const char* name, int idx, const char* val);
 
int declare_timer(const char* name, int idx, const char* val);

int declare_blinker(const char* name, int idx, const char* val);

int declare_serial(const char* name, int idx, const char* val);

int init_register(const char* name, int idx, const char* val);

int init_register_r(const char* name, const int idx, const char* val);

int define_reg_direction(const char* name, int idx, const char* val);

int define_reg_readonly(const char* name, int idx, const char* val);

int init_timer_set(const char* name, int idx, const char* val);

int init_timer_preset(const char* name, int idx, const char* val);

int init_timer_delay(const char* name, int idx, const char* val);

int init_blinker_set(const char* name, int idx, const char* val);

#endif //_INIT_H_

