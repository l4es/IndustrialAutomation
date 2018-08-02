About
-----

This is the location for the BeamSDK, a software development kit for creating
applications that make use of BeamConstruct functionalities. Such applications
can use the whole BeamConstruct GUI or just low level graphics and geometry
functions or every shade of complexity between these two extremes.

BeamSDK is free for non-commercial purposes and comes with a GPL license. To
get it, please contact us via https://halaser.eu/contact.php . For commercial
purposes and/or cases where it has to be used as closed source, it can be
licensed in a non-GPLed variant too, for details please contact us at
https://halaser.eu/contact.php

If you already own the BeamSDK, please unpack its contents into this directory.
Means copy the directories and files out of your SDKs archive-directory named
"BeamSDK" into this directory. When you have two directories "BeamSDK" after
copy operation, there is one too much and the SDK will not work. When you have
only one directory BeamSDK in OpenAPC-SDK and no more subdirectories with the
same name, everything is fine.

When you do NOT own the BeamSDK that gives you the possibility to develop
your own BeamConstruct-based application please visit
https://halaser.eu/feature.php#BeamSDK to find out how to get it.


Contents
--------

The BeamSDK provides the following data:

BeamConstruX - source codes of the BeamConstruct OCX/ActiveX implementation
    (can be compiled for and used with Windows only)

BeamEmbedded - source code of a plain WinAPI example that demonstrates how a
    libbeamconstruct-generated UI can be embedded into the GUI of an existing
    application; although this example uses C/C++ and WinAPI, the principle is
    the same for all kinds of development environments and development
    languages - the HWND of the frame/panel/canvas which has to be used for
    libbeamconstruct UI has to be handed over during initialisation

BeamEmbedded_csharp - source code of a plain WinAPI example that demonstrates
    how a libbeamconstruct-generated UI can be embedded into the GUI of an
    existing application; although this example uses C# and WinAPI, the
    principle is the same for all kinds of development environments and
    development languages - the HWND of the frame/panel/canvas which has to be
    used for libbeamconstruct UI has to be handed over during initialisation

BeamServer - full sources of a TCP/IP server implementation that makes
    BeamConstruct remote-controllable, ASCII commands can be sent via network
    connection; this application makes use of libbeamconstruct too

BeamSimple - a minimal C/C++ example that deomstrates how to create a
    Beamconstruct-like application out of an environment which is completely
    independent from the wxWidgets toolkit (which is used for libbeamconstruct)
    just by doing some plain library calls to libbeamconstruct.dll/.so

BeamSimple_csharp - a minimal C# example that demostrates how to create a
    Beamconstruct-like application out of an environment which is completely
    independent from the wxWidgets toolkit (which is used for libbeamconstruct)
    just by doing some plain library calls to libbeamconstruct.dll/.so

bin/bin64 - several additional binaries for 32 and 64 bit systems like
    link-libraries and the BeamConstruX.ocx (Windows only, no separate
    link-libraries are required for other operating systems)

CorrCorrect - full wxWidgets-based sourcecodes of CorrCorrect tool that comes
    with official OpenAPC package

custom_setup - directory with scripts and data that give you the possibility to
    create an own installation archive that refers to your product name and
    company instead to OpenAPC (including example configuration for customised
    BeamConstruct)

include - several include files required for developing BeamConstruct-based
    applications (please refer to the BeamSDK programmers manual for details)

libio_e1701 - full and fully functional sources of controller plug-in
    implementation for HALaser Systems E1701D XY2-100 laser scanner controller
    card

libio_e1701a - full and fully functional sources of controller plug-in
    implementation for HALaser Systems E1701A analogue/ILDA laser scanner
    controller card

libio_makeblock_xy - full and fully functional sources of controller plug-in
    implementation for Makeblock XY-Plotter

libio_sc_skeleton - complete sources of scanner controller plug-in base
    implementation, here all binary data structures are already handled as
    required, only the controller card dependent implementation is missing

beamsdk_manual.pdf - the programmers manual for the BeamSDK that describes
    all functions and structures required for developing own BeamConstruct-
    based applications
