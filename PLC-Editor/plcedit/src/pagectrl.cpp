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

#include "pagectrl.h"
#include "tabwidget.h"
#include "pagedata.h"
#include "highlighter_dec.h"
#include "highlighter_ins.h"
#include "editor.h"
#ifdef Q_WS_MAC
  #include <Carbon/Carbon.h>
#endif


PageCtrl::PageCtrl(QWidget *parent)
    : QSplitter(parent)
{ //! creating function
	
  //widget->visual settings for parent
  this->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  this->setMidLineWidth(0);
  this->setChildrenCollapsible(false); 
  this->show();
  
  //create FileList as child of PageCtrl
  FileList = new QListWidget(this);
  FileList->setFrameStyle(QFrame::Box | QFrame::Raised);
  FileList->setMidLineWidth(0);
  FileList->setAlternatingRowColors(true);
  FileList->setMinimumWidth(120);
  FileList->setMaximumWidth(320);
  FileList->move(0,0);
  FileList->hide();

  //create TabCtrl as child of PageCtrl
  TabCtrl  = new TabWidget(this);      
  TabCtrl->setUsesScrollButtons(true);
  TabCtrl->setLayoutDirection(Qt::LeftToRight);
  #if QT_VERSION >= 0x040500
    connect(TabCtrl, SIGNAL(tabMoved(int, int)), this, SLOT(moveFileListItem(int, int)));
  #endif
  
  #ifdef Q_WS_MAC
    //draw tab widget in brushed metal on Mac OS X
    TabCtrl->setAttribute(Qt::WA_MacBrushedMetal);
  #endif
  TabCtrl->hide();
  
  //create delete page button
  #if QT_VERSION < 0x040500
    delPageButton = new QToolButton(this);
    TabCtrl->setCornerWidget(delPageButton, Qt::TopRightCorner);
    delPageButton->setAutoRaise(true);
    #ifdef Q_WS_MAC
      delPageButton->setIcon(QIcon(":/images/delPageButtonMac_16x16.png"));
    #else
      delPageButton->setIcon(QIcon(":/images/delPageButton_16x16.png"));
    #endif
  #endif

  
  //presetting inhibit highlighters flag, needed for start up
  flg_inhibit_createHighlighters = false;
  //set supported POU extensions list
  definedPOUExtList << ".txt" << ".poe" << ".EXP" << ".awl" << ".scl" << ".IL" << ".ST" << ".asc";
  
  
  //diverse connections
  connect(FileList, SIGNAL(currentRowChanged(int)),     TabCtrl,SLOT(setCurrentIndex(int)));			//change Tab with FileList change
  connect(TabCtrl , SIGNAL(currentChanged(int)), 	this, 	SLOT(setFileListRow(int)));				//change FileList with Tab change
  connect(TabCtrl,  SIGNAL(currentChanged(int)), 	this, 	SIGNAL(currentChanged(int)));			//send SIGNAL with Tab change
  connect(TabCtrl,  SIGNAL(currentChanged(int)), 	this, 	SLOT(createHighlighters(int)));			//create syntax highlighters with Tab change
  connect(this,     SIGNAL(pageDeleted(int)), 		this, 	SLOT(createHighlighters(int)));			//create syntax highlighters if prev. page was deleted
  connect(TabCtrl,  SIGNAL(currentChanged(int)), 	this, 	SLOT(deleteCurrentHighlighters(int)));	//delete syntax highlighters with Tab change
  connect(TabCtrl,  SIGNAL(currentChanged(int)), 	this, 	SLOT(configureCurrentEditor(int)));		//set lines highlighters with Tab change
  #if QT_VERSION < 0x040500
    connect(delPageButton, SIGNAL(clicked()), 		this, 	SLOT(deletePage()));					//get access to delete pages function
  #endif
  #if QT_VERSION >= 0x040500
    connect(TabCtrl, SIGNAL(tabCloseRequested(int)),this, 	SLOT(deletePageIndex(int)));			//get access to delete pages function
  #endif
  
  //retranslate Ui  
  retranslateUi();
}


PageCtrl::~PageCtrl()
{ //virtual destructor

  delete FileList;
  delete TabCtrl;
}




//=============================================================================
// - - - public SLOTs - - -
void PageCtrl::changeSplitMode()
{ //! reversing splitter orientation
  
  if (TabCtrl->count() < 1)//if1
    return; 
  
  //if counter > 0 set splitter orientation
  QSplitter *splitter1 = qobject_cast<QSplitter*>(TabCtrl->currentWidget());

  if (splitMode == "v")
    splitter1->setOrientation(Qt::Vertical);

  if (splitMode == "h")
    splitter1->setOrientation(Qt::Horizontal);
}


void PageCtrl::changeInputWindow()
{ //! toggle focus and cursor for declaratins and Instructions

  if ((TabCtrl->count() < 1) || (TabCtrl->currentWidget()->findChild<Editor*>("TE2") == 0))
    return; 
    
  if (TabCtrl->currentWidget()->findChild<Editor*>("TE1")->hasFocus())
  {	TabCtrl->currentWidget()->findChild<Editor*>("TE2")->setFocus(); return; }
  
  if (TabCtrl->currentWidget()->findChild<Editor*>("TE2")->hasFocus())
  {	TabCtrl->currentWidget()->findChild<Editor*>("TE1")->setFocus(); return; }
  	
  //if nothing focused, set focus to Declarations
  TabCtrl->currentWidget()->findChild<Editor*>("TE1")->setFocus();	
}


void PageCtrl::configureCurrentEditor(int i)
{ //! set lines highlighter
  //! \param i is only needed for signal-slot handling

  if (TabCtrl->count() < 1)
    return; //return if no page is active
    
  TabCtrl->currentWidget()->findChild<Editor*>("TE1")->enableHighlightLines = flg_enableHighlightLines;
  TabCtrl->currentWidget()->findChild<Editor*>("TE1")->hiLineColor = hiLineColor;
  TabCtrl->currentWidget()->findChild<Editor*>("TE1")->printArea = printArea;
  
  if (TabCtrl->currentWidget()->findChild<Editor*>("TE2") == 0)
    return; //return if TE2 does not exist (maybe type is GLOBAL_VAR, TYPE OR CONFIG
    
  TabCtrl->currentWidget()->findChild<Editor*>("TE2")->enableHighlightLines = flg_enableHighlightLines;
  TabCtrl->currentWidget()->findChild<Editor*>("TE2")->hiLineColor = hiLineColor;
  TabCtrl->currentWidget()->findChild<Editor*>("TE2")->printArea = printArea;
}


void PageCtrl::copy()
{ //! copy from current inputwindow

  if (TabCtrl->count() < 1)
    return; 
  if (TabCtrl->currentWidget()->findChild<Editor*>("TE1")->hasFocus())
  	TabCtrl->currentWidget()->findChild<Editor*>("TE1")->copy();

  if (TabCtrl->currentWidget()->findChild<Editor*>("TE2") == 0)
    return; //return if TE2 does not exist (maybe type is GLOBAL_VAR, TYPE OR CONFIG
  
  if (TabCtrl->currentWidget()->findChild<Editor*>("TE2")->hasFocus())
    TabCtrl->currentWidget()->findChild<Editor*>("TE2")->copy();
}


