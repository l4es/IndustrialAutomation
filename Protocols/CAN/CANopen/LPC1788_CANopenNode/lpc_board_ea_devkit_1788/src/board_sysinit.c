/*
 * @brief Embedded Artists LPC1788 Sysinit file
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "board.h"

/* The System initialization code is called prior to the application and
   initializes the board for run-time operation. Board initialization
   includes clock setup and default pin muxing configuration. */

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* Pin muxing configuration */
STATIC const PINMUX_GRP_T pinmuxing[] = {
	/* CAN RD1 and TD1 */
	{0x0, 0,  (IOCON_FUNC1 | IOCON_MODE_INACT)},
	{0x0, 1,  (IOCON_FUNC1 | IOCON_MODE_INACT)},
	/* UART 0 debug port (via USB bridge) */
	{0x0, 2,  (IOCON_FUNC1 | IOCON_MODE_INACT)},
	{0x0, 3,  (IOCON_FUNC1 | IOCON_MODE_INACT)},
	/* I2S */
	{0x0, 4,  (IOCON_FUNC1 | IOCON_MODE_INACT)},	/* I2S RX clock */
	{0x0, 5,  (IOCON_FUNC1 | IOCON_MODE_INACT)},	/* I2S RX WS */
	{0x0, 6,  (IOCON_FUNC1 | IOCON_MODE_INACT)},	/* I2S RX SDA */
	{0x0, 7,  (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)},	/* I2S TX clock */
	{0x0, 8,  (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)},	/* I2S TX WS */
	{0x0, 9,  (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)},	/* I2S TX SDA */

	{0x0, 13, (IOCON_FUNC1 | IOCON_MODE_INACT)},	/* USB LED */
	{0x0, 14, (IOCON_FUNC3 | IOCON_MODE_INACT)},	/* USB Softconnect */
	/* SSP 0 */
	{0x0, 15, (IOCON_FUNC1 | IOCON_MODE_INACT)},	/* SSP CLK */
	{0x0, 16, (IOCON_FUNC1 | IOCON_MODE_INACT)},
	{0x0, 17, (IOCON_FUNC1 | IOCON_MODE_INACT)},
	{0x0, 18, (IOCON_FUNC1 | IOCON_MODE_INACT)},
	/* ADC */
	{0x0, 25, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_ADMODE_EN)},
	/* DAC */
	{0x0, 26, (IOCON_FUNC2 | IOCON_DAC_EN | IOCON_HYS_EN | IOCON_MODE_PULLUP)},
	/* USB */
	{0x0, 29, (IOCON_FUNC1 | IOCON_MODE_INACT)},
	{0x0, 30, (IOCON_FUNC1 | IOCON_MODE_INACT)},
	{0x0, 31, (IOCON_FUNC1 | IOCON_MODE_INACT)},

	/* ENET */
	{0x1, 0, (IOCON_FUNC1 | IOCON_MODE_INACT)},
	{0x1, 1, (IOCON_FUNC1 | IOCON_MODE_INACT)},
	{0x1, 4, (IOCON_FUNC1 | IOCON_MODE_INACT)},
	{0x1, 8, (IOCON_FUNC1 | IOCON_MODE_INACT)},
	{0x1, 9, (IOCON_FUNC1 | IOCON_MODE_INACT)},
	{0x1, 10, (IOCON_FUNC1 | IOCON_MODE_INACT)},
	{0x1, 14, (IOCON_FUNC1 | IOCON_MODE_INACT)},
	{0x1, 15, (IOCON_FUNC1 | IOCON_MODE_INACT)},
	{0x1, 16, (IOCON_FUNC1 | IOCON_MODE_INACT)},
	{0x1, 17, (IOCON_FUNC1 | IOCON_MODE_INACT)},
	{0x1, 27, (IOCON_FUNC4 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)}, /* CLKOUT */

	/* JOYSTICK */
	{2, 26, (IOCON_FUNC0 | IOCON_MODE_INACT)}, 	/* JOYSTICK_UP */
	{2, 23, (IOCON_FUNC0 | IOCON_MODE_INACT)}, 	/* JOYSTICK_DOWN */
	{2, 25, (IOCON_FUNC0 | IOCON_MODE_INACT)}, 	/* JOYSTICK_LEFT */
	{2, 27, (IOCON_FUNC0 | IOCON_MODE_INACT)}, 	/* JOYSTICK_RIGHT */
	{2, 22, (IOCON_FUNC0 | IOCON_MODE_INACT)}, 	/* JOYSTICK_PRESS */

	/*   For the EA LPC1788 VBUS is not connected.  Leave it as GPIO. */
	/* {0x1, 30, (IOCON_FUNC2 | IOCON_MODE_INACT)}, */ /* USB_VBUS */

	/* FIXME NOT COMPLETE */

	/* LEDs */
	{0x2, 26, (IOCON_FUNC0 | IOCON_MODE_INACT)},
	{0x2, 27, (IOCON_FUNC0 | IOCON_MODE_INACT)},
};

/* EMC clock delay */
#define CLK0_DELAY 7

