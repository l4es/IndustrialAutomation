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
** Contact e-mail: M. Rehfeldt info@plcedit.org
** Program URL   : http://www.plcedit.org
**
****************************************************************************/


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextBrowser>
#include <QUrl>
#include <QtNetwork>

class QAction;
class QLabel;
class PageCtrl;
class PrefDialog;
class FileViewDialog;
class SessionManager;
class Prototype;
class PrintPrepare;
class ImportExport;
class POUInfoDialog;
class FindDialog;
class FncDialog;
class FBDialog;
class HelpWidget;
class AboutDialog;
class UpdateInfo;
//class DiffDialog;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
//CLASS
    MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();
    void closeEvent(QCloseEvent *event);

//VAR PROJECT
    QString PROJECT_NAME;		//!< project name
    QString VERSION;			//!< current project version
    QString REVISION;			//!< current project revision
    QString PROJECT_DATE;		//!< project date 2005..20XX
    QString PROJECT_URL;		//!< project web url
    QString PROJECT_MAIL;		//!< project e-mail
    QString UPDATE_PATH;		//!< project update path
    QString DOWNLOAD_PATH;		//!< project download path
    
//VAR DIVERSE
    bool    PRGLangToolBarsVis; //!< visible flag for programming language toolbars
    enum    { maxRecentFiles = 200 };//!< number of recent files to be hold
    QAction *recentFilesAction[maxRecentFiles];//!< action file menu recent files, size=100

//MENUS
    QMenu *fileMenu;			//!< file menu
    QMenu *fileSubMenu1;		//!< file sub menu NEW
    QMenu *fileSubMenu2;		//!< file sub menu ALTER POU TYPE
    QMenu *fileSubMenu3;		//!< file sub menu SESSIONS
    QMenu *fileSubMenuRecentFiles;//!< file sub menu RECENT FILES
    QMenu *editMenu;			//!< edit menu
    QMenu *viewMenu;			//!< view menu
    QMenu *toolsMenu;			//!< tools menu
    QMenu *toolsSubMenu1;		//!< tools sub menu PROGRAMMING LANGUAGES
    QMenu *helpMenu;			//!< help menu
    
//TOOLBARS
    QToolBar *toolBarMain;		//!< toolbar main
    QToolBar *toolBarAWL;		//!< toolbar for IEC61131-3 instruction list (IL)
    QToolBar *toolBarSAWL;		//!< toolbar for Siemens S7 instruction list (IL)
    QToolBar *toolBarST;		//!< toolbar for IEC61131-3 structured text (ST)
    
    
protected:


public slots:
//SLOTS
  //menu FILE
    void createNewPage();
    void openFiles();
    void openFile(QString Str);
    void openRecentFile();
    void POUInfo();
    void alterPOUType();
    void showSessions();
    void saveSession();
    void savePage();
    void savePageAs();
    void batchConverter();
    void print();
    void closeAllPages();
  //menu EDIT    
    void find(); 
  //menu VIEW    
    void windowsAlignHor();
    void windowsAlignVer();
    void setCurrentDeclarationVisible();
    void setFileListVisible();
    void showFileView();
  //menu TOOLS    
    void openPrefDialog();
  //menu HELP    
    void about();
    void showHelpBrowser();
    void HelpBubbles();
    void sendBugReport();
  //DIVERSE    
    void checkForUpdates(); 
    void clearRecentFilesAction();
    QRect currentPrgLngToolBarGeometry();
    /*
    void doCompareWith();
    void doCompareWithNext();
    void doCompareWithPrev();
    */
    void fillRecentFilesAction(int i);
    void handleMessage(const QString& message);
    void httpRequestFinished(bool);
    void insertCMD();
    void insertFB();
    void retranslateUi();
    void setCurrentPrgLngToolBarGeometry(QSize s, QPoint p);
    void setInterfaceItemsEnabled();
    void switchPRGLang();
    void switchPRGLangToolBars(QString Str);
    void updateDeclarationsVisible(int i);
    void updateMainToolBarVisible(bool visible);
    void updateStatusBar();

signals:
    void fileType_modified();		//!< signal file type modified
    void needToShow();


