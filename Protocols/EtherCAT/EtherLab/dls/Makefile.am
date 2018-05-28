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

ACLOCAL_AMFLAGS = -I m4

SUBDIRS =

if ENABLE_SERVER
SUBDIRS += proto
endif

SUBDIRS += \
	lib \
	daemon \
	fltkguis \
	script \
	tool

EXTRA_DIST = \
	bootstrap.sh \
	dls.spec \
	dls.spec.in \
	gui/AboutDialog.cpp \
	gui/AboutDialog.h \
	gui/AboutDialog.ui \
	gui/LogWindow.cpp \
	gui/LogWindow.h \
	gui/LogWindow.ui \
	gui/MainWindow.cpp \
	gui/MainWindow.h \
	gui/MainWindow.ui \
	gui/SettingsDialog.cpp \
	gui/SettingsDialog.h \
	gui/SettingsDialog.ui \
	gui/UriDialog.cpp \
	gui/UriDialog.h \
	gui/UriDialog.ui \
	gui/dlsgui-mime.xml \
	gui/dlsgui.desktop \
	gui/dlsgui.pro \
	gui/dlsgui.qrc \
	gui/dlsgui.rc \
	gui/dlsgui_de.ts \
	gui/images/Edit-delete.svg \
	gui/images/Folder-remote.svg \
	gui/images/Folder.svg \
	gui/images/Network-idle.svg \
	gui/images/dlsgui-view.png \
	gui/images/dlsgui-view.svg \
	gui/images/dlsgui.ico \
	gui/images/dlsgui.png \
	gui/images/dlsgui.svg \
	gui/images/document-new.svg \
	gui/images/document-open.svg \
	gui/images/document-save-as.svg \
	gui/images/document-save.svg \
	gui/images/drive-harddisk.svg \
	gui/images/etherlabsign.svg \
	gui/images/system-log-out.svg \
	gui/images/view-refresh.svg \
	gui/main.cpp \
	gui/modeltest.cpp \
	gui/modeltest.h \
	revision.sh \
	widgets/Channel.cpp \
	widgets/Channel.h \
	widgets/ColorDelegate.cpp \
	widgets/ColorDelegate.h \
	widgets/DatePickerDialog.cpp \
	widgets/DatePickerDialog.h \
	widgets/DatePickerDialog.ui \
	widgets/Dir.cpp \
	widgets/Dir.h \
	widgets/DlsWidgets.pro \
	widgets/DlsWidgets.qrc \
	widgets/DlsWidgets/Graph.h \
	widgets/DlsWidgets/Layer.h \
	widgets/DlsWidgets/Model.h \
	widgets/DlsWidgets/Scale.h \
	widgets/DlsWidgets/Section.h \
	widgets/DlsWidgets/Translator.h \
	widgets/DlsWidgets/ValueScale.h \
	widgets/DlsWidgets_de.ts \
	widgets/ExportDialog.cpp \
	widgets/ExportDialog.h \
	widgets/ExportDialog.ui \
	widgets/FilterDialog.cpp \
	widgets/FilterDialog.h \
	widgets/FilterDialog.ui \
	widgets/Graph.cpp \
	widgets/Job.cpp \
	widgets/Job.h \
	widgets/Layer.cpp \
	widgets/Model.cpp \
	widgets/Node.cpp \
	widgets/Node.h \
	widgets/Plugin.cpp \
	widgets/Plugin.h \
	widgets/Scale.cpp \
	widgets/Section.cpp \
	widgets/SectionDialog.cpp \
	widgets/SectionDialog.h \
	widgets/SectionDialog.ui \
	widgets/SectionModel.cpp \
	widgets/SectionModel.h \
	widgets/TODO \
	widgets/Translator.cpp \
	widgets/ValueScale.cpp \
	widgets/WidgetCollection.cpp \
	widgets/WidgetCollection.h \
	widgets/images/Network-error.svg \
	widgets/images/Network-idle.svg \
	widgets/images/dialog-error.svg \
	widgets/images/dialog-information.svg \
	widgets/images/dialog-warning.svg \
	widgets/images/document-print.svg \
	widgets/images/document-properties.svg \
	widgets/images/document-save.svg \
	widgets/images/drive-harddisk.svg \
	widgets/images/edit-redo.svg \
	widgets/images/edit-undo.svg \
	widgets/images/go-next.svg \
	widgets/images/list-remove.svg \
	widgets/images/measure.svg \
	widgets/images/messages.svg \
	widgets/images/system-search.svg \
	widgets/images/utilities-system-monitor.svg \
	widgets/images/view-fullscreen.svg \
	widgets/images/view-refresh.svg \
	widgets/updateqm.pri

BUILT_SOURCES = \
	dls.spec

mydist:
	@REV=`hg id -i $(srcdir)` && \
		$(MAKE) dist-bzip2 distdir=$(PACKAGE)-$(VERSION)-$${REV}

dist-hook:
	if which hg >/dev/null 2>&1; then \
		hg id -i $(srcdir) 2>/dev/null >$(distdir)/revision; \
	fi

#------------------------------------------------------------------------------
