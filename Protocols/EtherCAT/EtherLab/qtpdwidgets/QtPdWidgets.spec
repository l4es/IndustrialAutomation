#----------------------------------------------------------------------------
#
# $Id$
#
# Copyright (C) 2009  Florian Pose <fp@igh-essen.com>
#
# This file is part of the QtPdWidgets library.
#
# The QtPdWidgets library is free software: you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the License,
# or (at your option) any later version.
#
# The QtPdWidgets library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
# General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with the QtPdWidgets Library. If not, see
# <http://www.gnu.org/licenses/>.
#
# vim: tw=78
#
#----------------------------------------------------------------------------

Name: QtPdWidgets
Version: 1.3.7
Release: 1

Summary: Process data widgets for Qt
License: LGPL-3.0+
Vendor: Ingenieurgemeinschaft IgH GmbH
Group: Productivity/Other
URL: http://etherlab.org/en/pdwidgets/index.php
Source: http://etherlab.org/download/pdwidgets/QtPdWidgets-%{version}.tar.bz2
BuildRoot: /tmp/%{name}-%{version}
BuildRequires: libqt4-devel
BuildRequires: libpdcom1
BuildRequires: pdcom-devel
BuildRequires: fdupes

#----------------------------------------------------------------------------
# Main Package
#----------------------------------------------------------------------------

%description
This is a bundle of process data widgets for Qt4. It uses the PdCom library to
access the actual process variables and it includes a plugin for the Qt
Designer. See http://etherlab.org/en/pdwidgets for more information.

#----------------------------------------------------------------------------
# Development package
#----------------------------------------------------------------------------

%package devel

Summary: Development files for Applications using %{name}
Group: Development/Libraries/C and C++

%description devel
This is a bundle of process data widgets for Qt4. It uses the PdCom library to
access the actual process variables and it includes a plugin for the Qt
Designer. See http://etherlab.org/en/pdwidgets for more information.

#----------------------------------------------------------------------------

%prep
%setup

%build
qmake PREFIX=%_prefix PDCOMPREFIX=%_prefix EXPATPREFIX=%_prefix
make

%install
qmake PREFIX=%_prefix PDCOMPREFIX=%_prefix EXPATPREFIX=%_prefix
make INSTALL_ROOT=%buildroot install
%fdupes %buildroot/%_prefix

%clean
rm -rf %buildroot

%files
%defattr(-,root,root)
%doc AUTHORS COPYING NEWS README TODO
/usr/lib*/libQtPdWidgets.so
/usr/lib*/qt4/plugins/designer/libQtPdWidgets.so

%files devel
%defattr(-,root,root)
/usr/include/QtPdWidgets
/usr/include/QtPdWidgets/*.h

#----------------------------------------------------------------------------
