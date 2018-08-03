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


#include <QtGui>

#include "sessionmanager.h"
#include "mainwindow.h"
#include "pagectrl.h"
#include "importexport.h"

   
SessionManager::SessionManager(MainWindow *parent, PageCtrl *pageCtrl, ImportExport *importExport) 
	: m_MainWindow(parent), m_PageCtrl(pageCtrl), m_ImportExport(importExport) //member VAR of PrefDialog
{ //! creating function
  //! \param PageCtrl class, ImportExport class

  setWindowFlags(windowFlags()^Qt::WindowContextHelpButtonHint);
    
  setupUi(this);
  retranslateUi();
  
  //first check if path exists and search sessions
  checkSessDir((QApplication::applicationDirPath()) + "/Sessions/");
  searchSessions();
  
  //set Button sizes
#ifdef Q_WS_MAC	
  //do nothing for Mac OS
#else
  Sess_pushButton_load->setMinimumHeight(25);
  Sess_pushButton_load->setMaximumHeight(25);
  Sess_pushButton_save->setMinimumHeight(25);
  Sess_pushButton_save->setMaximumHeight(25);
  Sess_pushButton_delete->setMinimumHeight(25);
  Sess_pushButton_delete->setMaximumHeight(25);
  Sess_closeButton->setMinimumHeight(25);
  Sess_closeButton->setMaximumHeight(25);	 
#endif

  //diverse connections
  connect(Sess_listWidget_Sessions, SIGNAL(itemSelectionChanged()), this, SLOT(setButtonsState())); 		//set allButtonState
  connect(Sess_pushButton_load, 	SIGNAL(clicked()), 				this, SLOT(loadSession()));				//load session
  connect(Sess_listWidget_Sessions, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(loadSession()));//load session
  connect(Sess_pushButton_delete, 	SIGNAL(clicked()), 				this, SLOT(deleteSession()));			//delete session
  connect(Sess_pushButton_save, 	SIGNAL(clicked()), 				this, SLOT(saveSession()));				//save session  
  connect(this, 					SIGNAL(sessionSaved()), 		this, SLOT(searchSessions()));			//re-searchSessions if any session saved
}


SessionManager::~SessionManager()
{ //! virtual destructor
}


void SessionManager::setButtonsState()
{ //! set ok Button state if session list is not empty

  if (Sess_listWidget_Sessions->count() > 0)
  {
    Sess_pushButton_load->setEnabled(true);
  	Sess_pushButton_delete->setEnabled(true);
  }  
  else
  {
    Sess_pushButton_load->setDisabled(true);  
    Sess_pushButton_delete->setDisabled(true);
  }
  
  if (m_PageCtrl->countPages() > 0)
  	Sess_pushButton_save->setEnabled(true);
  else 
  	Sess_pushButton_save->setDisabled(true);
}


void SessionManager::checkSessDir(QString dir)
{ //! check session directory, create path if not exists
	
  //create string sessions path
  sessDir = dir;
  //create Sessions path if not exists
  if (!QDir(dir).exists());
    QDir(dir).mkpath(dir);
  //create auto session folder path
  autoSessDir = sessDir + "/autoSession/";
  if (!QDir(autoSessDir).exists());
    QDir(dir).mkpath(autoSessDir);
}


void SessionManager::searchSessions()
{ //! search session files in session directory

  QDir directory = QDir(sessDir);
  QStringList files;

  //clear ListWidget
  Sess_listWidget_Sessions->clear();
  //get directory entryList and add items to ListWidget
  files = directory.entryList(QStringList("*.dat"), QDir::Files | QDir::NoSymLinks);
  Sess_listWidget_Sessions->addItems(files); 
}


void SessionManager::saveSession()
{ //! save file links to actual pages as session

  bool ok;
  QString str = QInputDialog::getText(this, m_MainWindow->PROJECT_NAME, msg_head_sessioncreate, QLineEdit::Normal, "Session", &ok, Qt::Tool);
  if (ok && !str.isEmpty())
    saveFilesAsSession(str);
  else
    return;
}


