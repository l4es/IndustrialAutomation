CLASSIC LADDER PROJECT
Copyright (C) 2001-2018 Marc Le Douarain
marc . le - douarain /AT\ laposte \DOT/ net
http://www.sourceforge.net/projects/classicladder
http://sites.google.com/site/classicladder
February 2001

Version 0.9.112 (6 April 2018)
------------------------------

A project to have a free ladder language in C.
Generally, you find this type of language on PLC to make the programs.
It allows you to realize little programs or bigger in an electric way.

This project is released under the terms of the LGPL license v3, and
LGPL v2 for EMC project usage.


GENESIS...
----------

I decided to program a ladder language only for test purposes at the start,
in february 2001. It was planned, that I would have to participate to a
new controller at work. And I was thinking that to have a ladder
language in it could be a nice option to considerate. And so I started to
code the first lines for calculating a rung with minimal elements and
displaying dynamically it under Gtk, to see if my first idea to realise all
this works.
And as quickly I've found that it advanced quite well, I've continued with
more complex elements : timer, multiples rungs, etc...
Voilà, here is this work...
And for the controller at job? Well, in fact, it's already a lot of work to
do the basic features of the traffic lights and I don't think we will add
a ladder language to it one day... ;-)


FEATURES...
-----------

Classic Ladder is coded 100% in C.
It can be used for educational purposes or anything you want...
The graphical user interface uses GTK.

In the actual version, the following elements are implemented :

   * Booleans elements
   * Rising / falling edges
   * New IEC Timers (since v0.7.120)
   * Timers (for compatibility, no more really usefull)
   * Monostables (for compatibility, no more really usefull)
   * Counters (since v0.7.80)
   * Compare of arithmetic expressions (since v0.4)

   * Booleans outputs
   * Set / Reset coils
   * Jumps
   * Calls to sub-routines (since v0.5.5)
   * Operate of arithmetic expressions (since v0.4)

Each rung can have a label (used for jumps) and a comment (displayed at the top).
They can be edited for the current rung, in the two edits at the top of the window.

There is a full editor for rungs since v0.3
You can:
    - add a rung after the current rung.
    - insert a rung before the current rung.
    - modify the current rung.
    - delete the current rung.
When editing, use the pointer of the toolbar if you want only to modify properties, else select the element
you want to place in the rung.

A popup menu (since v0.9.7) with the right click is available to edit without having to use the toolbar window.

Many rungs can be displayed at the same time, and you can use a vertical scollbar.
The current rung is the rung with the blue bars on each side. When window resized, it is automatically
selected by choosing the toppest rung which is completely visible. You can manually select one between the
others below (if you can see them with a large vertical window).


Since v0.5.5, the program can be split in many sections.
There is a window 'Sections manager' where you can see all the sections defined, add/delete a section,
and modify the order executions of the main sections.
There is two types of sections available : main or sub-routine (SR).
The main sections are refreshed directly in the following order : top sections first, bottom sections last.
The sub-routines sections are refreshed when called from a rung. Each sub-routine (SR) has a unique number
used in the (C)all coils.
Load & Run "example_many_sections" for a demo example.

Since v0.6.0, a new language has been integrated : a "sequential" also called "grafcet".
In the section manager, select the language wanted when you add a section.
Load & Run "example_sequential" for a demo example.
The editor for sequential is arrived in v0.7.0
When using the link button ('\.) in the toolbar, when clicking on transition, the top or
bottom part of it is very important !!!

Since v0.8.4, part copy of a rung possible in two steps: selection of a part in the current rung,
then copy part selected in memory somewhere in the current rung (another one possible).

Since v0.8.5, events logs available (with full events configuration of the assiocated variables the
user want to log). Each events can be configured as defaults.

Since v0.9.0, monitor target possible in IP (UDP) or Serial with rungs activity, sequential activity, read free/bool vars.
See new menu PLC/Connect. You have to open the same file project in local than the project running on the target
(you have to select the correct file, no control is done, if none you will debug bad elements and will do not
understand anything!!!).

Since v0.9.4, some projects infos (name, author) can be entered (in a tab of the free vars window).

Since v0.9.5, project can be send or received to/from another classicladder (via monitor protocol). Usefull for an
embedded target!

Since v0.9.11, a PSTN/GSM modem can be connected to be used for monitor protocol ! + can run just for one cycle.

ClassicLadder can run in real-time with RTLinux, RTAI or Xenomai (optional). See below.


REQUIREMENTS...
---------------
For using Classic Ladder as it is, you need :

   * Linux
   * Gtk version 2.10 or higher (Cairo used since v0.8.0)
   
A precompiled Linux binary executable is included in the archive that you can directly lanuch.
If required, you can recompile yourself the project, see below...


COMPILATION...
--------------
To recompile the sources, in a console :
type "cd src;make clean;make;cd.."
type "./classicladder" to launch this project.
or "./classicladder xxxxxxxxxx.clprj" to start with using project datas "xxxxxxxxxx.clprj".

Ensure you've the package "libgtk2.0-dev" installed before to compile
(package name on my Ubuntu9.04, perhaps another name on some distributions?).
Also you need the gcc compiler installed, see package "build-essential" if needed !
If you obtain an error: ‘GDK_KEY_Alt_L’ undeclared (first use in this function)
that means that you have old includes, you can copy files from "gdk_includes_update/"
to your "/usr/include/gtk-2.0/gdk/" or better: update your distribution...

To compile on PowerPC processor, in the Makefile, add the caracter '#' to comment
the line MAKE_IO_ACCESS.


GTK3 BUILD...
-------------
Included precompiled Linux binary executable compiled with GTK+2, but since v0.9.100 now available for GTK+3 to
be manually compiled:
install package "libgtk-3-dev" if needed, and then modify in Makefile: comment lines after "#Default GTK2 Version"
and uncomment lines after "#For GTK3 Version")


