Light OPC Server development library
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

OVERVIEW  [v0.888-0313]

The LightOPC provides easy way to implement full functional and fast
OPC servers (both v1 & v2 custom interfaces) on Win32 platform.

Examples of InProc (DLL) and out-of-proc (EXE) server are included
in this distribution.

The present version of LihgtOPC has following features:
	- bandwidth monitoring;
	- deadband calculations;
        - extensive logging;
	- application interface in pure "C";
        - BrowseAddresSpace is implemented;
        - complete DCOM compatibility;
        - both in-proc & out-of-proc servers supported;
        - "Free" & "Both" threading models.

The PublicGroups is left unimplemented yet.

Internal architecture of this server toolkit has several advantages and
provides noticeable high performance.
Data layout is optimized for better usage of CPU cache.
Threads synchronization designed in SysV style (including implementation of
read-write locks and conditions). In addition to minimal usage of OLE/COM
and Win32 API it provides high portability of source code.

************************************************************
 	DISTRIBUTION & COPYRIGHTS
************************************************************

The LightOPC is distributed under LGPL conditions (see COPYING.LIB.txt).

This toolkit has been designed by enthusiasts of the Lab43 team during
September - December 2000 as free and powerfull alternative to commercial
OPC development toolkits. First public version was released 8 March 2001.

See lightopc.h for copyrights & authors.

The latest version of this software can be found at:

	ftp://ftp.ipi.ac.ru/pub/LightOPC
	http://www.ipi.ac.ru/lab43/lopc-en.html
	http://www.ipi.ac.ru/lab43/lopc-ru.html


************************************************************
                USAGE
************************************************************

At the moment all instructions about usage of this toolkit are contained
in lightopc.h. Additional information can be obtained from the sample.

New information may be found at our web www.ipi.ac.ru/lab43.
See links above.

You may use binary distribution of LightOPC or build it from sources.
In any case you have to download and install several OPC stuff listed
in the BUILD section.

NOTE:  We provide unilog.def and lightopc.def files to simplify import
libraries generation. To make an import library you may simple run:
for MS Visual C++:
        lib /MACHINE:i386 /DEF:libname.def
for MINGW32/gcc:
        dlltool -d libname.def -l libname.a



************************************************************
                        BUILD
************************************************************

First, you have to download the OPC header files, GUID definitions and
install proxy-stub DLLs.
These files can be found at www.opcfoundation.org

[ You'll need at least following files:
 		opcerror.h
 		opccommn.h opccommn_i.c
		opcds.h opcda_i.c opcda_cats.c
 		opcproxy.dll opccommn_ps.dll
]

Currently we provide two Makefiles:
        makefile.MSVC - for MS Visual-C nmake;
        makefile.MINGW- for MINGW32/gcc and GNU make.

Just copy one of these files to "Makefile" and run your make
(or nmake.exe) utility.
In some environments you'll have to run vcvars32.bat (by VisualStudio) 
or adjust paths at the beginning of the makefile ( CCDIR and/or MSDIR ).

Additional information about MINGW32 might be found at www.mingw.org.

Build the unilog.dll first in the UNILOG directory.
~~~~~~~~~~~~~~~~~~~~~~~~~~
IMPORTANT: We have used the unilog.dll in several projects during
several years. In each new project the unilog has been improved up to
loosing compatibility with previous versions. Thus we recommend do not
put it in a commonly used directory. Rename it or place it closer to
your application instead.

Then build the lightopc.dll in LightOPC directory.
A few files from the unilog directory will be used.

NOTE: There are a few ".c" files in sources. These files can not be 
compiled in "C" mode under certain conditions: 
	- if gcc version is pre v3;
	- or the opcda.h is v3.00.
For these cases we've forced C++ compilation: 
-xc++ for gcc, and /TP for MS Visual C++.

At this step you'll obtain lightopc.lib (or libightopc.a), lightopc.def,
lightopc.dll and lightopc.h files. These files only and unilog.dll are
requred to build applications using LightOPC.

NOTE: Most important compile-time options are contained in the Makefile
      and options.h

NOTE: The LOPC library has extensive logging facility. Make sure you have
defined the USE_LOG macro to approprite value at compile time.

