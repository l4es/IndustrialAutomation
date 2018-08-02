#!/bin/sh

rm -f *.odb
./localclitest
./testalter
./testconcur
./testddl
./subsql testddl.sql
./testddl
./testfuzzy
./testidx
./testiref
./testleak
./testperf
./testindex
./testraw
./testsync
./testspat
./testconc update & ./testconc inspect 2