private:
//ACTIONS
  //menu FILE
    QAction *newPLC_PRGAction;		//!< action file menu new PROGRAM page
    QAction *newPLC_FBAction;		//!< action file menu new FUNTION_BLOCK page
    QAction *newPLC_FcAction;		//!< action file menu new FUNCTION page
    QAction *newPLC_VARAction;		//!< action file menu new GLOBAL_VAR page
    QAction *newPLC_TYPEAction;		//!< action file menu new TYPE
    QAction *openAction;			//!< action file menu show file open dialog
    QAction *POUInfoAction;			//!< action file menu show POU info dialog
    QAction *alterToPrgAction;		//!< action file menu alter POU to PROGRAM
    QAction *alterToFBAction;		//!< action file menu alter POU to FUNTION_BLOCK
    QAction *alterToFcnAction;		//!< action file menu alter POU to FUNCTION
    QAction *manageSessionsAction;	//!< action file menu show session dialog
    QAction *saveSessionsAction;	//!< action file menu open save session dialog to save current page as session
    QAction *saveAction;			//!< action file menu save current file
    QAction *saveAsAction;			//!< action file menu open save file dialog 
    QAction *batchConverterAction;	//!< action file menu open batch converter dialog
    QAction *printAction;			//!< action file menu open print dialog
    QAction *exportPDFAction;		//!< action file menu open PDF dialog
    QAction *closePageAction;		//!< action file menu close current page
    QAction *closeAllPagesAction;	//!< action file menu close all pages
    QAction *exitAction;			//!< action file menu quit application
  //menu EDIT
    QAction *undoAction;			//!< action edit menu undo
    QAction *redoAction;			//!< action edit menu redo
    QAction *cutAction;				//!< action edit menu cut
    QAction *copyAction;			//!< action edit menu copy
    QAction *pasteAction;			//!< action edit menu paste
    QAction *selectAllAction;		//!< action edit menu select all
    QAction *findAction;			//!< action edit menu find & replace
  //menu VIEW
    QAction *windowsHorAction;		//!< action view menu set windows to horizontal alignment 
    QAction *windowsVerAction;		//!< action view menu set windows to vertical alignment 
    QAction *declVisibleAction;		//!< action view set declaration input window visible
    QAction *MinimizeAction;		//!< action view minimize application window 
    QAction *ZoomAction;			//!< action view zoom application window to full screen
    QAction *fileListVisibleAction;	//!< action view set file list visible 
    QAction *changeInputWindowAction;//!<action view change input window for current page 
    QAction *fileViewAction;		//!< action view show output file in seperate window 
  //menu TOOLS
  	QAction *PrefAction;			//!< action tools menu show preferences dialog 
    QAction *prglng_AWLAction;		//!< action tools menu change programming language to IL 
    QAction *prglng_STAction;		//!< action tools menu change programming language to ST
    QAction *insertFBAction;		//!< action tools menu insert function block
  //menu HELP
    QAction *aboutAction;			//!< action help menu show project about dialog 
    QAction *aboutQtAction;			//!< action help menu show Qt about dialog 
    QAction *F1Action;				//!< action help menu show F1/help browser 
    QAction *helpBubblesAction;		//!< action help menu show help bubbles  
    QAction *bugReportAction;		//!< action help menu open e-mail to send bug report
    QAction *checkForUpdatesAction;	//!< action help menu check for program updates
  //TOOLBAR
    QAction *AWL_LDAction;			//!< action insert LD command for instruction list language 
    QAction *AWL_STAction;			//!< action insert ST command for instruction list language 
    QAction *AWL_JMPAction;			//!< action insert JMP command for instruction list language 
    QAction *AWL_FBCALAction;		//!< action insert FB call for instruction list language 
    QAction *SAWL_ANDAction;		//!< action insert AND command for siemens instruction list language 
    QAction *SAWL_ORAction;			//!< action insert OR command for siemens instruction list language 
    QAction *SAWL_AssignAction;		//!< action insert assignment sign for siemens instruction list language 
    QAction *SAWL_SPAAction;		//!< action insert SPA command for siemens instruction list language 
    QAction *SAWL_FBCALAction;		//!< action insert FB call for siemens instruction list language 
    QAction *SAWL_NWAction;			//!< action insert NETWORK block for siemens instruction list language 
    QAction *ST_IfThenElseAction;	//!< action insert if then else block for structured text language 
    QAction *ST_CaseAction;			//!< action insert case block for structured text language 
    QAction *ST_ForToDoAction;		//!< action insert for to do block for structured text language 
    QAction *ST_WhileDoAction;		//!< action insert while do block for structured text language 
    QAction *ST_RepeatUntilAction;	//!< action insert repeat until block for structured text language 
    QAction *ST_AssignAction;		//!< action insert assignment sign for structured text language 
    QAction *ST_FBCALAction;		//!< action insert FB call for structured text language 
  //DIVERSE

