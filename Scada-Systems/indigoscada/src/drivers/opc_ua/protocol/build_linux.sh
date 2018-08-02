#! /bin/bash
set -e
make -f linux_gcc.mak BUILD_TARGET=debug "$@"
make -f linux_gcc.mak BUILD_TARGET=release "$@"
