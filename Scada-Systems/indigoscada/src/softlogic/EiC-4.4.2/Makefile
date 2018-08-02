############################
# Part 1: LOCAL DEFINITIONS

       TOP = .


#############################
# Part 2: COMMON MAKE PARAMETERS

include $(TOP)/make.proj

# Standard bourne shell doesn't update PWD on directory change.
#       SHELL = /bin/ksh
#
#ifneq ($(PLATFORM),_SUNOS)
#ifneq ($(PLATFORM),_NETBSD)
#       SHELL = /usr/bin/ksh
#else
#       SHELL = /bin/ksh
#endif
#endif

#############################
# Part 3: LOCAL SOURCES 
#DIRS = ./config\
	$(TOP_DIR)/module/stdClib/src\
        ./src\
	./main \
	./test \
	./module


DIRS = ./config\
        ./src\
	./test \
	./module\
	./main \



##############################
# Part 4: LOCAL TARGETS  

##################
TREE =	include \
	include/sys \
	include/module\
	lib 

install::
	@for i in $(TREE)  ;\
	do \
		if [ ! -d $$i ] ; then \
			echo $(MKDIR) $(MKDIRFLAGS) $$i ;\
			$(MKDIR) $(MKDIRFLAGS) $$i ;\
		fi \
	done


###################
#
install:: headers

include $(CONFIG_DIR)/NormalNodeTargets


#########################
# Make binary distribution


#########################
# Make binary distribution

# EiC version number
EiC_NUM = 4.3.0

BINTREE = EiC\
        EiC/include \
        EiC/include/sys \
        EiC/doc\
        EiC/module\

bintree:
	@for i in $(BINTREE)  ;\
	do \
		if [ ! -d $$i ] ; then \
			echo $(MKDIR) $(MKDIRFLAGS) $$i ;\
			$(MKDIR) $(MKDIRFLAGS) $$i ;\
		fi \
	done

INCFILES = ./include/*.h 
            
INCSYSFILES = ./include/sys/*.h

DOCFILES =  ./eic.man

MODULES = ./module/MathStats\
        ./module/gnuplot\
        ./module/tools\
        ./module/examples\
        ./module/cgihtml\
        ./module/xv\

modules:
	@for i in $(MODULES)  ;\
	do \
		echo cp -r $$i/* EiC/$$i ;\
		mkdir EiC/$$i;\
		cp -r $$i/* EiC/$$i ;\
	done

binary: bintree modules install
	cp ./batch2.sh     EiC/batch2.sh
	cp ./Makefile.Bin  EiC/Makefile
	cp $(HOME)/bin/eic  EiC/eicc
	strip EiC/eicc
	cp $(INCFILES)   EiC/include 
	cp $(INCSYSFILES) EiC/include/sys
	cp $(DOCFILES)   EiC/doc
	cp README.BIN    EiC/README
	cp LICENCE       EiC/LICENCE
	cp Distribution.txt EiC/Distribution.txt
	cp make.rules    EiC/
	rm -f `find ./EiC/module -name "Makefile"`
	echo  "PLATFORM = $(PLATFORM)" > EiC/make.proj
	tar cvf EiC$(PLATFORM)_$(EiC_NUM).tar  EiC
	gzip EiC$(PLATFORM)_$(EiC_NUM).tar 
	mv  EiC$(PLATFORM)_$(EiC_NUM).tar.gz  EiC$(PLATFORM)_$(EiC_NUM).tgz
	rm -fr EiC

#############################
# Part 5: COMMON RULES and 
#     and default Targets

include $(CONFIG_DIR)/make.rules

#############################
# Part 6: DEPENDENCIES


clean::
	find . -name "*~"     | xargs $(RM)
	find . -name EiChist.lst  | xargs $(RM)
	find . -name a.out | xargs $(RM)
	find . -name "*.o" | xargs $(RM) 

clobber::clean
	$(RM) make.params make.proj make.rules NormalNodeTargets














