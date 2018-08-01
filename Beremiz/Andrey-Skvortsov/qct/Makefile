#
# Distribution logic
#

# Files not revisioned which are distributed
UI_FILES := qctlib/ui_dialog.py qctlib/ui_preferences.py
UI_FILES += qctlib/ui_select.py
EXTRAS := $(UI_FILES) doc/qct.1 doc/qct.1.html

# Revisioned files which are not distributed
PRUNE  := .hgtags .hgignore

VER := $(shell python setup.py --version)
DIST_NAME  := qct-$(VER)
PRUNE_LIST := $(foreach file, $(PRUNE), $(DIST_NAME)/$(file))

all: $(UI_FILES)

dist: $(EXTRAS)
	hg archive --type=files $(DIST_NAME)
	for i in $(EXTRAS) ; do cp $$i $(DIST_NAME)/$$i ; done
	$(RM) $(PRUNE_LIST)
	tar czf $(DIST_NAME).tar.gz $(DIST_NAME)/
	$(RM) -r $(DIST_NAME)/

ui_%.py : %.ui
	pyuic4 $< > $@

doc/qct.1:
	$(MAKE) -C doc

.PHONY: install site-install mac-install

install: $(UI_FILES)
	python setup.py install --home=~

site-install: $(UI_FILES)
	python setup.py install --force

egg: $(UI_FILES)
	python setup.py bdist_egg

clean:
	$(RM) $(UI_FILES)
	$(RM) qctlib/*.pyc qctlib/vcs/*.pyc
	$(RM) -r build/ dist/ qct.egg-info/
	$(RM) qct-*.tar.gz 
	$(MAKE) -C doc clean
