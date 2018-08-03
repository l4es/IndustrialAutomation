/***************************************************************************
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
#include <QDesktopServices>

#ifdef Q_WS_MAC
  #include <Carbon/Carbon.h>
#endif

#include "mainwindow.h"
#include "prefdialog.h"
#include "fbdialog.h"
#include "fncdialog.h"
#include "pagectrl.h"
#include "pouinfodialog.h"
#include "fileviewdialog.h"
#include "sessionmanager.h"
#include "prototype.h"
#include "printprepare.h"
#include "importexport.h"
#include "pouinfodialog.h"
#include "finddialog.h"
#include "helpwidget.h"
#include "aboutdialog.h"
#include "updateinfo.h"
//#include "diffdialog.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{  
  //! creating function
  /*! setting project data and window icon */
  /*! call create functions for actions, menus and toolbars */
  /*! instantiate project classes */	
	
  //Application information
  PROJECT_NAME 	= "PLCEdit";
  VERSION       = "2.2.4";
  REVISION      = "r129";
  PROJECT_DATE  = "2015";
  PROJECT_URL	= "plcedit.org";
  PROJECT_MAIL	= "info@plcedit.org";
  UPDATE_PATH	= "/documentation/version/version.txt";
  DOWNLOAD_PATH	= "/download/download.html";
  
  //set window icon, name and title
  setWindowIcon(QIcon(":/images/icon.png"));
  
  setObjectName(PROJECT_NAME);
  setWindowTitle(PROJECT_NAME);


  //create pageCtrl1 as central widget
  pageCtrl1 = new PageCtrl(this);
  setCentralWidget(pageCtrl1);
  
  setAcceptDrops(true);
  
  http = new QHttp(this);
  http_buffer = new QBuffer(this);

  //call main window create fcn.s
  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  retranslateUi();


  //prepare something needed at start-up
  FncTypeStr = "";


  //create dialogs
  prototype 		= new Prototype(pageCtrl1);
  printPrepare 		= new PrintPrepare(pageCtrl1, prototype, importExport);
  importExport 		= new ImportExport(pageCtrl1, prototype);
  fileViewDialog 	= new FileViewDialog(this, pageCtrl1, importExport); 
  sessionManager 	= new SessionManager(this, pageCtrl1, importExport);
  pouInfoDialog  	= new POUInfoDialog(this, pageCtrl1, prototype, importExport);
  findDialog  		= new FindDialog(this, pageCtrl1);
  fbDialog  		= new FBDialog(this, pageCtrl1);
  fncDialog  		= new FncDialog(this);
  helpBrowser  		= new HelpWidget(0);
  aboutDialog  		= new AboutDialog(this);
  updateInfo  		= new UpdateInfo(this);
  //diffDialog  	= new DiffDialog(this, pageCtrl1);
  
  //load settings from preferences file
  prefDialog = new PrefDialog(this, pageCtrl1, sessionManager, importExport, printPrepare, pouInfoDialog, fileViewDialog, findDialog, fncDialog, fbDialog, helpBrowser, aboutDialog, updateInfo);
  //open preferences and check if an error occurs
  if (prefDialog->openPref() == false) //show message if preferences file could not be found
    QMessageBox::critical(this, PROJECT_NAME + " " + msg_head_preferror, msg_text_preferror);

  if (prefDialog->Pref_checkBox_SessAutoLoad->isChecked())
  { //if session autoload is on demand, activate sessions functions
    sessionManager->searchSessions();
    //sessionManager->openAutoSession = true;  
    sessionManager->loadSession(); 
  }


  //connect SIGNALS & SLOTS for StatusBar updating
  connect(this, 	SIGNAL(fileType_modified()),this, SLOT(updateStatusBar())); //update statusbar if file/block type was modified
  connect(importExport, SIGNAL(modified()),         this, SLOT(updateStatusBar()));	//update statusbar if page was opened or saved
  connect(pageCtrl1, 	SIGNAL(modified()),         this, SLOT(updateStatusBar())); //update statusbar if page was modified
  connect(pageCtrl1, 	SIGNAL(currentChanged(int)),this, SLOT(updateStatusBar())); //update statusbar if page was changed
  connect(pageCtrl1, 	SIGNAL(cursorModified()),   this, SLOT(updateStatusBar())); //update statusbar id cursor was moved

  //connect SIGNALS & SLOTS for interface items
  connect(this, 	SIGNAL(fileType_modified()),this, SLOT(setInterfaceItemsEnabled())); //set interface items enabled if file/block type was modified
  connect(importExport, SIGNAL(modified()),         this, SLOT(setInterfaceItemsEnabled())); //set interface items enabled if page was opened or saved
  connect(pageCtrl1, 	SIGNAL(modified()),         this, SLOT(setInterfaceItemsEnabled())); //set interface items enabled if page was modified
  connect(pageCtrl1, 	SIGNAL(currentChanged(int)),this, SLOT(setInterfaceItemsEnabled())); //set interface items enabled if page was changed

  //connect diverse SIGNALS & SLOTS
  connect(pageCtrl1, 	SIGNAL(pageDeleted(int)), 	this, SLOT(fillRecentFilesAction(int))); //fill recent files action if page is deleted
  connect(pageCtrl1, 	SIGNAL(saveRequest()), 		this, SLOT(savePageAs())); 				 //save page as requested by class PageCtrl
  //connect(pageCtrl1, 	SIGNAL(compareWith()), 		this, SLOT(doCompareWith())); 		 	 //reqeuested file comparision with by class PageCtrl
  //connect(pageCtrl1, 	SIGNAL(compareWithNext()), 	this, SLOT(doCompareWithNext())); 		 //reqeuested file comparision with next by class PageCtrl
  //connect(pageCtrl1, 	SIGNAL(compareWithPrev()), 	this, SLOT(doCompareWithPrev())); 		 //reqeuested file comparision with previous by class PageCtrl
  connect(http, 	SIGNAL(done(bool)), 		this, SLOT(httpRequestFinished(bool)));
    

  //draw toolbars in brushed metal on Mac OS X
  //doesn't work correctly with Qt > v4.3 on Mac OS 10.5 and SDK 10.4u
  //Qt4.3.x shows metal toolabr but sometime application crashes
  //Qt4.4+  shows application window with minimum width set to > 1000pix
  #ifdef Q_WS_MAC 
    #if QT_VERSION < 0x040400 //patch to inhibit unified toolbar with Qt 4.4 and up
      setAttribute(Qt::WA_MacBrushedMetal);
      setUnifiedTitleAndToolBarOnMac(true); 
    #endif
  #endif
  
  
  //set interface items enabled
  setInterfaceItemsEnabled();
  
  //retranslate Ui  
  retranslateUi();
  
  //check for program updates if enabled
  if (prefDialog->Pref_checkBox_checkUpdatesAtStart->isChecked())
  {
    startup = true;  //set start flag to prevent "no updates available message" if check for updates at start is enabled
    checkForUpdates();
  }
}


MainWindow::~MainWindow()
{ //! virtual destructor

  //delete all created dialogs
  delete prefDialog;
  delete findDialog;
  delete fbDialog;
  delete fncDialog;
  delete pouInfoDialog;
  delete sessionManager;
  delete fileViewDialog;
  delete importExport;
  delete printPrepare;
  delete prototype;
  delete helpBrowser;
  delete aboutDialog;
  delete updateInfo;
  delete pageCtrl1;
}


void MainWindow::closeEvent(QCloseEvent *event)
{ //! re-implemented close event to check modification status of all pages
	
  bool modified=pageCtrl1->pagesModified(), unsavedNoticeFlag = pageCtrl1->unsavedNotice;
  int reply=0;

  //activate a message box if a page was changed
  if (unsavedNoticeFlag && modified)
    reply = QMessageBox::warning(this, PROJECT_NAME, msg_text_closepages, okButtonText, cancelButtonText, 0, 2 );

  if (reply == 0) //accept event if ok was pressed
    event->accept();
  if (reply > 0) //ignore event if cancel was pressed
    event->ignore();

  //fill recent files list with opened pages during application quit
  for (int i = 0; i < pageCtrl1->countPages(); i++)
    pageCtrl1->fillRecentFilesList(i);
  //call preferences dialog to save all settings
  if (sessionManager->saveAutoSession == true)
    sessionManager->saveFilesAsSession("autoSession");

  //save preferences
  prefDialog->savePref();
}


void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{ //! drag&drop acceptance for dragged "urls" (files)
	
  if (event->mimeData()->hasUrls())
    event->acceptProposedAction();
}
 
 
void MainWindow::dropEvent(QDropEvent *event)
{ //! drop event reimplementation for file opening
  bool ok= false;
  QString urlStr="", Str="";
  QStringList files;
  files.clear();
   
  if (event->mimeData()->hasUrls()) //if1
  {
	QList<QUrl> urls = event->mimeData()->urls();
	foreach (QUrl url, urls) 
	{ //get each path
	  urlStr = url.path();
	  #ifdef Q_WS_WIN			
	  //prevent strings with filenames like '/C:/...'
		if (urlStr[0] == '/') urlStr.remove(0, 1);
	  #endif	
	  files << urlStr; 
    }

    if (!files.isEmpty())//if2
	  ok = importExport->OpenPrepare(files);
    else return;
  }//if1
    
  if (ok == false)//if3
  {	//send error message
  	foreach (QString tmpStr, importExport->errorFiles) 
  	  Str = Str +"<br>"+ tmpStr;
  	QMessageBox::critical(this, PROJECT_NAME, msg_error_fileopen + "<br>" + Str);  
  }//if3

  event->acceptProposedAction();
  prefDialog->savePref();			//save the current file path if enabled
  this->showNormal();				//show the window normal
}



