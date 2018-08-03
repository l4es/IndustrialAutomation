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
#include <QDesktopServices>

#include "helpwidget.h"


HelpWidget::HelpWidget(QWidget *parent=0) 
	: QWidget(parent)
{ //! creting function

  setWindowFlags(windowFlags()^Qt::WindowContextHelpButtonHint);
    
  setupUi(this);
  retranslateUi();
  
  //set icons
  HelpWidget_backward->setIcon(QIcon(":/images/back_22x22.png"));
  HelpWidget_forward->setIcon(QIcon(":/images/forward_22x22.png"));
  HelpWidget_home->setIcon(QIcon(":/images/home_22x22.png"));
  HelpWidget_find->setIcon(QIcon(":/images/find_replace_22x22.png"));
  HelpWidget_internet->setIcon(QIcon(":/images/internet_24x24.png"));
  
  //set Button sizes
  #ifdef Q_WS_MAC	
    HelpWidget_backward->setMaximumHeight(24);
    HelpWidget_backward->setMaximumWidth(24);
    HelpWidget_forward->setMaximumHeight(24);
    HelpWidget_forward->setMaximumWidth(24);
    HelpWidget_home->setMaximumHeight(24);
    HelpWidget_home->setMaximumWidth(24);
    HelpWidget_find->setMaximumHeight(24);
    HelpWidget_find->setMaximumWidth(24);
    HelpWidget_findString->setMaximumHeight(24);
    HelpWidget_findString->setMaximumWidth(24);
    HelpWidget_internet->setMaximumHeight(24);
    HelpWidget_internet->setMaximumWidth(24);
  #endif
  
  HelpWidget_findString->setGeometry(HelpWidget_findString->x(), HelpWidget_findString->y(), HelpWidget_find->height(), HelpWidget_findString->width());	

  helpDir = "/Help/";
  helpFileName = "index.html";

  //setup help file for browser
  setHelpFile((QApplication::applicationDirPath() + helpDir), helpFileName);
  
  //diverse connections
  connect(HelpWidget_textbrowser, 	SIGNAL(anchorClicked(const QUrl &)), 	this, SLOT(checkAnchor(const QUrl &)));
  connect(HelpWidget_find, 			SIGNAL(clicked()), 						this, SLOT(find()));
  connect(HelpWidget_internet, 		SIGNAL(clicked()), 						this, SLOT(openWebsite()));
}

HelpWidget::~HelpWidget()
{ //! virtual destructor
}


void HelpWidget::checkAnchor(const QUrl &link)
{ //! check anchor to internal or external link

  QString str = link.toString();				//get string from url

  if (!str.contains("#") == true)				//check if internal anchor
  { //handle external links
    bool ok = QDesktopServices::openUrl(link);	//open external link with QDS
    setHelpFile((QApplication::applicationDirPath() + helpDir), helpFileName);//reset help file <-- workaround bec. openExternaLinks & openLinks working not together
  }
}


void HelpWidget::find()
{ //! find string of input field
 
  QString findStr = HelpWidget_findString->text();
  QTextDocument *doc = HelpWidget_textbrowser->document();	//get pointer to current document
  QTextCursor c = HelpWidget_textbrowser->textCursor();		//get cursor to current text
  
  //find string 
  QTextCursor newCursor = c;
  newCursor = doc->find(findStr, c);
  if (newCursor.isNull())
    newCursor = c; 											//if not set old to new cursor
  else
    HelpWidget_textbrowser->setTextCursor(newCursor);   	//if yes move text cursor to new cursor     
  
  //test if the string could be found after current position
  QTextCursor newCursor2 = newCursor;
  newCursor2 = doc->find(findStr, newCursor);
  if ((newCursor2.isNull()) || (newCursor.atEnd())) 		//reset cursor to start position if cursor is at file end or findStr does not exist after current position
    HelpWidget_textbrowser->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
}


void HelpWidget::openWebsite()
{ //! open the project website

  bool ok = QDesktopServices::openUrl(QUrl(QApplication::organizationDomain()));	//open external link with QDS
}


void HelpWidget::retranslateUi()
{ //! retranslate Ui

  this->setWindowTitle(tr("PLCEdit Help")); 
  
  HelpWidget_backward->setStatusTip(tr("Go back"));
  HelpWidget_backward->setToolTip(HelpWidget_backward->statusTip());
  
  HelpWidget_forward->setStatusTip(tr("Go forward"));
  HelpWidget_forward->setToolTip(HelpWidget_forward->statusTip());

  HelpWidget_home->setStatusTip(tr("Return to the beginning of the document"));
  HelpWidget_home->setToolTip(HelpWidget_home->statusTip());
  
  HelpWidget_find->setStatusTip(tr("Find search string"));
  HelpWidget_find->setToolTip(HelpWidget_find->statusTip());
  
  HelpWidget_internet->setStatusTip(tr("Open the project homepage"));
  HelpWidget_internet->setToolTip(HelpWidget_internet->statusTip());
}


void HelpWidget::setHelpFile(QString filePath, QString file)
{ //! set help file path and file name

  //fill textbrowser with help file
  QStringList path;
  path << (filePath);							//set path
  HelpWidget_textbrowser->setSearchPaths(path);	//set search path 
  HelpWidget_textbrowser->setSource(QUrl(file));//set file source
}
