CLASSIC LADDER PROJECT
Copyright (C) 2001-2014 Marc Le Douarain
marc . le - douarain /AT\ laposte \DOT/ net
http://www.sourceforge.net/projects/classicladder
http://sites.google.com/site/classicladder
January 2014


Using and installing ClassicLadder (as it is, or nearly) on ClassicLadderPLC
or another real-time embedded target:
----------------------------------------------------------------------------

In Makefile, comment GTK_INTERFACE and set XENOMAI_SUPPORT
and adjust CROSS (depending processor architecture)

For ClassicLadderPLC target, in classicladder.h, uncomment lines HARD_LIFE_LED_*
and EMBEDDED_MOUNT_*
For another embedded target, adjust lines EMBEDDED_MOUNT_START_PARAMS_CMD
and EMBEDDED_MOUNT_END_PARAMS_CMD if your root system is read-only, and has to be
remounted in read/write, time to write some files (project, log, set vars list).

make clean
make
make dist_embedded

On a blank target, install the classicladder_embedded.tar.gz in "/usr/local/",
or else use the update monitor function in the Gtk interface.

Launch it with "/usr/local/classicladder/classicladder_run.script"



Using ClassicLadder "motor-engine" in another embedded project:
---------------------------------------------------------------
(for developpers, document not finished)


Packages required to recompile: zlib*-dev (zlib1g-dev)

An embedded motor engine can be compiled without the Gtk interface,
GTK_INTERFACE must be put in comment in your Makefile.
To do not include following extra features: events log, monitor protocol (when you want only rung/grafcet motor engine used)
COMPLETE_PLC must be put in comment in your Makefile.


List of files to link:
calc.o files.o files_project.o arithm_eval.o manager.o arrays.o symbols.o

In your files, add #include "classicladder.h"
...

* At start, init with calling this function:
	ClassicLadder_AllocAll()
  return 0 if error, and then call ClassicLadder_FreeAll()

* At end, call this function:
	ClassicLadder_FreeAll( TRUE/*CleanAndRemoveTmpDir*/ );

* In a periodic thread, call:
	ClassicLadder_RefreshAllSections();

* Variables access:
...

* Loading/Saving projects files:
...