//=============================================================================
// - - - create functions - - - 
void MainWindow::createActions()
{ //! create actions for menus and toolbars

//menu FILE
  //create new PRG file action
  newPLC_PRGAction = new QAction(this);
  newPLC_PRGAction->setIcon(QIcon(":/images/newPRG_22x22.png"));
  connect(newPLC_PRGAction, SIGNAL(triggered()), this, SLOT(createNewPage()));

  //create new FB file action
  newPLC_FBAction = new QAction(this);
  newPLC_FBAction->setIcon(QIcon(":/images/newFB_22x22.png"));
  connect(newPLC_FBAction, SIGNAL(triggered()), this, SLOT(createNewPage()));

  //create new Fc file action
  newPLC_FcAction = new QAction(this);
  newPLC_FcAction->setShortcut(tr("Ctrl+Alt+F"));
  newPLC_FcAction->setIcon(QIcon(":/images/newFc_22x22.png"));
  connect(newPLC_FcAction, SIGNAL(triggered()), this, SLOT(createNewPage()));

  //create new VAR file action
  newPLC_VARAction = new QAction(this);
  newPLC_VARAction->setIcon(QIcon(":/images/newVARList_22x22.png"));
  connect(newPLC_VARAction, SIGNAL(triggered()), this, SLOT(createNewPage()));
  
  //create new TYPE file action
  newPLC_TYPEAction = new QAction(this);
  newPLC_TYPEAction->setIcon(QIcon(":/images/newTYPE_22x22.png"));
  connect(newPLC_TYPEAction, SIGNAL(triggered()), this, SLOT(createNewPage()));

  //create open file action
  openAction = new QAction(this);
  openAction->setIcon(QIcon(":/images/openFolder_22x22.png"));
  connect(openAction, SIGNAL(triggered()), this, SLOT(openFiles()));

  //create POU info action
  POUInfoAction = new QAction(this);
  connect(POUInfoAction, SIGNAL(triggered()), this, SLOT(POUInfo()));
  
  //create alterToPrg action
  alterToPrgAction = new QAction(this);
  connect(alterToPrgAction, SIGNAL(triggered()), this, SLOT(alterPOUType()));

  //create alterToFB action
  alterToFBAction = new QAction(this);
  connect(alterToFBAction, SIGNAL(triggered()), this, SLOT(alterPOUType()));
  
  //create alterToFcn action
  alterToFcnAction = new QAction(this);
  connect(alterToFcnAction, SIGNAL(triggered()), this, SLOT(alterPOUType()));
    
  //create manageSessions action
  manageSessionsAction = new QAction(this);
  connect(manageSessionsAction, SIGNAL(triggered()), this, SLOT(showSessions()));

  //create saveSessions action
  saveSessionsAction = new QAction(this);
  connect(saveSessionsAction, SIGNAL(triggered()), this, SLOT(saveSession()));
  
  //--
  //create save action
  saveAction = new QAction(this);
  saveAction->setIcon(QIcon(":/images/save_22x22.png"));
  connect(saveAction, SIGNAL(triggered()), this, SLOT(savePage()));
  
  //create save as action
  saveAsAction = new QAction(this);
  saveAsAction->setIcon(QIcon(":/images/save_as_22x22.png"));
  connect(saveAsAction, SIGNAL(triggered()), this, SLOT(savePageAs()));
  
  //create batch converting as action
  batchConverterAction = new QAction(this);
  batchConverterAction->setIcon(QIcon(":/images/batch_processing_22x22.png"));
  connect(batchConverterAction, SIGNAL(triggered()), this, SLOT(batchConverter()));

  //--
  //create print action
  printAction = new QAction(this);
  printAction->setIcon(QIcon(":/images/printer_22x22.png"));
  connect(printAction, SIGNAL(triggered()), this, SLOT(print()));

  //create export PDF action
  exportPDFAction = new QAction(this);
  exportPDFAction->setIcon(QIcon(":/images/pdf_print_22x22.png"));
  connect(exportPDFAction, SIGNAL(triggered()), this, SLOT(print()));

  //--
  //create close tab action
  closePageAction = new QAction(this);
  connect(closePageAction, SIGNAL(triggered()), pageCtrl1, SLOT(deletePage()));

  //create close all tabs action
  closeAllPagesAction = new QAction(this);
  connect(closeAllPagesAction, SIGNAL(triggered()), this, SLOT(closeAllPages()));

  //create close app action
  exitAction = new QAction(this);
  exitAction->setMenuRole(QAction::QuitRole);
  connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

//menu EDIT
  //create undo action
  undoAction = new QAction(this);
  undoAction->setIcon(QIcon(":/images/undo_22x22.png"));
  connect(undoAction, SIGNAL(triggered()), pageCtrl1, SLOT(undo()));
  
  //create redo action
  redoAction = new QAction(this);
  redoAction->setIcon(QIcon(":/images/redo_22x22.png"));
  connect(redoAction, SIGNAL(triggered()), pageCtrl1, SLOT(redo()));
  
  //create cut action
  cutAction = new QAction(this);
  connect(cutAction, SIGNAL(triggered()), pageCtrl1, SLOT(cut()));
  
  //create copy action
  copyAction = new QAction(this);
  connect(copyAction, SIGNAL(triggered()), pageCtrl1, SLOT(copy()));
  
  //create paste action
  pasteAction = new QAction(this);
  connect(pasteAction, SIGNAL(triggered()), pageCtrl1, SLOT(paste()));
  
  //create selectAll action
  selectAllAction = new QAction(this);
  connect(selectAllAction, SIGNAL(triggered()), pageCtrl1, SLOT(selectAll()));

  //create find&replace action
  findAction = new QAction(this);
  findAction->setIcon(QIcon(":/images/find_replace_22x22.png"));
  connect(findAction, SIGNAL(triggered()), this, SLOT(find()));

//menu VIEW
  //create splitter Horizontal orientation action
  windowsHorAction = new QAction(this);
  windowsHorAction->setCheckable(true);
  connect(windowsHorAction, SIGNAL(triggered()), this, SLOT(windowsAlignHor()));

  //create splitter Vertical orientation action
  windowsVerAction = new QAction(this);
  windowsVerAction->setCheckable(true);
  connect(windowsVerAction, SIGNAL(triggered()), this, SLOT(windowsAlignVer()));
   
  //create declarations visible action
  declVisibleAction = new QAction(this);
  declVisibleAction->setCheckable(true);
  declVisibleAction->setChecked(true); //used for start up
  connect(declVisibleAction, SIGNAL(triggered()), this, SLOT(setCurrentDeclarationVisible()));
  connect(pageCtrl1, SIGNAL(currentChanged(int)), this, SLOT(updateDeclarationsVisible(int)));

  //create filelist visible action
  fileListVisibleAction = new QAction(this);
  fileListVisibleAction->setCheckable(true);
  connect(fileListVisibleAction, SIGNAL(triggered()), this, SLOT(setFileListVisible()));
  
  //create minimize window action
  MinimizeAction = new QAction(this);
  connect(MinimizeAction, SIGNAL(triggered()), this, SLOT(showMinimized()));

  //create maximize window action  
  ZoomAction = new QAction(this);
  connect(ZoomAction, SIGNAL(triggered()), this, SLOT(showMaximized())); 

  //create change input window action
  changeInputWindowAction = new QAction(this);
  connect(changeInputWindowAction, SIGNAL(triggered()), pageCtrl1, SLOT(changeInputWindow()));
    
  //create file view action
  fileViewAction = new QAction(this);
  connect(fileViewAction, SIGNAL(triggered()), this, SLOT(showFileView()));

//menu TOOLS
  //create Pref Action
  PrefAction = new QAction(this);
  PrefAction->setMenuRole(QAction::PreferencesRole);
  connect(PrefAction, SIGNAL(triggered()), this, SLOT(openPrefDialog()));
  
  //create prg lang. sw. AWL action
  prglng_AWLAction = new QAction(this);
  connect(prglng_AWLAction, SIGNAL(triggered()), this, SLOT(switchPRGLang()));

  //create prg lang. sw. ST action
  prglng_STAction = new QAction(this);
  connect(prglng_STAction, SIGNAL(triggered()), this, SLOT(switchPRGLang()));
 
  //create call FB action
  insertFBAction = new QAction(this);
  connect(insertFBAction, SIGNAL(triggered()), this, SLOT(insertFB()));

//menu HELP
  //create F1 action
  F1Action = new QAction(this);
  connect(F1Action, SIGNAL(triggered()), this, SLOT(showHelpBrowser()));
  
  //create HelpBubbles action
  helpBubblesAction = new QAction(this);
  connect(helpBubblesAction, SIGNAL(triggered()), this, SLOT(HelpBubbles()));
  
  //create bug report action
  bugReportAction = new QAction(this);
  connect(bugReportAction, SIGNAL(triggered()), this, SLOT(sendBugReport()));

  //create check for program updates action
  checkForUpdatesAction = new QAction(this);
  connect(checkForUpdatesAction, SIGNAL(triggered()), this, SLOT(checkForUpdates()));

  //create about app action
  aboutAction = new QAction(this);
  aboutAction->setMenuRole(QAction::AboutRole);
  connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

  //create about Qt lib action
  aboutQtAction = new QAction(this);
  aboutQtAction->setMenuRole(QAction::AboutQtRole);
  connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
  
//TOOLBAR
  //create AWL LD action
  AWL_LDAction = new QAction(this);
  AWL_LDAction->setIcon(QIcon(":/images/LD_22x22.png"));
  connect(AWL_LDAction, SIGNAL(triggered()), this, SLOT(insertCMD()));

  //create AWL ST action
  AWL_STAction = new QAction(this);
  AWL_STAction->setIcon(QIcon(":/images/ST_22x22.png"));
  connect(AWL_STAction, SIGNAL(triggered()), this, SLOT(insertCMD()));

  //create AWL JMP action
  AWL_JMPAction = new QAction(this);
  AWL_JMPAction->setIcon(QIcon(":/images/JMP_22x22.png"));
  connect(AWL_JMPAction, SIGNAL(triggered()), this, SLOT(insertCMD()));

  //create AWL FB CAL action
  AWL_FBCALAction = new QAction(this);
  AWL_FBCALAction->setIcon(QIcon(":/images/FBCAL_22x22.png"));
  connect(AWL_FBCALAction, SIGNAL(triggered()), this, SLOT(insertCMD()));

  //create SAWL AND action
  SAWL_ANDAction = new QAction(this);
  SAWL_ANDAction->setIcon(QIcon(":/images/AND_22x22.png"));
  connect(SAWL_ANDAction, SIGNAL(triggered()), this, SLOT(insertCMD()));

  //create SAWL OR action
  SAWL_ORAction = new QAction(this);
  SAWL_ORAction->setIcon(QIcon(":/images/OR_22x22.png"));
  connect(SAWL_ORAction, SIGNAL(triggered()), this, SLOT(insertCMD()));

  //create SAWL Ass action
  SAWL_AssignAction = new QAction(this);
  SAWL_AssignAction->setIcon(QIcon(":/images/SAWL_Assign_22x22.png"));
  connect(SAWL_AssignAction, SIGNAL(triggered()), this, SLOT(insertCMD()));

  //create SAWL SPA action
  SAWL_SPAAction = new QAction(this);
  SAWL_SPAAction->setIcon(QIcon(":/images/SPA_22x22.png"));
  connect(SAWL_SPAAction, SIGNAL(triggered()), this, SLOT(insertCMD()));

  //create SAWL FB CAL action
  SAWL_FBCALAction = new QAction(this);
  SAWL_FBCALAction->setIcon(QIcon(":/images/FBCAL_22x22.png"));
  connect(SAWL_FBCALAction, SIGNAL(triggered()), this, SLOT(insertCMD()));

  //create SAWL NW action
  SAWL_NWAction = new QAction(this);
  SAWL_NWAction->setIcon(QIcon(":/images/NW_22x22.png"));
  connect(SAWL_NWAction, SIGNAL(triggered()), this, SLOT(insertCMD()));

  //create ST If Then Else action
  ST_IfThenElseAction = new QAction(this);
  ST_IfThenElseAction->setIcon(QIcon(":/images/IfThenElse_24x24.png"));
  connect(ST_IfThenElseAction, SIGNAL(triggered()), this, SLOT(insertCMD()));

  //create ST Case action
  ST_CaseAction = new QAction(this);
  ST_CaseAction->setIcon(QIcon(":/images/Case_24x24.png"));
  connect(ST_CaseAction, SIGNAL(triggered()), this, SLOT(insertCMD()));

  //create ST For To Do action
  ST_ForToDoAction = new QAction(this);
  ST_ForToDoAction->setIcon(QIcon(":/images/ForToDo_22x22.png"));
  connect(ST_ForToDoAction, SIGNAL(triggered()), this, SLOT(insertCMD()));

  //create ST While Do action
  ST_WhileDoAction = new QAction(this);
  ST_WhileDoAction->setIcon(QIcon(":/images/WhileDo_22x22.png"));
  connect(ST_WhileDoAction, SIGNAL(triggered()), this, SLOT(insertCMD()));

  //create ST Repeat Until action
  ST_RepeatUntilAction = new QAction(this);
  ST_RepeatUntilAction->setIcon(QIcon(":/images/RepeatUntil_22x22.png"));
  connect(ST_RepeatUntilAction, SIGNAL(triggered()), this, SLOT(insertCMD()));

  //create ST Assign action
  ST_AssignAction = new QAction(this);
  ST_AssignAction->setIcon(QIcon(":/images/Assign_22x22.png"));
  connect(ST_AssignAction, SIGNAL(triggered()), this, SLOT(insertCMD()));

  //create ST FB CAL action
  ST_FBCALAction = new QAction(this);
  ST_FBCALAction->setIcon(QIcon(":/images/FBCAL_22x22.png"));
  connect(ST_FBCALAction, SIGNAL(triggered()), this, SLOT(insertCMD()));

//DIVERSE

}