/* Keil SDRAM timing and chip Config */
STATIC const IP_EMC_DYN_CONFIG_T IS42S32800D_config = {
	EMC_NANOSECOND(64000000 / 4096),
	0x01,				/* Command Delayed */
	3,					/* tRP */
	7,					/* tRAS */
	EMC_NANOSECOND(70),	/* tSREX */
	EMC_CLOCK(0x01),	/* tAPR */
	EMC_CLOCK(0x05),	/* tDAL */
	EMC_NANOSECOND(12),	/* tWR */
	EMC_NANOSECOND(60),	/* tRC */
	EMC_NANOSECOND(60),	/* tRFC */
	EMC_NANOSECOND(70),	/* tXSR */
	EMC_NANOSECOND(12),	/* tRRD */
	EMC_CLOCK(0x02),	/* tMRD */
	{
		{
			EMC_ADDRESS_DYCS0,	/* EA Board uses DYCS0 for SDRAM */
			2,	/* RAS */

			EMC_DYN_MODE_WBMODE_PROGRAMMED |
			EMC_DYN_MODE_OPMODE_STANDARD |
			EMC_DYN_MODE_CAS_2 |
			EMC_DYN_MODE_BURST_TYPE_SEQUENTIAL |
			EMC_DYN_MODE_BURST_LEN_4,

			EMC_DYN_CONFIG_DATA_BUS_32 |
			EMC_DYN_CONFIG_LPSDRAM |
			EMC_DYN_CONFIG_8Mx16_4BANKS_12ROWS_9COLS |
			EMC_DYN_CONFIG_MD_SDRAM
		},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0}
	}
};

/* NorFlash timing and chip Config */
STATIC const IP_EMC_STATIC_CONFIG_T SST39VF320_config = {
	0,
	EMC_STATIC_CONFIG_MEM_WIDTH_16 |
	EMC_STATIC_CONFIG_CS_POL_ACTIVE_LOW |
	EMC_STATIC_CONFIG_BLS_HIGH /* |
							      EMC_CONFIG_BUFFER_ENABLE*/,

	EMC_NANOSECOND(0),
	EMC_NANOSECOND(35),
	EMC_NANOSECOND(70),
	EMC_NANOSECOND(70),
	EMC_NANOSECOND(40),
	EMC_CLOCK(4)
};

/* NandFlash timing and chip Config */
STATIC const IP_EMC_STATIC_CONFIG_T K9F1G_config = {
	1,
	EMC_STATIC_CONFIG_MEM_WIDTH_8 |
	EMC_STATIC_CONFIG_CS_POL_ACTIVE_LOW |
	EMC_STATIC_CONFIG_BLS_HIGH /* |
							      EMC_CONFIG_BUFFER_ENABLE*/,

	EMC_NANOSECOND(0),
	EMC_NANOSECOND(35),
	EMC_NANOSECOND(70),
	EMC_NANOSECOND(70),
	EMC_NANOSECOND(40),
	EMC_CLOCK(4)
};

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Setup system clocking */
void Board_SetupClocking(void)
{
	/* Enable PBOOST for CPU clock over 100MHz */
	Chip_SYSCTL_EnableBoost();

	Chip_SetupXtalClocking();
	
	/* SPIFI clocking will be derived from Main PLL with a divider of 2 (60MHz) */
	Chip_Clock_SetSPIFIClockDiv(2);
	Chip_Clock_SetSPIFIClockSource(SYSCTL_SPIFICLKSRC_MAINPLL);

}

/* Sets up system pin muxing */
void Board_SetupMuxing(void)
{
	int i, j;

	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_EMC);
	Chip_SYSCTL_PeriphReset(SYSCTL_RESET_IOCON);

	/* Setup data, address, and EMC control pins with high slew rate */
	for (i = 3; i <= 4; i++) {
		for (j = 0; j <= 31; j++) {
			Chip_IOCON_PinMuxSet(LPC_IOCON, (uint8_t) i, (uint8_t) j, (IOCON_FUNC1 | IOCON_FASTSLEW_EN));
		}
	}
	for (i = 16; i <= 31; i++) {
		Chip_IOCON_PinMuxSet(LPC_IOCON, 2, (uint8_t) i, (IOCON_FUNC1 | IOCON_FASTSLEW_EN));
	}

	/* Setup system level pin muxing */
	Chip_IOCON_SetPinMuxing(LPC_IOCON, pinmuxing, sizeof(pinmuxing) / sizeof(PINMUX_GRP_T));
}

/* Setup external memories */
void Board_SetupExtMemory(void)
{
	/* Setup EMC Delays */
	/* Move all clock delays together */
	LPC_SYSCTL->EMCDLYCTL = (CLK0_DELAY) | (CLK0_DELAY << 8) | (CLK0_DELAY << 16 | (CLK0_DELAY << 24));

	/* Setup EMC Clock Divider for divide by 2 */
	/* Setup EMC clock for a divider of 2 from CPU clock. Enable EMC clock for
	   external memory setup of DRAM. */
	Chip_Clock_SetEMCClockDiv(SYSCTL_EMC_DIV2);
	Chip_SYSCTL_PeriphReset(SYSCTL_RESET_EMC);

	/* Init EMC Controller -Enable-LE mode- clock ratio 1:1 */
	Chip_EMC_Init(1, 0, 0);

	/* Init EMC Dynamic Controller */
	Chip_EMC_Dynamic_Init((IP_EMC_DYN_CONFIG_T *) &IS42S32800D_config);

	/* Init EMC Static Controller CS0 */
	Chip_EMC_Static_Init((IP_EMC_STATIC_CONFIG_T *) &SST39VF320_config);

	
	/* Init EMC Static Controller CS1 */
	Chip_EMC_Static_Init((IP_EMC_STATIC_CONFIG_T *) &K9F1G_config);

	/* EMC Shift Control */
	LPC_SYSCTL->SCS |= 1;
}

/* Set up and initialize hardware prior to call to main */
void Board_SystemInit(void)
{
	Board_SetupMuxing();
	Board_SetupClocking();
	Board_SetupExtMemory();
}
