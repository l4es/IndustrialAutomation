# -*- makefile -*-
# Makefile for Generic Unix with GCC compiler

# Place where to copy GastDB header files
INCSPATH=/usr/local/include/gigabase

#Place where to copy Gigabase library
LIBSPATH=/usr/local/lib

#Place where to copy Gigabase subsql utility
BINSPATH=/usr/local/bin

OBJS = class.o compiler.o database.o replicator.o hashtab.o file.o symtab.o btree.o sync.o \
	cursor.o query.o pagepool.o wwwapi.o unisock.o blob.o container.o rtree.o exception.o localcli.o

CLI_OBJS = cli.o unisock.o repsock.o

INCS = gigabase.h stdtp.h class.h database.h cursor.h selection.h reference.h wwwapi.h array.h \
	cli.h file.h hashtab.h btree.h sync.h query.h datetime.h pagepool.h blob.h container.h rectangle.h timeseries.h config.h confdefs.h exception.h

CLI_INCS = cli.h

# Uncomment this line to support database encryption at CLI level
# CRYPT_LIBRARY=crypt/libcryptgb.a


#
# threads settings - please comment this lines for single-threaded libs
#
ifdef NO_PTHREADS
TFLAGS=-DNO_PTHREADS
THRLIBS=
SUFF=
else
ifneq (,$(findstring freebsd,$(OSTYPE)))
TFLAGS=-pthread
THRLIBS=
else 
TFLAGS=-D_REENTRANT -D_THREAD_SAFE
ifneq (,$(findstring bsdi,$(OSTYPE)))
THRLIBS=
else
THRLIBS=-lpthread
endif
endif
SUFF=_r
endif

STDLIBS = -lm  $(THRLIBS) $(CRYPT_LIBRARY)
ifneq (,$(findstring solaris,$(OSTYPE)))
SOCKLIBS = -lnsl -lsocket
TFLAGS += -fPIC
else
SOCKLIBS =
endif

VERSION=2
GB_LIB = libgigabase$(SUFF).a
GB_SHARED = libgigabase$(SUFF).so.${VERSION}
CLI_LIB = libcli$(SUFF).a
CLI_SHARED = libcli$(SUFF).so.${VERSION}
EXES = subsql forcerecovery
#GB_LIBS=-L. -lgigabase$(SUFF)
#CLI_LIBS=-L. -lcli$(SUFF)
GB_LIBS=$(GB_LIB)
CLI_LIBS=$(CLI_LIB)

EXAMPLES = guess testdb testleak testjoin testddl testperf testbatch testperf2 testsync testiref testtrav testidx testidx2 clitest clitest2 testalter localclitest testblob testraw testspat testreplic testsort

WEB_EXAMPLES = cgistub bugdb clidb

CC = g++
# !!! DEFS macro is deprecated - edit config.h header file instead
DEFS =

ifdef CRYPT_LIBRARY
DEFS = $(DEFS) -DSUPPORT_DATA_ENCRYPTION
endif


#CFLAGS = -c -Wall -O5 -g $(DEFS) $(TFLAGS)
CFLAGS = -c -Wall -O0 -g $(DEFS) $(TFLAGS)
#CFLAGS = -c -Wall -O0 -DGIGABASE_DEBUG=DEBUG_TRACE -g $(DEFS) $(TFLAGS)
SHFLAGS=-shared


LD = $(CC)
LDFLAGS = -g $(TFLAGS)

AR = ar
ARFLAGS = -cru

ifneq (,$(findstring freebsd,$(OSTYPE)))
RANLIB = ranlib
else
RANLIB = true
endif


all: $(GB_SHARED) $(CLI_SHARED) $(GB_LIB) $(CLI_LIB) $(EXES) $(EXAMPLES)

www: $(GB_SHARED) $(WEB_EXAMPLES)


class.o: class.cpp compiler.h compiler.d symtab.h $(INCS)
	$(CC) $(CFLAGS) class.cpp

compiler.o: compiler.cpp compiler.h compiler.d symtab.h $(INCS)
	$(CC) $(CFLAGS) compiler.cpp

sync.o: sync.cpp sync.h stdtp.h
	$(CC) $(CFLAGS) sync.cpp

query.o: query.cpp compiler.h compiler.d symtab.h $(INCS)
	$(CC) $(CFLAGS) query.cpp

database.o: database.cpp compiler.h compiler.d symtab.h $(INCS)
	$(CC) $(CFLAGS) database.cpp

