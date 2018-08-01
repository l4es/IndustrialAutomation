#!/usr/bin/env bash

# This script is to be executed as root to provision necessary stuff
# to run distribution build on a blank Ubuntu Xenial 64 image

set -xe
dpkg --add-architecture i386
apt-get update

apt-get install -y --no-install-recommends \
     autoconf        \
     automake        \
     bison           \
     build-essential \
     ca-certificates \
     flex            \
     gettext         \
     git             \
     lzma            \
     make            \
     mercurial       \
     mingw-w64       \
     nsis            \
     swig            \
     texinfo         \
     unrar           \
     unzip           \
     wget            \
     xvfb

     echo ttf-mscorefonts-installer msttcorefonts/accepted-mscorefonts-eula select true | debconf-set-selections
     echo ttf-mscorefonts-installer msttcorefonts/present-mscorefonts-eula note | debconf-set-selections

apt-get install -y --install-recommends \
     wine

apt-get clean -y
