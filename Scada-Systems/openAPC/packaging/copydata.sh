#!/bin/bash

rm -rf buildroot_full

mkdir -p buildroot_backup/usr/lib$1/openapc/
mkdir -p buildroot_backup/usr/bin/
mkdir -p buildroot_backup/usr/share/openapc/translations/
mkdir -p buildroot_backup/usr/share/openapc/img/
mkdir -p buildroot_backup/usr/share/openapc/codes/
mkdir -p buildroot_backup/usr/lib$1/openapc/macros/
mkdir -p buildroot_backup/usr/lib$1/openapc/flowplugins/
mkdir -p buildroot_backup/usr/lib$1/openapc/hmiplugins/

cp ../ControlRoom/ilPLC/ilPLC buildroot_backup/usr/bin/
cp ../ControlRoom/luaPLC/luaPLC buildroot_backup/usr/bin/
cp ../ControlRoom/IServer/OpenIServer buildroot_backup/usr/bin/
cp ../ControlRoom/OpenEditor buildroot_backup/usr/bin/
cp ../ControlRoom/OpenPlayer buildroot_backup/usr/bin/
cp ../ControlRoom/OpenHPlayer buildroot_backup/usr/bin/
cp ../ControlRoom/OpenDebugger buildroot_backup/usr/bin/
cp ../ControlRoom/OpenPlugger buildroot_backup/usr/bin/

cp -a ../liboapc/.libs/liboapc.so* buildroot_backup/usr/lib$1/
cp -a ../liboapc/.libs/liboapcwx.so* buildroot_backup/usr/lib$1/

cp -r ../flowplugins/*.so buildroot_backup/usr/lib$1/openapc/flowplugins/
cp -r ../hmiplugins/*.so buildroot_backup/usr/lib$1/openapc/hmiplugins/
cp -r ../macros/*.apcg buildroot_backup/usr/lib$1/openapc/macros/

mkdir -p buildroot_backup/usr/share/openapc/icons/

cp ../icons/*.* buildroot_backup/usr/share/openapc/icons/
cp ../img/*.* buildroot_backup/usr/share/openapc/img/
cp ../translations/*.* buildroot_backup/usr/share/openapc/translations/
rm buildroot_backup/usr/share/openapc/translations/custom*.property

##############

cp -r buildroot_backup buildroot_full