void MainWindow::createMenus()
{ //! create menu items

//menu FILE
  fileMenu = menuBar()->addMenu("File");
  fileSubMenu1 = fileMenu->addMenu("New");
  fileSubMenu1->addAction(newPLC_PRGAction);
  fileSubMenu1->addAction(newPLC_FBAction);
  fileSubMenu1->addAction(newPLC_FcAction);
  fileSubMenu1->addAction(newPLC_VARAction);
  fileSubMenu1->addAction(newPLC_TYPEAction);
  fileMenu->addAction(openAction);
  fileMenu->addAction(POUInfoAction);
  fileSubMenu2 = fileMenu->addMenu("Alter POU type");
  fileSubMenu2->addAction(alterToPrgAction);
  fileSubMenu2->addAction(alterToFBAction);
  fileSubMenu2->addAction(alterToFcnAction);
  fileSubMenu3 = fileMenu->addMenu("Sessions");
  fileSubMenu3->addAction(manageSessionsAction);
  fileSubMenu3->addAction(saveSessionsAction);
  fileSubMenuRecentFiles = fileMenu->addMenu("Recent files");
  for (int i = 0; i < maxRecentFiles; i++)
  { //create recent file list action
    recentFilesAction[i] = new QAction(this);
    recentFilesAction[i]->setVisible(false);
    fileSubMenuRecentFiles->addAction(recentFilesAction[i]);
    connect(recentFilesAction[i], SIGNAL(triggered()),this, SLOT(openRecentFile()));
  }
  fileMenu->addSeparator();
  fileMenu->addAction(saveAction);
  fileMenu->addAction(saveAsAction);
  fileMenu->addAction(batchConverterAction);
  fileMenu->addSeparator();
  fileMenu->addAction(printAction);
  fileMenu->addAction(exportPDFAction);
  fileMenu->addSeparator();
  fileMenu->addAction(closePageAction);
  fileMenu->addAction(closeAllPagesAction);
  fileMenu->addAction(exitAction);

//menu EDIT
  editMenu = menuBar()->addMenu("Edit");
  editMenu->addAction(undoAction);
  editMenu->addAction(redoAction);
  editMenu->addSeparator();
  editMenu->addAction(cutAction);
  editMenu->addAction(copyAction);
  editMenu->addAction(pasteAction);
  editMenu->addSeparator();
  editMenu->addAction(selectAllAction);
  editMenu->addSeparator();
  editMenu->addAction(findAction);

//menu VIEW
  viewMenu = menuBar()->addMenu("View");
  viewMenu->addAction(windowsHorAction);
  viewMenu->addAction(windowsVerAction);
  viewMenu->addAction(declVisibleAction);
  viewMenu->addAction(fileListVisibleAction);
  viewMenu->addSeparator();
  viewMenu->addAction(MinimizeAction);
  viewMenu->addAction(ZoomAction);
  viewMenu->addAction(changeInputWindowAction);
  viewMenu->addSeparator();
  viewMenu->addAction(fileViewAction);

//menu TOOLS
  toolsMenu = menuBar()->addMenu("Tools");
  toolsSubMenu1 = toolsMenu->addMenu("Programming language");
  toolsSubMenu1->addAction(prglng_AWLAction);
  toolsSubMenu1->addAction(prglng_STAction);
  toolsMenu->addAction(insertFBAction);
  toolsMenu->addSeparator();
  toolsMenu->addAction(PrefAction);

//menubar seperator
  menuBar()->addSeparator();

//menu HELP
  helpMenu = menuBar()->addMenu("Help");
  helpMenu->addAction(F1Action);
  //helpMenu->addAction(helpBubblesAction);
  helpMenu->addAction(bugReportAction);
  helpMenu->addSeparator();
  helpMenu->addAction(checkForUpdatesAction);
  helpMenu->addAction(aboutAction);
  //helpMenu->addAction(aboutQtAction);
}


