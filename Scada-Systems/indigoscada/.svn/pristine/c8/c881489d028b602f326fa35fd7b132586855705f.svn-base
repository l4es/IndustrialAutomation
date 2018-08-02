REM Batch file to make all Makefiles of Qwt and the examples. Why?
REM Qt-NC's tmake does not understand the subdirs template.

REM cd into directory, before invoking tmake is necessary.

tmake qwt.pro -o Makefile
cd examples
tmake examples.pro -o Makefile
cd bode
tmake bode.pro -o Makefile
cd ..\cpuplot
tmake cpuplot.pro -o Makefile
cd ..\curvdemo1
tmake curvdemo1.pro -o Makefile
cd ..\curvdemo2
tmake curvdemo2.pro -o Makefile
cd ..\data_plot
tmake data_plot.pro -o Makefile
cd ..\dials
tmake dials.pro -o Makefile
cd ..\event_filter
tmake event_filter.pro -o Makefile
cd ..\radio
tmake radio.pro -o Makefile
cd ..\realtime_plot
tmake realtime_plot.pro -o Makefile
cd ..\simple_plot
tmake simple_plot.pro -o Makefile
cd ..\sliders
tmake sliders.pro -o Makefile
cd ..\..

REM EOF
