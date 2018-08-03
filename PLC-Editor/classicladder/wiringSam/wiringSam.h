/* 
   wiringSam : a library to easily access to GPIOs pins of an Atmel AT91SAM
   like on "Arduino" cards
   Copyright (C) 2015 Marc Le Douarain
   Times functions taken directly from wiringPi library by Gordon Henderson. 
   
   This library is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser  General Public License for more details.

   You should have received a copy of the GNU Lesser  General Public License
   along with this library.  If not, see <http://www.gnu.org/licenses/>
   
   wiringSam v0.1 - 28 august 2015
*/

#ifndef	__WIRING_SAM_H__
#define	__WIRING_SAM_H__

#include <stdint.h>

// Controller ports list

#define WSAM_PIO_A 0
#define WSAM_PIO_B 1
#define WSAM_PIO_C 2
#define WSAM_PIO_D 3
#define WSAM_NBR_PIOS 4

// Pin modes

#define	INPUT			 0
#define	OUTPUT			 1

#define	LOW			 0
#define	HIGH			 1

// Pull up/down/none

#define	PUD_OFF			 0
#define	PUD_DOWN		 1
#define	PUD_UP			 2

typedef struct StructSamGpio
{
	volatile uint32_t EnableRegister;
	volatile uint32_t DisableRegister;
	volatile uint32_t StatusRegister;
	volatile uint32_t Reserved0;
	
	volatile uint32_t OutputEnableRegister;
	volatile uint32_t OutputDisableRegister;
	volatile uint32_t OutputStatusRegister;
	volatile uint32_t Reserved1;
	
	volatile uint32_t GlitchInputFilterEnableRegister;
	volatile uint32_t GlitchInputFilterDisableRegister;
	volatile uint32_t GlitchInputFilterStatusRegister;
	volatile uint32_t Reserved2;
	
	volatile uint32_t SetOutputDataRegister;
	volatile uint32_t ClearOutputDataRegister;
	volatile uint32_t OutputDataStatusRegister;
	volatile uint32_t PinDataStatusRegister;
	
	volatile uint32_t InterruptEnableRegister;
	volatile uint32_t InterruptDisableRegister;
	volatile uint32_t InterruptMaskRegister;
	volatile uint32_t InterruptStatusRegister;
	
	volatile uint32_t MultidriverEnableRegister;
	volatile uint32_t MultidriverDisableRegister;
	volatile uint32_t MultidriverStatusRegister;
	volatile uint32_t Reserved3;
	
	volatile uint32_t PullupDisableRegister;
	volatile uint32_t PullupEnableRegister;
	volatile uint32_t PullupStatusRegister;
	volatile uint32_t Reserved4;
	
	volatile uint32_t PeripheralSelectRegister1;
	volatile uint32_t PeripheralSelectRegister2;
	volatile uint32_t Reserved5;
	volatile uint32_t Reserved6;
	
	volatile uint32_t InputFilterSlowClockDisableRegister;
	volatile uint32_t InputFilterSlowClockEnableRegister;
	volatile uint32_t InputFilterSlowClockStatusRegister;
	volatile uint32_t SlowClockDividerDebouncingRegister;
	
	volatile uint32_t PadPulldownDisableRegister;
	volatile uint32_t PadPulldownEnableRegister;
	volatile uint32_t PadPulldownStatusRegister;
	volatile uint32_t Reserved7;
	
	volatile uint32_t OutputWriteEnable;
	volatile uint32_t OutputWriteDisable;
	volatile uint32_t OutputWriteStatus;
	volatile uint32_t Reserved8;
	
	/* ... */
	
}StructSamGpio;

#ifdef __cplusplus
extern "C"
{
#endif

extern int wiringSamSetup( void );
extern void wiringSamEnd( void );

extern void pinMode (int controller, int pin, int mode);
extern void pullUpDnControl (int controller, int pin, int pud);
extern int digitalRead (int controller, int pin);
extern void digitalWrite (int controller, int pin, int value);

extern void delay( unsigned int howLong );
extern void delayMicroseconds( unsigned int howLong );
extern unsigned int millis( void );
extern unsigned int micros( void );

#ifdef __cplusplus
}
#endif

#endif
