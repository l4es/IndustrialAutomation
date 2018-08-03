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

#include "pathfilenamedialog.h"


PathFileNameDialog::PathFileNameDialog(QDialog *parent) 
 : QDialog(parent)
{ //! creating function

	setupUi(this);
	retranslateUi();
	
	setWindowFlags( Qt::Tool | Qt::WindowStaysOnTopHint );
	
	//diverse connections
  	connect(PFD_pushButton_path, SIGNAL(clicked()), this, SLOT(PathDlg()));		//get Path
  	//set Button sizes
	#ifdef Q_WS_MAC	
  	//do nothing for Mac OS
	#else
 	 PFD_pushButton_close->setMinimumHeight(25);
 	 PFD_pushButton_close->setMaximumHeight(25);
  	 PFD_pushButton_ok->setMinimumHeight(25);
 	 PFD_pushButton_ok->setMaximumHeight(25);
	#endif  
}


PathFileNameDialog::~PathFileNameDialog()
{ //! virtual destructor
}


void PathFileNameDialog::presetPath(QString dir)
{ //! preset line edit "path"
  
  PFD_lineEdit_path->setText(QFileInfo(dir.simplified()).absolutePath() + "/");
}
		

void PathFileNameDialog::PathDlg()
{ //! get path from FileDialog and set into line edit "path"
 
  PFD_lineEdit_path->setText(QFileDialog::getExistingDirectory(this, msg_head_pathdialog, "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks)  + "/");
}


QString PathFileNameDialog::currentPath()
{ //! get current path from line edit "path"

  return(QFileInfo(PFD_lineEdit_path->text().simplified()).absolutePath() + "/"); 
}


QString PathFileNameDialog::currentFileName()
{ //! get current file extension from comboBox "compatibility"
	
  return(PFD_lineEdit_fileName->text().simplified());	
}


void PathFileNameDialog::retranslateUi()
{ //! retranslate Ui

  this->setWindowTitle("Save all pages as project");
	
  //ok Button
  PFD_pushButton_ok->setText(tr("Ok"));
  PFD_pushButton_ok->setStatusTip(tr("Accept inputs and close window"));
  PFD_pushButton_ok->setToolTip(PFD_pushButton_ok->statusTip());
  
  //close Button
  PFD_pushButton_close->setText(tr("Close"));
  PFD_pushButton_close->setStatusTip(tr("Cancel settings and close window"));
  PFD_pushButton_close->setToolTip(PFD_pushButton_close->statusTip());
  
  //label DIR
  PFD_label_path->setText(tr("Path:"));
  PFD_label_path->setStatusTip(tr("Set a path for file storage"));
  PFD_label_path->setToolTip(PFD_label_path->statusTip());
  //line edit DIR
  PFD_lineEdit_path->setStatusTip(PFD_label_path->statusTip());
  PFD_lineEdit_path->setToolTip(PFD_lineEdit_path->statusTip());
  //push button DIR
  PFD_pushButton_path->setStatusTip(PFD_label_path->statusTip());
  PFD_pushButton_path->setToolTip(PFD_pushButton_path->statusTip());
  
  //label file name
  PFD_label_fileName->setText(tr("Project name:"));
  PFD_label_fileName->setStatusTip(tr("Insert a project name"));
  PFD_label_fileName->setToolTip(PFD_label_fileName->statusTip());
  //combobox compatibility
  PFD_lineEdit_fileName->setStatusTip(PFD_label_fileName->statusTip());
  PFD_lineEdit_fileName->setToolTip(PFD_lineEdit_fileName->statusTip());  
  
  //messages
  msg_head_pathdialog =tr("Open Directory");
}