localcli.o: localcli.cpp compiler.h compiler.d symtab.h localcli.h $(INCS)
	$(CC) $(CFLAGS) localcli.cpp

replicator.o: replicator.cpp replicator.h sockio.h $(INCS)
	$(CC) $(CFLAGS) replicator.cpp

cursor.o: cursor.cpp compiler.h compiler.d $(INCS)
	$(CC) $(CFLAGS) cursor.cpp

hashtab.o: hashtab.cpp $(INCS)
	$(CC) $(CFLAGS) hashtab.cpp

pagepool.o: pagepool.cpp $(INCS)
	$(CC) $(CFLAGS) pagepool.cpp

file.o: file.cpp $(INCS)
	$(CC) $(CFLAGS) file.cpp

symtab.o: symtab.cpp symtab.h $(INCS)
	$(CC) $(CFLAGS) symtab.cpp

btree.o: btree.cpp $(INCS)
	$(CC) $(CFLAGS) btree.cpp

rtree.o: rtree.cpp $(INCS)
	$(CC) $(CFLAGS) rtree.cpp

exception.o: exception.cpp $(INCS)
	$(CC) $(CFLAGS) exception.cpp

blob.o: blob.cpp $(INCS)
	$(CC) $(CFLAGS) blob.cpp

container.o: container.cpp $(INCS)
	$(CC) $(CFLAGS) container.cpp

wwwapi.o: wwwapi.cpp wwwapi.h sockio.h stdtp.h sync.h
	$(CC) $(CFLAGS) wwwapi.cpp

unisock.o: unisock.cpp unisock.h sockio.h stdtp.h
	$(CC) $(CFLAGS) unisock.cpp

repsock.o: repsock.cpp unisock.h sockio.h stdtp.h sync.h
	$(CC) $(CFLAGS) repsock.cpp

cli.o: cli.cpp cli.h cliproto.h repsock.h sockio.h stdtp.h sync.h
	$(CC) $(CFLAGS) cli.cpp

libgigabase$(SUFF).a: $(OBJS)
	rm -f libgigabase$(SUFF).a
	$(AR) $(ARFLAGS) libgigabase$(SUFF).a $(OBJS)
	$(RANLIB) libgigabase$(SUFF).a

libgigabase$(SUFF).so.$(VERSION): $(OBJS)
	rm -f libgigabase$(SUFF).so.$(VERSION)
	$(CC) $(SHFLAGS) -o libgigabase$(SUFF).so.$(VERSION) $(OBJS)
	ln -f -s libgigabase$(SUFF).so.$(VERSION) libgigabase$(SUFF).so

libcli$(SUFF).a: $(CLI_OBJS)
	rm -f libcli$(SUFF).a
	$(AR) $(ARFLAGS) libcli$(SUFF).a $(CLI_OBJS)
	$(RANLIB) libcli$(SUFF).a

libcli$(SUFF).so.$(VERSION): $(CLI_OBJS)
	rm -f libcli$(SUFF).so.$(VERSION)
	$(CC) $(SHFLAGS) -o libcli$(SUFF).so.$(VERSION) $(CLI_OBJS)
	ln -f -s libcli$(SUFF).so.$(VERSION) libcli$(SUFF).so


clitest.o: clitest.c cli.h
	$(CC) $(CFLAGS) clitest.c

clitest: clitest.o $(CLI_LIB)
	$(LD) $(LDFLAGS) -o clitest clitest.o $(CLI_LIBS) $(STDLIBS) $(SOCKLIBS)

clitest2.o: clitest2.c cli.h
	$(CC) $(CFLAGS) clitest2.c

clitest2: clitest2.o $(GB_LIB)
	$(LD) $(LDFLAGS) -o clitest2 clitest2.o $(GB_LIB) $(STDLIBS)

testalter.o: testalter.c cli.h
	$(CC) $(CFLAGS) testalter.c

testalter: testalter.o $(GB_LIB)
	$(LD) $(LDFLAGS) -o testalter testalter.o $(GB_LIB) $(STDLIBS)

localclitest: clitest.o $(GB_LIB)
	$(LD) $(LDFLAGS) -o localclitest clitest.o $(GB_LIB) $(STDLIBS)

subsql.o: subsql.cpp subsql.h compiler.h compiler.d symtab.h $(INCS)
	$(CC) $(CFLAGS) subsql.cpp

