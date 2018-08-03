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

#include "updateinfo.h"
#include "mainwindow.h"


UpdateInfo::UpdateInfo(MainWindow *parent) 
 : m_MainWindow(parent)
{ //! creating function

  setWindowFlags(windowFlags()^Qt::WindowContextHelpButtonHint);
  
  setupUi(this);
  retranslateUi();


  //set Button sizes
  #ifdef Q_WS_MAC	
    UD_textBrowser->setFontPointSize(6);
  #else
    UD_pushButton_close->setMinimumHeight(25);
    UD_pushButton_close->setMaximumHeight(25);
  #endif

  UD_label_downloadlink->setOpenExternalLinks(true); 
}


UpdateInfo::~UpdateInfo()
{ //! virtual destructor
}


void UpdateInfo::getUpdateData(QString version, QString revision, QString changelog)
{ //! get dialog data function

  UD_label_version_hint->setText(msg_new_version.arg((version + " (" + revision + ")")));
  UD_label_downloadlink->setText("<b><a href=http://" + m_MainWindow->PROJECT_URL + m_MainWindow->DOWNLOAD_PATH + ">" + msg_download_now + "</a></b><br>");
  UD_textBrowser->setHtml(changelog);
}


void UpdateInfo::retranslateUi()
{ //! retranslate Ui

  this->setWindowTitle(m_MainWindow->PROJECT_NAME  + "  " + tr("Update"));
  
  //close Button
  UD_pushButton_close->setText(tr("Close"));
  UD_pushButton_close->setStatusTip(tr("Close update box"));
  UD_pushButton_close->setToolTip(UD_pushButton_close->statusTip());

  //messages
  msg_new_version = tr("Version %1 is available now.");
  msg_download_now = tr("Download now");

}
