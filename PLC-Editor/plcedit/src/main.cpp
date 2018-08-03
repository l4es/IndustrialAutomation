/****************************************************************************
**
** This file is part of PLCEdit, an open-source cross-platform editor 
** for PLC source files (e.g. from Moeller, CoDeSys and Siemens).
** Copyright (C) 2005-2010  M. Rehfeldt
**
** This software uses classes of Trolltech Qt toolkit and is freeware. 
** This file may be used under the terms of the GNU General Public License 
** version 2.0 or (at your option) any later version as published by the 
** Free Software Foundation and appearing in the file LICENSE.GPL included 
** in the packaging of this file. 
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** Contact e-mail: M. Rehfeldt <info@plcedit.org>
** Program URL   : http://www.plcedit.org
**
****************************************************************************/


#include <QtCore/QObject>


#include "mainwindow.h"
#include "singleapp/qtsingleapplication.h"

#ifdef Q_OS_WIN
  #define _WIN32_WINNT 0x0501
  #include <qt_windows.h>
#endif


int main(int argc, char **argv)
{ //main routine
	
  #ifdef Q_OS_WIN
    AllowSetForegroundWindow(ASFW_ANY);
  #endif

  QtSingleApplication instance("PLCEdit", argc, argv); //create instance
  //connect SIGNALS & SLOTS for quit app instance if last window is closed
  instance.connect(&instance, SIGNAL(lastWindowClosed()), &instance, SLOT(quit()));

  //collect arguments to message
  QString message;
  for (int i = 1; i < argc; i++) 
  {
    message += argv[i]; //concat arg
    if (i < argc-1)
      message += ";";	//add seperator if not last arg
  }

  //send message to instance and return 0 if an instance already exists
  if (instance.sendMessage(message))
    return 0;

  //create mainwindow show mainwindow and call handleMessage 
  MainWindow mw; 
  mw.show();
  mw.handleMessage(message);
  
  //connect SIGNALS & SLOTS for handle messages in mainwindow
  QObject::connect(&instance, SIGNAL(messageReceived(const QString&)), &mw, SLOT(handleMessage(const QString&)));
  //set window activation if no message is calling mainwindow
  instance.setActivationWindow(&mw, false); 
  //connect SIGNALS & SLOTS for set window active if needs to be shown
  QObject::connect(&mw, SIGNAL(needToShow()), &instance, SLOT(activateWindow()));


  return instance.exec();
}