void SessionManager::deleteSession()
{ //! delete selected session
	
  bool ok=false;
  int i=0;
  
  //check if any row is selected
  if (Sess_listWidget_Sessions->currentRow() < 0)
    return;

  //get current item text
  QFile file(sessDir + Sess_listWidget_Sessions->currentItem()->text());
  if (Sess_listWidget_Sessions->currentItem()->text() == "autoSession.dat")
  { //remove all files in autoSession folder if autoSession.dat is selected
  	QStringList content = QDir(autoSessDir).entryList();
  	for (i=0; i<content.size(); i++)
  	  ok = QDir(autoSessDir).remove(content.at(i));
  }
  //remove session file from directory
  ok = file.remove();
  //remove item from ListWidget
  Sess_listWidget_Sessions->takeItem(Sess_listWidget_Sessions->currentRow());	
  //call setButtonsState to disable buttons if ListWidget is empty
  setButtonsState();	
}


void SessionManager::loadSession()
{ //! load selected slot
	
  bool ok=false;
    
  //clear error list
  Sess_listWidget_errorlist->clear();

  if (Sess_listWidget_Sessions->count() < 1)
    return; //return if ListWidget is empty

  //set wait cursor
  QApplication::setOverrideCursor(Qt::WaitCursor);
      
  ok = loadFilesFromSession(); //elese load files from selected session
  
  Sess_listWidget_errorlist->addItems(m_ImportExport->errorFiles); 	//load errorFiles list
  
  //give std. cursor back
  QApplication::restoreOverrideCursor();
  
  if ((ok == false) && (showErrorList == true))
  { //if loadSessFiles returned with (0) & showErrorList flag=1 then:
  	this->show();  													//even show dialog
  	Sess_tabWidget->setCurrentIndex(1);								//activate error Tab
  }
  if ((ok == false) && (showErrorList == false))
  { //if loadSessFiles returned with (0) show error message   
  	QMessageBox::critical(this, m_MainWindow->PROJECT_NAME, msg_error_sessionopen);  
  }
  
  if ((Sess_checkBox_autoclose->isChecked()) && (ok == true))
  	reject();
  
  openAutoSession = false;	//reset openAutoSession flag
  setButtonsState(); 		//call setButtonState to enable save button
}


bool SessionManager::loadFilesFromSession()
{ //! load files of session list
  QStringList *tmpList = new QStringList();        //create new StringList
  QStringList tmpList2;        //create new StringList
  bool ok = false;
  int i=0, listLength=0;
  QString fileName, Str;
  QFile file;
   	 
  //create QFile - - -   
  if (openAutoSession == false) //if openAutoSession=0 create file from currentItem
    file.setFileName(sessDir + Sess_listWidget_Sessions->currentItem()->text()); 
  else				 			//else create file from Str
    file.setFileName(sessDir + "autoSession.dat");
  //check if session file exists
  if (!file.exists())
    return(false); //set error and return
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return(false); //set error and return 
  //get session file content into StringList
  while (!file.atEnd())
    tmpList->append(file.readLine());
  //close file
  file.close();
  //end of file opening - - - 
  
  //get lenght of tmpList
  listLength = tmpList->size();
  if (listLength > 5) 			//lenght of tmpList > 3  
  {	
     //Session mark --------------------------------------------
     for (i = 0; i < listLength; i++)              	//count to position of [...]
       if (tmpList->value(i).contains("[Session]")) break;
     if (i >= listLength-1) return(false); //return when error
     
     for (i = i+1; i < listLength; i++)             	//load file names
     { //get files       
       Str.clear();
       Str = tmpList->at(i);
       if (Str.startsWith("/autoSession/",Qt::CaseInsensitive))
         Str = sessDir + Str;       
       tmpList2.append(Str);
     }
     
     ok = m_ImportExport->OpenPrepare(tmpList2);
  }   
  else return(true); //true becc. file exists but it is empty
  
  delete tmpList;
  
  return(ok);
}


