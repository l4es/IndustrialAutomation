This package contains all data required for using E1803 Easy Interface.
It can be used "as is" together with libe1803.h (and libe1803.lib for
Windows) or within the OpenSDK which is available at

http://www.openapc.com/download.php

or in GIT at

https://git.fedorahosted.org/cgit/OpenAPC.git/tree/

-----------------------------------------------------------------------

When using "as is" libe1803.h contains all required definitions and
headers.

When using with OpenSDK the directory libe1803 has to be put into SDK's
directory.

In both cases for proper usage and compilation one of the environment
switches ENV_WINDOWS (for all Windows operating systems) or ENV_LINUX
(for all Linux operating systems) has to be defined.

Details and description of E1803 Easy Interface can be found in
specification at https://halaser.eu/e1803_manual.pdf

Example imlementations and demo applications that demonstrate how to
use methods of programming E1803 can be found in GIT too.
