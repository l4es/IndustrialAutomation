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
#include <QAction>
#include <QSettings>

#include "prefdialog.h"
#include "mainwindow.h"
#include "pagectrl.h"
#include "sessionmanager.h"
#include "importexport.h"
#include "printprepare.h"
#include "pouinfodialog.h"
#include "fileviewdialog.h"
#include "finddialog.h"
#include "fncdialog.h"
#include "fbdialog.h"
#include "helpwidget.h"
#include "aboutdialog.h"
#include "updateinfo.h"
//#include "diffdialog.h"


PrefDialog::PrefDialog(MainWindow *parent, PageCtrl *pageCtrl, SessionManager *sessionManager, ImportExport *importExport, PrintPrepare *printPrepare, POUInfoDialog *pouInfoDialog, FileViewDialog *fileViewDialog, FindDialog *findDialog, FncDialog *fncDialog, FBDialog *fbDialog, HelpWidget *helpWidget, AboutDialog *aboutDialog, UpdateInfo *updateInfo)
    : m_MainWindow(parent), m_PageCtrl(pageCtrl), m_SessionManager(sessionManager), m_ImportExport(importExport), m_PrintPrepare(printPrepare), m_POUInfoDialog(pouInfoDialog), m_FileViewDialog(fileViewDialog), m_FindDialog(findDialog), m_FncDialog(fncDialog), m_FBDialog(fbDialog), m_HelpWidget(helpWidget), m_AboutDialog(aboutDialog), m_UpdateInfo(updateInfo) //member VAR of PrefDialog
{ //! creating function
  //! \param PageCtrl class, SessionManager class, ImportExport class, PrintPrepare class, POUInfoDialog class, FileView class, FindDialog class, FcnDialog class, HelpWidget class

  setWindowFlags(windowFlags()^Qt::WindowContextHelpButtonHint);
    
  setupUi(this);
  retranslateUi();
  
  //set application data
  QApplication::setOrganizationName(m_MainWindow->PROJECT_NAME);
  QApplication::setOrganizationDomain(m_MainWindow->PROJECT_URL);
  QApplication::setApplicationName(m_MainWindow->PROJECT_NAME);

  //get application directory
  appDir = (QApplication::applicationDirPath());
  //create string Lang path
  langDir = (appDir + "/Lang/");
  //create Lang path if not exists
  if (!QDir(appDir + "/Lang/").exists());
    QDir(appDir).mkpath(appDir + "/Lang/");

  //install translator and get installed languages
  qApp->installTranslator(&appTranslator);
  getLanguages();
  
  //timer
  timer = new QTimer(this);
  setTimer(true);
  
  //different VAR settings at startup  
  m_PageCtrl->splitSize = 200;
  
  //set some width & sizes
  #ifdef Q_WS_MAC	
    //do nothing
  #else
    QFont font;
    font.setPointSize(8);
    Pref_comboBox_FontName->setFont(font);
  #endif


  //diverse connections
  connect(Pref_listWidget,                  SIGNAL(currentRowChanged(int)), Pref_stackedWidget, SLOT(setCurrentIndex(int)));    //change Stack with PrefList change
  connect(Pref_stackedWidget,               SIGNAL(currentChanged(int)),    this, SLOT(setPrefListRow(int)));                   //change PrefList with Stack change
  connect(Pref_resetButton,                 SIGNAL(clicked()),              this, SLOT(resetPrefDialog()));                     //reset button action
  connect(Pref_applyButton,                 SIGNAL(clicked()),              this, SLOT(applyPrefDialog()));                     //apply button action
  connect(Pref_cancelButton,                SIGNAL(clicked()),              this, SLOT(cancelDialog()));                        //cancel button action
  connect(Pref_okButton,                    SIGNAL(clicked()),              this, SLOT(okClickPrefDialog()));                   //ok button action
  connect(Pref_pushButton_presetDir,        SIGNAL(clicked()),              this, SLOT(setLastDir()));                          //set last Dir to ImportExport
  connect(Pref_pushButton_sessionDir,       SIGNAL(clicked()),              this, SLOT(setSessionDir()));                       //set last Dir to SessionManager
  connect(Pref_pushButton_clearRecentFiles, SIGNAL(clicked()),              this, SLOT(clearRecentFilesHistory()));             //clear recent files history
  connect(m_ImportExport,                   SIGNAL(modified()),             this, SLOT(getLastDir()));                          //get last Dir from ImportExport
  connect(m_ImportExport,                   SIGNAL(active()),               this, SLOT(stopTimer()));                           //stop timer if Imp/Exp. active to prevent uncontrolled file saving
  connect(m_ImportExport,                   SIGNAL(ready()),                this, SLOT(startTimer()));                          //start timer if Imp/Exp. ready
  connect(timer,                            SIGNAL(timeout()),              this, SLOT(saveAllPages()));                        //save all open pages if timer elapsed
  connect(Pref_checkBox_LinesHighlighter,   SIGNAL(clicked()),              this, SLOT(checkHiLineColor()));                    //check color for current line
  connect(Pref_toolButton_HiLineColor,      SIGNAL(clicked()),              this, SLOT(getHiLineColor()));                      //set color for current line


  //hide not used items
  Pref_checkBox_HelpBubbles->hide();
}


PrefDialog::~PrefDialog()
{ //! virtual destructor
}