void MainWindow::retranslateUi()
{ //! retranslate Ui elements for mainwindow

//menu head lines
  //file menu
  fileMenu->setTitle(tr("&File"));
  fileSubMenu1->setTitle(tr("&New"));
  fileSubMenu2->setTitle(tr("&Alter POU type"));
  fileSubMenu3->setTitle(tr("&Sessions"));
  fileSubMenuRecentFiles->setTitle(tr("&Recent files"));
  //fileSubMenuRecentFiles->setStatusTip(tr("List of recent deleted files"));
  //fileSubMenuRecentFiles->setToolTip(fileSubMenuRecentFiles->statusTip());
  
  //edit menu
  editMenu->setTitle(tr("&Edit"));
  
  //view menu
  viewMenu->setTitle(tr("&View"));
  
  //tools menu
  toolsMenu->setTitle(tr("&Tools"));
  toolsSubMenu1->setTitle(tr("&Programming language"));
  
  //help menu
  helpMenu->setTitle(tr("&Help"));


//menu FILE
  //create new PRG file action
  newPLC_PRGAction->setText(tr("New &Program"));
  newPLC_PRGAction->setShortcut(tr("Ctrl+Alt+P"));
  newPLC_PRGAction->setStatusTip(tr("Create a new program"));
  newPLC_PRGAction->setToolTip(newPLC_PRGAction->statusTip());
  
  //create new FB file action
  newPLC_FBAction->setText(tr("New Function&block"));
  newPLC_FBAction->setShortcut(tr("Ctrl+Alt+B"));
  newPLC_FBAction->setStatusTip(tr("Create a new function block"));
  newPLC_FBAction->setToolTip(newPLC_FBAction->statusTip());
  
  //create new Fc file action
  newPLC_FcAction->setText(tr("New &Function"));
  newPLC_FcAction->setShortcut(tr("Ctrl+Alt+F"));
  newPLC_FcAction->setStatusTip(tr("Create a new function"));
  newPLC_FcAction->setToolTip(newPLC_FcAction->statusTip());
  
  //create new VAR file action
  newPLC_VARAction->setText(tr("New &Variablelist"));
  newPLC_VARAction->setShortcut(tr("Ctrl+Alt+L"));
  newPLC_VARAction->setStatusTip(tr("Create a new global variable list"));
  newPLC_VARAction->setToolTip(newPLC_VARAction->statusTip());

  //create new TYPE file action
  newPLC_TYPEAction->setText(tr("New &Type"));
  newPLC_TYPEAction->setShortcut(tr("Ctrl+Alt+T"));
  newPLC_TYPEAction->setStatusTip(tr("Create a new type definition"));
  newPLC_TYPEAction->setToolTip(newPLC_TYPEAction->statusTip());
  
  //create open file action
  openAction->setText(tr("&Open..."));
  openAction->setShortcut(tr("Ctrl+O"));
  openAction->setStatusTip(tr("Open an existing file"));
  openAction->setToolTip(openAction->statusTip());
  
  //create POU info action
  POUInfoAction->setText(tr("&POU Info..."));
  POUInfoAction->setShortcut(tr("Ctrl+I"));
  POUInfoAction->setStatusTip(tr("Open POU info dialog"));
  POUInfoAction->setToolTip(POUInfoAction->statusTip());
  
  //create alterToPrg action
  alterToPrgAction->setText(tr("Program"));
  alterToPrgAction->setStatusTip(tr("Alter POU type to program"));
  alterToPrgAction->setToolTip(alterToPrgAction->statusTip());
  
  //create alterToFB action
  alterToFBAction->setText(tr("Functionblock"));
  alterToFBAction->setStatusTip(tr("Alter POU type to functionblock"));
  alterToFBAction->setToolTip(alterToFBAction->statusTip());
  
  //create alterToFcn action
  alterToFcnAction->setText(tr("Function..."));
  alterToFcnAction->setStatusTip(tr("Alter POU type to function"));
  alterToFcnAction->setToolTip(alterToFcnAction->statusTip());
    
  //create manageSessions action
  manageSessionsAction->setText(tr("&Manage Sessions..."));
  manageSessionsAction->setShortcut(tr("Ctrl+F11"));
  manageSessionsAction->setStatusTip(tr("Open session manager"));
  manageSessionsAction->setToolTip(manageSessionsAction->statusTip());
  
  //create saveSessions action
  saveSessionsAction->setText(tr("&Save Sessions..."));
  saveSessionsAction->setShortcut(tr("Ctrl+Alt+F11"));
  saveSessionsAction->setStatusTip(tr("Save actual pages as session"));
  saveSessionsAction->setToolTip(saveSessionsAction->statusTip());
  
  //--
  //create save action
  saveAction->setText(tr("&Save"));
  saveAction->setShortcut(tr("Ctrl+S"));
  saveAction->setStatusTip(tr("Save actual page"));
  saveAction->setToolTip(saveAction->statusTip());
  
  //create save as action
  saveAsAction->setText(tr("Save &As..."));
  saveAsAction->setShortcut(tr("Ctrl+Alt+S"));
  saveAsAction->setStatusTip(tr("Save actual page with new name"));
  saveAsAction->setToolTip(saveAsAction->statusTip());
  
  //create batch converting as action
  batchConverterAction->setText(tr("Batch converting..."));
  batchConverterAction->setShortcut(tr("Ctrl+D"));
  batchConverterAction->setStatusTip(tr("Convert all pages"));
  batchConverterAction->setToolTip(batchConverterAction->statusTip());
  
  //--
  //create print action
  printAction->setText(tr("&Print..."));
  printAction->setShortcut(tr("Ctrl+P"));
  printAction->setStatusTip(tr("Print actual page"));
  printAction->setToolTip(printAction->statusTip());
  
  //create export PDF action
  exportPDFAction->setText(tr("&Export as PDF..."));
  exportPDFAction->setStatusTip(tr("Export actual page to PDF file"));
  exportPDFAction->setToolTip(exportPDFAction->statusTip());
  
  //--
  //create close tab action
  closePageAction->setText(tr("&Close Page"));
  closePageAction->setShortcut(tr("Ctrl+W"));
  closePageAction->setStatusTip(tr("Close actual page"));
  closePageAction->setToolTip(closePageAction->statusTip());
  
  //create close all tabs action
  closeAllPagesAction->setText(tr("&Close All Pages"));
  closeAllPagesAction->setShortcut(tr("Ctrl+Alt+W"));
  closeAllPagesAction->setStatusTip(tr("Close all pages"));
  closeAllPagesAction->setToolTip(closeAllPagesAction->statusTip());
  
  //create close app action
  exitAction->setText(tr("E&xit"));
  exitAction->setShortcut(tr("Ctrl+Q"));
  exitAction->setStatusTip(tr("Exit ") + PROJECT_NAME);
  exitAction->setToolTip(exitAction->statusTip());
  
//menu EDIT
  //create undo action
  undoAction->setText(tr("&Undo"));
  undoAction->setShortcut(tr("Ctrl+Z"));
  undoAction->setStatusTip(tr("Undo last typing"));
  undoAction->setToolTip(undoAction->statusTip());
  
  //create redo action
  redoAction->setText(tr("&Redo"));
  redoAction->setShortcut(tr("Ctrl+Y"));
  redoAction->setStatusTip(tr("Redo last undo"));
  redoAction->setToolTip(redoAction->statusTip());
  
  //create cut action
  cutAction->setText(tr("Cu&t"));
  cutAction->setShortcut(tr("Ctrl+X"));
  cutAction->setStatusTip(tr("Cut selected text to clipboard"));
  cutAction->setToolTip(cutAction->statusTip());
  
  //create copy action
  copyAction->setText(tr("&Copy"));
  copyAction->setShortcut(tr("Ctrl+C"));
  copyAction->setStatusTip(tr("Copy selected text to clipboard"));
  copyAction->setToolTip(copyAction->statusTip());
  
  //create paste action
  pasteAction->setText(tr("&Paste"));
  pasteAction->setShortcut(tr("Ctrl+V"));
  pasteAction->setStatusTip(tr("Insert text from clipboard at selection"));
  pasteAction->setToolTip(pasteAction->statusTip());
  
  //create selectAll action
  selectAllAction->setText(tr("Select &all"));
  selectAllAction->setShortcut(tr("Ctrl+A"));
  selectAllAction->setStatusTip(tr("Select all text in the current window"));
  selectAllAction->setToolTip(selectAllAction->statusTip());
  
  //create find&replace action
  findAction->setText(tr("&Find..."));
  findAction->setShortcut(tr("Ctrl+F"));
  findAction->setStatusTip(tr("Find and replace the given item"));
  findAction->setToolTip(findAction->statusTip());
  
//menu VIEW
  //create splitter Horizontal orientation action
  windowsHorAction->setText(tr("Window &horizontal split mode"));
  windowsHorAction->setStatusTip(tr("Change window alignment to horizontal"));
  windowsHorAction->setToolTip(windowsHorAction->statusTip());
  
  //create splitter Vertical orientation action
  windowsVerAction->setText(tr("Window &vertical split mode"));
  windowsVerAction->setStatusTip(tr("Change window alignment to vertical"));
  windowsVerAction->setToolTip(windowsVerAction->statusTip());
   
  //create declarations visible action
  declVisibleAction->setText(tr("Show &Declarations"));
  declVisibleAction->setShortcut(Qt::ALT + Qt::Key_Backspace);
  declVisibleAction->setStatusTip(tr("Show declarations window"));
  declVisibleAction->setToolTip(declVisibleAction->statusTip());

  //create fileList visible action
  fileListVisibleAction->setText(tr("Show &File list"));
  fileListVisibleAction->setShortcut(tr("F4"));
  fileListVisibleAction->setStatusTip(tr("Show file list on the left main window side"));
  fileListVisibleAction->setToolTip(fileListVisibleAction->statusTip());

  //create minimize window action
  MinimizeAction->setText(tr("Minimize"));
  MinimizeAction->setShortcut(tr("Ctrl+M"));

  //create maximize window action  
  ZoomAction->setText(tr("Zoom"));

  //create change input window action
  changeInputWindowAction->setText(tr("&Change Inputwindow"));
  #ifdef Q_WS_WIN
    changeInputWindowAction->setShortcut(Qt::CTRL + Qt::Key_Tab);
  #else
    changeInputWindowAction->setShortcut(Qt::ALT + Qt::Key_Tab);
  #endif
  changeInputWindowAction->setStatusTip(tr("Change cursor input to opposite window"));
  changeInputWindowAction->setToolTip(changeInputWindowAction->statusTip());
    
  //create file view action
  fileViewAction->setText(tr("&Show File content"));
  fileViewAction->setShortcut(tr("F7"));
  fileViewAction->setStatusTip(tr("Show complete file content"));
  fileViewAction->setToolTip(fileViewAction->statusTip());
  
//menu TOOLS
  //create Pref Action
  PrefAction->setText(tr("&Preferences..."));
#ifdef Q_WS_MAC
  PrefAction->setShortcut(tr("Ctrl+,"));
#else
  PrefAction->setShortcut(tr("Ctrl+F12"));
#endif
  PrefAction->setStatusTip(tr("Open the preferences dialog"));
  PrefAction->setToolTip(PrefAction->statusTip());
  
  //create prg lang. sw. AWL action
  prglng_AWLAction->setText(tr("&IL"));
  prglng_AWLAction->setStatusTip(tr("Change programming language to IL/AWL"));
  prglng_AWLAction->setToolTip(prglng_AWLAction->statusTip());
  
  //create prg lang. sw. ST action
  prglng_STAction->setText(tr("&ST"));
  prglng_STAction->setStatusTip(tr("Change programming language to ST"));
  prglng_STAction->setToolTip(prglng_STAction->statusTip());
  
  //create call fb action 
  insertFBAction->setText(tr("Insert Function &block"));
  insertFBAction->setShortcut(tr("Ctrl+B"));
  insertFBAction->setStatusTip(tr("Insert function block with its call model"));
  insertFBAction->setToolTip(insertFBAction->statusTip());
  
//menu HELP
  //create about Qt lib action
  aboutQtAction->setText(tr("About &Qt ..."));
  aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
  aboutQtAction->setToolTip(aboutQtAction->statusTip());
 
  //create F1 action
  F1Action->setText(tr("&Help..."));
  F1Action->setShortcut(tr("F1"));
  F1Action->setStatusTip(tr("Show the application's help file"));
  F1Action->setToolTip(F1Action->statusTip());
  
  //create HelpBubbles action
  helpBubblesAction->setText(tr("Help bubbles"));
  helpBubblesAction->setStatusTip(tr("Show help bubbles under mouse"));
  helpBubblesAction->setToolTip(helpBubblesAction->statusTip());
  
  //create bug report action
  bugReportAction->setText(tr("Send bugreport"));
  bugReportAction->setStatusTip(tr("Send a bug report by e-mail"));
  bugReportAction->setToolTip(bugReportAction->statusTip());
  
  //create check for program updates action
  checkForUpdatesAction->setText(tr("Check for updates"));
  checkForUpdatesAction->setStatusTip(tr("Check for program updates"));
  checkForUpdatesAction->setToolTip(checkForUpdatesAction->statusTip());

  //create about app action
  aboutAction->setText(tr("&About..."));
  aboutAction->setStatusTip(tr("Show the application's About box"));
  aboutAction->setToolTip(aboutAction->statusTip());
  
//TOOLBAR
  //create AWL LD action
  AWL_LDAction->setStatusTip(tr("Insert LD command"));
  AWL_LDAction->setToolTip(AWL_LDAction->statusTip());
  
  //create AWL ST action
  AWL_STAction->setStatusTip(tr("Insert ST command"));
  AWL_STAction->setToolTip(AWL_STAction->statusTip());
  
  //create AWL JMP action
  AWL_JMPAction->setStatusTip(tr("Insert JMP command"));
  AWL_JMPAction->setToolTip(AWL_JMPAction->statusTip());
  
  //create AWL FB CAL action
  AWL_FBCALAction->setStatusTip(tr("Insert functionblock call"));
  AWL_FBCALAction->setToolTip(AWL_FBCALAction->statusTip());
  
  //create SAWL AND action
  SAWL_ANDAction->setStatusTip(tr("Insert AND command"));
  SAWL_ANDAction->setToolTip(SAWL_ANDAction->statusTip());
  
  //create SAWL OR action
  SAWL_ORAction->setStatusTip(tr("Insert OR command"));
  SAWL_ORAction->setToolTip(SAWL_ORAction->statusTip());
  
  //create SAWL Ass action
  SAWL_AssignAction->setStatusTip(tr("Insert assignement command"));
  SAWL_AssignAction->setToolTip(SAWL_AssignAction->statusTip());
  
  //create SAWL SPA action
  SAWL_SPAAction->setStatusTip(tr("Insert SPA command"));
  SAWL_SPAAction->setToolTip(SAWL_SPAAction->statusTip());
  
  //create SAWL FB CAL action
  SAWL_FBCALAction->setStatusTip(tr("Insert functionblock call"));
  SAWL_FBCALAction->setToolTip(SAWL_FBCALAction->statusTip());
  
  //create SAWL NW action
  SAWL_NWAction->setStatusTip(tr("Insert network block"));
  SAWL_NWAction->setToolTip(SAWL_NWAction->statusTip());
  
  //create ST If Then Else action
  ST_IfThenElseAction->setStatusTip(tr("Insert If Then Else block"));
  ST_IfThenElseAction->setToolTip(ST_IfThenElseAction->statusTip());
  
  //create ST Case action
  ST_CaseAction->setStatusTip(tr("Insert Case block"));
  ST_CaseAction->setToolTip(ST_CaseAction->statusTip());
  
  //create ST For To Do action
  ST_ForToDoAction->setStatusTip(tr("Insert For To Do loop"));
  ST_ForToDoAction->setToolTip(ST_ForToDoAction->statusTip());
  
  //create ST While Do action
  ST_WhileDoAction->setStatusTip(tr("Insert While Do loop"));
  ST_WhileDoAction->setToolTip(ST_WhileDoAction->statusTip());
  
  //create ST Repeat Until action
  ST_RepeatUntilAction->setStatusTip(tr("Insert Repeat Until loop"));
  ST_RepeatUntilAction->setToolTip(ST_RepeatUntilAction->statusTip());
  
  //create ST Assign action
  ST_AssignAction->setStatusTip(tr("Insert assignement"));
  ST_AssignAction->setToolTip(ST_AssignAction->statusTip());
  
  //create ST FB CAL action
  ST_FBCALAction->setStatusTip(tr("Insert functionblock call"));
  ST_FBCALAction->setToolTip(ST_FBCALAction->statusTip());
  	

//messages
  //bug report
  msg_subject_bugreport = tr("Bugreport");
  msg_body_bugreport = tr("Please report all noticed bugs and inconsistencies of"); 
  //POU altering PRG/FB/F
  msg_text_POUaltered = tr("POU type altered to");
  msg_error_POUaltered = tr("POU type was not altered.");
  //file open msg
  msg_head_fileopen = tr("Open POU files");
  msg_ext_fileopen = tr("Supported files (*.txt *.poe *.EXP *.awl *.scl *.IL *.ST *.asc)");
  msg_error_fileopen = tr("Problems during open file:");
  //file save
  msg_head_filesave = tr("Save POU file");;
  msg_ext_filesave = tr("plain POU (*.txt);;CoDeSys (*.EXP);;MELSOFT (*.asc);;MULTIPROG IL (*.IL);;MULTIPROG ST (*.ST);;STEP 7 AWL (*.AWL);;STEP 7 SCL (*.scl);;Sucosoft S40 (*.poe)");
  msg_error_filesave = tr("Unable to save POU file:");
  //printing
  msg_printedwith = tr("printed with");
  msg_printing = tr("Printing Document");
  msg_head_PDFExport = tr("Export to PDF");
  //pref. error at start-up
  msg_head_preferror = tr(" Preferences Error");
  msg_text_preferror = tr("A settings error was dected.\nThe defaults have been set.");
  //prototype
  msg_error_prototypesave = tr("Unable to save prototype file:");
  //page number in statusbar text
  msg_page_no = tr("page");
  //save session inputwindow 
  msg_head_sessioncreate = tr("Insert Session name:");
  //unsaved POU message for close pages 
  msg_text_closepages = tr("One or more of the POUs are unsaved.<br>Do you want to continue?");
  
  //buttons
  okButtonText = tr("OK");
  cancelButtonText = tr("Cancel");
}


