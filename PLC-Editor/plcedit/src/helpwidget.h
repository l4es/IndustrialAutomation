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


#ifndef HELPWIDGET_H
#define HELPWIDGET_H

#include <QWidget>

#include "ui_HelpWidget.h"


class HelpWidget : public QWidget, public Ui::HelpWidget
{
  Q_OBJECT

public:
  HelpWidget( QWidget *parent);
  virtual ~HelpWidget();

  void retranslateUi();


private slots:
  void checkAnchor(const QUrl &link);
  void find();
  void openWebsite();


private:
  void setHelpFile(QString filePath, QString file);
  
  QString helpDir;		//!< help file directory 
  QString helpFileName; //!< help file file name 

};
#endif