void SessionManager::saveFilesAsSession(QString sessionName)
{ //! save path information of all already save files from PageCtrl class 

  QStringList *tmpList = new QStringList();        //create new StringList
  int i=0;
  bool ok;
  QString filePath;
  
  
  //check if sessionName is empty
  if (!sessionName.isEmpty())
    filePath = sessDir + sessionName + ".dat";
  else
    return;


  //build file header
  tmpList->append(m_MainWindow->PROJECT_NAME + " " + m_MainWindow->VERSION + " Session File");	//Session File description
  tmpList->append("; author: M. Rehfeldt, " + m_MainWindow->PROJECT_URL);					//author & website information
  tmpList->append("; do not edit this file, use the sessions dialog");						//editing information
  tmpList->append(" ");
  tmpList->append("[Session]");

  //copy path information into tmpList
  for (i = 0; i < m_PageCtrl->countPages(); i++)
  { //get opened files without untiled/not already saved files
    if (!m_PageCtrl->filePathOf(i).isEmpty())          //name of page=file name
      if ((saveAutoSession == true) && (saveSessionAsPrj == true) && (sessionName == "autoSession"))
        tmpList->append("/autoSession/" + m_PageCtrl->tabTextOf(i));
      else
        tmpList->append(m_PageCtrl->filePathOf(i) + m_PageCtrl->tabTextOf(i));
  }
  
  if ((saveAutoSession == true) && (saveSessionAsPrj == true) && (sessionName == "autoSession"))
  { //delete all old files and use ImportExport classe store current files
    QStringList content = QDir(autoSessDir).entryList();
    for (i=0; i<content.size(); i++)
      ok = QDir(autoSessDir).remove(content.at(i));
    m_ImportExport->saveAllPages(autoSessDir, 99, 0); //path, 99 = use original extensions
  }

  //save Session file
  //create file
  QFile file(filePath);
  if (!file.open(QFile::WriteOnly | QIODevice::Text))
    //if not successful
	QMessageBox::critical(this, m_MainWindow->PROJECT_NAME, msg_error_sessionsave); 
  else
  { //create text stream
    QTextStream ts(&file);
    //set text codec
    ts.setCodec(QTextCodec::codecForName("UTF-8"));
    //save tmpList from saveprepare fcn. to file
    ts << tmpList->join("\n");
  }

  delete tmpList;   		//delete tmpList
  emit sessionSaved();  	//emit sessionSaved information
}


void SessionManager::retranslateUi()
{ //! retranslate Ui

  this->setWindowTitle(tr("Session Manager")); 
  
  //tab texts
  Sess_tabWidget->setTabText(0, tr("Session List"));
  Sess_tabWidget->setTabText(1, tr("Session Errors"));
  
  //close Button
  Sess_closeButton->setText(tr("Close"));
  Sess_closeButton->setStatusTip(tr("Close window"));
  Sess_closeButton->setToolTip(Sess_closeButton->statusTip());
  
  //load Button
  Sess_pushButton_load->setText(tr("Load"));
  Sess_pushButton_load->setStatusTip(tr("Load selected session"));
  Sess_pushButton_load->setToolTip(Sess_pushButton_load->statusTip());
  
  //save Button
  Sess_pushButton_save->setText(tr("Save"));
  Sess_pushButton_save->setStatusTip(tr("Save current pages to Session"));
  Sess_pushButton_save->setToolTip(Sess_pushButton_save->statusTip());
  
  //delete Button
  Sess_pushButton_delete->setText(tr("Delete"));
  Sess_pushButton_delete->setStatusTip(tr("Delete selected Session"));
  Sess_pushButton_delete->setToolTip(Sess_pushButton_delete->statusTip());
  
  //checkbox auto close
  Sess_checkBox_autoclose->setText(tr("Auto close"));
  Sess_checkBox_autoclose->setStatusTip(tr("Close window after session loading"));
  Sess_checkBox_autoclose->setToolTip(Sess_checkBox_autoclose->statusTip());

  //label session error list
  Sess_label_errorlist->setText(tr("Error list:"));
  Sess_label_errorlist->setStatusTip(tr("List of file opening errors"));
  Sess_label_errorlist->setToolTip(Sess_label_errorlist->statusTip()); 
 
  //messages
  msg_error_sessionopen = tr("The selected session file is faulty!\nMaybe the file was not created on\nthis platform. See the error list.");
  msg_error_sessionsave = tr("The session file could not be saved!\nPlease try again.");
  msg_head_sessioncreate = tr("Insert Session name:");
}
