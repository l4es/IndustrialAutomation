#------------------------------------------------------------------------------
#
#  LaTeX Makefile
#
#  $Id$
#
#------------------------------------------------------------------------------

FILE := dls_doku

LATEX_OPTIONS := -file-line-error-style

#------------------------------------------------------------------------------

first: dvi

all: dvi pdf

dvi:
	latex $(LATEX_OPTIONS) \\nonstopmode\\input{$(FILE).tex}

pdf:
	pdflatex $(LATEX_OPTIONS) $(FILE)

clean:
	@rm -f $(FILE).aux $(FILE).dvi $(FILE).idx \
			$(FILE).ilg $(FILE).ind $(FILE).log \
			$(FILE).out $(FILE).pdf $(FILE).ps \
			$(FILE).toc $(FILE).lot $(FILE).lof \
			$(FILE).lol $(FILE).glo \
			*.bak bilder/*.bak

#------------------------------------------------------------------------------