//=============================================================================
// - - - public SLOTs - - -
bool PrefDialog::openPref()
{ //! open preferences file and activate settings
	
  bool pref_ok = false;
  int j=-1;
  
  //set wait cursor
  QApplication::setOverrideCursor(Qt::WaitCursor);
   
  //check if file exists and set function return value pref_ok
  QFile file(((QApplication::applicationDirPath()) + "/" + m_MainWindow->PROJECT_NAME +".ini"));
  if (!file.exists())
    pref_ok = false; 
  else
  	pref_ok = true;
  //create settings object
  QSettings settings(((QApplication::applicationDirPath()) + "/" + m_MainWindow->PROJECT_NAME +".ini"), QSettings::IniFormat);
  
  
  //Interface --------------------------------------------
  settings.beginGroup("INTERFACE");
  
  //CheckBox ToolBarMain  [+1]
  Pref_checkBox_ToolBarMain->setChecked(settings.value("ToolBarMain", true).toBool());
  m_MainWindow->updateMainToolBarVisible(Pref_checkBox_ToolBarMain->isChecked()); //update Main ToolBar visibility
  //CheckBox Commands  [+2]
  Pref_checkBox_Commands->setChecked(settings.value("Commands", true).toBool());
  m_MainWindow->PRGLangToolBarsVis = Pref_checkBox_Commands->isChecked();
  m_MainWindow->updateStatusBar(); //update the statusBar to activate settings
  //ComboBox SplitMode  [+3]
  Pref_comboBox_SplitMode->setCurrentIndex(settings.value("SplitMode", 1).toInt());
  if (Pref_comboBox_SplitMode->currentIndex() == 0)
    m_MainWindow->windowsAlignHor(); else m_MainWindow->windowsAlignVer();
  //ComboBox TabOrientation [+4]
  Pref_comboBox_TabOrientation->setCurrentIndex(settings.value("TabOrientation", 0).toInt());
  m_PageCtrl->tabPosition = Pref_comboBox_TabOrientation->currentIndex() + 1;
  m_PageCtrl->setTabPosition(m_PageCtrl->tabPosition);
  //FileList visible [+5]
  Pref_checkBox_FileList->setChecked(settings.value("FileList", true).toBool());
  m_PageCtrl->flg_fileListVisible = !Pref_checkBox_FileList->isChecked(); 	//use negative value to
  m_MainWindow->setFileListVisible(); 										//set filelist visible with mainwindow.cpp
  //CheckBox HelpBubbles  [+6]
  Pref_checkBox_HelpBubbles->setChecked(settings.value("HelpBubbles", true).toBool());
  //ComboBox InterfaceLang  [+7]
  Pref_comboBox_Lang->setCurrentIndex(Pref_comboBox_Lang->findText(settings.value("InterfaceLang", "English").toString()));
  switchLang();
  //MainWindow settings [+8]/[+9]
  m_MainWindow->resize(settings.value("MainWindowSize", QSize(480, 320)).toSize());
  m_MainWindow->move(settings.value("MainWindowPos", QPoint(0, 45)).toPoint());
  //get main window maximized flag [+10]
  if (settings.value("MainWindowMaximized", false).toBool())
    m_MainWindow->showMaximized();
  //ToolBar main position settings [+11]/[+12], works not yet correctly
  m_MainWindow->toolBarMain->resize(settings.value("ToolBarMainSize", QSize(200, 30)).toSize());
  m_MainWindow->toolBarMain->move(settings.value("ToolBarMainPos", QPoint(0, 0)).toPoint());
  //ToolBar programming language position settings [+13]/[+14], works not yet correctly
  QSize  s = settings.value("CommandsSize", QSize(200, 30)).toSize();
  QPoint p = settings.value("CommandsPos", QPoint(45, 30)).toPoint();
  //m_MainWindow->setCurrentPrgLngToolBarGeometry(s, p);

  settings.endGroup();
         
         
  //Editor Settings --------------------------------------------
  settings.beginGroup("EDITOR");
  
  //ComboBox Prg Lang Index  [+1]
  Pref_comboBox_PrgLang->setCurrentIndex(settings.value("PrgLang", 0).toInt());
  if (Pref_comboBox_PrgLang->currentIndex() == 0)
    m_PageCtrl->prgLngStr = "IL"; else m_PageCtrl->prgLngStr = "ST"; 
  m_PageCtrl->setCurrentPrgLng(m_PageCtrl->prgLngStr); 
  m_MainWindow->updateStatusBar();
  //CheckBox do not use prg lang detection [+2]
  Pref_checkBox_usePrgLngDetection->setChecked(settings.value("UsePrgLngDetection", true).toBool());
  m_ImportExport->usePrgLngDetection = Pref_checkBox_usePrgLngDetection->isChecked();
  //Syntax highlighter [+3]
  Pref_checkBox_Syntaxhighlighter->setChecked(settings.value("Syntax", true).toBool());
  m_PageCtrl->flg_activate_highlighters = Pref_checkBox_Syntaxhighlighter->isChecked();
  if (m_PageCtrl->flg_activate_highlighters == true)
  	m_PageCtrl->createHighlighters(0);
  else
    m_PageCtrl->deleteCurrentHighlighters(0);
  //ComboBox Font Index  [+4]
  #ifdef Q_WS_WIN									 	//use Courier New for Win
    j = Pref_comboBox_FontName->findText(settings.value("Font", "Courier New").toString());
  #endif
  #ifdef Q_WS_MAC									 	//use Monaco for Mac
    j = Pref_comboBox_FontName->findText(settings.value("Font", "Monaco").toString());  
  #endif
  #ifdef Q_OS_LINUX												//use Sans Serif for Linux 
    j = Pref_comboBox_FontName->findText(settings.value("Font", "Sans Serif").toString());
  #endif
  Pref_comboBox_FontName->setCurrentIndex(j);  
  m_PageCtrl->font.setFamily(Pref_comboBox_FontName->currentText());
  //ComboBox Font Size Index  [+5]
  j = Pref_comboBox_FontSize->findText(settings.value("FontSize", 4).toString());
  if (j > -1)
    Pref_comboBox_FontSize->setCurrentIndex(j); else Pref_comboBox_FontSize->setCurrentIndex(5);
  m_PageCtrl->font.setPointSize(Pref_comboBox_FontSize->currentText().toInt());
  m_PageCtrl->font.setWeight(52);
  m_PageCtrl->setCurrentFont(m_PageCtrl->font);
  //lineEdit TabWidth [+6]
  Pref_spinBox_TabWidth->setValue(settings.value("TabWidth", 4).toInt());
  QFontMetrics fm(m_PageCtrl->font);
  m_PageCtrl->tabStopWidth = Pref_spinBox_TabWidth->value()*fm.width(" ");   	
  m_PageCtrl->setCurrentTabStops(m_PageCtrl->tabStopWidth);
  //enable highlighting lines [+7]
  Pref_checkBox_LinesHighlighter->setChecked(settings.value("HiLines", true).toBool());
  m_PageCtrl->flg_enableHighlightLines = Pref_checkBox_LinesHighlighter->isChecked();
  //highlighting color for current line [+8]
  hiLineColor = QColor(settings.value("HiLineColor", "#F0ECEC").toString()); //QColor(255, 225, 255) = thistle1 OR QColor(240, 236, 236)/#F0ECEC = ligth grey
  checkHiLineColor(); 
  setHiLineColor(hiLineColor);
  //activate highlighting current line with line color
  m_PageCtrl->configureCurrentEditor(0);

  settings.endGroup();


  //Session Settings --------------------------------------------
  settings.beginGroup("SESSIONS");
  
  //CheckBox Session autosave  [+1]
  Pref_checkBox_SessAutoSave->setChecked(settings.value("SessionAutoSave", true).toBool());
  m_SessionManager->saveAutoSession = Pref_checkBox_SessAutoSave->isChecked();
  //CheckBox Session autosave as project  [+2]
  Pref_checkBox_autoSaveAsProj->setChecked(settings.value("SessionAsProject", false).toBool());
  m_SessionManager->saveSessionAsPrj = Pref_checkBox_autoSaveAsProj->isChecked();
  //LineEdit Session directory  [+3]
  Pref_lineEdit_sessionDir->setText(settings.value("SessionDir", (appDir + "/Sessions/")).toString());
  if (Pref_lineEdit_sessionDir->text().isEmpty())
    Pref_lineEdit_sessionDir->setText(appDir + "/Sessions/");
  m_SessionManager->checkSessDir(Pref_lineEdit_sessionDir->text());
  //CheckBox Session autoload  [+4]
  Pref_checkBox_SessAutoLoad->setChecked(settings.value("SessionAutoLoad", false).toBool());
  if (Pref_checkBox_SessAutoLoad->isChecked())
  //if autoload is on demand, activate sessions functions
    m_SessionManager->openAutoSession = true;  
  //CheckBox Session error list  [+5]
  Pref_checkBox_SessError->setChecked(settings.value("SessionErrorList", true).toBool());
  if(Pref_checkBox_SessError->isChecked())
    m_SessionManager->showErrorList = true;
  else
    m_SessionManager->showErrorList = false;
  //CheckBox SessManAtStart  [+6]
  Pref_checkBox_StartSessionMan->setChecked(settings.value("SessionManagerAtStart", false).toBool());
  if (Pref_checkBox_StartSessionMan->isChecked())
  { //if session manager is on demand, activate sessions functions
    m_SessionManager->searchSessions();
    m_SessionManager->show();
  }
  
  settings.endGroup();
   
   
  //Save Settings --------------------------------------------
  settings.beginGroup("SAVE");
  
  //CheckBox unsaved notification [+1]
  Pref_checkBox_unsavednotification->setChecked(settings.value("UnsavedNotice", true).toBool());
  m_PageCtrl->unsavedNotice = Pref_checkBox_unsavednotification->isChecked();
  //CheckBox PTT file  [+2]
  Pref_checkBox_Prototype->setChecked(settings.value("PTTFile", false).toBool());
  m_ImportExport->savePTT = Pref_checkBox_Prototype->isChecked();
  //CheckBox auto save [+3]
  Pref_checkBox_autoSave->setChecked(settings.value("AutoSave", false).toBool());
  setTimer(Pref_checkBox_autoSave->isChecked());
  //SpinBox auto save interval [+4]
  Pref_spinBox_autoSaveInterval->setValue(settings.value("AutoSaveInterval", 5).toInt());
  //CheckBox use std. open/save directory [+5]
  Pref_checkBox_presetDir->setChecked(settings.value("UsePresetDir", false).toBool());
  m_ImportExport->usePresetDir = Pref_checkBox_presetDir->isChecked();
  //last directory from open-/savedialog  [+6]
  Pref_lineEdit_presetDir->setText(settings.value("LastDir", "/home/").toString());
  m_ImportExport->lastDir = Pref_lineEdit_presetDir->text();
  //ComboBox default POU Extension [+7]
  Pref_comboBox_defaultPOUExtension->setCurrentIndex(settings.value("defaultPOUExt", 0).toInt());
  m_PageCtrl->defaultPOUExt = getExtNumber(Pref_comboBox_defaultPOUExtension->currentText()); 
  //ComboBox fb call model conversion [+8]
  Pref_comboBox_callmodelconversion->setCurrentIndex(settings.value("FBModelConversion", 0).toInt());
  m_ImportExport->conversionOption = Pref_comboBox_callmodelconversion->currentIndex();
  
  settings.endGroup();


  //Print Settings --------------------------------------------
  settings.beginGroup("PRINT");
  
  //CheckBox print decl. [+1]
  Pref_checkBox_PrintDeclarations->setChecked(settings.value("PrDec", true).toBool());
  m_PrintPrepare->printDeclarations = Pref_checkBox_PrintDeclarations->isChecked();
  //CheckBox print instr. [+2]
  Pref_checkBox_PrintInstructions->setChecked(settings.value("PrIns", true).toBool());
  m_PrintPrepare->printInstructions = Pref_checkBox_PrintInstructions->isChecked();
  //CheckBox print Prototype file PTT [+4]
  Pref_checkBox_PrintPrototype->setChecked(settings.value("PrPTT", false).toBool());
  m_PrintPrepare->printPrototype = Pref_checkBox_PrintPrototype->isChecked();
  //CheckBox print POU Info [+5]
  Pref_checkBox_PrintPOUInfo->setChecked(settings.value("PrPOUI", false).toBool());
  m_PrintPrepare->printPOUI = Pref_checkBox_PrintPOUInfo->isChecked();
  //SpinBox print area [+6]
  Pref_spinBox_PrintArea->setValue(settings.value("PrArea", 80).toInt());
  m_PageCtrl->printArea = Pref_spinBox_PrintArea->value();
  //activate the printable area indicator
  m_PageCtrl->configureCurrentEditor(0);
  
  settings.endGroup();


  //Advanced Settings --------------------------------------------
  settings.beginGroup("ADVANCED");
  
  //CheckBox sort call model fb list [+1]
  Pref_checkBox_sortFBCallModelList->setChecked(settings.value("SortFBCallModelList", false).toBool());
  m_FBDialog->setSortingEnabled(Pref_checkBox_sortFBCallModelList->isChecked());

  //CheckBox use simple IEC call model for plain POUs [+2]
  Pref_checkBox_useSimpleIECCallModel->setChecked(settings.value("IECCall", false).toBool());
  m_FBDialog->flg_useSimpleIECCallModel = Pref_checkBox_useSimpleIECCallModel->isChecked();
  
  //ComboBox Sucosoft compatibility  [+3]
  Pref_comboBox_SucosoftComp->setCurrentIndex(settings.value("SucosoftVersion", 1).toInt());
  m_PageCtrl->Sucosoft_S40_VERSION = Pref_comboBox_SucosoftComp->currentText();
  if (Pref_comboBox_SucosoftComp->currentIndex() == 0)	
    m_PageCtrl->Sucosoft_KM_TEXT = "KM_TEXT = (c) Moeller GmbH, Bonn 1995-99";
  else 
    m_PageCtrl->Sucosoft_KM_TEXT = "KM_TEXT = (c) Moeller GmbH, Bonn 2001";

  //SpinBox max recent files [+4]
  Pref_spinBox_maxRecentFiles->setValue(settings.value("MaxRecentFiles", 10).toInt());
  m_PageCtrl->maxRecentFiles = Pref_spinBox_maxRecentFiles->value();

  //update dialog [+5]
  Pref_checkBox_checkUpdatesAtStart->setChecked(settings.value("CheckUpdatesAtStart", false).toBool());
  
  settings.endGroup();
  
  
  //Recent files list --------------------------------------------
  settings.beginGroup("RECENT-FILES");
  
  //at least stored file links
  int cnt = settings.value("Files", 0).toInt();
  
  //check if cnt > max allowed size
  if (cnt > m_PageCtrl->maxRecentFiles)
    cnt = m_PageCtrl->maxRecentFiles;
    
  //clear list and action
  m_PageCtrl->recentFiles.clear();
  m_MainWindow->clearRecentFilesAction(); 
  
  //fill list and action
  for (int i = 0; i < cnt ; i++)
    m_PageCtrl->recentFiles.append(settings.value(("File" + QString::number(i))).toString());
  m_MainWindow->fillRecentFilesAction(0);
  
  settings.endGroup();
  
  
  //restore cursor
  QApplication::restoreOverrideCursor();
  //return without error
  return(pref_ok);
}


