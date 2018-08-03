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

#include "finddialog.h"
#include "mainwindow.h"
#include "pagectrl.h"


FindDialog::FindDialog(MainWindow *parent, PageCtrl *pageCtrl) 
  : m_MainWindow(parent), m_PageCtrl(pageCtrl)
{ //! creating function
  //! \param PageCtrl class

  setWindowFlags(windowFlags()^Qt::WindowContextHelpButtonHint);
  setWindowFlags( Qt::Tool | Qt::WindowStaysOnTopHint );
  
  setupUi(this);
  retranslateUi();	 
	
  bwd = false;
  //FRD_pushButton_replaceAll->hide(); 
  timer = new QTimer(this); //timer for message at dialog


  //diverse connections
  connect(FRD_pushButton_next, 		SIGNAL(clicked()), 			this, SLOT(find_fwd()));
  connect(FRD_pushButton_previous,	SIGNAL(clicked()), 			this, SLOT(find_bwd()));
  connect(FRD_pushButton_replaceAll,SIGNAL(clicked()), 			this, SLOT(replaceAll()));
  connect(this,						SIGNAL(cursorAtStartPos()), this, SLOT(cursorAtStartPosMsg()));
  connect(timer, 					SIGNAL(timeout()), 			this, SLOT(clearStartPosMsg()));
	
  //set Button sizes
  #ifdef Q_WS_MAC	
  //do nothing for Mac OS
  #else
    FRD_pushButton_close->setMinimumHeight(25);
    FRD_pushButton_close->setMaximumHeight(25);
    FRD_pushButton_next->setMinimumHeight(25);
    FRD_pushButton_next->setMaximumHeight(25);
    FRD_pushButton_previous->setMinimumHeight(25);
    FRD_pushButton_previous->setMaximumHeight(25);
    FRD_pushButton_replaceAll->setMinimumHeight(25);
    FRD_pushButton_replaceAll->setMaximumHeight(25);
    QFont font;
    font.setPointSize(8);
    FRD_label_changeWindow->setFont(font); 
  #endif	

}


FindDialog::~FindDialog()
{ //! virtual destructor

  delete timer;
}


void FindDialog::clearStartPosMsg()
{ //! clear message label if inputwindow changed

  FRD_label_changeWindow->clear();
}


void FindDialog::cursorAtStartPosMsg()
{ //! activate message label if cursor inputwindow changed

  timer->start(1500);
  FRD_label_changeWindow->setText(msg_text_changewindow);
}
	

void FindDialog::showDialog()
{ //! show Dialog

  if (m_PageCtrl->countPages() > 0)
    FRD_lineEdit_findString->setText(m_PageCtrl->currentSelection());
    FRD_lineEdit_replaceString->clear();

  show();
  activateWindow();
}


void FindDialog::find_bwd()
{ //! activate find routine backwards
  bwd = true;
  find(false);
}


void FindDialog::find_fwd()
{ //! activate find routine in forward direction
  bwd = false;
  find(false);
}


void FindDialog::find(bool replaceAll)
{ //! find function, using input fields and check boxes of the dialog
  /*! The replacement function start at the current cursor position and stops at the end of the current window. */	
  //! \param replaceAll is used for replacement loop in the current selected window

  bool ok=true;

  clearStartPosMsg();
  
  if (window.isEmpty())
  	return;
  
  int options = 0;
  if ( bwd == true )
    options |= QTextDocument::FindBackward;
  if ( FRD_checkBox_match->isChecked() )
    options |= QTextDocument::FindWholeWords;
  if ( FRD_checkBox_caseSensitive->isChecked() )
    options |= QTextDocument::FindCaseSensitively;
  
  do { //call find routine in m_PageCtrl depending on while conditions
    ok = m_PageCtrl->find(window, FRD_lineEdit_findString->text(), FRD_lineEdit_replaceString->text(), (QTextDocument::FindFlags)options);
  }  while (ok && replaceAll); //if replace All is true repeat loop until find string cannot be found

  if (ok == false)
  {
    if (window == "Dec")
    {  window = "Ins"; m_PageCtrl->moveToStartInstructions();  }
    else
    {  window = "Dec"; m_PageCtrl->moveToStartDeclarations();  }
    emit cursorAtStartPos();
  } 
}


void FindDialog::replaceAll()
{ //! replace all routine

  find(true);
}


void FindDialog::retranslateUi()
{ //! retranslate Ui

  this->setWindowTitle(tr("Find and Replace")); 

  //replace all Button
  FRD_pushButton_replaceAll->setText(tr("Replace all"));
  FRD_pushButton_replaceAll->setStatusTip(tr("replace all found matches"));
  FRD_pushButton_replaceAll->setToolTip(FRD_pushButton_replaceAll->statusTip());

  //cancel Button
  FRD_pushButton_previous->setText(tr("Previous"));
  FRD_pushButton_previous->setStatusTip(tr("Find previous matching word"));
  FRD_pushButton_previous->setToolTip(FRD_pushButton_previous->statusTip());

  //cancel Button
  FRD_pushButton_next->setText(tr("Next"));
  FRD_pushButton_next->setStatusTip(tr("Find/replace next matching word"));
  FRD_pushButton_next->setToolTip(FRD_pushButton_next->statusTip());

  //close Button
  FRD_pushButton_close->setText(tr("Close"));
  FRD_pushButton_close->setStatusTip(tr("Close window"));
  FRD_pushButton_close->setToolTip(FRD_pushButton_close->statusTip());
  
  //label find string
  FRD_label_findString->setText(tr("Find:"));
  FRD_label_findString->setStatusTip(tr("Search for word in edit field"));
  FRD_label_findString->setToolTip(FRD_label_findString->statusTip());
  //line edit find string
  FRD_lineEdit_findString->setStatusTip(FRD_label_findString->statusTip());
  FRD_lineEdit_findString->setToolTip(FRD_lineEdit_findString->statusTip());
  
  //label replace string
  FRD_label_replaceString->setText(tr("Replace with:"));
  FRD_label_replaceString->setStatusTip(tr("Replace word in find field with word in replace with field"));
  FRD_label_replaceString->setToolTip(FRD_label_replaceString->statusTip());
  //line edit replace string
  FRD_lineEdit_replaceString->setStatusTip(FRD_label_replaceString->statusTip());
  FRD_lineEdit_replaceString->setToolTip(FRD_lineEdit_replaceString->statusTip());  

  //checkbox case sensitive
  FRD_checkBox_caseSensitive->setText(tr("Find case senitive")); 
  FRD_checkBox_caseSensitive->setStatusTip(tr("Search with case sensitive activated"));
  FRD_checkBox_caseSensitive->setToolTip(FRD_checkBox_caseSensitive->statusTip());
 
  //checkbox case match
  FRD_checkBox_match->setText(tr("Must match whole word")); 
  FRD_checkBox_match->setStatusTip(tr("Find string must match exact"));
  FRD_checkBox_match->setToolTip(FRD_checkBox_match->statusTip());
  
  //messages
  msg_text_changewindow = tr("The opposite window is now selected.");
}
