#!/bin/sh

#The next line should be glibtoolize for Mac OSX
# or libtoolize for Linux and others.  I tend to switch
# back and forth a lot so the repository could have
# it either way.
#glibtoolize --copy --force \
#&& autoheader \
#&& aclocal \
#&& automake --gnu --copy --add-missing \
#&& autoconf

autoreconf -iv
