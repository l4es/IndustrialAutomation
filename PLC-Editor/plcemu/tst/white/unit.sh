#!/bin/bash

echo    "- Starting UT for PLCEMU"
echo    ""
echo    "- Checkout if needed-"

unlink ./vm/plclib.c
link ../../src/vm/plclib.c ./vm/plclib.c

unlink ./vm/parser-il.c
link ../../src/vm/parser-il.c ./vm/parser-il.c

unlink ./vm/parser-ld.c
link ../../src/vm/parser-ld.c ./vm/parser-ld.c

unlink ./vm/parser-tree.c
link ../../src/vm/parser-tree.c ./vm/parser-tree.c

unlink ./vm/codegen.c
link ../../src/vm/codegen.c ./vm/codegen.c

unlink ./vm/init.c
link ../../src/vm/init.c ./vm/init.c

unlink ./config.c
link ../../src/config.c ./app/config.c

export OBJFORMAT=elf
export CC=gcc
date

echo    "- Compiling -"
make clean all

if [ ! -f vm/report.htm ]; 
then
    echo "Coverage FAILED (See report.htm)"
    exit 1
else
    echo -n "OK "
fi

echo " "