void MainWindow::createToolBars()
{ //! create toolbars

//main toolbar
  toolBarMain = addToolBar("toolBarMain");
  toolBarMain->setMovable(true);
  toolBarMain->setMinimumWidth(50);
  toolBarMain->setToolButtonStyle(Qt::ToolButtonIconOnly);
  toolBarMain->setIconSize(QSize(24, 24));
  toolBarMain->addAction(newPLC_PRGAction);
  toolBarMain->addAction(newPLC_FBAction);
  toolBarMain->addAction(newPLC_FcAction);
  toolBarMain->addSeparator();
  toolBarMain->addAction(openAction);
  toolBarMain->addAction(saveAction);
  toolBarMain->addAction(printAction);
  toolBarMain->addSeparator();
  toolBarMain->addAction(undoAction);
  toolBarMain->addAction(redoAction);
  toolBarMain->addAction(findAction);
  toolBarMain->addSeparator();

//iec61131-3 awl toolbar
  toolBarAWL = addToolBar("toolBarAWL");
  toolBarAWL->setMovable(true);
  toolBarAWL->setMinimumWidth(50);
  toolBarAWL->setToolButtonStyle(Qt::ToolButtonIconOnly);
  toolBarAWL->setIconSize(QSize(24, 24));
  toolBarAWL->addAction(AWL_LDAction);
  toolBarAWL->addAction(AWL_STAction);
  toolBarAWL->addAction(AWL_JMPAction);
  toolBarAWL->addSeparator();
  toolBarAWL->addAction(AWL_FBCALAction);

//siemens awl toolbar
  toolBarSAWL = addToolBar("toolBarSAWL");
  toolBarSAWL->setMovable(true);
  toolBarSAWL->setMinimumWidth(50);
  toolBarSAWL->setToolButtonStyle(Qt::ToolButtonIconOnly);
  toolBarSAWL->setIconSize(QSize(24, 24));
  toolBarSAWL->addAction(SAWL_ANDAction);
  toolBarSAWL->addAction(SAWL_ORAction);
  toolBarSAWL->addAction(SAWL_AssignAction);
  toolBarSAWL->addAction(SAWL_SPAAction);
  toolBarSAWL->addSeparator();
  toolBarSAWL->addAction(SAWL_FBCALAction);
  toolBarSAWL->addAction(SAWL_NWAction);
  //hide SAWLToolBar at creation time
  toolBarSAWL->hide(); 

//iec61131-3 ST toolbar
  toolBarST = addToolBar("toolBarST");
  toolBarST->setMovable(true);
  toolBarST->setMinimumWidth(50);
  toolBarST->setToolButtonStyle(Qt::ToolButtonIconOnly);
  toolBarST->setIconSize(QSize(24, 24));
  toolBarST->addAction(ST_IfThenElseAction);
  toolBarST->addAction(ST_CaseAction);
  toolBarST->addSeparator();
  toolBarST->addAction(ST_ForToDoAction);
  toolBarST->addAction(ST_WhileDoAction);
  toolBarST->addAction(ST_RepeatUntilAction);
  toolBarST->addSeparator();
  toolBarST->addAction(ST_AssignAction);
  toolBarST->addAction(ST_FBCALAction);

  #ifdef Q_WS_MAC
  //draw toolbars in brushed metal on Mac OS X
	toolBarMain->setAttribute(Qt::WA_MacBrushedMetal);
	toolBarAWL->setAttribute(Qt::WA_MacBrushedMetal);
	toolBarSAWL->setAttribute(Qt::WA_MacBrushedMetal);
	toolBarST->setAttribute(Qt::WA_MacBrushedMetal);
  #endif
}


void MainWindow::createStatusBar()
{ //! create statusbar

  st_page_no = new QLabel(msg_page_no);
  st_page_no->setAlignment(Qt::AlignHCenter);
  st_page_no->setMinimumSize(80,10);

  st_fileDate = new QLabel("");
  st_fileDate->setAlignment(Qt::AlignRight);
  st_fileDate->setMinimumSize(130,10);
  
  st_fileType = new QLabel("NULL");
  st_fileType->setAlignment(Qt::AlignHCenter);
  st_fileType->setMinimumSize(35,10);
  
  st_PRG_LNG = new QLabel("");
  st_PRG_LNG->setAlignment(Qt::AlignHCenter);
  st_PRG_LNG->setMinimumSize(35,10);

  st_Dec_cursor_pos = new QLabel("DEC: ");
  st_Dec_cursor_pos->setAlignment(Qt::AlignHCenter);
  st_Dec_cursor_pos->setMinimumSize(80,10);

  st_Ins_cursor_pos = new QLabel("INS: ");
  st_Ins_cursor_pos->setAlignment(Qt::AlignHCenter);
  st_Ins_cursor_pos->setMinimumSize(80,10);
  
  statusBar()->addWidget(st_page_no, 0);
  statusBar()->addWidget(st_fileDate, 0);
  statusBar()->addPermanentWidget(st_fileType, 0);
  statusBar()->addPermanentWidget(st_PRG_LNG, 0);
  statusBar()->addPermanentWidget(st_Dec_cursor_pos, 0);
  statusBar()->addPermanentWidget(st_Ins_cursor_pos, 0);
}


void MainWindow::setTypeData(int Type)
{ //! set POU type data depending on type input

 switch ( Type )
  {
    case 1  : { //Program
  	            pageCtrl1->setCurrentTypeData("PROGRAM", "", "END_PROGRAM");
                break;                                     //leave case contruct
              } //end case 1
    case 2  : { //Function block
  	            pageCtrl1->setCurrentTypeData("FUNCTION_BLOCK", "", "END_FUNCTION_BLOCK");
                break;                                     //leave case contruct
              } //end case 2
    case 3  : { //Function
                pageCtrl1->setCurrentTypeData("FUNCTION", FncTypeStr, "END_FUNCTION");
                break;                                    //leave case contruct
              } //end case 3
    case 4  : { //VAR List
  	            pageCtrl1->setCurrentTypeData("GLOBAL_VARIABLE_LIST", "", "(* @OBJECT_END := 'Globale_Variablen' *)\n(* @CONNECTIONS := Globale_Variablen\nFILENAME : ''\nFILETIME : 0\nEXPORT : 0\nNUMOFCONNECTIONS : 0\n*)\n");
                break;                                    //leave case contruct
              } //end case 4
    case 5  : { //VAR List
  	            pageCtrl1->setCurrentTypeData("TYPE", ":", "END_TYPE");
                break;                                    //leave case contruct
              } //end case 4
   default :  { //default Functionblock 
                pageCtrl1->setCurrentTypeData("FUNCTION_BLOCK", "", "END_FUNCTION_BLOCK");
                break;                                     //leave case contruct
              } //end default
  }//end switch

  emit fileType_modified(); 

}



//=============================================================================
// - - - SLOTs-MENU FCN.s, TOOLBAR FCN.s - - -
//MENU FILE
void MainWindow::createNewPage()
{ //! create new page as PRG, FB, F or VAR list by detecing the calling action
	
  QAction *action = qobject_cast<QAction *>(sender());
  QString pouInfo = "NAME, LNG\n  DESCRIPTION\n  version: 1.0, DATE, DEVELOPER";
  
  //ask for calling action
  //PLC_PRG ----------------------------------------------
  if (action  == newPLC_PRGAction)
  { //create empty page
    pageCtrl1->newPage();
    //set current type data
    setTypeData(1);
    //fill pages with presets, 1=insert string in TE1, 2=insert string in TE2  
    pageCtrl1->insertText("Dec", "VAR_GLOBAL\n\nEND_VAR\n\n\nVAR\n\nEND_VAR");  
    pageCtrl1->insertText("Ins", "(*main program: "+ pouInfo +" *)\n\n\n\n(*end of program*)");  
  }
  //PLC_FB -----------------------------------------------
  if (action  == newPLC_FBAction)
  { //create empty page
    pageCtrl1->newPage();
    //set current type data
    setTypeData(2);
    //fill pages with presets, 1=insert string in TE1, 2=insert string in TE2  
    pageCtrl1->insertText("Dec", "VAR_INPUT\n\nEND_VAR\n\n\nVAR_OUTPUT\n\nEND_VAR\n\n\nVAR\n\nEND_VAR");  
    pageCtrl1->insertText("Ins", "(*function_block: "+ pouInfo +" *)\n\n\n\n(*end of function_block*)");
  }
  //PLC_Fc -----------------------------------------------
  if (action  == newPLC_FcAction)
  { //create empty page
    pageCtrl1->newPage();
    if (fncDialog->exec())       //if accepted get Fcn name
    {
      FncTypeStr = fncDialog->currentName();
      //set current type data
      setTypeData(3);
      //fill pages with presets, 1=insert string in TE1, 2=insert string in TE2  
      pageCtrl1->insertText("Dec", "VAR_INPUT\n\nEND_VAR\n\n\nTYPE\n\nEND_TYPE\n\n\nVAR\n\nEND_VAR");  
      pageCtrl1->insertText("Ins", ("(*" + FncTypeStr.remove(FncTypeStr.indexOf(":"), 2) + " function: "+ pouInfo +" *)\n\n\n\n(*end of function*)"));
    }
    else                     //if rejected return without new page
      pageCtrl1->deletePage();
  }
  //PLC_VAR ----------------------------------------------
  if (action  == newPLC_VARAction)
  { //create empty page
    pageCtrl1->newPage();
    //set current type data
    setTypeData(4);
    //fill pages with presets, 1=insert string in TE1, 2=insert string in TE2  
    pageCtrl1->insertText("Dec", "(*global variable list: NAME,\n  DESCRIPTION\n*)\n\nVAR_GLOBAL\n\nEND_VAR\n");  
    //delete TE2 and MHC in current page
    pageCtrl1->delVARTypeItems();
  }
  //PLC_TYPE ----------------------------------------------
  if (action  == newPLC_TYPEAction)
  { //create empty page
    pageCtrl1->newPage();
    //set current type data
    setTypeData(5);
    //fill pages with presets, 1=insert string in TE1, 2=insert string in TE2  
    pageCtrl1->insertText("Dec", "(*type definition: NAME,\n  DESCRIPTION\n*)\n\nSTRUCT\n\nEND_STRUCT\n");  
    //delete TE2 and MHC in current page
    pageCtrl1->delVARTypeItems();
  }
  
  
  //external function calls
  //pageCtrl1->setCurrentKMHeader(importExport->createKMHeader("")); //setup new header
  pageCtrl1->setNewSplitterVisible();
  pageCtrl1->createHighlighters(0);
  pageCtrl1->setCurrentFileDate("*");
  //internal function calls
  updateDeclarationsVisible(0);
  updateStatusBar();
}


