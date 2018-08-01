#! gmake

# This is Makefile for Beremiz installer
#
# Invoke with "make -f path/to/Makefile" on a linux box
# in directory where build should happen.
#
# All those dependencies have to be installed :
#
#  Windows installer :
#  - wine (tested with 1.2 and 1.6. Fail with 1.4)
#  - mingw32
#  - flex
#  - bison
#  - tar
#  - unrar
#  - wget
#  - nsis
#  - libtool
#  - xmlstarlet
#  - xsltproc
#  - python-lxml
#
# WARNING : DISPLAY variable have to be defined to a valid X server
#           in case it would be a problem, run :
#           xvfb-run make -f /path/to/this/Makefile

version = 1.2-rc1

src := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
HGREMOTE ?= https://hg.beremiz.org/
HGROOT ?= $(abspath $(src)/..)
GITROOT := $(HGROOT)
DIST =
CPUS := `cat /proc/cpuinfo | grep -e 'processor\W*:\W*[[:digit:]]*' | nl -n ln | tail -n1 | cut -f1`
BLKDEV=/dev/null


CROSS_COMPILE=i686-w64-mingw32
CROSS_COMPILE_LIBS_DIR=$(shell dirname $(shell $(CROSS_COMPILE)-gcc -print-libgcc-file-name))
CC=$(CROSS_COMPILE)-gcc
CXX=$(CROSS_COMPILE)-g++

define get_runtime_libs
	cp $(CROSS_COMPILE_LIBS_DIR)/libgcc_s_sjlj-1.dll $(1)
	cp $(CROSS_COMPILE_LIBS_DIR)/libstdc++-6.dll $(1)
endef

distfiles = $(src)/distfiles
sfmirror = downloads
tmp := $(shell mktemp -d)

define hg_get_archive
test -d $(HGROOT)/`basename $(1)` || hg --cwd $(HGROOT) clone $(HGREMOTE)`basename $(1)`
hg -R $(HGROOT)/`basename $(1)` archive $(2) $(1)
hg -R $(HGROOT)/`basename $(1)` id -i | sed 's/\+//' > $(1)/revision
endef

define get_src_hg
rm -rf $(1)
$(call hg_get_archive, $(1), $(2))
endef

define get_src_git
rm -rf $(1)
mkdir $(1)
(cd $(GITROOT)/`basename $(1)`; git archive --format=tar $(2)) | tar -C $(1) -x
endef

define get_src_http
dld=$(distfiles)/`echo $(2) | tr ' ()' '___'`;( ( [ -f $$dld ] || wget $(1)/$(2) -O $$dld ) && ( [ ! -f $$dld.md5 ] && (cd $(distfiles);md5sum `basename $$dld`) > $$dld.md5 || (cd $(distfiles);md5sum -c `basename $$dld.md5`) ) ) &&
endef