MODBUS SERVER INCLUDED (SLAVE TO CONNECT TO A SCADA)...
-------------------------------------------------------
ClassicLadder has a Modbus/TCP server integrated. Default port is 9502, standard
502 requires that the application has been launched with root privileges.
List of functions code supported are: 1, 2, 3, 4, 5, 6, 15 and 16.
Modbus bits and words correspondence table is actually not parametrable and correspond
directly to the %B and %W variables.
Infos on modbus protocol are available here:
http://www.modbus.org
http://www.sourceforge.net/projects/jamod
http://www.modicon.com/techpubs/toc7.html


LOGS / REMOTE ALARMS...
-----------------------
First define logs corresponding to vars to log: first var %Bxxxx / nbr vars (minimum = 1!!!!) for each log event.
When an event is created, in case of many vars, parameter value = offset from first var %Bxxxx.
Remote alarms (8 slots) can be defined for each log event.
Each remote alarm slot corresponds to a SMS or email destination.
For email, external command "mailsend" required in /usr/bin (https://code.google.com/p/mailsend/)
For SMS send, a Wavecom chip modem has been used an successfully tested. Do not forget to define center SMS server
(depending of GSM phone operator used).


EMBEDDED VERSION...
-------------------
If you want to make an embedded version of classicladder without the GTK interface:
comment the GTK_INTERFACE line in the Makefile + see CROSS & OWN_CFLAGS & OWN_LIBSFLAGS to
adjust to the correct processor architecture of the target if required.
Type 'CTRL-C' if you want to exit the Linux application, or send kill signal (if daemonized,
only kill signal can be used).

Since v0.6.4 it is possible to define maximum sizes for the differents arrays to save
a lot of memory if you're short on it on your target.
You must give a config file at startup... Well, no since v0.7.110, the sizes defined
are saved in the project file, and are used for the first project file loaded (in the
commande line). Since v0.9.2, the project file loaded at startup can also be save in
the preferences file saved in your home directory (from the gtk config window), or 
in priority from "classicladder_prefs" near the executable or "/etc/classicladder_prefs"
(if already available there).


REAL-TIME SUPPORT WITH XENOMAI 2 OR 3 (IN USER-SPACE)...
--------------------------------------------------------
Added in v0.7.92, september 2006.
Xenomai version 3 requires ClassicLadder v0.9.030 minimum
See http://www.xenomai.org
xeno-config command must be in your path.
Uncomment the corresponding line in the Makefile.
Then type "cd src;make clean;make;cd.." to compile the project.
With a "./classicladder" you will launch the real-time version !!!
Xenomai rules! And is advised instead of a RTLinux/RTAI kernel module version.
To have real-time in user-land is really easier than with the module, the future
is here, and now !!!


REAL-TIME SUPPORT WITH RTLINUX...
---------------------------------
(no more maintained...)
To have RTLinux v3 installed before is required (see http://www.rtlinux.org)
Be sure that the hello example works after RTLinux installation (perhaps
you will have to type dmesg to verify if the hello texts was correctly displayed).
With this version, the refresh of the rungs is done in real-time.
Verify the symbolic link (or real directory depending how you've installed)
"/usr/rtlinux" pointing on the rtlinux directory, exists.
Here, I've done the following :
cd /usr
ln -sf /usr/src/your-rtlinux+linux-dir/rtlinux-3.1 rtlinux


In a console, type the following to recompile and run :
make clean;make rtl_support
su
./run_rt

run_rt script accepts two optional arguments : first for the name of project to load,
second for a config file with sizes to alloc at startup -config file is now obsolete, new
test needed-

I've tested here the real-time version with RTLinux v3.2pre3 and Linux kernel v2.4.20
compiled with gcc v3.2


REAL-TIME SUPPORT WITH RTAI...
------------------------------
(no more maintained...)
Verify the symbolic link (or real directory depending how you've installed)
"/usr/src/rtai" pointing on the rtai directory, exists.
Here, I've done the following :
ln -sf /usr/src/your-rtai  /usr/src/rtai

In a console, type the following to recompile and run :
make clean
make rtai_support
su
./run_rt

run_rt script accepts two optional arguments : first for the name of project to load,
second for a config file with sizes to alloc at startup.

Tested with 2.4.22 kernel patched with ADEOS and compiled with gcc-3.2 running
on a Debian (Sid) system.


HARDWARE (LOCAL INPUTS/OUTPUTS)...
----------------------------------
Since v0.6.5, hardware interface has been completely rewritten (before limited
to the parallels ports only).
Firstly, if you use ClassicLadder to drive real things, it can crash, and I
will not be responsible if it produces any damages !

You can configure any logical inputs/ouputs with any addresses ports (x86
architecture) or a Comedi device (see www.comedi.org). Comedi is a collection
of drivers for many data acquisition boards.
You configure the mapping in the config window : tabs "Inputs" and "Outputs".
In each line, you set the first logical %I/%Q that will be mapped with the
hardware you will set.
For a direct port access, you then give the address in hexa to use.
For a comedi device, you then give the sub-device associated.
After, you set the first channel offset and the number of channels
to map.
For example : %I5 mapped with 4 channels means that %I5, %I6, %I7 and %I8
are affected to an hardware channel. So be carrefull not to overlap.
For some GPIOs on which direction must be configured, you can use "Config" to send
a config data value to a config port (v0.9.4).

Comedi or direct access can works with Linux version or RTLinux version.
For direct access under Linux, the application must be launched as
root, as it used the ioperm( ) functions.

Per default, Comedi support is not compiled. Take a look at the Makefile
and uncomment the COMEDI_SUPPORT line.
Comedi is included since RLinux3.2pre2, but you still must download
the comedilib archive (comedi_config) !!!

Here I've made a test with the builtin parallel port.
You will find the 2 projects : 'parallel_port_direct' and
'parallel_port_comedi'.

Here the pins and I/O map for the direct project:

* -- Parallel port 1 --
Inputs     DB25Pin     ClassicLadder
S3         15          I3
S4         13          I4
S5         12          I5
S6         10          I6
S7         11          I7
Outputs    DB25Pin     Classicladder
D0         2           Q0
D1         3           Q1
D2         4           Q2
D3         5           Q3
D4         6           Q4
D5         7           Q5
D6         8           Q6
D7         9           Q7

I've tested with 3 DELs and 4 switches on the parallel port 1.
The switches on inputs are linked to ground (pin 24) :
           _n_
   (Sx) o--   --O (24)

The DELs are linked in serial with a resistance of 330 ohms to ground (pin 25) :
                      ,
   (Dx) o--[ 330 ]--|>|--o (25)


HARDWARE (DISTRIBUTED INPUTS/OUTPUTS)...
----------------------------------------
ClassicLadder can use distributed inputs/outputs on modules using the modbus
protocol ("master": pooling slaves).
The slaves and theirs I/O can be configured in the config window.
2 exclusive modes are available : ethernet using Modbus/TCP and serial using Modbus/RTU.
For the serial one, the serial port and the speed must be correctly fixed, and are used
only at startup of classicladder (started in argument with that projectfile). No parity is used.
If no port name for serial is set, IP mode will be used...
The slave address is the slave address (Modbus/RTU) or the IP address.
The IP address can be followed per the port number to use (xx.xx.xx.xx:pppp) else
the port 502 will be used per default.
Per default, the modbus address element 1 is the first one (0 in the frame). Offset that
can be parametred.
2 products have been used for tests: a Modbus/TCP one (Adam-6051, http://www.advantech.com)
and a serial Modbus/RTU one (http://www.ipac.ws)
See examples: adam-6051 and modbus_rtu_serial.
Web links: http://www.modbus.org and this interesting one: http://www.iatips.com/modbus.html


LINKS...
--------
You should take a look at the MAT project. Classicladder has been integrated to this project to
have a relay ladder logic to program this PLC.
http://mat.sourceforge.net/

Another project using ClassicLadder: EMC2, used to control machine tools.
http://www.linuxcnc.org/

Comedi project at www.comedi.org


KNOWN LIMITATIONS /  BUGS...
----------------------------
* Modbus master for distributed I/O, should be use with caution...
  Generally, the logic used on a PLC is: get inputs, apply logic (rungs/grafcet), and then
  set ouputs, but it is absolutely not the case with distributed I/O. Perhaps it could cause
  some troubles...? Moreover, not running in a real-time thread !
* RTLinux/RTAI will no more work since v0.9.015 (objects memory allocation of project datas after
  project size configuration loaded, done each time and not only at startup for first project loaded)
* Registers blocks are a really new function (since v0.9.022), and has not been deeply tested !


FUTURE / TO DO LIST...
----------------------
see file TODO.txt