void MainWindow::openFiles()
{ //! open files implementation with add. call prefDialog->savePref() to save file path
	
  bool ok = false;
  QString Str = "";
  
  //create opendialog and get selected files
  QStringList files = QFileDialog::getOpenFileNames(this, msg_head_fileopen, importExport->lastDir, msg_ext_fileopen);
   
  //file handling
  if (!files.isEmpty())//if1
	ok = importExport->OpenPrepare(files);
  else return;

  if (ok == false)//if2
  {	//send error message
  	foreach (QString tmpStr, importExport->errorFiles) 
  	  Str = Str +"<br>"+ tmpStr;
  	QMessageBox::critical(this, PROJECT_NAME, msg_error_fileopen + "<br>" + Str);  
  }
  
  prefDialog->savePref(); //save the current file path if enabled
}


void MainWindow::openFile(QString Str)
{ //! open single file implemantation from string, with add. prefDialog->savePref() to save file path;  
  /*! the function is used by "singleApp" instance to open files by mouse "right-click" */
 
  bool ok=false;
  QStringList files;
  files.clear();
  
  if (Str.isEmpty())
  	return;
  	
  files << Str.split(";");
  
  //file handling
  if (!files.isEmpty())//if1
	ok = importExport->OpenPrepare(files);
  else return;

  if (ok == false)//if2
  {	//send error message
  	Str = "";
  	foreach (QString tmpStr, importExport->errorFiles) 
  	  Str = Str +"<br>"+ tmpStr;
  	QMessageBox::critical(this, PROJECT_NAME, msg_error_fileopen + "<br>" + Str);  
  }
  
  prefDialog->savePref();			//save the current file path if enabled
  this->showNormal();				//show the window normal	
}


void MainWindow::openRecentFile()
{ //! open recent file from FILE menu

  QAction *action = qobject_cast<QAction *>(sender());
  if (action)
    openFile(action->data().toString());
}


void MainWindow::POUInfo()
{ //! call POU info dialog

  pouInfoDialog->setDialog();
  pouInfoDialog->setPrototype();
  pouInfoDialog->exec();
}


void MainWindow::alterPOUType()
{ //! alter POU type to Prg, FB or Fcn
	
  QAction *action = qobject_cast<QAction *>(sender());
  QString Str= "";

  if (pageCtrl1->currentFileType().contains("GLOBAL_VARIABLE_LIST") || pageCtrl1->currentFileType().contains("TYPE") || pageCtrl1->currentFileType().contains("CONFIGURATION")) 
    return;

  //ask for calling action
  //alter to Prg ----------------------------------------------
  if (action  == alterToPrgAction)
  {
    setTypeData(1);
    Str = msg_text_POUaltered + " PROGRAM";    
  }
  //alter to FB -----------------------------------------------
  if (action  == alterToFBAction)
  {
    setTypeData(2);
    Str = msg_text_POUaltered + " FUNCTION_BLOCK";
  }
    
  //alter to Fcn ----------------------------------------------
  if (action  == alterToFcnAction)
  {
    //FncDialog dialog(this);  //create function dialog
    if (fncDialog->exec())     //if accepted get Fcn name
    {
      FncTypeStr = fncDialog->currentName();
      //set current type data
      setTypeData(3);
      Str = msg_text_POUaltered + " FUNCTION";  
    }
    else
      Str = msg_error_POUaltered;    
  }
  
  if(pageCtrl1->countPages() > 0)
    QMessageBox::information(this, PROJECT_NAME, Str);  

}


void MainWindow::showSessions()
{ //! call session manager

  //load sessions into ListWidget
  sessionManager->searchSessions();
  sessionManager->setButtonsState();
  sessionManager->show();
}


void MainWindow::saveSession()
{ //! save file links to actual pages as session

  bool ok;
  QString str = QInputDialog::getText(this, PROJECT_NAME, msg_head_sessioncreate, QLineEdit::Normal, "Session", &ok, Qt::Tool);
  if (ok && !str.isEmpty())
    sessionManager->saveFilesAsSession(str);
  else
    return;
}


void MainWindow::savePage()
{ //! save actual page, with add. call prefDialog->savePref() to save file path
	
  QString filePath="", fn="";
  bool error=false;
  
  //return if no page was create
  if (pageCtrl1->countPages() < 1)
    return;
  
  //test if file path is empty == new created file?
  filePath = pageCtrl1->currentFilePath();
  if ((filePath.isEmpty()) || (pageCtrl1->currentTabText() == "untitled" ))
    savePageAs();
  else
  {	
    //set file name
    fn = (filePath + pageCtrl1->currentFileName() + pageCtrl1->currentFileExtStr());
    //execute save routine	
    error = importExport->savePage(fn);	//save page
    //error messages
    if (error == 1)
      QMessageBox::critical(this, PROJECT_NAME, msg_error_filesave + "<br>" + fn);  
    if (error == 2)
      QMessageBox::critical(this, PROJECT_NAME, msg_error_prototypesave + "<br>" + fn.remove(fn.lastIndexOf("."), fn.length()) + ".ptt");  
  }
  
  //save preferences   	
  prefDialog->savePref();
}


void MainWindow::savePageAs()
{ //! save actual page using the save dialog, with add. call prefDialog->savePref() to save file path
  /*! (enforces called save routine to store GLOBAL_VARIABEL_LISTs as CoDeSys .EXP only) */
	
  bool error=false;
  int i=0;
  QString fn="", selectedFilter, extensionStr="", tmpStr="";
  QStringList tmpList;
  
  tmpList = msg_ext_filesave.split(";;", QString::SkipEmptyParts); //get all filter items
  for (i=0; i < tmpList.size(); i++)
  {
    tmpStr = tmpList.at(i);
    if (tmpStr.contains(pageCtrl1->definedPOUExtList.at(pageCtrl1->defaultPOUExt.toInt())))
      break;
  }  
  selectedFilter = tmpList[i]; //preset selectedFilter string by default POU extension
  
  //get file name from dialog
  fn = QFileDialog::getSaveFileName(this, msg_head_filesave, (importExport->lastDir + pageCtrl1->currentFileName()), msg_ext_filesave, &selectedFilter);

  //return if a file name is not set
  if (fn.isEmpty())
    return; //release timer in pref. dialog and return	
  
  //cut off existing file extension
  if (fn.lastIndexOf("/") < fn.lastIndexOf("."))
    fn.remove(fn.lastIndexOf("."), fn.length());

  //check which filter was selected and add it to the filename
  for (i=0; i < pageCtrl1->definedPOUExtList.count(); i++)
    if (selectedFilter.contains(pageCtrl1->definedPOUExtList[i], Qt::CaseInsensitive))
    {
      fn.append(pageCtrl1->definedPOUExtList[i]); //add correct extension
      break;
    }

  //force save routine to store TYPEs as CoDeSys .EXP only!
  if (pageCtrl1->currentFileType().contains("TYPE"))
    fn = fn.remove(fn.lastIndexOf("."), fn.length()) + ".EXP";
  //force save routine to handle GLOBAL_VARIABEL_LIST and CONFIGURATION
  if ((pageCtrl1->currentFileType().contains("GLOBAL_VARIABLE_LIST")) || (pageCtrl1->currentFileType().contains("CONFIGURATION")))
    if (i == 7) //force to store "CONFIGURATION" as MELSOFT .asc if selected
      fn = fn.remove(fn.lastIndexOf("."), fn.length()) + ".asc";
    else//force sto store GLOBAL_VARIABEL_LISTs as CoDeSys .EXP if not MELSOFT is selected
      fn = fn.remove(fn.lastIndexOf("."), fn.length()) + ".EXP";

  error = importExport->savePage(fn);
  //error messages
  if (error == 1)
    QMessageBox::critical(this, PROJECT_NAME, msg_error_filesave + "<br>" + fn);  
  if (error == 2)
    QMessageBox::critical(this, PROJECT_NAME, msg_error_prototypesave + "<br>" + fn.remove(fn.lastIndexOf("."), fn.length()) + ".ptt");  

  prefDialog->savePref();
}


void MainWindow::batchConverter()
{ //! call batch converter function

  importExport->batchConverter();
}
	

void MainWindow::print()
{ //! print current page
	
  int i;
  QString Str;
  QFont font;
  QPrinter printer;

  if (pageCtrl1->countPages() < 1)
  	return;

  QStringList *tmpList = new QStringList();
  QTextEdit *TEPrint   = new QTextEdit();  
  TEPrint->hide(); //hide TEPrint, becc. its only for printing

  //set font family for TEPrint 
  #ifdef Q_WS_WIN	 //use Courier New for Win	
    font.setFamily("Courier New");								
  #else
    font.setFamily("Monaco");
  #endif
  font.setPointSize(8);  							//font size 8, was pageCtrl1->font.pointSize()
  font.setWeight(pageCtrl1->font.weight());			//font weight = value from class PageCtrl
  TEPrint->setFont(font);							//set font to TEPrint
  TEPrint->setTabStopWidth(pageCtrl1->currentTabStopWidth());//tabstops = value from current page

  //clear printList
  tmpList->clear();
  //set print file headline
  tmpList->append(msg_printedwith + " " + PROJECT_NAME + " " + VERSION + "\n");
  //get data from pageCtrl1 + current Sucosoft header
  printPrepare->prepareCurrentPage(tmpList, importExport->createSucosoftHeader(pageCtrl1->currentSucosoftHeader()));
  
  //fill TEPrint with tmpList content
  for (i=0; i < tmpList->size(); i++)//for1
       Str.append(tmpList->value(i));
    TEPrint->append(Str);
 
  //reference action which requests the print
  QAction *action = qobject_cast<QAction *>(sender()); 
  if (action  == printAction)
  {    
    //call printer dialog
    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(msg_printing);
    if (dialog->exec() != QDialog::Accepted)
      return;
    //set document name
    printer.setDocName(pageCtrl1->currentFileName());
    //span the page to full size
    printer.setFullPage(true); 
    //set color mode
    printer.setColorMode(QPrinter::Color);
    //print the content of TEPrint
    TEPrint->print(&printer);
  }
  
  if (action == exportPDFAction)
  {
    #ifndef QT_NO_PRINTER
    QString fileName = QFileDialog::getSaveFileName(this, msg_head_PDFExport, (importExport->lastDir + pageCtrl1->currentFileName()), "*.pdf"); 
    if (!fileName.isEmpty()) 
    {
      if (QFileInfo(fileName).suffix().isEmpty())
        fileName.append(".pdf");
       QPrinter printer(QPrinter::HighResolution);
       printer.setFullPage(true);
       printer.setOutputFormat(QPrinter::PdfFormat);
       printer.setOutputFileName(fileName);
       TEPrint->document()->print(&printer);
    }
    #endif
  }
  
  //delete TEPrint and tmpList
  delete TEPrint;
  delete tmpList;
}