void PrefDialog::resetPrefDialog()
{ //reset Preferences dialog
  
  //page Interface
  Pref_checkBox_ToolBarMain->setChecked(true);
  Pref_checkBox_Commands->setChecked(true);
  Pref_comboBox_SplitMode->setCurrentIndex(1);			//item 0 = vertical
  Pref_comboBox_TabOrientation->setCurrentIndex(0);		//item 0 = north
  m_PageCtrl->flg_fileListVisible = false;			//use negative value to
  m_MainWindow->setFileListVisible();				//set filelist visible with mainwindow.cpp
  Pref_checkBox_HelpBubbles->setChecked(true);	
  Pref_comboBox_Lang->setCurrentIndex(Pref_comboBox_Lang->findText("English"));	//try to find item 'English'
  //page Editor
  Pref_comboBox_PrgLang->setCurrentIndex(0);			//item 0 = AWL
  Pref_checkBox_usePrgLngDetection->setChecked(true);
  Pref_checkBox_Syntaxhighlighter->setChecked(true);		//true = highlighters active
  m_PageCtrl->flg_activate_highlighters = true;
  m_PageCtrl->createHighlighters(0);
#ifdef Q_WS_WIN									 			//use Courier New for Win
    Pref_comboBox_FontName->setCurrentIndex(Pref_comboBox_FontName->findText("Courier New"));  
#endif
#ifdef Q_WS_MAC									 			//use Monaco for Mac
    Pref_comboBox_FontName->setCurrentIndex(Pref_comboBox_FontName->findText("Monaco"));  
#endif
#ifdef Q_OS_LINUX											//use Sans Serif for Linux 
    Pref_comboBox_FontName->setCurrentIndex(Pref_comboBox_FontName->findText("Sans Serif"));
#endif
  Pref_comboBox_FontSize->setCurrentIndex(4);			//item 5 = 10
  Pref_spinBox_TabWidth->setRange(2, 12);
  Pref_spinBox_TabWidth->setValue(4);				//4 = 25/6.25
  Pref_checkBox_LinesHighlighter->setChecked(true);
  setHiLineColor(QColor(255, 225, 255));
  //page Session Settings
  Pref_checkBox_SessAutoSave->setChecked(true);
  Pref_checkBox_autoSaveAsProj->setChecked(false);
  m_SessionManager->checkSessDir(appDir);
  Pref_lineEdit_sessionDir->setText(appDir + "/Sessions/");
  Pref_checkBox_SessAutoLoad->setChecked(false);
  Pref_checkBox_SessError->setChecked(true);
  Pref_checkBox_StartSessionMan->setChecked(false);
  //page Save Settings  
  Pref_checkBox_Prototype->setChecked(false);  
  Pref_checkBox_autoSave->setChecked(false);  
  Pref_spinBox_autoSaveInterval->setValue(5);
  Pref_checkBox_presetDir->setChecked(false);
  Pref_lineEdit_presetDir->setText("/home/");
  Pref_comboBox_defaultPOUExtension->setCurrentIndex(0);	//item 0 = plain POU
  Pref_comboBox_callmodelconversion->setCurrentIndex(0);	//item 0 = do nothing
  //page Print Settings
  Pref_checkBox_PrintDeclarations->setChecked(true);
  Pref_checkBox_PrintInstructions->setChecked(true);
  Pref_checkBox_PrintPrototype->setChecked(true);
  Pref_checkBox_PrintPOUInfo->setChecked(false); 
  Pref_spinBox_PrintArea->setValue(80);
  //page Advanced
  Pref_checkBox_sortFBCallModelList->setChecked(false);
  Pref_checkBox_useSimpleIECCallModel->setChecked(false);
  Pref_comboBox_SucosoftComp->setCurrentIndex(1);		//item 1 = Sucosoft V5.x
  Pref_spinBox_maxRecentFiles->setValue(10);
  clearRecentFilesHistory(); //clear recent files
  Pref_checkBox_checkUpdatesAtStart->setChecked(false);

  //diverse non visible settings
  m_PageCtrl->splitSize = 200;
  
  #ifdef Q_WS_WIN
    m_MainWindow->setGeometry(0, 25, 480, 320);
  #endif
  #ifdef Q_WS_MAC
  	m_MainWindow->setGeometry(0, 45, 600, 400);
  #endif
  #ifdef Q_OS_LINUX
  	m_MainWindow->setGeometry(0, 45, 480, 320);
  #endif  
}


