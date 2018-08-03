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

#include "aboutdialog.h"
#include "mainwindow.h"


AboutDialog::AboutDialog(MainWindow *parent) 
 : m_MainWindow(parent)
{ //! creating function

  setWindowFlags(Qt::FramelessWindowHint);
  setWindowFlags(windowFlags()^Qt::WindowContextHelpButtonHint);
  
  setupUi(this);
  retranslateUi();


  //set Button sizes
  #ifdef Q_WS_MAC	
    AD_textBrowser->setFontPointSize(6);
  #else
   AD_pushButton_close->setMinimumHeight(25);
   AD_pushButton_close->setMaximumHeight(25);
  #endif
  
}


AboutDialog::~AboutDialog()
{ //! virtual destructor
}


void AboutDialog::retranslateUi()
{ //! retranslate Ui

  this->setWindowTitle(tr("About") + " " + m_MainWindow->PROJECT_NAME);
  
  //close Button
  AD_pushButton_close->setText(tr("Close"));
  AD_pushButton_close->setStatusTip(tr("Close about box"));
  AD_pushButton_close->setToolTip(AD_pushButton_close->statusTip());

  //labels
  AD_projectStr_label->setText(m_MainWindow->PROJECT_NAME);
  AD_versionStr_label->setText(m_MainWindow->VERSION + "   (" + m_MainWindow->REVISION + ")");
  AD_copyrightStr_label->setText("<p>copyright &copy; "+ QString("2005-%1").arg(m_MainWindow->PROJECT_DATE) +", M. Rehfeldt</p>");
  AD_webStr_label->setText("<a href=http://www." + m_MainWindow->PROJECT_URL +">" + "http://" + m_MainWindow->PROJECT_URL + "</a>");
  AD_webStr_label->setOpenExternalLinks(true);  
  
  //messages
  msg_text_plce = tr("PLCEdit is a powerfull programming notepad for PLC source code, compatible to");
  
  msg_text_about.clear();
  msg_text_about.append("<p><font size=2>");
  msg_text_about.append(msg_text_plce + ":<ul>");
  msg_text_about.append("<li>" + tr("plain POU sources") + "</li>");
  msg_text_about.append("<li>" + tr("3S CoDeSys V2.3") + "</li>");
  msg_text_about.append("<li>" + tr("KW-Software MULTIPROG") + "</li>");
  msg_text_about.append("<li>" + tr("Mitsubishi MELSOFT IEC") + "</li>");
  msg_text_about.append("<li>" + tr("Moeller Sucosoft S40") + "</li>");
  msg_text_about.append("<li>" + tr("Siemens STEP 7") + "</li></ul>");
  msg_text_about.append("<br>");
  msg_text_about.append(tr("This programm is distributed under the terms of the GPL v2 and is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.") + "<br>");
  msg_text_about.append("</font></p>");
  msg_text_about.append("<p><font size=2><i>" + tr("Special Thanks to") + ":<br>");
  msg_text_about.append("Jean-Luc Biord (" + tr("for programming QDevelop") + ")<br>");
  msg_text_about.append("Pavel Fric (" + tr("Czech translation") + ")<br>");
  msg_text_about.append("</i></font></p>");
  AD_textBrowser->setHtml(msg_text_about);
}