void PageCtrl::createHighlighters(int i)
{ //! slot create syntax highlighters of current page
  //! \param i is only needed for signal-slot handling

  if ((flg_inhibit_createHighlighters == true) || (flg_activate_highlighters == false) || (TabCtrl->count() < 1))
  	return; //return if the open function is still in progress or highlighters not allowed or no page exists
  if ((TabCtrl->currentWidget()->findChild<PageData*>("pageData")->pd_highlighters) == "ON")
    return; //return if the highlighters were already created
  
  //set wait cursor
  QApplication::setOverrideCursor(Qt::WaitCursor);
  
  //create highlighters for declarations and if exists also the instructions TextEdit components
  highlighter_dec     = new Highlighter_Dec(TabCtrl->currentWidget()->findChild<Editor*>("TE1")->document());
  highlighter_dec->setObjectName("highlighter_dec");
  if (TabCtrl->currentWidget()->findChild<Editor*>("TE2") != 0)
  {
    highlighter_ins     = new Highlighter_Ins(TabCtrl->currentWidget()->findChild<Editor*>("TE2")->document());
    highlighter_ins->setObjectName("highlighter_ins");
  }
 
  TabCtrl->currentWidget()->findChild<PageData*>("pageData")->pd_highlighters = "ON"; //set label text to "ON"

  //restore cursor
  QApplication::restoreOverrideCursor();
}


void PageCtrl::cut()
{ //! cut from current inputwindow

  if (TabCtrl->count() < 1)
    return; 
  if (TabCtrl->currentWidget()->findChild<Editor*>("TE1")->hasFocus())
  	TabCtrl->currentWidget()->findChild<Editor*>("TE1")->cut();

  if (TabCtrl->currentWidget()->findChild<Editor*>("TE2") == 0)
    return; //return if TE2 does not exist (maybe type is GLOBAL_VAR, TYPE OR CONFIG
    
  if (TabCtrl->currentWidget()->findChild<Editor*>("TE2")->hasFocus())
    TabCtrl->currentWidget()->findChild<Editor*>("TE2")->cut();
}


void PageCtrl::deleteCurrentHighlighters(int i)
{ //! slot delete syntax highlighters of current page
  //! \param i is only needed for signal-slot handling

  if ((flg_inhibit_createHighlighters == true) || (flg_activate_highlighters == true) || (TabCtrl->count() < 1))
  	return; //return if the open function is still in progress or highlighters allowed or no page exists
  if ((TabCtrl->currentWidget()->findChild<PageData*>("pageData")->pd_highlighters) == "OFF")
    return; //return if the highlighters were already deleted

  //set wait cursor
  QApplication::setOverrideCursor(Qt::WaitCursor);
  //delete declarations highlighter and if exists also the instruction highlighter
  delete qobject_cast<Highlighter_Dec*>(TabCtrl->currentWidget()->findChild<Editor*>("TE1")->document()->findChild<Highlighter_Dec*>("highlighter_dec"));
  if (TabCtrl->currentWidget()->findChild<Editor*>("TE2") != 0)
    delete qobject_cast<Highlighter_Ins*>(TabCtrl->currentWidget()->findChild<Editor*>("TE2")->document()->findChild<Highlighter_Ins*>("highlighter_ins"));
  
  TabCtrl->currentWidget()->findChild<PageData*>("pageData")->pd_highlighters = "OFF";  //set label text to "OFF"

  //restore cursor
  QApplication::restoreOverrideCursor();
}


void PageCtrl::deletePage()
{ //! delete current page

  int index=0, reply=0;
  
  //delete tab + list item if counter > 0
  if (TabCtrl->count() > 0)
  {
    index = TabCtrl->currentIndex(); //get current index

    if (pageModifiedOf(index) && unsavedNotice)
    { //send mesag if page of index is modified and unsaved notice is enabled
      reply = QMessageBox::warning(this, "PLCEdit", msg_text_closepage, okButtonText, cancelButtonText, saveButtonText, 0, 2 );
      if (reply == 2) //emit save request if button save was used
        emit saveRequest();
      if (reply > 0) //return if button save or cancel was used
        return;
    }
    
    setPageCtrlSignalsBlocked(true);

    fillRecentFilesList(index);
    FileList->takeItem(index);
    delete TabCtrl->currentWidget();
   
    setPageCtrlSignalsBlocked(false);
   
    emit pageDeleted(index); //emit page deleted signal
  }
  
  //hide TabCtrl and FileList if counter < 1
  if (TabCtrl->count() < 1)
  {
  	FileList->hide(); TabCtrl->hide();
  }
  	
  emit modified(); //emit modiefied signal
}


void PageCtrl::deletePageIndex(int index)
{ //! delete page index

  TabCtrl->setCurrentIndex(index);
  deletePage();
}


