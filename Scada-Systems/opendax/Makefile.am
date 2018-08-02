ACLOCAL_AMFLAGS = -I m4
SUBDIRS = lib dax modules

AUX_DIST = $(ac_aux_dir)/install-sh

EXTRA_DIST = common.h \
			 config/config.* \
			 config/depcomp \
			 config/install-sh \
			 config/ltmain.sh \
			 config/missing \
			 config/mkinstalldirs \
			 modules/test/daxtest.lua \
			 modules/test/tests/* \
			 modules/daxlua/*.lua \
			 etc/*.conf.sample \
			 doc/*.tex \
			 modules/arduino/sketch/*

include_HEADERS = opendax.h

confdir = $(sysconfdir)/$(PACKAGE)
conf_DATA = etc/opendax.conf.sample \
			etc/modbus.conf.sample \
			etc/daxlua.conf.sample