void MainWindow::closeAllPages()
{ //! delete all pages

  bool modified=pageCtrl1->pagesModified(), unsavedNoticeFlag = pageCtrl1->unsavedNotice;

  //activate a message box if a page was changed
  if (unsavedNoticeFlag && modified)
    if(QMessageBox::warning(this, PROJECT_NAME, msg_text_closepages, okButtonText, cancelButtonText, 0, 2 ) == 1)
      return;

  //close pages
  pageCtrl1->unsavedNotice = false;  			//set unsaved notice to falso to override message in class PageCtrl
  for (int i=pageCtrl1->countPages(); i>-1 ; i--)
    pageCtrl1->deletePage();					//close pages if message was accepted with YES
  pageCtrl1->unsavedNotice = unsavedNoticeFlag;	//reset unsaved notice
}


//MENU EDIT
void MainWindow::find()
{ //! call find & replace dialog

  if (!pageCtrl1->activeInputWindow().isEmpty())
  {
    findDialog->window = pageCtrl1->activeInputWindow();
    findDialog->showDialog();
  }
}


//MENU VIEW
void MainWindow::windowsAlignHor()
{ //! set window alignment to horizontal

  windowsHorAction->setChecked(true);
  windowsVerAction->setChecked(false);
  pageCtrl1->splitMode = "h";
  for (int i=0; i < pageCtrl1->countPages(); i++)
    pageCtrl1->setSplitModeOf(i, pageCtrl1->splitMode);
  //if SLOT was called by internal action set prefDialog value  
  if (qobject_cast<QAction *>(sender()) == windowsHorAction)
    prefDialog->Pref_comboBox_SplitMode->setCurrentIndex(0);
}


void MainWindow::windowsAlignVer()
{ //! set window alignment to vertical
   
  windowsHorAction->setChecked(false);
  windowsVerAction->setChecked(true);
  pageCtrl1->splitMode = "v";
  for (int i=0; i < pageCtrl1->countPages(); i++)
    pageCtrl1->setSplitModeOf(i, pageCtrl1->splitMode);
  //if SLOT was called by internal action set prefDialog value
  if (qobject_cast<QAction *>(sender()) == windowsVerAction)
    prefDialog->Pref_comboBox_SplitMode->setCurrentIndex(1);
}


void MainWindow::showFileView()
{ //! show file content in seperate window
    
  if (pageCtrl1->countPages() < 1)
  	return;

  int Ext = pageCtrl1->currentFileExt();
    
  //force save routine to store TYPEs as CoDeSys .EXP only!
  if (pageCtrl1->currentFileType().contains("TYPE"))
    Ext = 2;
  //force save routine to handle GLOBAL_VARIABEL_LIST and CONFIGURATION
  if ((pageCtrl1->currentFileType().contains("GLOBAL_VARIABLE_LIST")) || (pageCtrl1->currentFileType().contains("CONFIGURATION")))
    if (Ext == 7) 
      Ext = 7; //force to store "CONFIGURATION" as MELSOFT .asc if selected
    else 
      Ext = 2; //force sto store GLOBAL_VARIABEL_LISTs as CoDeSys .EXP if not MELSOFT is selected

  //set file extension in dialog
  fileViewDialog->setFileExt(Ext);
  //set FileViewDialog visible 
  fileViewDialog->show();
}


void MainWindow::setCurrentDeclarationVisible()
{ //! show & hide declarations section
	
  if (pageCtrl1->currentDeclarationIsVisible() == true)
  {  
    pageCtrl1->setCurrentDeclarationVisible(false);
  	declVisibleAction->setChecked(false);
  }
  else
  {  
    pageCtrl1->setCurrentDeclarationVisible(true);
  	declVisibleAction->setChecked(true);
  }
}


void MainWindow::setFileListVisible()
{ //! show & hide file list

  if (pageCtrl1->flg_fileListVisible == false)
  {  
  	pageCtrl1->setFileListVisible(true);
  	fileListVisibleAction->setChecked(true);
  	prefDialog->Pref_checkBox_FileList->setChecked(true);
  }
  else
  {  
  	pageCtrl1->setFileListVisible(false);
  	fileListVisibleAction->setChecked(false);
  	prefDialog->Pref_checkBox_FileList->setChecked(false);
  }

}


//MENU TOOLS
void MainWindow::openPrefDialog()
{ //! open preferences dialog

  prefDialog->savePref();
  prefDialog->exec();
}


//MENU HELP
void MainWindow::showHelpBrowser()
{ //! show help browser

  helpBrowser->show();
}


void MainWindow::HelpBubbles()
{ //! hide & show tool tips

}


void MainWindow::sendBugReport()
{ //! send a prepared bug report to the developer
	
  QString subject = (PROJECT_NAME + " " + VERSION + " " + REVISION + " " + msg_subject_bugreport);
  QString body = (msg_body_bugreport + " " + PROJECT_NAME + ".");
  
  bool ok = QDesktopServices::openUrl("mailto:" + PROJECT_MAIL + "?subject=" + subject + "&body=" + body);
}


void MainWindow::checkForUpdates()
{ //! check for program update

  delete http_buffer; 					//delete old buffer
  http_buffer = new QBuffer(this);		//create new buffer
  http->setHost(PROJECT_URL);			//set host
  http->get(UPDATE_PATH, http_buffer);	//get data into buffer
}


void MainWindow::httpRequestFinished(bool error)
{ //! embedded http function to check for program update
	
  QString tmpStr="", tmpStr2="", version="", revision="", changelog="";
  int revision_int=0, version_int=0;
  bool rd_error=true;
  QStringList *tmpList = new QStringList();
  
  //update check
  if (!error)
  { //if slot was called without any error go ahead to check web path
    tmpStr = http_buffer->data(); 		//get buffer data
    tmpList->append(tmpStr.split("\n"));
    
    if ((tmpList->size() > 4) && tmpList->contains("[version]"))  //check size and if version hint is embedded
      rd_error = false; //set error if first line in version.txt is not correct
   
    //web data check
    if (!rd_error)
    { //if no read error is set go ahead to check version/update information
      tmpStr = tmpList->at(tmpList->indexOf("[version]")+1);		//get version
      version = tmpStr; version_int = (tmpStr.remove(".")).toInt();

      tmpStr = tmpList->at(tmpList->indexOf("[revision]")+1);		//get vrevision
      revision = tmpStr; revision_int = (tmpStr.remove("r")).toInt();

      for (int i = tmpList->indexOf("[changelog]")+1; i < tmpList->size(); i++)
        changelog.append(tmpList->at(i));							//get changelog

      tmpStr = VERSION;
      tmpStr2= REVISION;
      if ((version_int <= (tmpStr.remove(".")).toInt()) && (revision_int <= (tmpStr2.remove("r")).toInt())) //check version & revision
      { //show message only if the startup flag is not active
        if (!startup) //prevent any message during program start
          QMessageBox::information(this, PROJECT_NAME, tr("No program updates available."));
      }
      else 
      { //if program version < web version show update message box
        updateInfo->getUpdateData(version, revision, changelog);
        updateInfo->exec();
      }
    }
  }
  
  //error message
  if (error || rd_error &! startup) 
  { //if any error occured show message but not during program start
    switch (QMessageBox::critical(this, PROJECT_NAME, tr("Check for updates failed."), tr("&Try again"), tr("Cancel"), 0, 1)) 
    { //selection how to deal with the user input
      case 0: checkForUpdates(); return; break; // Try again
      case 1: return; break; // Cancel
    }
  }

  delete tmpList;
  startup = false;
}


void MainWindow::about()
{ //! open about dialog
  
  //move dialog into middle position of main window
  aboutDialog->move((this->pos().x() + (this->width()/2) - (aboutDialog->width()/2)), (this->pos().y() + (this->height()/2) - (aboutDialog->height()/2) + 20));
  
  //show dialog
  aboutDialog->exec();
}





//=============================================================================
// - - - diverse SLOTs - - - 
void MainWindow::clearRecentFilesAction()
{ //! clear recent files action

  for (int i = 0; i < maxRecentFiles; i++)
    recentFilesAction[i]->setVisible(false);
}


QRect MainWindow::currentPrgLngToolBarGeometry()
{ //! return current visible programming language toolbar geometry
  
  if (toolBarAWL->isVisible())
    return(toolBarAWL->geometry());

  if (toolBarSAWL->isVisible())
    return(toolBarSAWL->geometry());
    
  if (toolBarST->isVisible())
    return(toolBarST->geometry());

  return(QRect(0, 0, 30, 100));
}

/*
void MainWindow::doCompareWith()
{ //! slot handle file comparision with

  int tabIndex = pageCtrl1->currentTabIndex();

  diffDialog->selectRightFile();
    
  if (!pageCtrl1->filePathOf(tabIndex).isEmpty())
    diffDialog->readLeftFile(pageCtrl1->filePathOf(tabIndex) + pageCtrl1->fileNameOf(tabIndex) + pageCtrl1->fileExtStrOf(tabIndex));
  //else
    //diffDialog->readLeftFile();

  diffDialog->show();
}


void MainWindow::doCompareWithNext()
{ //! slot handle file comparision with next

  int tabIndex = pageCtrl1->currentTabIndex();
  
  if (!pageCtrl1->filePathOf(tabIndex).isEmpty())
    diffDialog->readLeftFile(pageCtrl1->filePathOf(tabIndex) + pageCtrl1->fileNameOf(tabIndex) + pageCtrl1->fileExtStrOf(tabIndex));
  //else
    //diffDialog->readLeftFile();
  
  if (!pageCtrl1->filePathOf(tabIndex+1).isEmpty())
    diffDialog->readRightFile(pageCtrl1->filePathOf(tabIndex+1) + pageCtrl1->fileNameOf(tabIndex+1) + pageCtrl1->fileExtStrOf(tabIndex+1));
  //else
    //diffDialog->readLeftFile();
  
  diffDialog->show();
}


void MainWindow::doCompareWithPrev()
{ //! slot handle file comparision with previous

  int tabIndex = pageCtrl1->currentTabIndex();

  if (!pageCtrl1->filePathOf(tabIndex-1).isEmpty())
    diffDialog->readLeftFile(pageCtrl1->filePathOf(tabIndex) + pageCtrl1->fileNameOf(tabIndex) + pageCtrl1->fileExtStrOf(tabIndex));
  //else
    //diffDialog->readLeftFile();

  if (!pageCtrl1->filePathOf(tabIndex).isEmpty())
    diffDialog->readRightFile(pageCtrl1->filePathOf(tabIndex-1) + pageCtrl1->fileNameOf(tabIndex-1) + pageCtrl1->fileExtStrOf(tabIndex-1));
  //else
    //diffDialog->readLeftFile();
    
  diffDialog->show();
}
*/

