#!/bin/bash

#----------------------------------------------------------------------------
#
# $Id$
#
# Overcome the crappy implementation of qmake's dist target.
#
#----------------------------------------------------------------------------

distdir=$1
shift
distfiles=$@

set -e

rm -rf $distdir
mkdir $distdir

hg log --style=changelog > $distdir/ChangeLog

for f in $distfiles; do
    target=$distdir/`dirname $f`
    mkdir -p $target
    cp -a $f $target
done

tar cjvf $distdir.tar.bz2 $distdir
rm -rf $distdir

#----------------------------------------------------------------------------
