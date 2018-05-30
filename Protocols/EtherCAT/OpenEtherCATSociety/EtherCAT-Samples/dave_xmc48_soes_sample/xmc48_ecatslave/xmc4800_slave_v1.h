#ifndef __XMC4800_SLAVE_V1_H__
#define __XMC4800_SLAVE_V1_H__

#include "utypes.h"

/**
 * This function gets input values and updates Rb.Buttons
 */
void cb_get_Buttons();

/**
 * This function sets output values according to Wb.LEDs
 */
void cb_set_LEDs();

/**
 * This function sets an application loop callback function.
 */
void set_application_loop_hook(void (*callback)(void));

/**
 * Main function for SOES application
 */
void soes (void);

/**
 * Initialize the SOES stack
 */
void soes_init (void);

#endif /* __XMC4800_SLAVE_V1_H__ */