ifdef NO_PTHREADS
subsql: subsql.o $(GB_LIB)
	$(LD) $(LDFLAGS) -o subsql subsql.o $(GB_LIBS) $(STDLIBS) $(SOCKLIBS) 
else
server.o: server.cpp server.h cli.h cliproto.h subsql.h compiler.h compiler.d symtab.h $(INCS)
	$(CC) $(CFLAGS) server.cpp

subsql: subsql.o server.o $(GB_LIB)
	$(LD) $(LDFLAGS) -o subsql subsql.o server.o $(GB_LIBS) $(STDLIBS) $(SOCKLIBS)
endif

bugdb.o: bugdb.cpp bugdb.h $(INCS)
	$(CC) $(CFLAGS) bugdb.cpp

bugdb: bugdb.o  $(GB_LIB)
	$(LD) $(LDFLAGS) -o bugdb bugdb.o $(GB_LIBS) $(STDLIBS) $(SOCKLIBS)

clidb.o: clidb.cpp clidb.h $(INCS)
	$(CC) $(CFLAGS) clidb.cpp

clidb: clidb.o  $(GB_LIB)
	$(LD) $(LDFLAGS) -o clidb clidb.o $(GB_LIBS) $(STDLIBS) $(SOCKLIBS)

forcerecovery.o: forcerecovery.cpp database.h
	$(CC) $(CFLAGS) forcerecovery.cpp 

forcerecovery: forcerecovery.o
	$(LD) $(LDFLAGS)  -o forcerecovery forcerecovery.o

cgistub.o: cgistub.cpp stdtp.h sockio.h
	$(CC) $(CFLAGS) cgistub.cpp

cgistub: cgistub.o
	$(LD) $(LDFLAGS) -o cgistub cgistub.o $(GB_LIBS) $(STDLIBS) $(SOCKLIBS)

guess.o: guess.cpp $(INCS)
	$(CC) $(CFLAGS) guess.cpp

guess: guess.o $(GB_LIB)
	$(LD) $(LDFLAGS) -o guess guess.o $(GB_LIBS) $(STDLIBS)

testdb.o: testdb.cpp $(INCS)
	$(CC) $(CFLAGS) testdb.cpp

testdb: testdb.o $(GB_LIB)
	$(LD) $(LDFLAGS) -o testdb testdb.o $(GB_LIBS) $(STDLIBS)

testraw.o: testraw.cpp $(INCS)
	$(CC) $(CFLAGS) testraw.cpp

testraw: testraw.o $(GB_LIB)
	$(LD) $(LDFLAGS) -o testraw testraw.o $(GB_LIBS) $(STDLIBS)

testleak.o: testleak.cpp $(INCS)
	$(CC) $(CFLAGS) testleak.cpp

testleak: testleak.o $(GB_LIB)
	$(LD) $(LDFLAGS) -o testleak testleak.o $(GB_LIBS) $(STDLIBS)

testjoin.o: testjoin.cpp $(INCS)
	$(CC) $(CFLAGS) testjoin.cpp

testjoin: testjoin.o $(GB_LIB)
	$(LD) $(LDFLAGS) -o testjoin testjoin.o $(GB_LIBS) $(STDLIBS)

testddl.o: testddl.cpp $(INCS)
	$(CC) $(CFLAGS) testddl.cpp

testddl: testddl.o $(GB_LIB)
	$(LD) $(LDFLAGS) -o testddl testddl.o $(GB_LIBS) $(STDLIBS)

testblob.o: testblob.cpp $(INCS)
	$(CC) $(CFLAGS) testblob.cpp

testblob: testblob.o $(GB_LIB)
	$(LD) $(LDFLAGS) -o testblob testblob.o $(GB_LIBS) $(STDLIBS)

testperf.o: testperf.cpp $(INCS)
	$(CC) $(CFLAGS) testperf.cpp

testperf: testperf.o $(GB_LIB)
	$(LD) $(LDFLAGS) -o testperf testperf.o $(GB_LIBS) $(STDLIBS)

testbatch.o: testbatch.cpp $(INCS)
	$(CC) $(CFLAGS) testbatch.cpp

testbatch: testbatch.o $(GB_LIB)
	$(LD) $(LDFLAGS) -o testbatch testbatch.o $(GB_LIBS) $(STDLIBS)

testspat.o: testspat.cpp $(INCS)
	$(CC) $(CFLAGS) testspat.cpp

testspat: testspat.o $(GB_LIB)
	$(LD) $(LDFLAGS) -o testspat testspat.o $(GB_LIBS) $(STDLIBS)

