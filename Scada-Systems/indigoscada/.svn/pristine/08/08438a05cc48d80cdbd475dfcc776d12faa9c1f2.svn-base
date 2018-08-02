#########################
# Part 1: LOCAL DEFINITIONS

       TOP = ..
     EXTRA = 
  WARNINGS = 
  INCLUDES = 
       LIB = 

  DEP_LIBS = 
      LIBS = 
 
#########################
# Part 2: COMMON MAKE PARAMETERS

include $(TOP)/make.proj

#########################
# Part 3: LOCAL SOURCES 

#########################
# Part 4: LOCAL TARGETS  


#### make EiC.tex

TexFiles = EiC.tex FigPageSetup.tex  Library.tex  preface.tex\
           EiCgram.tex   Intro2EiC.tex  grammar.tex preproc.tex

all::

EiC.dvi: $(TexFiles)
	latex EiC
	makeindex EiC.idx
	bibtex EiC
	latex EiC
	latex EiC
	dvips -o EiC.ps EiC


install::EiC.dvi
	$(DUMMY_TARGET)

clean::
	find . -name "*~" | xargs $(RM)
	$(RM) *.aux *.dvi *.ind *.ilg *.blg *.bbl *.log *.idx *.toc
	$(RM) *.ps  

clobber::clean
	rm -f install

####### HEADERS
headers::

#########################
# Part 5: COMMON RULES and 
#     and default Targets

include $(CONFIG_DIR)/make.rules

#########################
# Part 6: DEPENDENCIES

