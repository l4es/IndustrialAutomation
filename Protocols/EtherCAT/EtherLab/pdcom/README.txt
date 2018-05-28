
$Id$

This is the README file of the PdCom library, that is part of the EtherLab
project (http://etherlab.org/en). The home of PdCom is
http://etherlab.org/en/pdcom. The Library is released under the terms and
conditions of the GNU Lesser General Public License (LGPL), version 3 or (at
your option) any later version.

*** Documentation ***

The library documentation resides in the header files and can be brought to
HTML, LaTeX and man format with the doxygen tool. To generate the
documentation, make sure that doxygen is installed and then call:

    mkdir build
    cd build
    cmake ..
    make doc

Then point your favourite browser to doc/html/index.html.

*** Building and Installing ***

To just build and install the library, call:

    mkdir build
    cd build
    cmake ..
    make
    make install

** Further Information ***

For questions of any kind, subscribe to the etherlab-users mailing list at
http://etherlab.org/en.

Have fun with PdCom!

