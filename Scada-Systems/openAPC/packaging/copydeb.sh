#!/bin/bash

mkdir ControlRoom
mkdir ControlRoom/DEBIAN
mkdir ControlRoom/usr
mkdir ControlRoom/usr/bin
mkdir ControlRoom/usr/lib
mkdir ControlRoom/usr/lib/openapc
mkdir ControlRoom/usr/lib/openapc/macros
mkdir ControlRoom/usr/lib/openapc/flowplugins
mkdir ControlRoom/usr/lib/openapc/hmiplugins
mkdir ControlRoom/usr/share
mkdir ControlRoom/usr/share/openapc
mkdir ControlRoom/usr/share/openapc/img
mkdir ControlRoom/usr/share/openapc/icons
mkdir ControlRoom/usr/share/openapc/translations

cp ../ControlRoom/ilPLC/ilPLC ControlRoom/usr/bin/
cp ../ControlRoom/luaPLC/luaPLC ControlRoom/usr/bin/
cp ../ControlRoom/IServer/OpenIServer ControlRoom/usr/bin/
cp ../ControlRoom/OpenEditor ControlRoom/usr/bin/
cp ../ControlRoom/OpenPlayer ControlRoom/usr/bin/
cp ../ControlRoom/OpenHPlayer ControlRoom/usr/bin/
cp ../ControlRoom/OpenDebugger ControlRoom/usr/bin/
cp ../ControlRoom/OpenPlugger ControlRoom/usr/bin/

cp -a ../liboapc/.libs/liboapc.so* ControlRoom/usr/lib/
cp -a ../liboapc/.libs/liboapcwx.so* ControlRoom/usr/lib/

cp -r ../flowplugins/*.so ControlRoom/usr/lib/openapc/flowplugins/
cp -r ../hmiplugins/*.so ControlRoom/usr/lib/openapc/hmiplugins/
cp -r ../macros/*.apcg ControlRoom/usr/lib/openapc/macros/

cp ../icons/*.* ControlRoom/usr/share/openapc/icons/
cp ../img/*.* ControlRoom/usr/share/openapc/img/
cp ../translations/*.* ControlRoom/usr/share/openapc/translations/
rm ControlRoom/usr/share/openapc/translations/custom*.property

