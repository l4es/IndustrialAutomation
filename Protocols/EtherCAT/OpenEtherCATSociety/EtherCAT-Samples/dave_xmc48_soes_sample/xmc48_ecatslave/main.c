/*
 * main.c
 *
 *  Created on: 2017 Nov 23 19:44:27
 *  Author: rtlaka
 */




/**

 * @brief main() - Application entry point
 *
 * <b>Details of function</b><br>
 * This routine is the application entry point. It is invoked by the device startup code. 
 */
#include "xmc_gpio.h"
#include "config.h"
#include "esc_hw.h"
#include "xmc4800_slave.h"

#define GPIO_BUTTON1   P15_13

#define GPIO_LED1		P1_14
#define GPIO_LED2		P0_14

static const XMC_GPIO_CONFIG_t gpio_config_btn = {
  .mode = XMC_GPIO_MODE_INPUT_INVERTED_PULL_UP,
  .output_level = 0,
  .output_strength = 0
};
static const XMC_GPIO_CONFIG_t gpio_config_led1 = {
  .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
  .output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,
  .output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE
};
static const XMC_GPIO_CONFIG_t gpio_config_led2 = {
  .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
  .output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,
  .output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE
};
/**
 * This function gets input values and updates Rb.Buttons
 */
void cb_get_Buttons(void)
{
	Rb.Buttons.Button = (uint8_t)XMC_GPIO_GetInput(GPIO_BUTTON1);
}
/**
 * This function sets output values according to Wb.LEDs
 */
void cb_set_LEDs(void)
{
	if(Wb.LEDs.LED1)
	{
		XMC_GPIO_SetOutputHigh(GPIO_LED1);
	}
	else
	{
		XMC_GPIO_SetOutputLow(GPIO_LED1);
	}

	if(Wb.LEDs.LED2)
	{
		XMC_GPIO_SetOutputHigh(GPIO_LED2);
	}
	else
	{
		XMC_GPIO_SetOutputLow(GPIO_LED2);
	}
}

/* Configuration parameters for SOES
 * SM and Mailbox parameters comes from the
 * generated config.h
 */
static esc_cfg_t config =
{
   .user_arg = NULL,
   .use_interrupt = 0,
   .watchdog_cnt = 5000,
   .mbxsize = MBXSIZE,
   .mbxsizeboot = MBXSIZEBOOT,
   .mbxbuffers = MBXBUFFERS,
   .mb[0] = {MBX0_sma, MBX0_sml, MBX0_sme, MBX0_smc, 0},
   .mb[1] = {MBX1_sma, MBX1_sml, MBX1_sme, MBX1_smc, 0},
   .mb_boot[0] = {MBX0_sma_b, MBX0_sml_b, MBX0_sme_b, MBX0_smc_b, 0},
   .mb_boot[1] = {MBX1_sma_b, MBX1_sml_b, MBX1_sme_b, MBX1_smc_b, 0},
   .pdosm[0] = {SM2_sma, 0, 0, SM2_smc, SM2_act},
   .pdosm[1] = {SM3_sma, 0, 0, SM3_smc, SM3_act},
   .pre_state_change_hook = NULL,
   .post_state_change_hook = NULL,
   .application_hook = NULL,
   .safeoutput_override = NULL,
   .pre_object_download_hook = NULL,
   .post_object_download_hook = NULL,
   .rxpdo_override = NULL,
   .txpdo_override = NULL,
   .esc_hw_interrupt_enable = NULL,
   .esc_hw_interrupt_disable = NULL,
   .esc_hw_eep_handler = ESC_eep_handler
};

int main(void)
{
  XMC_GPIO_Init(GPIO_BUTTON1, &gpio_config_btn);
  XMC_GPIO_Init(GPIO_LED1, &gpio_config_led1);
  XMC_GPIO_Init(GPIO_LED2, &gpio_config_led2);

  ecat_slv_init(&config);

  while(1)
  {
    ecat_slv();
  }
}
