/**********************************************************************
** Copyright (C) 2000-2007 Trolltech ASA.  All rights reserved.
**
** This file is part of Qt Designer.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "qplatformdefs.h"

// SCO OpenServer redefines raise -> kill
#if defined(raise)
# undef raise
#endif

#include "mainwindow.h"
#include "formwindow.h"

#include "designerapp.h"

#include <qtextstream.h>
#include <qobjectlist.h>
#include <qsettings.h>
#include <qsplashscreen.h>
#include <qdir.h>

#include <stdlib.h>
#include <signal.h>

// SCO OpenServer redefines raise -> kill
#if defined(raise)
# undef raise
#endif

#if defined(Q_WS_WIN)
#include <qt_windows.h>
#include <process.h>
#endif

#if defined(Q_OS_UNIX)
#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

static void signalHandler( QT_SIGNAL_ARGS )
{
    QFile f( QDir::homeDirPath() + "/.designerargs" );
    f.open( IO_ReadOnly );
    QString args;
    f.readLine( args, f.size() );
    QStringList lst = QStringList::split( " ", args );
    for ( QStringList::Iterator it = lst.begin(); it != lst.end(); ++it ) {
	QString arg = (*it).stripWhiteSpace();
	if ( arg[0] != '-' ) {
	    QObjectList* l = MainWindow::self->queryList( "FormWindow" );
	    FormWindow* fw = (FormWindow*) l->first();
#if 0 // ### what's this dead code for?
	    FormWindow* totop;
#endif
	    bool haveit = FALSE;
	    while ( fw ) {
		haveit = haveit || fw->fileName() == arg;
#if 0 // ### what's this dead code for?
		if ( haveit )
		    totop = fw;
#endif
		fw = (FormWindow*) l->next();
	    }
	    if ( !haveit )
		MainWindow::self->openFormWindow( arg );
	}
    }
    MainWindow::self->raise();
    MainWindow::self->setActiveWindow();
}

#if defined(Q_C_CALLBACKS)
}
#endif

#endif

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

static void exitHandler( QT_SIGNAL_ARGS )
{
    QDir d( QDir::homeDirPath() );
    d.remove( ".designerpid" );
    exit( -1 );
}

#if defined(Q_C_CALLBACKS)
}
#endif

#include "qwidgetfactory.h" //apa+++ 13-07-2012

int main( int argc, char *argv[] )
{
    QApplication::setColorSpec( QApplication::ManyColor );

    DesignerApplication a( argc, argv );

    DesignerApplication::setOverrideCursor( Qt::WaitCursor );

	QWidgetFactory::loadImages("../Bitmaps"); //apa+++ 13-07-2012

	QString hm_dir = QDir::homeDirPath();

    bool creatPid = FALSE;
    if ( a.argc() > 1 ) {
	QString arg1 = a.argv()[ 1 ];
	if ( arg1 == "-client" ) {
	    QFile pf( QDir::homeDirPath() + "/.designerpid" );
	    if ( pf.exists() && pf.open( IO_ReadOnly ) ) {
		QString pidStr;
		pf.readLine( pidStr, pf.size() );
		QFile f( QDir::homeDirPath() + "/.designerargs" );
		f.open( IO_WriteOnly );
		QTextStream ts( &f );
		for ( int i = 1; i < a.argc(); ++i )
		    ts << a.argv()[ i ] << " ";
		ts << endl;
		f.close();
#if defined(Q_OS_UNIX)
		if ( kill( pidStr.toInt(), SIGUSR1 ) == -1 )
		    creatPid = TRUE;
		else
		    return 0;
#elif defined(Q_OS_WIN32)
		if ( !GetProcessVersion( pidStr.toUInt() ) ) {
		    creatPid = TRUE;
		} else {
		    if ( a.winVersion() & Qt::WV_NT_based )
			    SendMessage( HWND_BROADCAST, RegisterWindowMessage((TCHAR*)"QT_DESIGNER_OPEN_FILE"), 0, 0 );
		    else
			    SendMessage( HWND_BROADCAST, RegisterWindowMessageA("QT_DESIGNER_OPEN_FILE"), 0, 0 );
		    return 0;
		}
#endif
	    } else {
		creatPid = TRUE;
	    }
	} else if(arg1 == "-debug_stderr") {
	    extern bool debugToStderr; //outputwindow.cpp
	    debugToStderr = TRUE;
	}
    }

    if ( creatPid ) {
	QFile pf( QDir::homeDirPath() + "/.designerpid" );
	pf.open( IO_WriteOnly );
	QTextStream ts( &pf );
#if defined(Q_OS_UNIX)
	signal( SIGUSR1, signalHandler );
#endif
	ts << getpid() << endl;

	pf.close();
	signal( SIGABRT, exitHandler );
	signal( SIGFPE, exitHandler );
	signal( SIGILL, exitHandler );
	signal( SIGINT, exitHandler );
	signal( SIGSEGV, exitHandler );
	signal( SIGTERM, exitHandler );
    }

    extern void qInitImages_designerlib();
    qInitImages_designerlib();

    QSettings config;
    QString keybase = DesignerApplication::settingsKey();
    config.insertSearchPath( QSettings::Windows, "/Enscada" ); //apa modified
    QStringList pluginPaths = config.readListEntry( keybase + "PluginPaths" );
    if (pluginPaths.count())
	QApplication::setLibraryPaths(pluginPaths);
	
	QApplication::addLibraryPath("../plugins");

    QSplashScreen *splash = a.showSplash();

	MainWindow *mw = new MainWindow( creatPid, FALSE, "/designer" );
    a.setMainWidget( mw );
    mw->setCaption( "IndigoSCADA HMI designer" );

//apa begin removed
//    MainWindow *mw = new MainWindow( creatPid );
//    a.setMainWidget( mw );
//#if defined(QT_NON_COMMERCIAL)
//    mw->setCaption( "Qt Designer by Trolltech for non-commercial use" );
//#else
//    mw->setCaption( "Qt Designer by Trolltech" );
//#endif
//  if ( config.readBoolEntry( keybase + "Geometries/MainwindowMaximized", FALSE ) ) {
//	int x = config.readNumEntry( keybase + "Geometries/MainwindowX", 0 );
//	int y = config.readNumEntry( keybase + "Geometries/MainwindowY", 0 );
//	mw->move( x, y );
//	mw->showMaximized();
//    } else {
	mw->show();
//    }
    if ( splash )
	splash->finish( mw );
    delete splash;

    QApplication::restoreOverrideCursor();
    for ( int i = 1; i < a.argc(); ++i ) {
	QString arg = a.argv()[ i ];
	if ( arg[0] != '-' )
	    mw->fileOpen( "", "", arg );
    }

    return a.exec();
}
