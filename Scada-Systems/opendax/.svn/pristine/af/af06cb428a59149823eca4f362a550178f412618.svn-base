#  PyDAX - A Python extension module for OpenDAX 
#  OpenDAX - An open source data acquisition and control system 
#  Copyright (c) 2011 Phil Birkelbach
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
# 
#  This is the setup script for the extension module
 
from distutils.core import setup, Extension

PackageName = 'PyDAX'
Description = 'Python Interface Module to the OpenDAX API'
Version = '0.6'
SourceFiles = ['pydax.c']
IncludeDirs = ['.']
Libraries = ['dax']

pydax = Extension('pydax',
                  include_dirs = IncludeDirs,
                  libraries = Libraries,
                  sources = SourceFiles)

setup(name = PackageName, version = Version,
      description = Description,
      ext_modules = [pydax])

