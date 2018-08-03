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


#ifndef FNCDIALOG_H
#define FNCDIALOG_H

#include <QDialog>

#include "ui_FncDialog.h"


class FncDialog : public QDialog, public Ui::FncDialog
{
  Q_OBJECT
   
public:
  FncDialog(QWidget *parent = 0);
  virtual ~FncDialog();
  QString currentName();
  void retranslateUi();

	
private slots:

  void okButtonState();
  void setokButtonState();
  void resetTreeWidget(const QString &text);
  void resetUserDefinedType();
	
};
#endif
