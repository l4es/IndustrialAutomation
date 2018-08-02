This package contains all data required for using E1701 Easy Interface.
It can be used "as is" together with libe1701.h (and libe1701.lib for
Windows) or within the OpenSDK which is available at

http://www.openapc.com/download.php

or in GIT at

https://sourceforge.net/p/oapc/code/ci/master/tree/

-----------------------------------------------------------------------

When using "as is" libe1701.h contains all required definitions and
headers.

When using with OpenSDK the directory libe1701 has to be put into SDK's
directory.

In both cases for proper usage and compilation one of the environment
switches ENV_WINDOWS (for all Windows operating systems) or ENV_LINUX
(for all Linux operating systems) has to be defined.

Details and description of E1701 Easy Interface can be found in
specification at https://halaser.eu/e1701_manual.pdf

Example imlementations and demo applications that demonstrate how to
use methods of programming E1701 (via programming commands, via
programming interface,...) from within different languages (C/C++,
C#,...) can be found in GIT too.