void PrefDialog::savePref()
{ //! save preferences file from current application status
  
  //create settings object
  QSettings settings(((QApplication::applicationDirPath()) + "/" + m_MainWindow->PROJECT_NAME +".ini"), QSettings::IniFormat);

  //head of INI File --------------------------------------------
  /* not usable  
  settings.setValue((m_MainWindow->PROJECT_NAME + " Preferences Version " + m_MainWindow->VERSION), 0);//INI File description
  settings.setValue("; author: M. Rehfeldt, " + m_MainWindow->PROJECT_URL, 0);						//author & website information
  settings.setValue("; do not edit this file, use the preferences dialog", 0);						//editing information
  */


  //Interface --------------------------------------------
  settings.beginGroup("INTERFACE");
  
  //CheckBox ToolBarMain  [+1]
  settings.setValue("ToolBarMain", Pref_checkBox_ToolBarMain->isChecked());
  //CheckBox Commands  [+2]
  settings.setValue("Commands", Pref_checkBox_Commands->isChecked());
  //ComboBox SplitMode  [+3]
  settings.setValue("SplitMode", QString::number(Pref_comboBox_SplitMode->currentIndex())); 
  //ComboBox TabOrientation [+4]
  settings.setValue("TabOrientation", QString::number(Pref_comboBox_TabOrientation->currentIndex())); 
  //FileList visible [+5]
  settings.setValue("FileList", Pref_checkBox_FileList->isChecked());
  //CheckBox Help  [+6]
  settings.setValue("HelpBubbles", Pref_checkBox_HelpBubbles->isChecked());
  //ComboBox Lang Index  [+7]
  settings.setValue("InterfaceLang", Pref_comboBox_Lang->itemText(Pref_comboBox_Lang->currentIndex()));
  //MainWindow position settings [+8]/[+9]
  settings.setValue("MainWindowSize", m_MainWindow->size());
  settings.setValue("MainWindowPos", m_MainWindow->pos());
  //get main win maximized flag [+10]
  settings.setValue("MainWindowMaximized", m_MainWindow->isMaximized());
  //ToolBar main position settings [+11]/[+12]
  settings.setValue("ToolBarMainSize",  m_MainWindow->toolBarMain->size());
  settings.setValue("ToolBarMainPos",  m_MainWindow->toolBarMain->pos());
  //ToolBar programming language position settings [+13]/[+14]
  QRect r = m_MainWindow->currentPrgLngToolBarGeometry();
  settings.setValue("CommandsSize", QSize(r.height(), r.width()));
  settings.setValue("CommandsPos", QPoint(r.x(), r.y()));
  
  settings.endGroup();


  //Editor Settings --------------------------------------------
  settings.beginGroup("EDITOR");
  
  //ComboBox Prg Lang Index  [+1]
  settings.setValue("PrgLang", QString::number(Pref_comboBox_PrgLang->currentIndex()));
  //CheckBox use prg lang detection [+2]
  settings.setValue("UsePrgLngDetection", Pref_checkBox_usePrgLngDetection->isChecked());
  //Syntax highlighter [+3]
  settings.setValue("Syntax", Pref_checkBox_Syntaxhighlighter->isChecked());
  //ComboBox Font Index  [+4]
  settings.setValue("Font", Pref_comboBox_FontName->currentText());
  //ComboBox Font Size Index  [+5]
  settings.setValue("FontSize", Pref_comboBox_FontSize->currentText());
  //ComboBox TabWidth [+6]
  settings.setValue("TabWidth", QString::number(Pref_spinBox_TabWidth->value())); //simply for prevent any '\n'
  //CheckBox enable highlighting lines [+7]
  settings.setValue("HiLines", Pref_checkBox_LinesHighlighter->isChecked());
  //toolbutton highlighting color for current line [+8]
  settings.setValue("HiLineColor", hiLineColor.name());

  settings.endGroup();
  
  
  //Session Settings --------------------------------------------
  settings.beginGroup("SESSIONS");
  
  m_SessionManager->saveAutoSession  = Pref_checkBox_SessAutoSave->isChecked();
  m_SessionManager->saveSessionAsPrj = Pref_checkBox_autoSaveAsProj->isChecked();
  
  //CheckBox Session autosave  [+1]
  settings.setValue("SessionAutoSave", Pref_checkBox_SessAutoSave->isChecked());
  //CheckBox Session autosave as project  [+2]
  settings.setValue("SessionAsProject", Pref_checkBox_autoSaveAsProj->isChecked());
  //LineEdit Session directory  [+3]
  settings.setValue("SessionDir", Pref_lineEdit_sessionDir->text());
  //CheckBox Session autoload  [+4]
  settings.setValue("SessionAutoLoad", Pref_checkBox_SessAutoLoad->isChecked());
  //CheckBox Session error list  [+5]
  settings.setValue("SessionErrorList", Pref_checkBox_SessError->isChecked());
  //CheckBox SessManAtStart  [+6]
  settings.setValue("SessionManagerAtStart", Pref_checkBox_StartSessionMan->isChecked());
  
  settings.endGroup();
  
  
  //Save Settings --------------------------------------------
  settings.beginGroup("SAVE");

  //CheckBox unsaved notification [+1]
  settings.setValue("UnsavedNotice", Pref_checkBox_unsavednotification->isChecked());
  //CheckBox save PTT file  [+2]
  settings.setValue("PTTFile", Pref_checkBox_Prototype->isChecked());
  //CheckBox auto save [+3]
  settings.setValue("AutoSave", Pref_checkBox_autoSave->isChecked());
  //SpinBox auto save interval [+4]
  settings.setValue("AutoSaveInterval", QString::number(Pref_spinBox_autoSaveInterval->value())); 
  //CheckBox use preset open/save directory [+5]
  settings.setValue("UsePresetDir", Pref_checkBox_presetDir->isChecked());
  //lineEdit open/save directory  [+6]
  settings.setValue("LastDir", Pref_lineEdit_presetDir->text());
  //ComboBox default POU Extension [+7]
  settings.setValue("defaultPOUExt", QString::number(Pref_comboBox_defaultPOUExtension->currentIndex()));
  //ComboBox fb call model conversion [+8]
  settings.setValue("FBModelConversion", QString::number(Pref_comboBox_callmodelconversion->currentIndex())); 
  
  settings.endGroup();


  //Print Settings --------------------------------------------
  settings.beginGroup("PRINT");

  //CheckBox print decl. [+1]
  settings.setValue("PrDec", Pref_checkBox_PrintDeclarations->isChecked());
  //CheckBox print instr. [+2]
  settings.setValue("PrIns", Pref_checkBox_PrintInstructions->isChecked());
  //CheckBox print POE Info [+4]
  settings.setValue("PrPTT", Pref_checkBox_PrintPrototype->isChecked());
  //CheckBox print Prototype [+5]
  settings.setValue("PrPOUI", Pref_checkBox_PrintPOUInfo->isChecked());
  //SpinBox print area [+6]
  settings.setValue("PrArea", QString::number(Pref_spinBox_PrintArea->value()));
  
  settings.endGroup();


  //Advanced Settings --------------------------------------------
  settings.beginGroup("ADVANCED");

  //CheckBox sort call model fb list [+1]
  settings.setValue("SortFBCallModelList", Pref_checkBox_sortFBCallModelList->isChecked());
  //CheckBox use simple IEC call model for plain POUs [+2]
  settings.setValue("IECCall", Pref_checkBox_useSimpleIECCallModel->isChecked());
  //ComboBox Sucosoft compatibility  [+3]
  settings.setValue("SucosoftVersion", QString::number(Pref_comboBox_SucosoftComp->currentIndex())); 
  //SpinBox max recent files [+4]
  settings.setValue("MaxRecentFiles", Pref_spinBox_maxRecentFiles->value());  
  m_PageCtrl->maxRecentFiles = Pref_spinBox_maxRecentFiles->value();//set new value
  m_PageCtrl->correctRecentFilesListLength();						//correct list
  //update dialog [+5]
  settings.setValue("CheckUpdatesAtStart", Pref_checkBox_checkUpdatesAtStart->isChecked());
 
  settings.endGroup();


  //Recent files list --------------------------------------------
  settings.beginGroup("RECENT-FILES");
  
  settings.remove("");
  settings.setValue("Files", QString::number(m_PageCtrl->recentFiles.count()));
  for (int i = 0; i < m_PageCtrl->recentFiles.count() ; i++)
    settings.setValue(("File" + QString::number(i)), m_PageCtrl->recentFiles[i]);

  settings.endGroup();
}




