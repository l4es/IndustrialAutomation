#------------------------------------------------------------------------------
#
# $Id$
#
# This file is part of the Data Logging Service (DLS).
#
# DLS is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# DLS is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along
# with DLS. If not, see <http://www.gnu.org/licenses/>.
#
#------------------------------------------------------------------------------

initdir = $(sysconfdir)/init.d
profiledir = $(sysconfdir)/profile.d
sysdir = $(sysconfdir)/sysconfig

noinst_SCRIPTS = dls_quota.pl dls_status.pl init.sh profile.sh sysconfig
EXTRA_DIST = $(noinst_SCRIPTS)

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	$(mkinstalldirs) $(DESTDIR)$(initdir)
	$(mkinstalldirs) $(DESTDIR)$(profiledir)
	$(mkinstalldirs) $(DESTDIR)$(sysdir)
	$(INSTALL_SCRIPT) $(srcdir)/dls_quota.pl $(DESTDIR)$(bindir)/dls_quota
	$(INSTALL_SCRIPT) $(srcdir)/dls_status.pl $(DESTDIR)$(bindir)/dls_status
	$(INSTALL_SCRIPT) $(srcdir)/init.sh $(DESTDIR)$(initdir)/dls
	$(INSTALL_DATA) $(srcdir)/profile.sh $(DESTDIR)$(profiledir)/dls.sh
	$(INSTALL_DATA) $(srcdir)/sysconfig $(DESTDIR)$(sysdir)/dls

#------------------------------------------------------------------------------
