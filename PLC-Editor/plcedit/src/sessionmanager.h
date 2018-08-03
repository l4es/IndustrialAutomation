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


#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QDialog>

#include "ui_SessionDialog.h"

class MainWindow; 	//get access to MainWindow class
class PageCtrl; 	//get access to PageCtrl class
class ImportExport;	//get access to ImportExport class


class SessionManager : public QDialog, public Ui::SessionDialog
{
  Q_OBJECT
public:
	SessionManager(MainWindow *parent, PageCtrl *pageCtrl, ImportExport *importExport); //MainWindow & PageCtrl are a member VARs of this class
	virtual ~SessionManager();

    bool openAutoSession; 	//!< flag open auto session at start-up
    bool saveAutoSession; 	//!< flag save auto session file
    bool saveSessionAsPrj;	//!< flag auto session as project
    bool showErrorList;		//!< flag show error list for sessions
    
    void saveFilesAsSession(QString sessionName);

signals:
  	void sessionSaved();	//!< signal session is saved
    
public slots:
        void checkSessDir(QString dir);
   	void loadSession();
        void retranslateUi();
        void saveSession();
        void searchSessions();
        void setButtonsState();

private slots:
	void deleteSession();
	
private:
        bool loadFilesFromSession();
	
    QString sessDir;
    QString autoSessDir;
    
    QString msg_error_sessionopen;
    QString msg_error_sessionsave;
    QString msg_head_sessioncreate;

	//Instances
	MainWindow *m_MainWindow; 		//!< instance of MainWindow class
    PageCtrl *m_PageCtrl; 			//!< instance of PageCtrl class	
    ImportExport *m_ImportExport;	//!< instance of ImportExport class
};
#endif
