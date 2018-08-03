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


#ifndef POUINFODIALOG_H
#define POUINFODIALOG_H

#include <QDialog>

#include "ui_POUInfoDialog.h"

class PageCtrl; 		//get access to PageCtrl class
class Prototype; 		//get access to Prototype class
class ImportExport;		//get access to Imp/Exp class


class POUInfoDialog : public QDialog, public Ui::POUInfoDialog
{
  Q_OBJECT
   
public:
  POUInfoDialog(QWidget *parent, PageCtrl *pageCtrl, Prototype *prototype, ImportExport *importExport);
  virtual ~POUInfoDialog();
	
public:
	

public slots:
	void setDialog();
	void setPrototype();
	void retranslateUi();
		

private slots:
    void getDataFromDialog();
    
    
private:
	void getSucosoftHeaderFromDialog();
	void getMULTIPROGDataFromDialog();
	void setDialogFromSucosoftHeader();
	void setDialogFromMULTIPROGData();
	void setInterfaceItemsEnabled();

	
	PageCtrl *m_PageCtrl; 				//!< instance of PageCtrl class
    Prototype *m_Prototype;				//!< instance of Prototype class
	ImportExport *m_ImportExport;		//!< instance of ImportExport class
};
#endif