void PageCtrl::newPage()
{ //! create a new page
  /*! pages contain a splitter, two inputwindows and some labels for page specific data
   *  PLCEdit uses no database to store file specific data. The file specific data are
   *  stored into hidden labels on the pages. Only the Splitter and the TextEdits are visible. 
   *  The content/components of the page can be edited by diverse public functions. 
   */
  /*!
   * page components: 
   * - splitter						= seperates the Editors, parent is TabCtrl 
   *   - TE1 						= contains declarations, parent is splitter 
   *   - TE2 						= contains instructions, parent is splitter 
   *   - pageData					= contains the following data strings, parent is splitter 
   *     - pd_FilePath				= contains path to file without file name, parent is pageData 
   *     - pd_FileExt 				= contains file extension, parent is pageData 
   *     - pd_FileType				= contains POU type mark, parent is pageData 
   *     - pd_FncType				= contains type of function, parent is pageData 
   *     - pd_FileEnd 				= contains POU end mark, parent is pageData 
   *     - pd_PROGLNG  				= contains programming language of file, parent is pageData 
   *     - pd_SucosoftHeader		= contains Sucosoft header, parent is pageData 
   *     - pd_MULTIPROGDescription	= contains MULTIPROG description section, parent is pageData 
   *     - pd_MULTIPROGProperties 	= contains MULTIPROG properties section, parent is pageData
   *     - pd_MULTIPROGWorkSheetName= contains MULTIPROG worksheet name, parent is pageData 
   *     - pd_STEPDBData 			= contains STEP 7 DB Data, parent is pageData
   *     - pd_MELSOFTConfig 		= contains MELSOFT configuration data, parent is pageData
   *     - pd_highlighters			= contains highlighter is allowance, parent is pageData 
   *     .
   *   .
   * . 
   */
    
  //Splitter container -> items are countet below
  QSplitter *splitter = new QSplitter(TabCtrl);  	//create new QSplitter as child of TabCtrl
  splitter->setObjectName("splitter");             	//set ObjectName to splitter
  splitter->hide();									//hide splitter for faster file opening
  //TextEdit elements (0), (1)
  Editor *TE1      = new Editor(splitter);  		//create Editor 1 as child of splitter
  Editor *TE2      = new Editor(splitter);  		//create Editor 2 as child of splitter
  TE1->setObjectName("TE1");                      	//set ObjectName to TE1
  TE2->setObjectName("TE2");                      	//set ObjectName to TE2
  TE1->setFont(font);              					//set Font of TE1
  TE2->setFont(font);              					//set Font of TE2
  TE1->setTabStopWidth(tabStopWidth);              	//set TabStop width of TE1
  TE2->setTabStopWidth(tabStopWidth);              	//set TabStop width of TE2
  //page data
  PageData *pageData = new PageData(splitter);		//create the page data class as child of splitter
  pageData->setObjectName("pageData");				//set ObjectName to pageData
  pageData->pd_FileExt = defaultPOUExt;				//set pd_FileExt string with default value
  pageData->pd_PROGLNG = prgLngStr;					//set pd_PROGLNG string with current value
  
  
  //add List item with name 'untitled'
  FileList->addItem(pageData->pd_FileName);
  //add splitter as new Tab to TabCtrl with name 'untitled'
  TabCtrl->addTab(splitter, pageData->pd_FileName);
  //set new splitter to current widget
  TabCtrl->setCurrentWidget(splitter);
  //configure editor (lines highlighters and print area indicator for TE1 & TE2, needed to prevent uncontrolled indications)
  configureCurrentEditor(0);
  //set splitter orientation
  setCurrentSplitMode(splitMode);
  //set set split size
  setCurrentSplitSize(splitSize);
  //set tab orientation
  setTabPosition(tabPosition);
  //set List and Tab component visibility
  setFileListVisible(flg_fileListVisible);
  setTabCtrlVisible(true);

  //diverse connections
  connect(TE1, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));
  connect(TE2, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));
  connect(TE1, SIGNAL(contentModified(bool)), 	this, SLOT(setPageModified(bool)));
  connect(TE2, SIGNAL(contentModified(bool)), 	this, SLOT(setPageModified(bool)));
  connect(TE1, SIGNAL(compareWith()), 			this, SIGNAL(compareWith()));
  connect(TE2, SIGNAL(compareWith()), 			this, SIGNAL(compareWith()));
  connect(TE1, SIGNAL(compareWithNext()), 		this, SLOT(compareWithNextCheck()));
  connect(TE2, SIGNAL(compareWithNext()), 		this, SLOT(compareWithNextCheck()));
  connect(TE1, SIGNAL(compareWithPrev()), 		this, SLOT(compareWithPrevCheck()));
  connect(TE2, SIGNAL(compareWithPrev()), 		this, SLOT(compareWithPrevCheck()));
  
  //emit modified information
  emit modified();
}


void PageCtrl::paste()
{ //! paste into current inputwindow

  if (TabCtrl->count() < 1)
    return; 
  if (TabCtrl->currentWidget()->findChild<Editor*>("TE1")->hasFocus())
  	TabCtrl->currentWidget()->findChild<Editor*>("TE1")->paste();

  if (TabCtrl->currentWidget()->findChild<Editor*>("TE2") == 0)
    return; //return if TE2 does not exist (maybe type is GLOBAL_VAR, TYPE OR CONFIG
    
  if (TabCtrl->currentWidget()->findChild<Editor*>("TE2")->hasFocus())
    TabCtrl->currentWidget()->findChild<Editor*>("TE2")->paste();
}


void PageCtrl::redo()
{ //! redo last input on current inputwindow

  if (TabCtrl->count() < 1)
    return; 
    
  if (TabCtrl->currentWidget()->findChild<Editor*>("TE1")->hasFocus())
  	TabCtrl->currentWidget()->findChild<Editor*>("TE1")->redo();
  	
  if (TabCtrl->currentWidget()->findChild<Editor*>("TE2") == 0)
    return; //return if TE2 does not exist (maybe type is GLOBAL_VAR, TYPE OR CONFIG
    
  if (TabCtrl->currentWidget()->findChild<Editor*>("TE2")->hasFocus())
    TabCtrl->currentWidget()->findChild<Editor*>("TE2")->redo();
}


void PageCtrl::selectAll()
{ //! select all text in current inputwindow

  if (TabCtrl->count() < 1)
    return; 
  if (TabCtrl->currentWidget()->findChild<Editor*>("TE1")->hasFocus())
  	TabCtrl->currentWidget()->findChild<Editor*>("TE1")->selectAll();

  if (TabCtrl->currentWidget()->findChild<Editor*>("TE2") == 0)
    return; //return if TE2 does not exist (maybe type is GLOBAL_VAR, TYPE OR CONFIG
    
  if (TabCtrl->currentWidget()->findChild<Editor*>("TE2")->hasFocus())
    TabCtrl->currentWidget()->findChild<Editor*>("TE2")->selectAll();
}


void PageCtrl::setNewSplitterVisible()
{ //! set visibility of new created page
	
  if (TabCtrl->count() < 1)
    return;  
    
  TabCtrl->currentWidget()->setVisible(true);  
}


void PageCtrl::setPageModified(bool modified)
{ //! slot page modified to handle changed content of declarations and instructions
  //! \param modified is only needed for signal-slot handling
  /*! The current tab and file list text will be marked with a modified tag. */

  QString tmpStr;
  
  setPageCtrlSignalsBlocked(true);
  
  if (currentTabText().contains(".")) 		//set modified tag in tab text
    tmpStr = currentFileName() + currentFileExtStr();
  else
    tmpStr = currentFileName();
  
  if(modified == true)
    setCurrentTabText("* " + tmpStr);
  else
    setCurrentTabText(tmpStr);
    
  setCurrentFileListItemText(currentTabText()); //set modified tag in file list item text
  
  setPageCtrlSignalsBlocked(false);
}


void PageCtrl::undo()
{ //! undo last typing in current inputwindow

  if (TabCtrl->count() < 1)
    return; 
  if (TabCtrl->currentWidget()->findChild<Editor*>("TE1")->hasFocus())
  	TabCtrl->currentWidget()->findChild<Editor*>("TE1")->undo();

  if (TabCtrl->currentWidget()->findChild<Editor*>("TE2") == 0)
    return; //return if TE2 does not exist (maybe type is GLOBAL_VAR, TYPE OR CONFIG
    
  if (TabCtrl->currentWidget()->findChild<Editor*>("TE2")->hasFocus())
    TabCtrl->currentWidget()->findChild<Editor*>("TE2")->undo();
}




//=============================================================================
// - - - public SIGNALs - - - 
/* EMPTY */