//=============================================================================
// - - - private SLOTs - - -
void PrefDialog::applyPrefDialog()
{ //! apply changes from preferences dialog

  switchLang(); //cal switch lang function
  savePref(); 	//call save action 
  openPref(); 	//call open action to assume changes into main window
}


void PrefDialog::cancelDialog()
{ //! cancel settings by re-open preferences file

  openPref();
}


void PrefDialog::clearRecentFilesHistory()
{ //! clear recent files history

  //clear lists
  m_PageCtrl->recentFiles.clear();
  //clear mainwindow action
  m_MainWindow->clearRecentFilesAction();

  //clear settings file section
  QSettings settings(((QApplication::applicationDirPath()) + "/" + m_MainWindow->PROJECT_NAME +".ini"), QSettings::IniFormat);
  settings.beginGroup("RECENT-FILES");
  settings.remove("");
  settings.endGroup();
}


void PrefDialog::checkHiLineColor()
{ //! check highlighting color for current line against lightness and saturation

  if ((hiLineColor.value() > 250) || (hiLineColor.saturation() < 2))
  { //normally lightness > 245 and saturation < 5 are useless
    Pref_checkBox_LinesHighlighter->setChecked(false);
    m_PageCtrl->flg_enableHighlightLines = Pref_checkBox_LinesHighlighter->isChecked();
  }
}


void PrefDialog::getHiLineColor()
{ //! get highlighting color for current line from color dialog

  hiLineColor = QColorDialog::getColor(QColor(240, 236, 236)); //QColor(255, 225, 255) = thistle1 OR QColor(240, 236, 236)/#F0ECEC = ligth grey

  if (!hiLineColor.isValid()) //if color is not valid, reset to std. color
    hiLineColor = QColor(240, 236, 236);

  setColorButton(hiLineColor);
  checkHiLineColor(); 
}


void PrefDialog::getLastDir()
{ //! get last directory from ImportExport class
	
  Pref_lineEdit_presetDir->setText(m_ImportExport->lastDir);	
}


void PrefDialog::okClickPrefDialog()
{ //! close preferences dialog
	
  applyPrefDialog();//apply changes
  emit accept();    //emit accepted signal
  close();          //close dialog
}