void MainWindow::fillRecentFilesAction(int i)
{ //! fill recent files action with list items from class PageCtrl
  //! \param i is only needed for signal-slot handling

  for (int i = 0; i < pageCtrl1->recentFiles.count(); i++)
  {
      recentFilesAction[i]->setText(QFileInfo(pageCtrl1->recentFiles[i]).fileName());
      recentFilesAction[i]->setData(pageCtrl1->recentFiles[i]);
      recentFilesAction[i]->setVisible(true);
  }
}


void MainWindow::handleMessage(const QString& message)
{ //! handle received mesage from singleapplication class

  QString filename(message);
  openFile(filename);
  emit needToShow();
}


void MainWindow::insertCMD()
{ //! insert commands by detected calling action from toolbars

  if (pageCtrl1->countPages() > 0)//if1
  { //ask for #tabs>0
    QString Str;                                          //create place holder
    QAction *action = qobject_cast<QAction *>(sender());  //reference action

    //ask for calling action - case ;-)
  //AWL ----------------------------------------------
    if (action  == AWL_LDAction)
    { //insert command
      Str = "LD ";
      //call insertText fcn. of class and insert string in TE2
      pageCtrl1->insertText("Ins", Str);
      return;
    }
    if (action == AWL_STAction)
    { //insert command
      Str = "ST ";
      //call insertText fcn. of class and insert string in TE2
      pageCtrl1->insertText("Ins", Str);
      return;
    }
    if (action == AWL_JMPAction)
    { //insert command
      Str = "JMP ";
      //call insertText fcn. of class and insert string in TE2
      pageCtrl1->insertText("Ins", Str);
      return;
    }
    if (action == AWL_FBCALAction)
    { //insert command
      insertFB();
      return;
    }
  //SAWL ---------------------------------------------
    if (action  == SAWL_ANDAction)
    { //insert command
      Str = "U ";
      //call insertText fcn. of class and insert string in TE2
      pageCtrl1->insertText("Ins", Str);
      return;
    }
    if (action == SAWL_ORAction)
    { //insert command
      Str = "O ";
      //call insertText fcn. of class and insert string in TE2
      pageCtrl1->insertText("Ins", Str);
      return;
    }
    if (action == SAWL_AssignAction)
    { //insert command
      Str = " = ";
      //call insertText fcn. of class and insert string in TE2
      pageCtrl1->insertText("Ins", Str);
      return;
    }
    if (action == SAWL_SPAAction)
    { //insert command
      Str = "SPA ";
      //call insertText fcn. of class and insert string in TE2
      pageCtrl1->insertText("Ins", Str);
      return;
    }
    if (action == SAWL_FBCALAction)
    { //insert command
      insertFB();
      return;
    }
    if (action == SAWL_NWAction)
    { //insert command
      Str = "NETWORK\nTITLE = <Title of Network>\n//\n//  <Network Comment> \n//\n";
      //call insertText fcn. of class and insert string in TE2
      pageCtrl1->insertText("Ins", Str);
      return;
    }
  //ST -----------------------------------------------
    if (action  == ST_IfThenElseAction)
    { //insert command
      Str = "IF  THEN\n     ;\nELSE\n     ;\nEND_IF;\n"; //insert command
      //call insertText fcn. of class and insert string in TE2
      pageCtrl1->insertText("Ins", Str);
      return;
    }
    if (action == ST_CaseAction)
    { //insert command
      Str = "CASE  OF\n 	1: ;\n 	2..3: ;\n 	ELSE ;\nEND_CASE;\n";
      //call insertText fcn. of class and insert string in TE2
      pageCtrl1->insertText("Ins", Str);
      return;
    }
    if (action == ST_ForToDoAction)
    { //insert command
      Str = "FOR I :=  TO  DO \n     ;\nEND_FOR;\n";
      //call insertText fcn. of class and insert string in TE2
      pageCtrl1->insertText("Ins", Str);
      return;
    }
    if (action  == ST_WhileDoAction)
    { //insert command
      Str = "WHILE  DO\n	 ;\nEND_WHILE;\n"; //insert command
      //call insertText fcn. of class and insert string in TE2
      pageCtrl1->insertText("Ins", Str);
      return;
    }
    if (action == ST_RepeatUntilAction)
    { //insert command
      Str = "REPEAT\n	 ;\nUNTIL\nEND_REPEAT;\n";
      //call insertText fcn. of class and insert string in TE2
      pageCtrl1->insertText("Ins", Str);
      return;
    }
    if (action == ST_AssignAction)
    { //insert command
      Str = " :=  ;";
      //call insertText fcn. of class and insert string in TE2
      pageCtrl1->insertText("Ins", Str);
      return;
    }
    if (action == ST_FBCALAction)
    { //insert command
      insertFB();
      return;
    }
  }//if1
}


void MainWindow::insertFB()
{ //! insert function block call model

  fbDialog->fillBlockList();
  if (fbDialog->exec())    //if accepted get Fcn name
    //call insertText fcn. of class PageCtrl and insert string in TE2
    pageCtrl1->insertText("Ins", fbDialog->selectedBlockModel() + "\n");
}


void MainWindow::switchPRGLang()
{ //! switch progrmaming language by mainwindow menu items
	
  QAction *action = qobject_cast<QAction *>(sender());  //reference action
  //set programming language in base calss PageCtrl
  if (action == prglng_AWLAction)
  {
  	pageCtrl1->setCurrentPrgLng("IL");
  	//prefDialog->Pref_comboBox_PrgLang->setCurrentIndex(0);
  }
  if (action == prglng_STAction)
  {
    pageCtrl1->setCurrentPrgLng("ST");
  	//prefDialog->Pref_comboBox_PrgLang->setCurrentIndex(1);
  }
  //contact preferences dialog
  prefDialog->savePref();
  //update statusbar
  updateStatusBar();
}


void MainWindow::setCurrentPrgLngToolBarGeometry(QSize s, QPoint p)
{ //! set programming language toolbar geometry
  
  toolBarAWL->resize(s);   toolBarAWL->move(p);
  toolBarSAWL->resize(s);  toolBarSAWL->move(p);
  toolBarST->resize(s);    toolBarST->move(p);
}


void MainWindow::setInterfaceItemsEnabled()
{ //! set enable property for some menu & toolbar items
	
  bool enable = false, varlist = false;
  
  if (pageCtrl1->countPages() < 1)
    enable = false;
  else 
  	enable = true;

  if ((pageCtrl1->currentFileType() == "GLOBAL_VARIABLE_LIST") || (pageCtrl1->currentFileType() == "CONFIGURATION"))
    varlist = true;
  else 
    varlist = false;
  
  //menu FILE
  POUInfoAction->setEnabled(enable);
  alterToPrgAction->setEnabled(enable);
  alterToFBAction->setEnabled(enable);
  alterToFcnAction->setEnabled(enable);
  saveSessionsAction->setEnabled(enable);
  saveAction->setEnabled(enable);
  saveAsAction->setEnabled(enable);
  batchConverterAction->setEnabled(enable);
  printAction->setEnabled(enable);
  exportPDFAction->setEnabled(enable);
  closePageAction->setEnabled(enable);
  closeAllPagesAction->setEnabled(enable);
  
  //menu EDIT
  undoAction->setEnabled(enable);
  redoAction->setEnabled(enable);
  cutAction->setEnabled(enable);
  copyAction->setEnabled(enable);
  pasteAction->setEnabled(enable);
  selectAllAction->setEnabled(enable);
  findAction->setEnabled(enable);
  
  //menu VIEW
  windowsHorAction->setEnabled(enable);
  windowsVerAction->setEnabled(enable);
  declVisibleAction->setEnabled(enable);
  fileListVisibleAction->setEnabled(enable);
  changeInputWindowAction->setEnabled(enable);
  fileViewAction->setEnabled(enable);
  
  //menu TOOLS
  prglng_AWLAction->setEnabled(enable);
  prglng_STAction->setEnabled(enable);
  insertFBAction->setEnabled((enable &! varlist));

  //toolbar MAIN
  
  //toolbar AWL
  toolBarAWL->setEnabled((enable &! varlist));
  
  //toolbar SAWL
  toolBarSAWL->setEnabled((enable &! varlist));
  
  //toolBar ST
  toolBarST->setEnabled((enable &! varlist));
}


void MainWindow::switchPRGLangToolBars(QString Str)
{ //! switch progrmaming language toolbars

  if (Str.contains("IL"))//if1
  { //if AWL detected hide ST page
    toolBarST->hide();
    //select correct AWL toolbar from current file extension
    if (pageCtrl1->currentFileExt() == 3) //if2
    { toolBarAWL->hide(); toolBarSAWL->show(); }
    else
    { toolBarAWL->show(); toolBarSAWL->hide(); }//if2
  }
  else
  { //if AWL not detected hide AWL page
    toolBarST->show(); toolBarAWL->hide(); toolBarSAWL->hide(); 
  }//if1
   
  if (PRGLangToolBarsVis == false)//if3
  { //hide all ToolBars if requested from preferences dialog
    toolBarST->hide(); toolBarAWL->hide(); toolBarSAWL->hide();
  }//if3
}


void MainWindow::updateDeclarationsVisible(int i)
{ //! update declarations visible status in menu view
  //! \param i is only needed for signal-slot handling
	
  if (pageCtrl1->currentDeclarationIsVisible() == true)
  	declVisibleAction->setChecked(true);
  else
  	declVisibleAction->setChecked(false);
}


void MainWindow::updateMainToolBarVisible(bool visible)
{ //! update main toolbar visible status

  toolBarMain->setVisible(visible);
}


void MainWindow::updateStatusBar()
{ //! update statusbar
	
  QString prglng;

  //show current üage of pages
  st_page_no->setText(" " + msg_page_no + " " + QString::number(pageCtrl1->currentTabIndex()+1) + "/" + QString::number(pageCtrl1->countPages()) + " ");
  //show file storage date
  st_fileDate->setText(" " + pageCtrl1->currentFileDate() + " ");
  //show file type as PRG, FB, FC : XYZ, VAR
  st_fileType->setText(" " + pageCtrl1->currentFileTypeShort() + " ");
  //get current programming language
  if (pageCtrl1->countPages() > 0)
    prglng = pageCtrl1->currentPrgLng();
  else
    prglng = pageCtrl1->prgLngStr;
  //show programming language
  st_PRG_LNG->setText(" " + prglng + " ");
  //show cursor position of declaration window
  st_Dec_cursor_pos->setText(" DEC " + pageCtrl1->cursorPosDec() + " ");
  //show cursor position of instruction window
  st_Ins_cursor_pos->setText(" INS " + pageCtrl1->cursorPosIns() + " ");
  
  //update programming language ToolBars
  switchPRGLangToolBars(prglng);
}
