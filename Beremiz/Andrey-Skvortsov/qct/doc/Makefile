SOURCES=$(wildcard *.[0-9].txt)
MAN=$(SOURCES:%.txt=%)
HTML=$(SOURCES:%.txt=%.html)
PREFIX=/usr/local
MANDIR=$(PREFIX)/man
INSTALL=install -c

all: man html

man: $(MAN)

html: $(HTML)

%: %.xml
	xmlto man $*.xml

%.xml: %.txt
	asciidoc -d manpage -b docbook $*.txt

%.html: %.txt
	asciidoc -b html4 $*.txt || asciidoc -b html $*.txt

install: man
	for i in $(MAN) ; do \
	  subdir=`echo $$i | sed -n 's/..*\.\([0-9]\)$$/man\1/p'` ; \
	  mkdir -p $(MANDIR)/$$subdir ; \
	  $(INSTALL) $$i $(MANDIR)/$$subdir ; \
	done

clean:
	$(RM) $(MAN) $(MAN:%=%.xml) $(MAN:%=%.html)
