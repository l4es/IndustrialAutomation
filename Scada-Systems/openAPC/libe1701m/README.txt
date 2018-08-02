This package contains all data required for using E1701M binary API
interface. It can be used "as is" together with libe1701m.h (and
libe1701m.lib for Windows) or within the OpenSDK which is available
from http://www.openapc.com/download.php or in GIT at
https://git.fedorahosted.org/cgit/OpenAPC.git/tree .

When using "as is" libe1701m.h contains all required definitions
and headers.

When using with OpenSDK the directory libe1701m has to be put into
SDK's directory.

In both cases for proper usage and compilation one of the
environment switches ENV_WINDOWS (for all Windows operating systems)
or ENV_LINUX (for all Linux operating systems) has to be defined.

libe1701m_test contains a small example application that
demonstrates usage of E1701M binary API. This project requires
liboapc out of OpenSDK.

Details and description of E1701M Binary API can be found in
specification at http://www.halaser.eu/e1701m_manual.pdf
