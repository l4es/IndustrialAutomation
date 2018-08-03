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


#ifndef UPDATEINFO_H
#define UPDATEINFO_H

#include <QDialog>

#include "ui_UpdateDialog.h"

class MainWindow; 		//get access to MainWindow class


class UpdateInfo : public QDialog, public Ui::UpdateDialog
{
  Q_OBJECT
   
public:
  UpdateInfo(MainWindow *);
	virtual ~UpdateInfo();

	
public slots:
	void getUpdateData(QString version, QString revision, QString changelog);
	void retranslateUi();


signals:


private:
  QString msg_new_version;			//!< message text for new version 
  QString msg_download_now;			//!< message text for download link
    

  //Instances
  MainWindow *m_MainWindow; 		//!< instance of MainWindow class

};
#endif



