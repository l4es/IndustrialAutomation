
         PLCEdit - open project for a free PLC source code editor -

         (c) 2005-2015 M. Rehfeldt <info-at-plcedit.org>




System Requirements
~~~~~~~~~~~~~~~~~~~

PLCEdit has been developed with a platform independent development environment. 
PLCEdit works on Microsoft Windows, Linux and Mac OS 10.4+ (ppc/i386). 

min. system requirements:
-------------------------
Computer: x86 or ppc compatible processor from 466 MHz
RAM	: 256 MB
Harddisc: 30 MB
Operating System: MS Windows(R) 98/NT4.0SP4a, Mac OS 10.4, Linux with X11

recommended system requirements:
--------------------------------
Computer: x86 or ppc compatible processor from 1.6 GHz
RAM	: 1024 MB
Harddisc: 100 MB
Operating System: MS Windows(R) 7, Mac OS 10.6+, Ubuntu 9.04 with X11



Installation
~~~~~~~~~~~~

The download is available from http://www.plcedit.org, the official website.
PLCEdit is distributed as Windows executable, Mac OS X application and
as binary package + source for most Linux distributions.
 
An other options to get PLCEdit running is to compile it on your platform.
You need Qt (from Nokia/QtSoftware) at least version 4.5.x to compile PLCEdit.
The latest Qt package is available for Windows, Linux and Mac at the official 
website http://www.qtsoftware.com/.
PLCEdit requests no special configurations of Qt, but it could be useful to 
configure Qt as release static, eg. to get PLCEdit portable.

How to compile PLCEdit:
0. Prerequisites: Qt 4.X, a compiler distribution: MinGW, GCC, etc.
   Linux and Mac OS X contains already a compiler. For Windows you can 
   use the free MinGW collection, available at: http://www.mingw.org/.
1. Extract the source from the package.
2. `cd` to the directory containing the package's source files and type
   `qmake PLCEdit.pro` on Windows and Linux systems or type on Mac OS X
   `qmake -spec macx-xcode PLCEdit.pro' to generate the Makefile.
3. Type `make` on Linux and Mac OS X or type `mingw32-make` on Windows 
   to compile the package. Mac OS users maybe need to open the generated 
   XCode file and run XCode (if qmake PLCEdit.pro was entered). 
   (PLCEdit's devlopment IDE is QDevelop.)
4. The executable file 'PLCEdit' is built in the directory 'release/'.
5. On Linux and Windows copy the folders 'Lang' and 'Help' into the folder 
   release (or PLCEdit's application directory). 
   On Mac OS X rightclick on the application file in the folder release, 
   select "show package content", open the folder Contents, next open the 
   folder MacOS and put in the folders 'Lang' and 'Help'.
   NOTICE: If PLCEdit cannot found this folders in its application directory 
   you will miss the translations and the help.
6. PLCEdit uses also a folder Sessions, which will be created at start up,
   if it does not exist. If you have already used PLCEdit and the session 
   function repeat the copy procedure from paragraph 5. for the sessions).
 


Documentation
~~~~~~~~~~~~~

Up to date documentation can be found at: http://www.plcedit.org
The help files are located in the folder Help in the application directory: /Help/index.html
The source code documentation can be found at the directory: /Docs/html/index.html



License
~~~~~~~~~~~~~

Copyright (C) 2005-2010 M. Rehfeldt <info-at-plcedit.org>

PLCEdit is free software. You can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 2 of the License, or (at your option) any later
version. The license file (license.GPL) is delivered within this package.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; WITHOUT even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