NOTE: There are UL_PUBLIC and LO_PUBLIC macro. For optimization reasons
you may define them to the "__declspec(dllimport)".
(Or left them undefined if you're compiling LOPC for the first time(s);).


************************************************************
                SAMPLES
************************************************************

Sample OPC server is contained in LightOPC/sample.cpp.
This is really quick-and-dirty program, but it is not very large.
On another hand the sample is showing several (though not all)
unique and exotique features of the LightOPC.

The only reason to use C++ in the sample is implementation of
IClassFactory. You are free to write your own programs in pure C.

Run "nmake test" to make samples LOPCSAMP.EXE and LOPCSAMD.DLL.
(note the difference in last characters ^^               ^^)

Before you run the samples make sure the unilog.dll and lightopc.dll
are copied in the same directoy as the samples contained.

lopcsamd.dll is self-registerable. Run
        regsvr32 lopcsampd.dll          - to register it;
        regsvr32 /u lopcsampd.dll       - to unregister.

lopcsamp.exe accepts following keys:
        /r   =  regiser
        /u   =  unregiser
        /daemon = run in daemon mode (i.e. never terminates).
Keys are case-sensitive and can not be specified together.

Registered object's names are OPC.LightOPC-exe and OPC.LightOPC-dll.



************************************************************
                LOGGING
************************************************************

At the first run of a sample or lightopc.dll loading the logging 
is initiated.

The logs are: 
LightOPC - default for lightopc.dll;
LOPC-dll - LightOPC redirected by lopcsamd.dll;
LOPC-exe - LightOPC redirected by lopcsamp.exe;
Lopc-Sample-Dll - internal of lopcsamd.dll;
Lopc-Sample-Exe - internal of lopcsamp.exe.

In the registery of your machine the following keys will be created:
HKEY_LOCAL_MACHINE\SOFTWARE\unilog.dll\LightOPC
HKEY_LOCAL_MACHINE\SOFTWARE\unilog.dll\LOPC-dll
HKEY_LOCAL_MACHINE\SOFTWARE\unilog.dll\LOPC-exe
HKEY_LOCAL_MACHINE\SOFTWARE\unilog.dll\Lopc-Sample-Dll
HKEY_LOCAL_MACHINE\SOFTWARE\unilog.dll\Lopc-Sample-Exe

Content of these keys does manage the logging rules.

The value 'level' does set the level; the 7 mean maximum verbosity.

The value 'path' does set the destination path. By default all messages
are directed to EventLog. It's unusable on Windows 95/98.
For redirecting log to a file simple put the filename in this value
(without initial '|'). Use fully qualified filenames with drive letter.
Or include reference to a global path like %TMP% or %SystemRoot%.

The value 'size' does limit of a log file (in kilobytes).
The special values: -1 == unlimited;
		     0 == truncate on each run.


The highest logging level can be restricted at compile time 
by USE_LOG macro. 
It seems similar to: -DUSE_LOG=7 or -DUSE_LOG=ll_DEBUG,
or even -DUSE_LOG=-1 to remove the logging completely.

The following keys are created too:

HKLM\SYSTEM\CurrentControlSet\Services\EventLog\Application\LightOPC
HKLM\SYSTEM\CurrentControlSet\Services\EventLog\Application\LOPC-dll
HKLM\SYSTEM\CurrentControlSet\Services\EventLog\Application\LOPC-exe
HKLM\SYSTEM\CurrentControlSet\Services\EventLog\Application\Lopc-Sample-dll
HKLM\SYSTEM\CurrentControlSet\Services\EventLog\Application\Lopc-Sample-exe
(HKLM = HKEY_LOCAL_MACHINE)

These keys should point to actual location of unilog.dll and they're 
used by EventViewer to format messages.

If EventViewer does show lightopc' messages misformatted there may be
two reasons:
1)   unilog.dll has been moved to another location.
2)   these keys may be protected by NT/W2k security and unilog.dll was
     unable to create/update them without administrative rigths.

Simply remove these keys or write there right location of unilog.dll
manually.

See unilog.h for more details about logging.


************************************************************
FEEDBACK:

		master@ipi.ac.ru
************************************************************