void PrefDialog::retranslateUi()
{ //! retranslate Ui

  this->setWindowTitle("Preferences");

  //reset Button
  Pref_resetButton->setText(tr("Default"));
  Pref_resetButton->setStatusTip(tr("Reset Interface to default values"));
  Pref_resetButton->setToolTip(Pref_resetButton->statusTip());
  
  //ok Button
  Pref_okButton->setText(tr("Ok"));
  Pref_okButton->setStatusTip(tr("Accept settings and close window"));
  Pref_okButton->setToolTip(Pref_okButton->statusTip());
  
  //cancel Button
  Pref_cancelButton->setText(tr("Cancel"));
  Pref_cancelButton->setStatusTip(tr("Cancel settings and close window"));
  Pref_cancelButton->setToolTip(Pref_cancelButton->statusTip());

  //apply Button
  Pref_applyButton->setText(tr("Apply"));
  Pref_applyButton->setStatusTip(tr("Accept settings"));
  Pref_applyButton->setToolTip(Pref_applyButton->statusTip());

  //pref. page list
  Pref_listWidget->item(0)->setText(tr("Interface"));
  Pref_listWidget->item(1)->setText(tr("Editor"));
  Pref_listWidget->item(2)->setText(tr("Sessions"));
  Pref_listWidget->item(3)->setText(tr("Save"));
  Pref_listWidget->item(4)->setText(tr("Print"));
  Pref_listWidget->item(5)->setText(tr("Advanced"));



  //main toolbar
  Pref_checkBox_ToolBarMain->setText(tr("Main tools"));
  Pref_checkBox_ToolBarMain->setStatusTip(tr("Show main toolbar"));
  Pref_checkBox_ToolBarMain->setToolTip(Pref_checkBox_ToolBarMain->statusTip());
  
  //cmd. toolbar
  Pref_checkBox_Commands->setText(tr("Commands"));
  Pref_checkBox_Commands->setStatusTip(tr("Show commands toolbar"));
  Pref_checkBox_Commands->setToolTip(Pref_checkBox_Commands->statusTip());
  
  //checkbox file list
  Pref_checkBox_FileList->setText(tr("File list"));
  Pref_checkBox_FileList->setStatusTip(tr("Show file list on the left mainwindow site"));
  Pref_checkBox_FileList->setToolTip(Pref_checkBox_FileList->statusTip());
  
  //checkbox help bubbles
  Pref_checkBox_HelpBubbles->setText(tr("Help bubbles"));
  Pref_checkBox_HelpBubbles->setStatusTip(tr("Show Help bubbles under mouse cursor"));
  Pref_checkBox_HelpBubbles->setToolTip(Pref_checkBox_HelpBubbles->statusTip());

  //label split mode
  Pref_label_SplitMode->setText(tr("Split mode"));
  Pref_label_SplitMode->setStatusTip(tr("Set splitmode of active window"));
  Pref_label_SplitMode->setToolTip(Pref_label_SplitMode->statusTip());
  //combobox split mode
  Pref_comboBox_SplitMode->setItemText(0, tr("Horizontal"));
  Pref_comboBox_SplitMode->setItemText(1, tr("Vertical"));
  Pref_comboBox_SplitMode->setStatusTip(Pref_label_SplitMode->statusTip());
  Pref_comboBox_SplitMode->setToolTip(Pref_comboBox_SplitMode->statusTip());
  
  //label tab orient.
  Pref_label_TabOrientation->setText(tr("Tab orientation"));
  Pref_label_TabOrientation->setStatusTip(tr("Set tab orientation"));
  Pref_label_TabOrientation->setToolTip(Pref_label_TabOrientation->statusTip());
  //combobox tab orientation
  Pref_comboBox_TabOrientation->setItemText(0, tr("North"));
  Pref_comboBox_TabOrientation->setItemText(1, tr("South"));
  Pref_comboBox_TabOrientation->setItemText(2, tr("East"));
  Pref_comboBox_TabOrientation->setItemText(3, tr("West"));
  Pref_comboBox_TabOrientation->setStatusTip(Pref_label_TabOrientation->statusTip());
  Pref_comboBox_TabOrientation->setToolTip(Pref_comboBox_TabOrientation->statusTip());
  
  //label language
  Pref_label_Lang->setText(tr("Language"));
  Pref_label_Lang->setStatusTip(tr("Set interface language"));
  Pref_label_Lang->setToolTip(Pref_label_Lang->statusTip());
  //combobox language
  Pref_comboBox_Lang->setStatusTip(Pref_label_Lang->statusTip());
  Pref_comboBox_Lang->setToolTip(Pref_comboBox_Lang->statusTip());



  //label progr. language
  Pref_label_PrgLang->setText(tr("Programming language"));
  Pref_label_PrgLang->setStatusTip(tr("Set programming language"));
  Pref_label_PrgLang->setToolTip(Pref_label_PrgLang->statusTip());
  //combobox language
  Pref_comboBox_PrgLang->setItemText(0, tr("IL/AWL"));
  Pref_comboBox_PrgLang->setItemText(1, tr("Structured Text"));
  Pref_comboBox_PrgLang->setStatusTip(Pref_label_PrgLang->statusTip());
  Pref_comboBox_PrgLang->setToolTip(Pref_comboBox_PrgLang->statusTip());

  //checkBox do not use prg lang detection 
  Pref_checkBox_usePrgLngDetection->setText(tr("Use prog. language detection for .txt and .exp"));
  Pref_checkBox_usePrgLngDetection->setStatusTip(tr("Use programming language detection for .txt and .exp files."));
  Pref_checkBox_usePrgLngDetection->setToolTip(Pref_checkBox_usePrgLngDetection->statusTip()); 
   
  //checkbox syntax highlighter
  Pref_checkBox_Syntaxhighlighter->setText(tr("Syntax highlighter"));
  Pref_checkBox_Syntaxhighlighter->setStatusTip(tr("Activate syntax highlighters"));
  Pref_checkBox_Syntaxhighlighter->setToolTip(Pref_checkBox_Syntaxhighlighter->statusTip());

  //label font
  Pref_label_FontName->setText(tr("Font"));
  Pref_label_FontName->setStatusTip(tr("Set font"));
  Pref_label_FontName->setToolTip(Pref_label_FontName->statusTip());
  //combobox font
  Pref_comboBox_FontName->setStatusTip(Pref_label_FontName->statusTip());
  Pref_comboBox_FontName->setToolTip(Pref_comboBox_FontName->statusTip());

  //label font size
  Pref_label_FontSize->setText(tr("Font size"));
  Pref_label_FontSize->setStatusTip(tr("Set font size"));
  Pref_label_FontSize->setToolTip(Pref_label_FontSize->statusTip());
  //combobox font size
  Pref_comboBox_FontSize->setStatusTip(Pref_label_FontSize->statusTip());
  Pref_comboBox_FontSize->setToolTip(Pref_comboBox_FontSize->statusTip());

  //label tab width
  Pref_label_TabWidth->setText(tr("Tab width"));
  Pref_label_TabWidth->setStatusTip(tr("Set tabulator width"));
  Pref_label_TabWidth->setToolTip(Pref_label_TabWidth->statusTip());
  //spinbox tab width
  Pref_spinBox_TabWidth->setStatusTip(Pref_label_TabWidth->statusTip());
  Pref_spinBox_TabWidth->setToolTip(Pref_spinBox_TabWidth->statusTip());
  
  //checkbox LinesHighlighter
  Pref_checkBox_LinesHighlighter->setText(tr("Highlight current line"));
  Pref_checkBox_LinesHighlighter->setStatusTip(tr("Highlight the current line"));
  Pref_checkBox_LinesHighlighter->setToolTip(Pref_checkBox_LinesHighlighter->statusTip());
  //toolbutton highlighting line color
  Pref_toolButton_HiLineColor->setStatusTip(tr("Color dialog for current line highlighter. Too dark or to light colors will be rejected."));
  Pref_toolButton_HiLineColor->setToolTip(Pref_toolButton_HiLineColor->statusTip());



  //checkbox Session auto save
  Pref_checkBox_SessAutoSave->setText(tr("Session autosave"));
  Pref_checkBox_SessAutoSave->setStatusTip(tr("Activate autosave for sessions"));
  Pref_checkBox_SessAutoSave->setToolTip(Pref_checkBox_SessAutoSave->statusTip());

  //checkbox Session auto save as prj.
  Pref_checkBox_autoSaveAsProj->setText(tr("Autosave as Project"));
  Pref_checkBox_autoSaveAsProj->setStatusTip(tr("Activate autosave as project for sessions"));
  Pref_checkBox_autoSaveAsProj->setToolTip(Pref_checkBox_autoSaveAsProj->statusTip());

  //Session directory selection
  Pref_label_sessionDir->setText(tr("Session path:"));
  Pref_label_sessionDir->setStatusTip(tr("Select a path to store all sessions"));
  Pref_label_sessionDir->setToolTip(Pref_label_sessionDir->statusTip());
  Pref_lineEdit_sessionDir->setStatusTip(Pref_label_sessionDir->statusTip());
  Pref_lineEdit_sessionDir->setToolTip(Pref_label_sessionDir->statusTip());
  Pref_pushButton_sessionDir->setStatusTip(Pref_label_sessionDir->statusTip());
  Pref_pushButton_sessionDir->setToolTip(Pref_label_sessionDir->statusTip());

  //checkbox Session start with manager
  Pref_checkBox_StartSessionMan->setText(tr("Start with session manager"));
  Pref_checkBox_StartSessionMan->setStatusTip(tr("Load session manager at startup"));
  Pref_checkBox_StartSessionMan->setToolTip(Pref_checkBox_StartSessionMan->statusTip());

  //checkbox Session session autoload
  Pref_checkBox_SessAutoLoad->setText(tr("Session autoload"));
  Pref_checkBox_SessAutoLoad->setStatusTip(tr("Load 'auto-session' at startup"));
  Pref_checkBox_SessAutoLoad->setToolTip(Pref_checkBox_SessAutoLoad->statusTip());

  //checkbox Session error list
  Pref_checkBox_SessError->setText(tr("Session error list"));
  Pref_checkBox_SessError->setStatusTip(tr("Show error list for session file loading error"));
  Pref_checkBox_SessError->setToolTip(Pref_checkBox_SessError->statusTip());



  //checkbox unsaved notification
  Pref_checkBox_unsavednotification->setText(tr("Notification for unsaved files"));
  Pref_checkBox_unsavednotification->setStatusTip(tr("Give a notification for unsaved files if the page gets closed"));
  Pref_checkBox_unsavednotification->setToolTip(Pref_checkBox_unsavednotification->statusTip());

  //checkbox prototype
  Pref_checkBox_Prototype->setText(tr("Save with prototype"));
  Pref_checkBox_Prototype->setStatusTip(tr("Save with prototype file"));
  Pref_checkBox_Prototype->setToolTip(Pref_checkBox_Prototype->statusTip());

  //checkbox auto save files
  Pref_checkBox_autoSave->setText(tr("Auto save"));
  Pref_checkBox_autoSave->setStatusTip(tr("Auto save all opened files"));
  Pref_checkBox_autoSave->setToolTip(Pref_checkBox_autoSave->statusTip());

  //label auto save interval
  Pref_label_autoSaveInterval->setText(tr("Interval:"));
  Pref_label_autoSaveInterval->setStatusTip(tr("Auto save interval timer"));
  Pref_label_autoSaveInterval->setToolTip(Pref_label_autoSaveInterval->statusTip());
  //spinbox auto save files
  Pref_spinBox_autoSaveInterval->setStatusTip(Pref_label_autoSaveInterval->statusTip());
  Pref_spinBox_autoSaveInterval->setToolTip(Pref_spinBox_autoSaveInterval->statusTip());

  //checkbox presetting DIR
  Pref_checkBox_presetDir->setText(tr("Use the following path as standard"));
  Pref_checkBox_presetDir->setStatusTip(tr("Set a path for open and save dialogs"));
  Pref_checkBox_presetDir->setToolTip(Pref_checkBox_presetDir->statusTip());
  //line edit preset. DIR
  Pref_lineEdit_presetDir->setStatusTip(Pref_checkBox_presetDir->statusTip());
  Pref_lineEdit_presetDir->setToolTip(Pref_lineEdit_presetDir->statusTip());
  //push button preset. DIR
  Pref_pushButton_presetDir->setText(tr("..."));
  Pref_pushButton_presetDir->setStatusTip(Pref_checkBox_presetDir->statusTip());
  Pref_pushButton_presetDir->setToolTip(Pref_pushButton_presetDir->statusTip());

  //label default POU extension
  Pref_label_defaultPOUExtension->setText(tr("Default POU Extension"));
  Pref_label_defaultPOUExtension->setStatusTip(tr("Set the default extension for new POUs"));
  Pref_label_defaultPOUExtension->setToolTip(Pref_label_defaultPOUExtension->statusTip());
  //combobox default POU extension
  Pref_comboBox_defaultPOUExtension->setItemText(0, tr("plain POU (.txt)"));
  Pref_comboBox_defaultPOUExtension->setItemText(1, tr("CoDeSys (.EXP)"));
  Pref_comboBox_defaultPOUExtension->setItemText(2, tr("MELSOFT (.asc)"));
  Pref_comboBox_defaultPOUExtension->setItemText(3, tr("MULTIPROG IL (.IL)"));
  Pref_comboBox_defaultPOUExtension->setItemText(4, tr("MULTIPROG ST (.ST)"));
  Pref_comboBox_defaultPOUExtension->setItemText(5, tr("STEP 7 AWL (.awl)"));
  Pref_comboBox_defaultPOUExtension->setItemText(6, tr("STEP 7 SCL (.scl)"));
  Pref_comboBox_defaultPOUExtension->setItemText(7, tr("Sucosoft S40 (.poe)"));
  Pref_comboBox_defaultPOUExtension->setStatusTip(Pref_label_defaultPOUExtension->statusTip());
  Pref_comboBox_defaultPOUExtension->setToolTip(Pref_comboBox_defaultPOUExtension->statusTip());
  
  //label fb call conversion
  Pref_label_callmodelconversion->setText(tr("Conversion of function block call model:"));
  Pref_label_callmodelconversion->setStatusTip(tr("Select a conversion for the function block call model"));
  Pref_label_callmodelconversion->setToolTip(Pref_label_callmodelconversion->statusTip());
  //combobox conversion
  Pref_comboBox_callmodelconversion->setStatusTip(Pref_label_callmodelconversion->statusTip());
  Pref_comboBox_callmodelconversion->setToolTip(Pref_label_callmodelconversion->statusTip()); 
  //combobox conversion item texts
  Pref_comboBox_callmodelconversion->setItemText(0, tr("do nothing"));
  Pref_comboBox_callmodelconversion->setItemText(1, tr("all into selected compatibility call model"));
  Pref_comboBox_callmodelconversion->setItemText(2, tr("outputs into simple IEC call model"));
  Pref_comboBox_callmodelconversion->setItemText(3, tr("all into simple IEC call model"));



  //checkbox print declarations
  Pref_checkBox_PrintDeclarations->setText(tr("Declarations"));
  Pref_checkBox_PrintDeclarations->setStatusTip(tr("Print with declarations section"));
  Pref_checkBox_PrintDeclarations->setToolTip(Pref_checkBox_PrintDeclarations->statusTip());

  //checkbox print instructions
  Pref_checkBox_PrintInstructions->setText(tr("Instructions"));
  Pref_checkBox_PrintInstructions->setStatusTip(tr("Print with instructions section"));
  Pref_checkBox_PrintInstructions->setToolTip(Pref_checkBox_PrintInstructions->statusTip());

  //checkbox print protoype
  Pref_checkBox_PrintPrototype->setText(tr("Prototype"));
  Pref_checkBox_PrintPrototype->setStatusTip(tr("Print with prototype section"));
  Pref_checkBox_PrintPrototype->setToolTip(Pref_checkBox_PrintPrototype->statusTip());

  //checkbox print POU info
  Pref_checkBox_PrintPOUInfo->setText(tr("POU Info"));
  Pref_checkBox_PrintPOUInfo->setStatusTip(tr("Print with POU Info section"));
  Pref_checkBox_PrintPOUInfo->setToolTip(Pref_checkBox_PrintPOUInfo->statusTip()); 

  //label print area
  Pref_label_PrintArea->setText(tr("Print area indicator"));
  Pref_label_PrintArea->setStatusTip(tr("Set printable area indicator for declarations and instructions"));
  Pref_label_PrintArea->setToolTip(Pref_label_PrintArea->statusTip());
  //spinbox print area
  Pref_spinBox_PrintArea->setStatusTip(Pref_label_PrintArea->statusTip());
  Pref_spinBox_PrintArea->setToolTip(Pref_spinBox_PrintArea->statusTip());



  //checkbox sort fb call model list
  Pref_checkBox_sortFBCallModelList->setText(tr("Sort function block call model list"));
  Pref_checkBox_sortFBCallModelList->setStatusTip(tr("Sort entries of function block call model list by name. It is better to leave the list unsorted, because it is not guaranteed that the correct call model will be inserted if some FBs have the same name."));
  Pref_checkBox_sortFBCallModelList->setToolTip(Pref_checkBox_sortFBCallModelList->statusTip());
  
  //checkbox use simple IEC call model for plain POUs
  Pref_checkBox_useSimpleIECCallModel->setText(tr("Use simple IEC call model"));
  Pref_checkBox_useSimpleIECCallModel->setStatusTip(tr("Insert function blocks with the simple IEC call model."));
  Pref_checkBox_useSimpleIECCallModel->setToolTip(Pref_checkBox_useSimpleIECCallModel->statusTip());

  //label Sucosoft compatibility
  Pref_label_SucoSoft->setText(tr("Sucosoft compatibility"));
  Pref_label_SucoSoft->setStatusTip(tr("Set Moeller Sucosoft compatibility"));
  Pref_label_SucoSoft->setToolTip(Pref_label_SucoSoft->statusTip());
  //combobox Sucosoft compatibility
  Pref_comboBox_SucosoftComp->setStatusTip(Pref_label_SucoSoft->statusTip());
  Pref_comboBox_SucosoftComp->setToolTip(Pref_comboBox_SucosoftComp->statusTip());
  
  //label clear recent files history
  Pref_label_RecentFiles->setText(tr("Recent files history"));
  Pref_label_RecentFiles->setStatusTip(tr("Set the history depth of recent files"));
  Pref_label_RecentFiles->setToolTip(Pref_label_RecentFiles->statusTip());
  //spinbox recent files
  Pref_spinBox_maxRecentFiles->setStatusTip(Pref_label_RecentFiles->statusTip());
  Pref_label_RecentFiles->setToolTip(Pref_spinBox_maxRecentFiles->statusTip());
  //pushbutton clear recent files history
  Pref_pushButton_clearRecentFiles->setText(tr("Clear"));
  Pref_pushButton_clearRecentFiles->setStatusTip("Clear the history of recent files");
  Pref_pushButton_clearRecentFiles->setToolTip(Pref_pushButton_clearRecentFiles->statusTip());
  
  //checkbox updates at statrtup
  Pref_checkBox_checkUpdatesAtStart->setText(tr("check for updates during program start"));


  //messages
  msg_head_lastDirDialog = tr("Set standard file save directory");
  msg_head_sessionDirDialog = tr("Set sessions directory");
}