//=============================================================================
// - - - diverse public functions - - - 
QString PageCtrl::activeInputWindow()
{ //! return active inputwindow of current page as short name (Dec, Ins)
	
  if (TabCtrl->count() < 1)
  	return("");
  
  if (TabCtrl->currentWidget()->findChild<Editor*>("TE1")->hasFocus())
  	return("Dec");
  if (TabCtrl->currentWidget()->findChild<Editor*>("TE2") != 0)
    if (TabCtrl->currentWidget()->findChild<Editor*>("TE2")->hasFocus())
  	  return("Ins");	
  //if page exists but no textedit has focus, set Instruction section on focus
  this->blockSignals(true);
  if (TabCtrl->currentWidget()->findChild<Editor*>("TE2") != 0)
  {
    TabCtrl->currentWidget()->findChild<Editor*>("TE2")->setFocus();
    TabCtrl->currentWidget()->findChild<Editor*>("TE2")->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
  }
  this->blockSignals(false);
  return("Ins");
}


int PageCtrl::countPages()
{ //! count number of tabs

 return(TabCtrl->count());
}


QString PageCtrl::currentDeclaration()
{ //! return current declaration content

  if (TabCtrl->count() > 0)
    return(TabCtrl->currentWidget()->findChild<Editor*>("TE1")->toPlainText());
  else
    return("");
}


bool PageCtrl::currentDeclarationIsVisible()
{ //! return visibility of current declaration inputwindow
  
  if (TabCtrl->count() < 1)
    return(false);  
    
  return(TabCtrl->currentWidget()->findChild<Editor*>("TE1")->isVisible());  
}


QString PageCtrl::currentFileDate()
{ //! return current file date

  if (TabCtrl->count() > 0)
    return(TabCtrl->currentWidget()->findChild<PageData*>("pageData")->pd_FileDate);
  else
    return("");
}


QString PageCtrl::currentFileEnd()
{ //! return current file end

  if (TabCtrl->count() > 0)
    return(TabCtrl->currentWidget()->findChild<PageData*>("pageData")->pd_FileEnd);
  else
    return("");
}


int PageCtrl::currentFileExt()
{ //! return curent file extension
  
  if (TabCtrl->count() > 0)
    return((TabCtrl->currentWidget()->findChild<PageData*>("pageData")->pd_FileExt).toInt());
  else
    return(0);
} 


QString PageCtrl::currentFileExtStr()
{ //! return curent file extension string

  return(fileExtStrOf(currentTabIndex()));
} 


QString	PageCtrl::currentFileName()
{ //! return current filename without the extension string
  
  return(fileNameOf(currentTabIndex()));
}


QString PageCtrl::currentFilePath()
{ //! return curent file path (without file name)
	
  return(filePathOf(currentTabIndex()));
}


QString PageCtrl::currentFileType()
{ //! return current file type

  return(fileTypeOf(currentTabIndex()));
}


QString PageCtrl::currentFileTypeShort()
{ //! return current file type as short information:
  /*! - nothing set = 'NULL' */
  /*! - set but not detectable = 'error' */
  /*! - PROGRAM or ORGANIZATION_BLOCK = 'PRG' */
  /*! - FUNCTION_BLOCK = 'FB' */
  /*! - FUNCTION = 'FC + currentFncType()' */
  /*! - GLOBAL_VARIABLE_LIST = 'VAR' */
  
  if (TabCtrl->count() < 1)
    return("NULL");
  
  QString str = currentFileType();

  if ((str == "PROGRAM") || (str == "ORGANIZATION_BLOCK"))
    return("PRG");
  if (str == "FUNCTION_BLOCK")
    return("FB");
  if (str == "FUNCTION")
    return("FC" + currentFncType());
  if (str == "GLOBAL_VARIABLE_LIST")
    return("VAR");
  if (str == "TYPE")
    return(str);
  if (str == "CONFIGURATION")
    return("CONFIG");
  else
  if (str.isEmpty())
    return("NULL");
  else
    return("error");
}


QString PageCtrl::currentFncType()
{ //! return current function type

  return(fncTypeOf(currentTabIndex()));
}


QString PageCtrl::currentInstruction()
{ //! return current instruction content
  
  return(instructionOf(currentTabIndex()));
}


QString PageCtrl::currentMELSOFTConfig()
{ //return MELSOFT config of current page
 
  return(MELSOFTConfigOf(currentTabIndex()));
}


QString PageCtrl::currentMULTIPROGDescription()
{ //return MULTIPROG description of current page
 
  return(MULTIPROGDescriptionOf(currentTabIndex()));
}


QString PageCtrl::currentMULTIPROGProperties()
{ //return MULTIPROG properties of current page
 
  return(MULTIPROGPropertiesOf(currentTabIndex()));
}


QString PageCtrl::currentMULTIPROGWorkSheetName()
{ //return MULTIPROG worksheet name of current page
 
  return(MULTIPROGWorkSheetNameOf(currentTabIndex()));
}


QString PageCtrl::currentSTEP7DBData()
{ //return STEP 7 DB Data of current page
 
  return(STEP7DBDataOf(currentTabIndex()));
}


QString PageCtrl::currentSucosoftHeader()
{ //return Sucosoft header of current page
 
  return(SucosoftHeaderOf(currentTabIndex()));
}


QString PageCtrl::currentPrgLng()
{ //! return programming language of current page
  
  return(prgLngOf(currentTabIndex()));
}


QString PageCtrl::currentSelection()
{ //! return selected text of current input window
	
  if (TabCtrl->count() < 1)//if < 1 return std. value
  	return("");
  if (TabCtrl->currentWidget()->findChild<Editor*>("TE1")->hasFocus())
  	return(TabCtrl->currentWidget()->findChild<Editor*>("TE1")->textCursor().selectedText());
  	
  if (TabCtrl->currentWidget()->findChild<Editor*>("TE2") != 0)
    if (TabCtrl->currentWidget()->findChild<Editor*>("TE2")->hasFocus())
  	  return(TabCtrl->currentWidget()->findChild<Editor*>("TE2")->textCursor().selectedText());	
  return("");
}
	

QString	PageCtrl::currentSplitMode()
{ //! return splitter mode of current page

  if (TabCtrl->count() < 1)//if < 1 return std. value
    return("v"); 
  if (qobject_cast<QSplitter*>(TabCtrl->currentWidget())->orientation() == Qt::Horizontal);
    return("v");
  if (qobject_cast<QSplitter*>(TabCtrl->currentWidget())->orientation() == Qt::Vertical);
    return("h");    
}
	
	
int	PageCtrl::currentSplitSize()
{ //! return splitter size of current page

  if (TabCtrl->count() < 1)//if < 1 return std. value
    return(100); 
  
  QList<int> Sizes(qobject_cast<QSplitter*>(TabCtrl->currentWidget())->sizes());  //create list for size values
  return(Sizes.at(0));
}


int PageCtrl::currentTabIndex()
{ //! return current tab/page index

  if (TabCtrl->count() < 1)
    return(-1); //return if no page is active
    
  return(TabCtrl->currentIndex());
}


int PageCtrl::currentTabStopWidth()
{  //! return current tabstop width information

  if (TabCtrl->count() < 1)//if < 1 return std. value
  	return(tabStopWidth);
  return(TabCtrl->currentWidget()->findChild<Editor*>("TE1")->tabStopWidth());
}


QString	PageCtrl::currentTabText()
{ //! return current tab text = filename + extension

  return(tabTextOf(currentTabIndex()));
}


QString PageCtrl::cursorPosDec()
{ //! return cursor position of current declaration inputwindow
  int row=0, col=0;
  
  if (TabCtrl->count() > 0)
  {  
    QTextCursor cursor(TabCtrl->currentWidget()->findChild<Editor*>("TE1")->textCursor());
	row = cursor.blockNumber()+1;
	col = cursor.position() - cursor.block().position()+1;  
  }
  return(QString::number(row) + ":" + QString::number(col));
}


QString PageCtrl::cursorPosIns()
{ //! return cursor position of current instruction inputwindow
	
  int row=0, col=0;
  
  if ((TabCtrl->count() > 0) && (TabCtrl->currentWidget()->findChild<Editor*>("TE2") != 0))
  {  
    QTextCursor cursor(TabCtrl->currentWidget()->findChild<Editor*>("TE2")->textCursor());
	row = cursor.blockNumber()+1;
	col = cursor.position() - cursor.block().position()+1;
  }
  return(QString::number(row) + ":" + QString::number(col));
}


QString PageCtrl::declarationOf(int index)
{ //! return declaration content of tab/page index

  if (TabCtrl->count() < 1)
  return("");
  
  return(TabCtrl->widget(index)->findChild<Editor*>("TE1")->toPlainText());
}

QString PageCtrl::strFromDefinedPOUExtListAt(int index)
{ //! return extension string from POU extension definition list at index

  if ((index >= 0) && (index < definedPOUExtList.size())) 
    return(definedPOUExtList.at(index));
  else
    return(".txt");
}


void PageCtrl::delVARTypeItems()
{ //! delete items which are not used in GLOBAL_VAR lists (instruction inputwindow, moeller header)
	
  delete TabCtrl->currentWidget()->findChild<Editor*>("TE2");
}


void PageCtrl::fillCurrentDeclaration(QString Str)
{ //! clear and fill declaration inputwindow

  if (TabCtrl->count() < 1)//if < 1 return
    return; 

  TabCtrl->currentWidget()->findChild<Editor*>("TE1")->setPlainText(Str);
}


void PageCtrl::fillCurrentInstruction(QString Str)
{ //! clear and fill instruction inputwindow

  if (TabCtrl->count() < 1)//if < 1 return
    return; 

  if (TabCtrl->currentWidget()->findChild<Editor*>("TE2") != 0)
    TabCtrl->currentWidget()->findChild<Editor*>("TE2")->setPlainText(Str);
}


void PageCtrl::fillDeclarationOf(int index, QString Str)
{ //! clear and fill declaration inputwindow of index

  if (TabCtrl->count() < 1)//if < 1 return
    return; 

  TabCtrl->widget(index)->findChild<Editor*>("TE1")->setPlainText(Str);
}


void PageCtrl::fillInstructionOf(int index, QString Str)
{ //! clear and fill instruction inputwindow of index

  if (TabCtrl->count() < 1)//if < 1 return
    return; 

  if (TabCtrl->widget(index)->findChild<Editor*>("TE2") != 0)
    TabCtrl->widget(index)->findChild<Editor*>("TE2")->setPlainText(Str);
}


void PageCtrl::fillRecentFilesList(int index)
{ //! fill recent files list with item at index
  
  if ((TabCtrl->count() < 1) || (index >= TabCtrl->count()))
    return;
  
  QString str = filePathOf(index) + fileNameOf(index) + fileExtStrOf(index);

  QMutableStringListIterator i(recentFiles);
  while (i.hasNext()) //check if file in list does exist or is already enclosed
    if ((!QFile::exists(i.next())) || (i.peekPrevious() == str))
      i.remove(); 

  if ((!filePathOf(index).isEmpty()) && (fileNameOf(index) != "untitled"))
  { //get current file name and full file path
    recentFiles.prepend(str); //prepend new file
    //check current list length and correct it if needed
    correctRecentFilesListLength();
  }
}


void PageCtrl::correctRecentFilesListLength()
{ //! correct length of recent files list

  if (recentFiles.count() < maxRecentFiles)
    return; //return is current length ok
  
  int cnt = recentFiles.count();
  for (int i=maxRecentFiles; i < cnt; i++)
    recentFiles.removeAt(i);
}


QString PageCtrl::fileEndOf(int index)
{ //! return file end of tab/page index

  if (TabCtrl->count() < 1)
    return("");
    
  return(TabCtrl->widget(index)->findChild<PageData*>("pageData")->pd_FileEnd);
}


int PageCtrl::fileExtOf(int index)
{ //! return file extension of tab/page index
  	
  if (TabCtrl->count() < 1)
    return(0);
    
  return((TabCtrl->widget(index)->findChild<PageData*>("pageData")->pd_FileExt).toInt());
}


QString PageCtrl::fileExtStrOf(int index)
{ //! return file extension as string of tab/page index
  
  if (TabCtrl->count() < 1)//if < 1 return std. value
    return("");

  //get extension of page index as string from definition list
  return(strFromDefinedPOUExtListAt(fileExtOf(index)));
} 


QString	PageCtrl::fileNameOf(int index)
{ //! return filename of tab/page index without the extension string
  
  if (TabCtrl->count() < 1)//if < 1 return std. value
    return(""); 
  
  return(TabCtrl->widget(index)->findChild<PageData*>("pageData")->pd_FileName);
}


QString PageCtrl::fncTypeOf(int index)
{ //! return function type of tab/page index

  if (TabCtrl->count() < 1)
  return("");
  
  return(TabCtrl->widget(index)->findChild<PageData*>("pageData")->pd_FncType);
}


QString PageCtrl::filePathOf(int index)
{ //! return file path of tab/page index
	
  if (TabCtrl->count() < 1)
  return("");
  
  return(TabCtrl->widget(index)->findChild<PageData*>("pageData")->pd_FilePath);
}


QString PageCtrl::fileTypeOf(int index)
{ //! return file type of tab/page index
	
  if (TabCtrl->count() < 1)
    return("");

  return(TabCtrl->widget(index)->findChild<PageData*>("pageData")->pd_FileType);
}


bool PageCtrl::find(QString window, QString fStr, QString rStr, QTextDocument::FindFlags options)
{ //! find given fStr or replace with rStr in window by using QTextDocument::FindFlags
  //! \param window must be set to 'Dec' or 'Ins'
  
  if (TabCtrl->count() < 1)
    return(false); 						//return if not tab exists
   
  Editor *TE = 0;						//create a Editor pointer
    
  if (window == "Dec") 
    TE = TabCtrl->currentWidget()->findChild<Editor*>("TE1"); //get TE from Declarations

  if ((window == "Ins") && (TabCtrl->currentWidget()->findChild<Editor*>("TE2") != 0)) 
    TE = TabCtrl->currentWidget()->findChild<Editor*>("TE2"); //get TE from Instructions

  QTextDocument *doc = TE->document();	//get pointer to current document
  QTextCursor c = TE->textCursor();		//get cursor to current text

  
  //find string 
  QTextCursor newCursor = c;
  newCursor = doc->find(fStr, c, options);
  if (newCursor.isNull())
    newCursor = c; 						//if not set old to new cursor
  else
  {
    TE->setTextCursor(newCursor);   	//if yes move text cursor to new cursor  
    if (!rStr.isEmpty())
    {
      newCursor.removeSelectedText();
      newCursor.insertText(rStr);
    }    
  }
  
  //test if the string could be found after ccurrent position
  QTextCursor newCursor2 = newCursor;
  newCursor2 = doc->find(fStr, newCursor, options);
  if (newCursor2.isNull())
    return(false); 						//if not return with=0
  else
    return(true);  						//if yes return with=1   
}


void PageCtrl::insertText(QString window, QString Str)
{ //! insert Str into window
  //! \param window must be set to 'Dec' or 'Ins'
  /*! This function calls the function insertTextOf(QString window, QString Str, int index),
   *  by using currentTabIndex() to get the current index.
   *  Inputs on 'Ins' windows will be rejected if file type is GLOBAL_VARIABLE_LIST. 
   */
  
  insertTextOf(window, Str, currentTabIndex());
}


void PageCtrl::insertTextOf(QString window, QString Str, int index)
{ //! insert Str into window on tab/page index
  //! \param window must be set to 'Dec' or 'Ins'
  /*! Inputs on 'Ins' windows will be rejected if file type is GLOBAL_VARIABLE_LIST. */

  if (TabCtrl->count() < 1)
    return; //return if no page is active
  
  if (window == "Dec")
  { //get acces to TE1, get actual curso pos., insert string
    Editor *TE1 = TabCtrl->widget(index)->findChild<Editor*>("TE1");
    QTextCursor cursor(TE1->textCursor());
    //edit block
    cursor.beginEditBlock();
    cursor.insertText(Str);
    cursor.endEditBlock();
  }
  if ((window == "Ins") && (TabCtrl->currentWidget()->findChild<Editor*>("TE2") != 0))
  { //get acces to TE2, get actual curso pos., insert string
    Editor *TE2 = TabCtrl->widget(index)->findChild<Editor*>("TE2");
    QTextCursor cursor(TE2->textCursor());
    //edit block
    cursor.beginEditBlock();
    cursor.insertText(Str);
    cursor.endEditBlock();
  }
}


QString PageCtrl::instructionOf(int index)
{ //! return instruction content of tab/page index

  if ((TabCtrl->count() < 1) || (TabCtrl->currentWidget()->findChild<Editor*>("TE2") == 0))
    return("");
  
  return(TabCtrl->widget(index)->findChild<Editor*>("TE2")->toPlainText());
}


QString PageCtrl::MELSOFTConfigOf(int index)
{ //! return MELSOFT config of tab/page index
 
  if (TabCtrl->count() < 1)
    return(""); //return if no page is active
     
  return(TabCtrl->widget(index)->findChild<PageData*>("pageData")->pd_MELSOFTConfig);	
}


void PageCtrl::moveToStartDeclarations()
{ //! move anchor to start for declarations
	
  if (TabCtrl->count() < 1)
    return; //return if no page is active
  
  TabCtrl->currentWidget()->findChild<Editor*>("TE1")->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
}


void PageCtrl::moveToStartInstructions()
{ //! move anchor to start for instructions
	
  if ((TabCtrl->count() < 1) || (TabCtrl->currentWidget()->findChild<Editor*>("TE2") == 0))
    return; //return if no page is active or if current file is a GLOBAL VARIABLE LIST
  
  TabCtrl->currentWidget()->findChild<Editor*>("TE2")->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
}


QString PageCtrl::MULTIPROGDescriptionOf(int index)
{ //! return MULTIPROG description section of tab/page index
 
  if (TabCtrl->count() < 1)
    return(""); //return if no page is active
     
  return(TabCtrl->widget(index)->findChild<PageData*>("pageData")->pd_MULTIPROGDescription);	
}


QString PageCtrl::MULTIPROGPropertiesOf(int index)
{ //! return MULTIPROG properties section of tab/page index
 
  if (TabCtrl->count() < 1)
    return(""); //return if no page is active
     
  return(TabCtrl->widget(index)->findChild<PageData*>("pageData")->pd_MULTIPROGProperties);	
}


QString PageCtrl::MULTIPROGWorkSheetNameOf(int index)
{ //! return MULTIPROG worksheet name of tab/page index
	
  if (TabCtrl->count() < 1)
    return("Name: "); //return if no page is active
     
  return(TabCtrl->widget(index)->findChild<PageData*>("pageData")->pd_MULTIPROGWorkSheetName);
}


bool PageCtrl::pageModifiedOf(int index)
{ //! return page modified status

  if (TabCtrl->count() < 1)
    return(""); //return if no page is active
  
  bool modifiedTE1 = TabCtrl->widget(index)->findChild<Editor*>("TE1")->contentIsModified(); 
  bool modifiedTE2 = false;
  if (TabCtrl->currentWidget()->findChild<Editor*>("TE2") != 0)
    modifiedTE2 = TabCtrl->widget(index)->findChild<Editor*>("TE2")->contentIsModified();
    
  return(modifiedTE1 || modifiedTE2);
}


bool PageCtrl::pagesModified()
{  //! return if any page is modified

  bool modified = false;
  
  for (int i=0; i < countPages(); i++)
    if(pageModifiedOf(i) == true)
    { modified = true; break; }
  
  return(modified);
}


QString PageCtrl::prgLngOf(int index)
{ //! return programming language of tab/page index
  
  if (TabCtrl->count() < 1)
    return(""); //return if no page is active

  return(TabCtrl->widget(index)->findChild<PageData*>("pageData")->pd_PROGLNG);
}


void PageCtrl::setCurrentDeclarationVisible(bool visible)
{ //! set visibility of current declaration inputwindow 
	  
  if (TabCtrl->count() < 1)
    return;  
    
  TabCtrl->currentWidget()->findChild<Editor*>("TE1")->setVisible(visible);  
}


void PageCtrl::setCurrentFileDate(QString Str)
{ //! set file date string of current page

  if (TabCtrl->count() < 1)
    return; //return if no page is active

  TabCtrl->currentWidget()->findChild<PageData*>("pageData")->pd_FileDate = Str;
}


void PageCtrl::setCurrentFileEnd(QString Str)
{ //! set file end string of current page

  if (TabCtrl->count() < 1)
    return; //return if no page is active

  TabCtrl->currentWidget()->findChild<PageData*>("pageData")->pd_FileEnd = Str;
}


void PageCtrl::setCurrentFileExt(QString Str)
{ //! set file extension string of current page

  if (TabCtrl->count() < 1)
    return; //return if no page is active

  TabCtrl->currentWidget()->findChild<PageData*>("pageData")->pd_FileExt = Str;
}


void PageCtrl::setCurrentFileListItemText(QString Str)
{ //! insert FileList item named in Str

  if (TabCtrl->count() < 1)
    return; //return if no page is active

  FileList->insertItem(TabCtrl->currentIndex(), Str);
  FileList->takeItem(TabCtrl->currentIndex()+1);
  FileList->setCurrentRow(TabCtrl->currentIndex());
}


void PageCtrl::setCurrentFileName(QString Str)
{ //! set file path for current page

  if (TabCtrl->count() < 1)
    return; //return if no page is active

  setCurrentTabText(Str);
  
  Str = Str.remove(Str.lastIndexOf("."), Str.length());
  TabCtrl->currentWidget()->findChild<PageData*>("pageData")->pd_FileName = Str;
}


void PageCtrl::setCurrentFilePath(QString Str)
{ //! set file path for current page

  if (TabCtrl->count() < 1)
    return; //return if no page is active

  TabCtrl->currentWidget()->findChild<PageData*>("pageData")->pd_FilePath = Str;
}


void PageCtrl::setCurrentFileType(QString Str)
{ //! set file type of current page
  
  if (TabCtrl->count() < 1)
    return; //return if no page is active

  TabCtrl->currentWidget()->findChild<PageData*>("pageData")->pd_FileType = Str;
} 


void PageCtrl::setCurrentFncType(QString Str)
{ //! set function type of current page

  if (TabCtrl->count() < 1)
    return; //return if no page is active

  TabCtrl->currentWidget()->findChild<PageData*>("pageData")->pd_FncType = Str;
}


void PageCtrl::setCurrentFont(QFont font)
{ //! set font of current instruction and declaration inputwindows

  if (TabCtrl->count() < 1)
    return; //return if no page is active
    
  //set Font of TE1
  TabCtrl->currentWidget()->findChild<Editor*>("TE1")->setFont(font); 
  //set Font of TE2
  if (TabCtrl->currentWidget()->findChild<Editor*>("TE2") != 0)
    TabCtrl->currentWidget()->findChild<Editor*>("TE2")->setFont(font); 
}


void PageCtrl::setCurrentMELSOFTConfig(QString Str)
{ //! set MELSOFT config for current tab/page
	
  if (TabCtrl->count() < 1)
    return; //return if no page is active

  TabCtrl->currentWidget()->findChild<PageData*>("pageData")->pd_MELSOFTConfig = Str; 
}


void PageCtrl::setCurrentMULTIPROGDescription(QString Str)
{ //! set MULTIPROG description section for current tab/page
	
  if (TabCtrl->count() < 1)
    return; //return if no page is active

  TabCtrl->currentWidget()->findChild<PageData*>("pageData")->pd_MULTIPROGDescription = Str; 
}


void PageCtrl::setCurrentMULTIPROGProperties(QString Str)
{ //! set MULTIPROG properties section for current tab/page
	
  if (TabCtrl->count() < 1)
    return; //return if no page is active

  TabCtrl->currentWidget()->findChild<PageData*>("pageData")->pd_MULTIPROGProperties = Str; 
}


void PageCtrl::setCurrentMULTIPROGWorkSheetName(QString Str)
{ //! set MULTIPROG worksheet name for current tab/page
	
  if (TabCtrl->count() < 1)
    return; //return if no page is active

  TabCtrl->currentWidget()->findChild<PageData*>("pageData")->pd_MULTIPROGWorkSheetName = Str; 
}


void PageCtrl::setCurrentPrgLng(QString Str)
{ //! set programming language for current tab/page
  
  if (TabCtrl->count() < 1)
    return; //return if no page is active

  TabCtrl->currentWidget()->findChild<PageData*>("pageData")->pd_PROGLNG = Str;
}


void PageCtrl::setCurrentSplitMode(QString Str)
{ //! set splitter mode of current tab/page

  if (TabCtrl->count() < 1)//if < 1 return std. value
    return; 
   
  if (Str.contains("v"))  //set splitter orientation
    qobject_cast<QSplitter*>(TabCtrl->currentWidget())->setOrientation(Qt::Horizontal);
  else
    qobject_cast<QSplitter*>(TabCtrl->currentWidget())->setOrientation(Qt::Vertical);

}


void PageCtrl::setCurrentSplitSize(int i)
{ //! set split size for current tab/page

  Editor *TE2 = TabCtrl->currentWidget()->findChild<Editor*>("TE2");
  QSplitter *splitter1 = qobject_cast<QSplitter*>(TabCtrl->currentWidget());
  
  QList<int> Sizes;                         //create list for size values
  Sizes << i << 100;                		//fill list
  splitter1->setStretchFactor(splitter1->indexOf(TE2), 1); //set stretchfactor for TE2
  splitter1->setSizes(Sizes);               //set size values
}


void PageCtrl::setCurrentSTEP7DBData(QString Str)
{ //! set STEP 7 DB Data for current tab/page
	
  if (TabCtrl->count() < 1)
    return; //return if no page is active

  TabCtrl->currentWidget()->findChild<PageData*>("pageData")->pd_STEP7DBData = Str; 
}


void PageCtrl::setCurrentSucosoftHeader(QString Str)
{ //! set Sucosoft header for current tab/page

  if ((TabCtrl->count() < 1) || (TabCtrl->currentWidget()->findChild<Editor*>("TE2") == 0))
    return; //return if no page is active 

  TabCtrl->currentWidget()->findChild<PageData*>("pageData")->pd_SucosoftHeader = Str;	
}


void PageCtrl::setCurrentTypeData(QString filetype, QString functiontype, QString fileendmark)
{ //! set type of file, type of function, end of file string for current tab/page
  
  if (TabCtrl->count() < 1)
    return; //return if no page is active
    
  TabCtrl->currentWidget()->findChild<PageData*>("pageData")->pd_FileType = filetype; 
  TabCtrl->currentWidget()->findChild<PageData*>("pageData")->pd_FncType = functiontype;	
  TabCtrl->currentWidget()->findChild<PageData*>("pageData")->pd_FileEnd = fileendmark;  
}


void PageCtrl::setCurrentTabStops(int width)
{ //! set tab width of current instruction and declaration inputwindows

  if (TabCtrl->count() < 1)
    return; //return if no page is active
    
  //set TabStop width of TE1
  TabCtrl->currentWidget()->findChild<Editor*>("TE1")->setTabStopWidth(width); 
  //set TabStop width of TE2
  if (TabCtrl->currentWidget()->findChild<Editor*>("TE2") != 0)
    TabCtrl->currentWidget()->findChild<Editor*>("TE2")->setTabStopWidth(width); 
}


void PageCtrl::setCurrentTabText(QString Str)
{ //! set tab text of current tab/page

  if (TabCtrl->count() < 1)
    return; //return if no page is active

  TabCtrl->setTabText(TabCtrl->currentIndex(), Str);
}


