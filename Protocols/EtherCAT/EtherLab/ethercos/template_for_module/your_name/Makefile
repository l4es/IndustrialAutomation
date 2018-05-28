SCILAB_VERSION = 4.1.2
SCILAB_DIR = /usr/local/scilab-$(SCILAB_VERSION)
SCILAB = $(SCILAB_DIR)/bin/scilab


MACROSRC = *.bin names lib *.cosf
HELPSRC   = *.htm

MACROS_DIR = $(PWD)
all:
	$(SCILAB)  -nb -nw -f builder.sce;
install:
	echo "This is only a Module for the HART-Toolbox. Please install this Toolbox!"
clean:
	cd macros; rm $(MACROSRC); 
	cd help/en_US; rm $(HELPSRC); 
	cd src; make clean
rtai:
	cd RTAI; make 