void PrefDialog::saveAllPages()
{ //! save all pages if "auto save" is enabled

  if (Pref_checkBox_autoSave->isChecked() == false)
    return;
  m_ImportExport->saveAllPages("", 99, 0); //"" and 99 for using respective file path and extension
}


void PrefDialog::setColorButton(QColor color)
{ //! set the color for toolbutton HiLineColor

  QPixmap pixMap(25, 25);
  pixMap.fill(color);
  Pref_toolButton_HiLineColor->setIcon(pixMap);
}


void PrefDialog::setHiLineColor(QColor color)
{ //! set highlighting color for current line for toolbutton HiLineColor and m_PageCtrl->hiLineColor 

  hiLineColor = color;
  setColorButton(color);
  m_PageCtrl->hiLineColor = color;
}


void PrefDialog::setLastDir()
{ //! set last directory to ImportExport class
	
  Pref_lineEdit_presetDir->setText(QFileDialog::getExistingDirectory(this, msg_head_lastDirDialog, "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks)  + "/");
  m_ImportExport->lastDir = Pref_lineEdit_presetDir->text();	
}


void PrefDialog::setPrefListRow(int i)
{ //! slot set row in preferences page list by i
  Pref_listWidget->setCurrentRow(i);	
}

void PrefDialog::setSessionDir()
{ //! set Sessions directory

  Pref_lineEdit_sessionDir->setText(QFileDialog::getExistingDirectory(this, msg_head_sessionDirDialog, "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks)  + "/");
  if (Pref_lineEdit_sessionDir->text().isEmpty())
    Pref_lineEdit_sessionDir->setText(appDir + "/Sessions/");
  m_SessionManager->checkSessDir(Pref_lineEdit_sessionDir->text());
}


void PrefDialog::startTimer()
{ //! slot start timer, if enabled in preferences dialog

  setTimer(Pref_checkBox_autoSave->isChecked());
}


void PrefDialog::stopTimer()
{ //! slot stop timer

  setTimer(false);
}




//=============================================================================
// - - - diverse private functions - - - 
QString PrefDialog::getExtNumber(QString str)
{ //! return extension from str as number

  int i = m_PageCtrl->definedPOUExtList.size()-1;
  //get extension number by starting from last item and counting down to zero position
  for (i; i >= 0; i--)
    if (str.contains(m_PageCtrl->definedPOUExtList.at(i)))
      break;
  return(QString::number(i));
}


void PrefDialog::getLanguages()
{ //! get language files and fill into Pref_comboBox_Lang

  QDir dir(langDir);
  QStringList fileNames = dir.entryList(QStringList("*.qm"));
 
  for (int i=0; i<fileNames.size(); i++)
  {
    QTranslator translator;
    translator.load(fileNames[i], langDir);
    Pref_comboBox_Lang->addItem(translator.translate("tansl. language", "LANG"));
  }
}


void PrefDialog::setTimer(bool set)
{ //! set timer status by set

  timer->stop(); 	//first stop timer
  if (set == true)	//if true restart timer
    timer->start(Pref_spinBox_autoSaveInterval->value()*60000);//value*1000ms*60s = minutes
}


QString PrefDialog::strFragment(QString str)
{ //! get fragment of str (remove from position 0 to next '=' sign)
  
  return((str.remove(0, str.indexOf("=")+1)).simplified());
}


void PrefDialog::switchLang()
{ //! switch interface language
 
  QString locale = Pref_comboBox_Lang->itemText(Pref_comboBox_Lang->currentIndex());
   
  if (locale != language)
  {
  	language = locale;
  
    appTranslator.load(locale, langDir);
    //m_DiffDialog->retranslateUi();
    m_FBDialog->retranslateUi(); 
    m_FileViewDialog->retranslateUi();
    m_FindDialog->retranslateUi();
    m_FncDialog->retranslateUi(); 
    m_HelpWidget->retranslateUi();
    m_ImportExport->retranslateUi();
    m_MainWindow->retranslateUi();
    m_PageCtrl->retranslateUi();
    m_POUInfoDialog->retranslateUi();
    m_SessionManager->retranslateUi();
    m_AboutDialog->retranslateUi();
    m_UpdateInfo->retranslateUi();
    retranslateUi();
  }
}