testsort.o: testsort.cpp $(INCS)
	$(CC) $(CFLAGS) testsort.cpp

testsort: testsort.o $(GB_LIB)
	$(LD) $(LDFLAGS) -o testsort testsort.o $(GB_LIBS) $(STDLIBS)

testreplic.o: testreplic.cpp replicator.h $(INCS)
	$(CC) $(CFLAGS) testreplic.cpp

testreplic: testreplic.o $(GB_LIB)
	$(LD) $(LDFLAGS) -o testreplic testreplic.o $(GB_LIB) $(STDLIBS) $(SOCKLIBS)

testinc.o: testinc.cpp $(INCS)
	$(CC) $(CFLAGS) testinc.cpp

testinc: testinc.o $(GB_LIB)
	$(LD) $(LDFLAGS) -o testinc testinc.o $(GB_LIBS) $(STDLIBS)

testperf2.o: testperf2.cpp $(INCS)
	$(CC) $(CFLAGS) testperf2.cpp

testperf2: testperf2.o $(GB_LIB)
	$(LD) $(LDFLAGS) -o testperf2 testperf2.o $(GB_LIBS) $(STDLIBS)

testidx.o: testidx.cpp $(INCS)
	$(CC) $(CFLAGS) testidx.cpp

testidx: testidx.o $(GB_LIB)
	$(LD) $(LDFLAGS) -o testidx testidx.o $(GB_LIBS) $(STDLIBS)

testidx2.o: testidx2.cpp $(INCS)
	$(CC) $(CFLAGS) testidx2.cpp

testidx2: testidx2.o $(GB_LIB)
	$(LD) $(LDFLAGS) -o testidx2 testidx2.o $(GB_LIBS) $(STDLIBS)

testsync.o: testsync.cpp $(INCS)
	$(CC) $(CFLAGS) testsync.cpp

testsync: testsync.o $(GB_LIB)
	$(LD) $(LDFLAGS) -o testsync testsync.o $(GB_LIBS) $(STDLIBS)

testiref.o: testiref.cpp $(INCS)
	$(CC) $(CFLAGS) testiref.cpp

testiref: testiref.o $(GB_LIB)
	$(LD) $(LDFLAGS) -o testiref testiref.o $(GB_LIBS) $(STDLIBS)

testtrav.o: testtrav.cpp $(INCS)
	$(CC) $(CFLAGS) testtrav.cpp

testtrav: testtrav.o $(GB_LIB)
	$(LD) $(LDFLAGS) -o testtrav testtrav.o $(GB_LIBS) $(STDLIBS)

java: javacli.jar CliTest.class

javacli.jar: javacli/*.java
	javac -g javacli/*.java
	jar cvf javacli.jar javacli/*.class
	javadoc -public javacli

CliTest.class: CliTest.java javacli.jar
	javac CliTest.java



install: subsql installib
	mkdir -p $(BINSPATH)
	cp subsql $(BINSPATH)
	strip $(BINSPATH)/subsql

installib: $(GB_LIB) $(CLI_LIB) $(GB_SHARED) $(CLI_SHARED)
	mkdir -p $(INCSPATH)
	cp $(INCS) $(CLI_INCS) $(INCSPATH)
	mkdir -p $(LIBSPATH)
	cp $(GB_LIB) $(CLI_LIB) $(GB_SHARED) $(CLI_SHARED) $(LIBSPATH)
	ln -f -s $(LIBSPATH)/$(GB_SHARED) $(LIBSPATH)/libgigabase$(SUFF).so
	ln -f -s $(LIBSPATH)/$(CLI_SHARED) $(LIBSPATH)/libcli$(SUFF).so
	cp subsql $(BINSPATH)
	strip $(BINSPATH)/subsql


cleanobj:
	rm -fr *.o *.tgz core *~ javacli/*~ javacli/*.class *.jar *.class  cxx_repository config.log config.status libtool Makefile

cleandbs:
	rm -f *.dbs


clean: cleanobj cleandbs
	rm -fr *.a *.la *.lo .libs *.so *.so.* $(EXAMPLES) $(WEB_EXAMPLES) $(EXES)

tgz:	clean
	chmod +x mkversion install-sh configure bootstrap  depcomp ltconfig mkinstalldirs
	cd ..; tar cvzf gigabase.tgz gigabase

copytgz: tgz
	mcopy -o ../gigabase.tgz a:
