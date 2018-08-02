# OPC Foundation UA ANSI C Stack

The OPC Foundation has formally released the OPC Unified Architecture ANSI C Stack and Sample Code to the community.

Please review official site page (http://opcfoundation.github.io/UA-AnsiC/) for:
 * Overview
 * Licensing
 * Sample Applications overview

## Contributing

We strongly encourage community participation and contribution to this project. First, please fork the repository and commit your changes there. Once happy with your changes you can generate a 'pull request'.

You must agree to the contributor license agreement before we can accept your changes. The CLA and "I AGREE" button is automatically displayed when you perform the pull request. You can preview CLA [here](https://opcfoundation.org/license/cla/ContributorLicenseAgreementv1.0.pdf).

OPC UA, empowering the Industrial Internet of Things (IIOT) and Industrie 4.0.

## Runtime Dependencies

OpenSSL v1.0.2j was used in development and testing of this stack and is required for the Crypto and PKI implementations, and must be separately installed on the build machine at specific locations. 
If you need to use a different version (because of bug fixes or availability in your system) you may have to update the implementation particularly if the OpenSSL API has changed.

OpenSSL has several algorithms which are patented and have import/export restrictions to some states, therefore please download a copy from the project website(http://www.openssl.org/source). 
Please consult the OpenSSL documentation for help building the library.

## Building the Stack

### Windows

Open the Visual Studio Command Shell .
Make sure that perl installed on the build machine and the path executable is available in the path.  Any perl will do, even cygwin.
Download and extract the latest openssl-1.0.2 source tar ball to the stack root folder.
Then cd to the root folder and execute build_win32.bat or build_win64.bat depending on your target architecture.
This will automatically build openssl, the OPC UA Stack and the sample server.
Dependencies are not supported, it will always be a full build.

Visual studio 2013 projects for the stack and example server are available as well. 
Note: the OpenSSL libraries must be compiled using the above mentioned steps before building the project using Visual Studio.

### Linux

Open a terminal window.
Make sure you have the libssl-dev package installed from your distribution.
Then cd to the root folder and execute: ./build_linux.sh
This builds both debug and release binaries.
Dependencies and incremental builds are supported.
To force a full build use: ./build_linux.sh clean all

This is an example how to build with non-standard compiler options:
./build_linux.sh CROSS_COMPILE=arm-linux-gnueabi- MACHINE_TYPE=arm MACHINE_OPT="-g -fsanitize=address -fno-omit-frame-pointer"

### CMake

Alternatively, the stack can be built using cmake on both Windows and Linux. 
To do so, install cmake and openssl, then change to the root of the repo and run 
```
mkdir build
cd build
cmake .. 
cmake --build .
```

## Examples

An OPC UA Nano Embedded server example implementation is provided by the stack.
Note: there is a sample AnsiCServer but it is included as-is and not fully supported, the community is welcome to extend this example application.

## Notes for developers

The stack consists of two main components:
	- the platform-independent core component
	- the platform layer component.
The core stack configuration is "core/opcua_configuration.h".
The configuration of the platform layer depends on the implementation. The win32
platform layer (included) is configured in "platforms/win32/opcua_p_interface.h"
and "platforms/win32/opcua_platformdefs.h".
Detailed information exists at the top of these files.
All settings are described within the source code.

- OPC UA 1.03 compatible.
- windows and linux layers work with 32 and 64 bit O/S.
- windows and linux implement full IPv6 support.
- pki store implementation reworked to be more flexible.
- conformant with strict aliasing rules.
- enumeral values are checked on receive.
- https protocol reworked and now basically stable.
- negotiates tls1 to tls1_2, supports DHE protocols.
- tested with gcc's sanitizer asan, tsan and ubsan.

### Package file structure description

The following tree shows the directory layout as required by the included project:

```
- /-- UA-AnsiC
- |  |- Stack                   
- |     |- core                      Configuration and utilities
- |     |- platforms
- |        |- linux                  Platform adaption to OpenSSL and linux API
- |        |- win32                  Platform adaption to OpenSSL and Win32 API
- |     |- proxystub
- |        |- clientproxy            Client side top level API (optional)
- |        |- serverstub             Server side top level API (optional)
- |     |- securechannel             OPC UA secure conversation
- |     |- stackcore                 Base types and interfaces
- |     |- transport
- |        |- https                  HTTPS transport (optional)
- |        |- tcp                    OPC TCP Binary transport
- |- AnsiCSample						Simple example of an OPC UA nano embedded server
- |- openssl-1.0.2j                  Required third-party libraries
```

Windows and linux build scripts, as well as Visual Studio 2013 solution can be found in the root folder.

## Known issues


