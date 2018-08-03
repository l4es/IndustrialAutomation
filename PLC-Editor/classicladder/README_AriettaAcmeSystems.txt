CLASSIC LADDER PROJECT
Copyright (C) 2001-2018 Marc Le Douarain
marc . le - douarain /AT\ laposte \DOT/ net
http://www.sourceforge.net/projects/classicladder
http://sites.google.com/site/classicladder
December 2015


Some tips here if you want to compile and use ClassicLadder "embbeded" for the Arietta card (AcmeSystems)
and with access to its GPIO (using wiringSam library directly included with ClassicLadder sources).

Two versions can be obtained, a classic and a real-time with Xenomai3.
WITH THE CLASSIC ONE YOU WILL HAVE A PLC BUT NOT IN "REAL-TIME", do not forget about it, could be important for some usages...
This document doesn't explain how to compil and install Xenomai on the Arietta...

Inputs are configured with "pull-up" enabled, so you have to put them to GND to change their state.
Per-default, 2 GPIOs (numbers PA22 & PA21) are used as outputs for respectively life led + user led %QLED0.
If you want to use others GPIOs, see below to adjust "target_embedded_plc_arietta.h" file.


First of course, download Classicladder (Linux sources) archive,
and then in a console terminal:

"tar zxvf classicladderxxxx.tar.gz"


Some files in sub-directory "src" of "classicladder" directory have to been edited before compilation:

- In "Makefile",
put in comment line:
# GTK_INTERFACE = 1
also:
# MAKE_X86_IO_ACCESS = 1
uncomment line:
MAKE_ATMEL_SAM_GPIO_ACCESS = 1
and put in comment lines:
# OWN_CFLAGS = -march=i486
# OWN_LIBS = -march=i486
If cross-compiled, uncomment:
CROSS = arm-linux-gnueabi-
Optional, to run on a real-time Xenomai, uncomment line (sorry at the top!):
XENOMAI_SUPPORT = 1

- In "target_embedded_plc_arietta.h",
as explained before, adjust GPIOs outputs used for life and user leds at line:
#define HARD_LIFE_USERS_LEDS_PORTS { WSAM_PIO_A ,WSAM_PIO_A };
#define HARD_LIFE_USERS_LEDS_GPIO { 21, 22 };
if your root partition isn't mounted in "read-only", comment the 2 lines EMBEDDED_MOUNT_***

go back in the console:
"cd classicladder"
"cd src"
"make clean"
"make"

for more than a fast try (see below), you can also generate an emmbedded update file to upload to a target, with :
"make dist_embedded"

If zlib needed for cross-compilation see at the bottom what I've used...

"sudo mkdir /usr/local/classicladder"

For a fast try,
"cd .." 
"sudo ./classicladder"
You should see the life led flashing (on output GPIO PA21 if not modified, and if you have connected a led+resistor on it)...
then you are ready to upload a project in it from network !


A little example project is available "AriettaExample.clprj".
In this project (if not changed) :
- input GPIO PC17 is mapped to %I1 (Arietta Push Button on board),
- input GPIO PC0 is mapped to %I2
- output GPIO PB14 is mapped to %Q1
- output GPIO PB13 is mapped to %Q2
(see in config window, tabs 'Physical inputs' and 'Physical outputs')
(PortAdr/SubDev colummn = Arietta GPIO port selected: 0=PortA / 1=PortB / 2=PortC / 3=PortD)

After, you should perhaps copy classicladder binary executable and scripts to /usr/local/classicladder with
"sudo cp classicladder* /usr/local/classicladder"
and then do the necessary so that at startup it is running automatically
(you can add /usr/local/classicladder/classicladder_run.script to /etc/rc.local)
An '/etc/classicladder_prefs' should be copied to be able to adjust some parameters
(daemonize, project to load at startup saved, run/stop state saved, serial monitor, ...)


Cross-compilation of zlib library and install in "arm-linux-gnueabi" directory :
--------------------------------------------------------------------------------
- tar zxvf zlib-1.2.8.tar.gz
- cd zlib-1.2.8
- export CHOST=arm-linux-gnueabi
- ./configure --prefix=/usr/arm-linux-gnueabi
- make
- sudo make install
Then to install on target, copy the libz.so fil into /lib/arm-linux-gnueabi and then :
cd /lib/arm-linux-gnueabi and  ls -sf libz.so.1 libz.so


See README.txt for others informations...


Marc.
