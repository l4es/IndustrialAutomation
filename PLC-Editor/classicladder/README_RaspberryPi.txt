CLASSIC LADDER PROJECT
Copyright (C) 2001-2015 Marc Le Douarain
marc . le - douarain /AT\ laposte \DOT/ net
http://www.sourceforge.net/projects/classicladder
http://sites.google.com/site/classicladder
September 2014


Note that an early ClassicLadder version modified for RaspberryPi has been done by Heli Tejedor
(see http://heli.xbot.es/?p=43), different without using WiringPi library.


Some tips here if you want to compile and use ClassicLadder "embbeded" for the RaspberryPi card
and with access to its GPIO (using external wiringPi library).
It does not explain how to compile a real-time version with Xenomai (not tested for now by myself).
SO YOU WILL HAVE A PLC BUT NOT IN "REAL-TIME", do not forget about it, could be important for some usages...

Inputs are configured with "pull-up" enabled, so you have to put them to GND to change their state.
Per-default, 2 GPIOs (numbers 27 & 17) are used as outputs for life led + user led %QLED0.
If you want to use others GPIOs, see below to adjust "target_embedded_raspberrypi.h" file.


First of course, download WiringPi (http://wiringpi.com/) and Classicladder (Linux sources) archive,
and then in a console terminal:

"tar zxvf wiringPixxxxxx.tar.gz"
"cd wiringPixxxxxxxx"
"./build"

"tar zxvf classicladderxxxx.tar.gz"

Some files in sub-directory "src" of "classicladder" directory have to been edited before compilation:
- In "Makefile",
put in comment line:
# GTK_INTERFACE = 1
also:
# MAKE_X86_IO_ACCESS = 1
uncomment line:
MAKE_RASPBERRYPI_GPIO_ACCESS = 1
and put in comment lines:
# OWN_CFLAGS = -march=i486
# OWN_LIBS = -march=i486

- In "target_embedded_raspberrypi.h",
as explained before, adjust GPIOs outputs used for life and user leds at line:
#define HARD_LIFE_USERS_LEDS_GPIO { 27, 17 };

go back in the console:
"cd classicladder"
"cd src"
"make clean"
"make"

"sudo mkdir /usr/local/classicladder"

For a fast try,
"cd .." 
"sudo ./classicladder"
You should see the life led flashing (on output GPIO27 if not modified, and if you have connected a led+resistor on it)...
then you are ready to upload a project in it from network !


A little example project is available "RaspberryPiExample.clprj".
In this project (if not changed) :
- input GPIO11 is mapped to %I1,
- input GPIO9 is mapped to %I2
- output GPIO25 is mapped to %Q1
- output GPIO24 is mapped to %Q2
(see in config window, tabs 'Physical inputs' and 'Physical outputs')


After, you should perhaps copy classicladder binary executable to /usr/local/classicladder with
"sudo cp classicladder /usr/local/classicladder"
and then do the necessary so that at startup it is running automatically...
An '/etc/classicladder_prefs' should be copied to be able to adjust some parameters
(daemonize, project to load at startup saved, run/stop state saved, serial monitor, ...)


See README.txt for others informations...


Marc.