void PageCtrl::setFileListVisible(bool visible)
{ //! set visibility of FileList

  flg_fileListVisible = visible;
  
  if ((TabCtrl->count() < 1) || (FileList->isVisible() == visible))
    return; 
  
  FileList->setVisible(visible);		//set file list visible
  //FileList->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
  FileList->setGeometry(0, 0, FileList->maximumWidth()*0.80, TabCtrl->height());
}


void PageCtrl::setMELSOFTConfigOf(int index, QString Str)
{ //! set MELSOFT config of tab/page index
	
  if (TabCtrl->count() < 1)
    return; //return if no page is active

  TabCtrl->widget(index)->findChild<PageData*>("pageData")->pd_MELSOFTConfig = Str; 
}


void PageCtrl::setMULTIPROGPropertiesOf(int index, QString Str)
{ //! set MULTIPROG properties section of tab/page index
	
  if (TabCtrl->count() < 1)
    return; //return if no page is active

  TabCtrl->widget(index)->findChild<PageData*>("pageData")->pd_MULTIPROGProperties = Str; 
}


void PageCtrl::setPageCtrlSignalsBlocked(bool blocked)
{ //! set signals of FileList and TabCtrl blocked

  FileList->blockSignals(blocked); 
  TabCtrl->blockSignals(blocked); 
}


void PageCtrl::setPrgLngOf(int index, QString Str)
{ //! set programming language of tab/page index
  
  if (TabCtrl->count() < 1)
    return; //return if no page is active

  TabCtrl->widget(index)->findChild<PageData*>("pageData")->pd_PROGLNG = Str;
}


void PageCtrl::setPageModifiedOf(int index, bool modified)
{ //! set page modified status of tab/page index

  if (TabCtrl->count() < 1)
    return; //return if no page is active

  //set content modified for TE1
  TabCtrl->widget(index)->findChild<Editor*>("TE1")->setContentModified(modified);
  //set content modified for TE2
  if (TabCtrl->currentWidget()->findChild<Editor*>("TE2") != 0)
    TabCtrl->widget(index)->findChild<Editor*>("TE2")->setContentModified(modified);
}


void PageCtrl::setSplitModeOf(int index, QString Str)
{ //! set split mode of tab/page index

  if (TabCtrl->count() < 1)//if < 1 return std. value
    return; 
   
  if (Str.contains("v"))  //set Splitter orientation
    qobject_cast<QSplitter*>(TabCtrl->widget(index))->setOrientation(Qt::Horizontal);
  else
    qobject_cast<QSplitter*>(TabCtrl->widget(index))->setOrientation(Qt::Vertical);
}


void PageCtrl::setTabCtrlVisible(bool visible)
{ //! set visibility of TabCtrl

  if ((TabCtrl->count() < 1) || (TabCtrl->isVisible() == visible))
    return; 

  TabCtrl->setGeometry(FileList->width(), 0, TabCtrl->maximumWidth(), TabCtrl->height());
  TabCtrl->setVisible(visible);		//set tab ctrl. visible
}


void PageCtrl::setTabPosition(int pos)
{ //! set tab orientation by selected postion
  //! \param pos 1=north, 2=south, 3=east, 4=west, default is north

  switch ( pos )
  {
    case 1  : { //north position
  	            TabCtrl->setTabPosition(QTabWidget::North);//set Tab pos. to north
                break;                                     //leave case contruct
              } //end case 1
    case 2  : { //south position
  	            TabCtrl->setTabPosition(QTabWidget::South);//set Tab pos. to north
                break;                                     //leave case contruct
              } //end case 2
    case 3  : { //east position
  	            TabCtrl->setTabPosition(QTabWidget::East);//set Tab pos. to north
                break;                                    //leave case contruct
              } //end case 3
    case 4  : { //west position
  	            TabCtrl->setTabPosition(QTabWidget::West);//set Tab pos. to north
                break;                                    //leave case contruct
              } //end case 4
   default :  { //empty
                TabCtrl->setTabPosition(QTabWidget::North);//set Tab pos. to north
                break;                                     //leave case contruct
              } //end default
  }//end switch
}


QString PageCtrl::STEP7DBDataOf(int index)
{ //! return STEP 7 DB Data of index
	
  if (TabCtrl->count() < 1)
    return(""); //return if no page is active
     
  return(TabCtrl->widget(index)->findChild<PageData*>("pageData")->pd_STEP7DBData);
}


QString PageCtrl::SucosoftHeaderOf(int index)
{ //! return Sucosoft header of tab/page index
 
  if (TabCtrl->count() < 1)
    return(""); //return if no page is active
     
  return(TabCtrl->widget(index)->findChild<PageData*>("pageData")->pd_SucosoftHeader);	
}


QString PageCtrl::tabTextOf(int index)
{ //! return tab text of tab/page index = filename + extension

  if (TabCtrl->count() < 1)//if < 1 return std. value
    return(""); 

  return(TabCtrl->tabText(TabCtrl->indexOf(TabCtrl->widget(index))));
}




//=============================================================================
// - - - private SLOTs - - -
void PageCtrl::compareWithNextCheck()
{ //! check if next editor window exists

  if (currentTabIndex()+1 < countPages())
    emit compareWithNext();
  else
    emit compareWith();
}


void PageCtrl::compareWithPrevCheck()
{ //! check if previous editor window exists

  if (currentTabIndex() > 0)
    emit compareWithPrev();
  else
    emit compareWith();
}


void PageCtrl::cursorPositionChanged()
{ //! slot curosr postion changed, does emit cursor modified signal
	
  emit cursorModified();
}


#if QT_VERSION >= 0x040500
  void PageCtrl::moveFileListItem(int from, int to)
  { //! slot for move the corresponding file list item according to a moved tab

    setPageCtrlSignalsBlocked(true);
    
    QListWidgetItem *listItem = FileList->item(from); 	//get to move list item
    FileList->takeItem(from);							//take item at old position
    FileList->insertItem(to, listItem->text());			//insert item at new position

    setPageCtrlSignalsBlocked(false); 
}
#endif


void PageCtrl::setFileListRow(int index)
{ //! slot for set FileList row by index
	
  FileList->setCurrentRow(index);	
}



//=============================================================================
// - - - diverse private functions - - - 
QString PageCtrl::strippedFileName(const QString &fullFileName)
{ //! strip filename from fullFileName (path + filename)
  
  return(QFileInfo(fullFileName).fileName().simplified());
}


void PageCtrl::retranslateUi()
{ //! retranslate Ui

  #if QT_VERSION < 0x040500
    delPageButton->setToolTip(tr("Close page"));
  #endif
  
  //message unsaved POU
  msg_text_closepage = tr("The current POU is unsaved.<br>Do you want to continue?");
  
  //buttons
  okButtonText = tr("OK");
  cancelButtonText = tr("Cancel");
  saveButtonText = tr("Save");
}
