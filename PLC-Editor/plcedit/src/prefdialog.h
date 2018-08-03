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


#ifndef PREFDIALOG_H
#define PREFDIALOG_H

#include <QDialog>
#include <QtCore/QTranslator>

#include "ui_PrefDialog.h"

class MainWindow; 		//get access to MainWindow class
class PageCtrl; 		//get access to PageCtrl class
class SessionManager;	//get access to SessionManager class
class ImportExport;		//get access to ImportExport class
class PrintPrepare;		//get access to PrintPrepare class
class POUInfoDialog;	//get access to POU info dialog class
class FileViewDialog;	//get access to file view dialog class
class FindDialog;		//get access to find dialog class
class FncDialog;		//get access to fcn dialog class
class FBDialog;			//get access to fcn dialog class
class HelpWidget;		//get access to help widget
class AboutDialog;		//get access to about dialog
class UpdateInfo;		//get access to update dialog
//class DiffDialog;		//get access to diff dialog class


class PrefDialog : public QDialog, public Ui::PrefDialog
{
  Q_OBJECT
   
public:
  PrefDialog(MainWindow *parent, PageCtrl *pageCtrl, SessionManager *sessionManager, ImportExport *importExport, PrintPrepare *printPrepare, POUInfoDialog *pouInfoDialog, FileViewDialog *fileViewDialog, FindDialog *findDialog, FncDialog *fncDialog, FBDialog *fbDialog, HelpWidget *helpWidget, AboutDialog *aboutDialog, UpdateInfo *updateInfo); //member VARs of this class
  virtual ~PrefDialog();
  
		
public slots:
    bool openPref();
    void resetPrefDialog();
    void savePref();

	
private slots:
    void 	setPrefListRow(int i);
    void 	applyPrefDialog();
    void	cancelDialog();
    void 	clearRecentFilesHistory();
    void	checkHiLineColor();
    void 	okClickPrefDialog();
    void	getHiLineColor();
    void	getLastDir(); 
    void 	retranslateUi();
    void	saveAllPages();
    void	setColorButton(QColor color);
    void	setHiLineColor(QColor color);
    void	setLastDir(); 
    void        setSessionDir();
    void	startTimer();
    void	stopTimer();

    
      
private:
    MainWindow 		*m_MainWindow; 		//!< instance of MainWindow class
    PageCtrl 		*m_PageCtrl; 		//!< instance of PageCtrl class
    SessionManager 	*m_SessionManager;	//!< instance of SessionManager class
    ImportExport 	*m_ImportExport;	//!< instance of ImportExport class
    PrintPrepare 	*m_PrintPrepare;	//!< instance of PrintPrepare class
    POUInfoDialog 	*m_POUInfoDialog;	//!< instance of POUInfoDialog class
    FileViewDialog 	*m_FileViewDialog;	//!< instance of FileViewDialog class
    FindDialog 		*m_FindDialog;		//!< instance of FindDialog class
    FncDialog 		*m_FncDialog;		//!< instance of FcnDialog class
    FBDialog 		*m_FBDialog;		//!< instance of FBDialog class
    HelpWidget		*m_HelpWidget;		//!< instance of HelpWidget class
    AboutDialog		*m_AboutDialog;		//!< instance of AboutDialog class
    UpdateInfo		*m_UpdateInfo;		//!< instance of UpdateInfo class
    //DiffDialog	*m_DiffDialog;		//!< instance of DiffDialog

    QTranslator 	appTranslator;		//!< application translator
    QTimer 		*timer;			//!< "auto save" timer
    QColor		hiLineColor;		//!< highlighting color for current line
    
    QString             appDir;                 //!< application directory
    QString	 	langDir;		//!< language directory
    QString		language;		//!< current interface language
    QString 		msg_head_lastDirDialog; //!< message string for "preset directory" dialog
    QString             msg_head_sessionDirDialog;//!< message string for "session directory" dialog

    QString		getExtNumber(QString str);
    void                getLanguages();
    void 		setTimer(bool set);
    QString             strFragment(QString str);
    void 		switchLang();
};
#endif
