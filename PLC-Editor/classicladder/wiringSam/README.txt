
wiringSam is a little library to easily access to GPIOs pins of an Atmel AT91SAM like on "Arduino" cards.
It is written in C, using direct memory access with mmap().
Version 0.1 - 28 August 2015
Copyright (C) 2015 Marc Le Douarain.


To use it, just add the 2 files : wiringSam.h and wiringSam.c to your files project.

Then in your code,
add #include "wiringSam.h"


And here are the functions list available :

int wiringSamSetup( void ) => should be called first
void wiringSamEnd( void ) => to call at end of your program (optional)

void pinMode (int controller, int pin, int mode) => select pin between INPUT/OUTPUT
void pullUpDnControl (int controller, int pin, int pud) => set pull-up (PUD_UP) / pull-down (PUD_DOWN) / none (PUD_OFF) on the input pin
int digitalRead (int controller, int pin) => give state of the input pin: LOW/HIGH
void digitalWrite (int controller, int pin, int value) => set state of the output pin: LOW/HIGH

in previous fonctions, controller =  WSAM_PIO_A / WSAM_PIO_B / WSAM_PIO_C/ WSAM_PIO_D to select A/B/C/D controller PIO ; pin=0 to 31.

void delay( unsigned int howLong )
void delayMicroseconds( unsigned int howLong )
unsigned int millis( void )
unsigned int micros( void )


See little example code included "test_at91sam_gpio.c".


Marc.

