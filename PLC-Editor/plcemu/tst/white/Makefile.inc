CFLAGS+= --static -c -Wno-implicit-function-declaration -fprofile-arcs -ftest-coverage -std=gnu99 -Wno-unknown-pragmas  

LIBS= -lcunit -lgcov 

.SUFFIXES:
.SUFFIXES: .out .o .po .so .c .cc

LINUX_VERS=$(firstword $(subst ., ,$(shell uname -r)))

# Determine Major and minor release number for the compiler
GCC_MAJOR=$(word 1, $(subst ., ,$(shell gcc --version)))
GCC_MINOR=$(word 2, $(subst ., ,$(shell gcc --version)))

CC      ?= gcc
CXX     ?= g++
CPP      = cpp
LD       = ld
AR       = ar
RANLIB   = ranlib
SIZE     = size
NM       = nm
LORDER   = lorder


# General Make Environment
ifndef SHELL
SHELL=/bin/sh
endif
ifndef SH
SH=${SHELL}
endif
ifndef MAKE
MAKE=gmake
endif
ifndef ECHO
ECHO=echo
endif
ifndef DATE
DATE:=$(shell date +%m%d%Y)
endif
ifndef MAKEDEPEND
MAKEDEPEND=/usr/X11R6/bin/makedepend
endif
ifndef WHOAMI
WHOAMI:=$(shell id | sed 's/^[^(]*(\([^)]*\)).*$$/\1/')
endif
ifndef SED
SED=sed
endif
ifndef EGREP
EGREP=egrep
endif
ifndef SORT
SORT=sort
endif
ifndef AWK
AWK=awk
endif
ifndef PATCH
PATCH=patch
endif
ifndef CAT
CAT=cat
endif
ifndef GROFF
GROFF=groff
endif
ifndef TSORT
TSORT=tsort
endif
ifndef EMPTY
EMPTY=/dev/null
endif
ifndef PWD_MKDB
PWD_MKDB=pwd_mkdb
endif

# Filesystem Commands
RM=rm -f
ifndef CHMOD
CHMOD=chmod
endif
ifndef CHOWN
CHOWN=chown
endif
ifndef CP
CP=cp
endif
ifndef MV
MV=mv
endif
ifndef DD
DD=dd
endif
ifndef TAR
TAR=tar
endif
ifndef LINK
LINK=ln -sf
endif
ifndef 	HARDLINK
HARDLINK = ln -f
endif
ifndef GZIP
GZIP=gzip
endif
ifndef TOUCH
TOUCH=touch
endif
ifndef TEE
TEE=tee
endif

ifdef INSTALL_OWNER
INSTALL_OWNER_CMD=-o ${INSTALL_OWNER}
endif
ifdef INSTALL_GROUP
INSTALL_GROUP_CMD=-g ${INSTALL_GROUP}
endif
ifndef INSTALL_OWNER
INSTALL_OWNER=root
endif
ifndef INSTALL_GROUP
INSTALL_GROUP=wheel
endif
ifndef INSTALL_MODE
INSTALL_MODE=0644
endif
ifndef INSTALL_BIN_MODE
INSTALL_BIN_MODE=0755
endif
ifndef INSTALL_LIB_MODE
INSTALL_LIB_MODE=0444
endif
ifndef INSTALL_MAN_MODE
INSTALL_MAN_MODE=0444
endif
ifndef INSTALL_LOG_MODE
INSTALL_LOG_MODE=0600
endif
ifndef INSTALL
INSTALL=install -c ${INSTALL_OWNER_CMD} ${INSTALL_GROUP_CMD} -m ${INSTALL_MODE}
endif
ifndef INSTALL_BIN
INSTALL_BIN=install -c ${INSTALL_OWNER_CMD} ${INSTALL_GROUP_CMD} -m ${INSTALL_BIN_MODE}
endif
ifndef INSTALL_LIB
INSTALL_LIB=install -c ${INSTALL_OWNER_CMD} ${INSTALL_GROUP_CMD} -m ${INSTALL_LIB_MODE}
endif
ifndef INSTALL_MAN
INSTALL_MAN=install -c ${INSTALL_OWNER_CMD} ${INSTALL_GROUP_CMD} -m ${INSTALL_MAN_MODE}
endif
ifndef INSTALL_LOG
INSTALL_LOG=install -c ${INSTALL_OWNER_CMD} ${INSTALL_GROUP_CMD} -m ${INSTALL_LOG_MODE}
endif
ifndef INSTALL_PRODUCT
INSTALL_PRODUCT = install -c ${INSTALL_OWNER_CMD} ${INSTALL_GROUP_CMD} -m ${INSTALL_BIN_MODE}
endif
ifndef MKDIR
MKDIR=mkdir -p -m ${INSTALL_BIN_MODE}
endif

BASE_LDFLAGS      :=

BASE_CFLAGS       := -Wall -pedantic -g
ifdef DEBUG
 BASE_CFLAGS      += -DDEBUG
 BASE_CFLAGS      += -DTESTING
endif

BASE_PRE_IFLAGS   :=

BASE_POST_IFLAGS  :=

ALL_CFLAGS= ${BASE_CFLAGS} ${CFLAGS}
ALL_IFLAGS= ${BASE_PRE_IFLAGS} ${IFLAGS} ${BASE_POST_IFLAGS} ${CXX_IFLAGS}
ALL_LDFLAGS= ${BASE_LDFLAGS} ${LDFLAGS}
ALL_LIBS= ${LIBS} ${OTHER_LIBS} ${BASE_LIBS}

ifndef OBJS
OBJS= $(filter %.o, ${SRCS:.c=.o} ${SRCS:.cc=.o} ${SRCS:.C=.o} ${SRCS:.cpp=.o} ${SRCS:.cxx=.o} ${OTHER_OBJS})
endif

CLEAN_OTHERS+= ${OTHER_OBJS}

ifndef GCNO
GCNO= $(filter %.gcno, ${SRCS:.c=.gcno} ${SRCS:.cc=.gcno} ${SRCS:.C=.gcno} ${SRCS:.cpp=.gcno} ${SRCS:.cxx=.gcno} ${OTHER_GCNO})
endif

#  The do-subdirs function is used by recursive targets like "all" and "clean"
#    It calls "make" on each of the directories listed in SUBDIRS

ifndef do-subdirs
define do-subdirs
	@for entry in ${SUBDIRS}; do \
		(if test -d $${entry}; then \
			${MAKE} -C $${entry} $@; \
		else \
			echo "*** Warning:  couldn't find subdirectory $${entry}"; \
		fi); \
	done
endef
endif

# vi:set syntax=make:

