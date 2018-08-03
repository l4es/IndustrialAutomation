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


#ifndef FINDDIALOH_H
#define FINDDIALOG_H

#include <QDialog>
#include <QTimer>

#include "ui_FindReplaceDialog.h"


class MainWindow; 		//get access to MainWindow class
class PageCtrl; 		//get access to PageCtrl class


class FindDialog : public QDialog, public Ui::FindReplaceDialog
{
  Q_OBJECT
  
public:
  FindDialog(MainWindow *, PageCtrl *pageCtrl);
  virtual ~FindDialog();

  QString window;

public slots:
  void showDialog();
  void retranslateUi();
  
 
signals:
    void cursorAtStartPos();		//!< signal cursor was changed to the start position of opposite window

  
private slots:
  void clearStartPosMsg();
  void cursorAtStartPosMsg();
  void find_bwd();
  void find_fwd();
  void find(bool replaceAll);
  void replaceAll();

  
private:
    bool bwd;						//!< flag search backwards
    QString msg_text_changewindow;	//!< message changed inputwindow information
    
    QTimer	*timer;					//!< timer for emporary message popup
	MainWindow *m_MainWindow; 		//!< instance of MainWindow class
    PageCtrl *m_PageCtrl; 			//!< instance of PageCtrl class
};
#endif
