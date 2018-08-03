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


#ifndef FBDIALOG_H
#define FBDIALOG_H

#include <QDialog>

#include "ui_FBDialog.h"

class PageCtrl; 		//get access to PageCtrl class


class FBDialog : public QDialog, public Ui::FBDialog
{
  Q_OBJECT
   
public:
  FBDialog(QWidget *parent = 0, PageCtrl *pageCtrl = 0);
  virtual ~FBDialog();
  
  bool flg_useSimpleIECCallModel;  //!< flag use least IEC61131-3 call model, used by preferences dialog
  
  void fillBlockList();
  QString selectedBlockModel();
  void setSortingEnabled(bool enable);
  void retranslateUi();

	
private slots:
  void setFBD_checkBox_insertdatatypesState(bool state);
  void setFBD_checkBox_insertIOCommentsState(bool state);
  void setOkButtonState();
  void showCurrentFBofFBs(int i);
  

private:
  QString getCallModelItem(QString line, QString searchStr1, QString searchStr2, QString addStr);
  QString setItemComment(QString line);
  QString getIOComment(QString line);
  QString getDataType(QString line);
  QString getInVARSectionLine(QString FBName, int Ext, QString line);
  QString getOutVARSectionLine(QString FBName, int Ext, QString line);
  
  PageCtrl 		*m_PageCtrl; 		//!< instance of PageCtrl class
    
};
#endif