define get_src_pypi
$(call get_src_http,https://pypi.python.org/packages/$(1),$(2))
endef

define get_src_sf
$(call get_src_http,https://$(sfmirror).sourceforge.net/project/$(1),$(2))
endef

all: Beremiz-$(version).exe $(targets_add)


ifneq ("$(DIST)","")
include $(src)/$(DIST).mk
endif

build:
	rm -rf build
	mkdir -p build

# native toolchain, pre-built
mingwdir=build/mingw

define get_mingw
$(call get_src_sf,mingw/MinGW/Base/$(1),$(2)) tar -C $(mingwdir) -xf $$dld
endef
define get_msys
$(call get_src_sf,mingw/MSYS/Base/$(1),$(2)) tar -C $(mingwdir) -xf $$dld
endef
mingw: |build
	rm -rf $(mingwdir)
	mkdir -p $(mingwdir)
	# windows.h
	$(call get_mingw,w32api/w32api-5.0.1,w32api-5.0.1-mingw32-dev.tar.xz)
	# mingw runtime
	$(call get_mingw,mingwrt/mingwrt-5.0.1,mingwrt-5.0.1-mingw32-dll.tar.xz)
	$(call get_mingw,mingwrt/mingwrt-5.0.1,libmingwex-5.0.1-mingw32-dll-0.tar.xz)
	$(call get_mingw,isl/isl-0.18,libisl-0.18-1-mingw32-dll-15.tar.xz)
	# mingw headers and lib
	$(call get_mingw,mingwrt/mingwrt-5.0.1,mingwrt-5.0.1-mingw32-dev.tar.xz)
	# binutils
	$(call get_mingw,binutils/binutils-2.28,binutils-2.28-1-mingw32-bin.tar.xz)
	# C compiler
	$(call get_mingw,gcc/Version6/gcc-6.3.0,gcc-core-6.3.0-1-mingw32-bin.tar.xz)
	$(call get_mingw,gcc/Version6/gcc-6.3.0,libgcc-6.3.0-1-mingw32-dll-1.tar.xz)
	# dependencies
	$(call get_mingw,gmp/gmp-6.1.2,libgmp-6.1.2-2-mingw32-dll-10.tar.xz)
	$(call get_mingw,mpc/mpc-1.0.3,libmpc-1.0.3-1-mingw32-dll-3.tar.xz)
	$(call get_mingw,mpfr/mpfr-3.1.5,libmpfr-3.1.5-1-mingw32-dll-4.tar.xz)
	$(call get_mingw,gettext/gettext-0.18.3.2-2,libintl-0.18.3.2-2-mingw32-dll-8.tar.xz)
	$(call get_mingw,gettext/gettext-0.18.3.2-2,libgettextpo-0.18.3.2-2-mingw32-dll-0.tar.xz)
	$(call get_mingw,libiconv/libiconv-1.14-3,libiconv-1.14-3-mingw32-dll.tar.lzma)

	# make, bash, and dependencies
	$(call get_msys,bash/bash-3.1.23-1,bash-3.1.23-1-msys-1.0.18-bin.tar.xz)
	$(call get_msys,coreutils/coreutils-5.97-3,coreutils-5.97-3-msys-1.0.13-bin.tar.lzma)
	$(call get_msys,libiconv/libiconv-1.14-1,libiconv-1.14-1-msys-1.0.17-bin.tar.lzma)
	$(call get_msys,libiconv/libiconv-1.14-1,libiconv-1.14-1-msys-1.0.17-dll-2.tar.lzma)
	$(call get_msys,gettext/gettext-0.18.1.1-1,libintl-0.18.1.1-1-msys-1.0.17-dll-8.tar.lzma)
	$(call get_msys,regex/regex-1.20090805-2,libregex-1.20090805-2-msys-1.0.13-dll-1.tar.lzma)
	$(call get_msys,termcap/termcap-0.20050421_1-2,libtermcap-0.20050421_1-2-msys-1.0.13-dll-0.tar.lzma)
	$(call get_msys,make/make-3.81-3,make-3.81-3-msys-1.0.13-bin.tar.lzma) 
	$(call get_msys,msys-core/msys-1.0.19-1,msysCORE-1.0.19-1-msys-1.0.19-bin.tar.xz)
	$(call get_msys,termcap/termcap-0.20050421_1-2,libtermcap-0.20050421_1-2-msys-1.0.13-dll-0.tar.lzma)
	touch $@

msiexec = WINEPREFIX=$(tmp) msiexec
wine = WINEPREFIX=$(tmp) wine
pydir = build/python
pysite = $(pydir)/Lib/site-packages

python: |build
	rm -rf $(pydir)
	mkdir -p $(pydir)
	
	# Python
	$(call get_src_http,http://www.python.org/ftp/python/2.7.3,python-2.7.3.msi)\
	$(msiexec) /qn /a $$dld TARGETDIR=.\\$(pydir)
	
	# WxPython 2.8 (needs running inno unpacker in wine)
	$(call get_src_sf,innounp/innounp/innounp%200.36,innounp036.rar)\
	unrar e $$dld innounp.exe $(tmp)
	$(call get_src_sf,wxpython/wxPython/2.8.12.1,wxPython2.8-win32-unicode-2.8.12.1-py27.exe)\
	$(wine) $(tmp)/innounp.exe -d$(tmp)/wx28 -x $$dld
	cp -R $(tmp)/wx28/\{code_GetPythonDir\}/* $(pydir)
	cp -R $(tmp)/wx28/\{app\}/* $(pysite)
	
	# WxPython 3.0 (needs running inno unpacker in wine)
	$(call get_src_sf,wxpython/wxPython/3.0.2.0,wxPython3.0-win32-3.0.2.0-py27.exe)\
	$(wine) $(tmp)/innounp.exe -d$(tmp)/wx30 -x $$dld
	cp -R $(tmp)/wx30/\{code_GetPythonDir\}/* $(pydir)
	cp -R $(tmp)/wx30/\{app\}/* $(pysite)
	
	# wxPython fails if VC9.0 bullshit is not fully here.
	$(call get_src_http,http://download.microsoft.com/download/1/1/1/1116b75a-9ec3-481a-a3c8-1777b5381140,vcredist_x86.exe)\
	cp $$dld $(tmp)
	$(wine) $(tmp)/vcredist_x86.exe /qn /a
	cp $(tmp)/drive_c/windows/winsxs/x86_Microsoft.VC90.CRT*/* $(pydir)
	
	# MathPlotLib
	$(call get_src_http,https://github.com/downloads/matplotlib/matplotlib,matplotlib-1.2.0.win32-py2.7.exe)\
	unzip -d $(tmp)/mathplotlib $$dld ; [ $$? -eq 1 ] #silence error unziping .exe
	cp -R $(tmp)/mathplotlib/PLATLIB/* $(pysite)
	
	# pywin32
	$(call get_src_sf,pywin32/pywin32/Build216,pywin32-216.win32-py2.7.exe)\
	unzip -d $(tmp)/pw32 $$dld ; [ $$? -eq 1 ] #silence error unziping .exe
	cp -R $(tmp)/pw32/PLATLIB/* $(pysite)
	
	# zope.interface (twisted prereq)
	$(call get_src_pypi,9d/2d/beb32519c0bd19bda4ac38c34db417d563ee698518e582f951d0b9e5898b,zope.interface-4.3.2-py2.7-win32.egg)\
	unzip -d $(tmp) $$dld
	cp -R $(tmp)/zope $(pysite)
	
	# six (pyopenssl prereq)
	$(call get_src_pypi,67/4b/141a581104b1f6397bfa78ac9d43d8ad29a7ca43ea90a2d863fe3056e86a,six-1.11.0-py2.py3-none-any.whl)\
	unzip -d $(tmp) $$dld 
	cp -R $(tmp)/six.py $(pysite)
	
	# enum34 (cryptography prereq)
	$(call get_src_pypi,bf/3e/31d502c25302814a7c2f1d3959d2a3b3f78e509002ba91aea64993936876,enum34-1.1.6.tar.gz)\
	tar -C $(tmp) -xzf $$dld
	cp -R $(tmp)/enum34-1.1.6/enum $(pysite)
	
	# cryptography (pyopenssl prereq)
	$(call get_src_pypi,17/08/04dc376411968ac21f2e11a8d7d70e936a654172567672fa79710a137bd9,cryptography-2.1.4-cp27-cp27m-win32.whl)\
	unzip -d $(tmp) $$dld 
	cp -R $(tmp)/cryptography $(pysite)
	
	# pyopenssl (twisted/ssl prereq)
	$(call get_src_pypi,79/db/7c0cfe4aa8341a5fab4638952520d8db6ab85ff84505e12c00ea311c3516,pyOpenSSL-17.5.0-py2.py3-none-any.whl)\
	unzip -d $(tmp) $$dld 
	cp -R $(tmp)/OpenSSL $(pysite)
	
	# pyasn1 (service identity prereq)
	$(call get_src_pypi,eb/3d/b7d0fdf4a882e26674c68c20f40682491377c4db1439870f5b6f862f76ed,pyasn1-0.4.2.tar.gz)\
	tar -C $(tmp) -xzf $$dld
	cp -R $(tmp)/pyasn1-0.4.2/pyasn1 $(pysite)
	
	# pyasn1-modules (service identity prereq)
	$(call get_src_pypi,ab/76/36ab0e099e6bd27ed95b70c2c86c326d3affa59b9b535c63a2f892ac9f45,pyasn1-modules-0.2.1.tar.gz)\
	tar -C $(tmp) -xzf $$dld
	cp -R $(tmp)/pyasn1-modules-0.2.1/pyasn1_modules $(pysite)
	
	# characteristic (service identity prereq)
	$(call get_src_pypi,dc/66/54b7a4758ea44fbc93895c7745060005272560fb2c356f2a6f7448ef9a80,characteristic-14.3.0.tar.gz)\
	tar -C $(tmp) -xzf $$dld
	cp -R $(tmp)/characteristic-14.3.0/characteristic.py $(pysite)
	
	# service identity (twisted prereq)
	$(call get_src_pypi,de/2a/cab6e30be82c8fcd2339ef618036720eda954cf05daef514e386661c9221,service_identity-17.0.0.tar.gz)\
	tar -C $(tmp) -xzf $$dld
	cp -R $(tmp)/service_identity-17.0.0/src/service_identity $(pysite)
	
	# txaio (autobahn prereq)
	$(call get_src_pypi,d6/95/d0c67304515f352342bc8fd14e5a3e7ca924134608acb730916073b18464,txaio-2.8.2.tar.gz)\
	tar -C $(tmp) -xzf $$dld
	cp -R $(tmp)/txaio-2.8.2/txaio $(pysite)
	
	# python-msgpack (autobahn prereq)
	$(call get_src_pypi,59/ca/b8048e184a2edb5b3cd46f38be130e87cbce77f4168ed62344bc33df3e1b,msgpack_python-0.4.8-cp27-cp27m-win32.whl)\
	unzip -d $(tmp) $$dld 
	cp -R $(tmp)/msgpack $(pysite)
	
	# u-msgpack-python (autobahn prereq)
	$(call get_src_pypi,b8/ff/5730f61767d5acecac103343d66ebf631ebd672fa14e50472f05545749c2,u-msgpack-python-2.4.1.tar.gz)\
	tar -C $(tmp) -xzf $$dld
	cp -R $(tmp)/u-msgpack-python-2.4.1/umsgpack.py $(pysite)
	
	# cffi (cryptography prereq)
	$(call get_src_pypi,5e/67/01ae6ae50d168d3b92f2a80bf62e07537171d0a1938c8bceb7e64d36829f,cffi-1.11.2-cp27-cp27m-win32.whl)\
	unzip -d $(tmp) $$dld 
	cp -R $(tmp)/cffi $(tmp)/_cffi_backend.pyd $(pysite)
	
	# Twisted
	$(call get_src_pypi,fe/cb/97504d68c2f4300fb121f700cc2fae10e1856ba043e964acfab02e120835,Twisted-17.9.0-cp27-cp27m-win32.whl)\
	unzip -d $(tmp) $$dld 
	cp -R $(tmp)/twisted $(pysite)
	
	# Autobahn
	$(call get_src_pypi,e4/2e/01a64212b1eb580d601fa20f146c962235e3493795f46e3b254597ec635d,autobahn-17.10.1.tar.gz)\
	tar -C $(tmp) -xzf $$dld
	cp -R $(tmp)/autobahn-17.10.1/autobahn $(pysite)
	
	# Nevow
	$(call get_src_pypi,source/N/Nevow,Nevow-0.10.0.tar.gz)\
	tar -C $(tmp) -xzf $$dld
	for i in nevow formless twisted; do cp -R $(tmp)/Nevow-0.10.0/$$i $(pysite); done
	
	# Numpy
	$(call get_src_pypi,fd/32/196073188f5b8b464e0fabb470f971fa5dcd91b55726a43b40b008212358,numpy-1.13.3-2-cp27-none-win32.whl)\
	unzip -d $(tmp) $$dld
	cp -R $(tmp)/numpy $(pysite)
	
	# SimpleJson
	$(call get_src_pypi,source/s/simplejson,simplejson-2.2.1.tar.gz)\
	tar -C $(tmp) -xzf $$dld
	cp -R $(tmp)/simplejson-2.2.1/simplejson/ $(pysite)
	
	# Zeroconf
	$(call get_src_pypi,6b/88/48dbe88b10098f98acef33218763c5630b0081c7fd0849ab4793b1e9b6d3,zeroconf-0.19.1-py2.py3-none-any.whl)\
	unzip -d $(tmp)/zeroconf $$dld
	cp -R $(tmp)/zeroconf/*.py* $(pysite)
	
	# netifaces
	$(call get_src_pypi,05/00/c719457bcb8f14f9a7b9244c3c5e203c40d041a364cf784cf554aaef8129,netifaces-0.10.6-py2.7-win32.egg)\
	unzip -d $(tmp)/netifaces $$dld
	cp -R $(tmp)/netifaces/*.py* $(pysite)	
	
	# WxGlade
	$(call get_src_http,https://bitbucket.org/wxglade/wxglade/get,034d891cc947.zip)\
	unzip -d $(tmp) $$dld
	mv $(tmp)/wxglade-wxglade-034d891cc947 $(pysite)/wxglade
	
	# Pyro
	$(call get_src_pypi,61/68/0978adae315261b87acd216517c2c7f00780396e4d1426c5412458c6a28f,Pyro-3.16.tar.gz)\
	tar -C $(tmp) -xzf $$dld
	mv $(tmp)/Pyro-3.16/Pyro $(pysite)
	
	# Lxml
	$(call get_src_pypi,c7/e6/26a600d9828554ca9de35d3d1daf3779028bea37025f3dd25e9d58d63bac,lxml-4.1.1-cp27-cp27m-win32.whl)\
	unzip -d $(tmp) $$dld
	cp -R $(tmp)/lxml $(pysite)
	
	touch $@

	
OSSLVER=openssl-1.0.1p
MINGPFX=i686-w64-mingw32-

openssl:
	# Build Openssl
	$(call get_src_http,https://openssl.org/source,$(OSSLVER).tar.gz)\
	tar -C $(tmp) -xzf $$dld
	cd $(tmp)/$(OSSLVER); \
	CC=$(MINGPFX)gcc ./Configure mingw && \
	make all build-shared CROSS_COMPILE=$(MINGPFX) SHARED_LDFLAGS=-static-libgcc
	mv $(tmp)/$(OSSLVER) .;
	
	touch $@
	
M2CRVER=M2Crypto-0.22.5

m2crypto: openssl
	# Build M2crypto
	$(call get_src_pypi,54/f5/6fa9bca4a18cc36c0c84c73d41d8e521c8cb70f077b11297efcd985242a6,M2Crypto-0.22.5.tar.gz)\
	tar -C $(tmp) -xzf $$dld
	cd $(tmp)/$(M2CRVER); \
	patch -p1 < $(src)/M2Crypto-mingw-cross-compile-fix.patch && \
	PYTHONLIB=$(abspath $(pydir))/libs \
	PYTHONINC=$(abspath $(pydir))/include/ \
	MINGCCPREFIX=$(MINGPFX) \
	    python setup.py build build_ext \
	        --openssl=$(CURDIR)/$(OSSLVER) -cmingw32
	
	# Copy openssl dlls directly in M2Crypto package directory
	cp -a $(CURDIR)/$(OSSLVER)/*.dll $(tmp)/$(M2CRVER)/build/lib.win32-2.7/M2Crypto 
	
	# Move result into python site packages
	mv $(tmp)/$(M2CRVER)/build/lib.win32-2.7/M2Crypto $(pysite)
	
	touch $@

matiecdir = build/matiec
matiec: |build
	$(call get_src_hg,$(tmp)/matiec)
	cd $(tmp)/matiec ;\
	autoreconf;\
	automake --add-missing;\
	./configure --host=$(CROSS_COMPILE);\
	make -j$(CPUS);
	rm -rf $(matiecdir)
	mkdir -p $(matiecdir)
	mv $(tmp)/matiec/*.exe $(matiecdir)
	
	# install necessary shared libraries from local cross-compiler
	$(call get_runtime_libs,$(matiecdir))
	
	mv $(tmp)/matiec/lib $(matiecdir)
	touch $@

examples: |build
	rm -rf  examples
	mkdir -p examples

beremiz: | build examples
	$(call get_src_hg,build/beremiz)
	$(call tweak_beremiz_source)
	rm -rf examples/canopen_tests
	mkdir -p examples/canopen_tests
	mv build/beremiz/tests/canopen_* examples/canopen_tests
	rm -rf examples/base_tests
	mkdir -p examples/base_tests
	mv build/beremiz/tests/* examples/base_tests
	touch $@

CFbuild = build/CanFestival-3
CFconfig = $(CFbuild)/objdictgen/canfestival_config.py
canfestival: mingw
	rm -rf $(CFbuild)
	$(call get_src_hg,$(CFbuild))
	cd $(CFbuild); \
	./configure --can=tcp_win32 \
				--cc=$(CC) \
				--cxx=$(CXX) \
				--target=win32 \
				--wx=0
	$(MAKE) -C $(CFbuild)
	cd $(CFbuild); find . -name "*.o" -exec rm {} ';' #remove object files only
	touch $@

targets=python m2crypto mingw matiec beremiz
Beremiz-$(version).exe: $(targets) $(src)/license.txt $(src)/install.nsi $(targets_ex)
	sed -e 's/\$$BVERSION/$(version)/g' $(src)/license.txt > build/license.txt
	sed -e 's/\$$BVERSION/$(version)/g' $(src)/install.nsi |\
	sed -e 's/\$$BEXTENSIONS/$(extensions)/g' |\
        makensis -

clean_installer:
	rm -rf build Beremiz-$(version).exe $(targets) $(targets_ex)


