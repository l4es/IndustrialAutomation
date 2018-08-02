# pro file for the Qwt designer plugin
#
# Please note that the Qwt designer integration 
# is limited  by the current (Qt <= 3.1.x) design 
# of the Qt plugin API. As soon as TrollTech will allow
# individual dialogs for complex widgets the result
# will be more satisfying.
#
# Qwt designer plugin needs Qt >= 3.x. It is
# not intended to support Qt 2.x like the Qwt lib
# does. 

TARGET          = qwtplugin
TEMPLATE        = lib
CONFIG         += qt warn_on thread debug plugin
MOC_DIR         = moc
OBJECTS_DIR     = obj 
INCLUDEPATH    += ../include 
DESTDIR         = plugins/designer

unix:LIBS      += -L../lib -lqwt
win32:LIBS    += ../lib/qwt.lib 

linux-g++:QMAKE_CXXFLAGS += -fno-exceptions 

target.path=C:/scada/plugins
INSTALLS += target

HEADERS  += qwtplugin.h
SOURCES  += qwtplugin.cpp
IMAGES   += \
        pixmaps/qwtplot.png \
        pixmaps/qwtanalogclock.png \
        pixmaps/qwtcounter.png \
        pixmaps/qwtcompass.png \
        pixmaps/qwtdial.png \
        pixmaps/qwtknob.png \
        pixmaps/qwtpushbutton.png \
        pixmaps/qwtscale.png \
        pixmaps/qwtslider.png \
        pixmaps/qwtthermo.png \
        pixmaps/qwtwheel.png \
        pixmaps/qwtwidget.png \
		pixmaps/led_by0000.png \
		pixmaps/led_by0001.png \
		pixmaps/led_rg0000.png \
		pixmaps/led_rg0001.png \
		pixmaps/plcdnumbericon.png \
		pixmaps/pmetericon.png \
		pixmaps/pswitchicon.png \
		pixmaps/ptankicon.png \
		pixmaps/pthermometericon.png \
		pixmaps/ptoggleicon.png 