//STATUSBAR items
    QLabel *st_page_no;				//!< statusbar label for number of page in comparision to all pages
    QLabel *st_fileDate;            //!< statusbar label for information of file storage date
    QLabel *st_fileType;			//!< statusbar label for file/block type of current page
    QLabel *st_PRG_LNG;				//!< statusbar label for programming language of current page
    QLabel *st_Dec_cursor_pos; 		//!< statusbar label for cursor position in decalration section
    QLabel *st_Ins_cursor_pos; 		//!< statusbar label for cursor position in instruction section

//VAR DIVERSE
    QString FncTypeStr; 			//!< current function type string
    bool startup;					//!< program start flag 
    
//MESSAGES
    QString msg_subject_bugreport;	//!< message e-mail subject for bug report 
    QString msg_body_bugreport;		//!< message e-mail body for bug report  
    QString msg_text_POUaltered;	//!< message information for POU altered 
    QString msg_error_POUaltered;	//!< message error for POU altering 
    QString msg_head_fileopen;		//!< message head file open dialog
    QString msg_ext_fileopen;		//!< message extension for file open dialog
    QString msg_error_fileopen;		//!< message error file open 
    QString msg_head_filesave;		//!< message head for file save dialog
    QString msg_ext_filesave;		//!< message extension for file save dialog
    QString msg_error_filesave;		//!< message error file save 
    QString msg_printedwith;		//!< message printed with, will be included on head of print outs
    QString msg_printing;			//!< message printing active
    QString msg_head_PDFExport;		//!< message head for PDF export dialog
    QString msg_head_preferror;		//!< message head for preferences error
    QString msg_text_preferror;		//!< message text for preferences error
    QString msg_error_prototypesave;//!< message error for prototype storage
    QString msg_page_no;			//!< message page number in statusbar
    QString msg_head_sessioncreate;	//!< message save session
    QString msg_text_closepages;	//!< message unsaved POUs for close pages routine
    QString okButtonText;			//!< messagebox button text ok
    QString cancelButtonText;		//!< messagebox button text cancel
    
//INSTANCES   
    PageCtrl 		*pageCtrl1;		//!< instance of PageCtrl class
    PrefDialog 		*prefDialog;	//!< instance of PrefDialog class
    FileViewDialog 	*fileViewDialog;//!< instance of FileViewDialog class
    SessionManager 	*sessionManager;//!< instance of SessionManager class
    Prototype 		*prototype;		//!< instance of Prototype class
    PrintPrepare 	*printPrepare;	//!< instance of PrintPrepare class
    ImportExport 	*importExport;	//!< instance of ImportExport class
    POUInfoDialog 	*pouInfoDialog;	//!< instance of POUInfoDialog class
    FindDialog 		*findDialog;	//!< instance of FindDialog class
    FBDialog		*fbDialog;		//!< instance of FBDialog class
    FncDialog		*fncDialog;		//!< instance of FcnDialog class
    HelpWidget		*helpBrowser;	//!< instance of HelpWidget class
    AboutDialog		*aboutDialog;	//!< instance of AboutDialog class
    UpdateInfo		*updateInfo;	//!< instance of UpdateInfo class
    //DiffDialog		*diffDialog;	//!< instance of DiffDialog
    QHttp 			* http; 
    QBuffer 		* http_buffer;
    
//FUNCTIONS
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void setTypeData(int Type);
    
};

#endif
