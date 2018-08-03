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


#ifndef BATCHDIALOG_H
#define BATCHDIALOG_H

#include <QDialog>

#include "ui_BatchDialog.h"


class BatchDialog : public QDialog, public Ui::BatchDialog
{
  Q_OBJECT
  
public:
	BatchDialog(QDialog *parent);
	virtual ~BatchDialog();


public:
	void 	presetPath(QString dir);
	QString selectedCompatibilityStr();
	int		selectedCompatibility();
	int		selectedConversion();
	QString selectedPath();

	
public slots:
	void retranslateUi();


signals:
	
	
private slots:
	void PathDlg();
	
   
private:
  QString msg_head_pathdialog; //!< message head of dialog



};
#endif

