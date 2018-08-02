@echo off

date /T > launch.log
time /T >> launch.log


set PATH=c:\python24;C:\Program Files\NSIS;C:\Program Files\WinRAR;%PATH%

del *.py >> launch.log 2>>&1
copy \\michael\hdd\Prog\Projects\free-scada\trunk\make\*.py . >> launch.log 2>&1

python autobuild.py >> launch.log 2>&1

date /T >> launch.log
time /T >> launch.log
