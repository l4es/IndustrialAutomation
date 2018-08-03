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

#include "batchdialog.h"


BatchDialog::BatchDialog(QDialog *parent) 
 : QDialog(parent)
{ //! creating function

  setWindowFlags(windowFlags()^Qt::WindowContextHelpButtonHint);

  setupUi(this);
  retranslateUi();

	
  //diverse connections
  connect(BD_pushButton_path, SIGNAL(clicked()), this, SLOT(PathDlg()));		//get Path
  
  //set Button sizes
  #ifdef Q_WS_MAC	
  //do nothing for Mac OS
  #else
    BD_pushButton_close->setMinimumHeight(25);
    BD_pushButton_close->setMaximumHeight(25);
    BD_pushButton_ok->setMinimumHeight(25);
    BD_pushButton_ok->setMaximumHeight(25);
  #endif  	

}


BatchDialog::~BatchDialog()
{ //! virtual destructor
}


void BatchDialog::presetPath(QString dir)
{ //! preset line edit "path"
  
  BD_lineEdit_path->setText(QFileInfo(dir.simplified()).absolutePath() + "/");
}
		

void BatchDialog::PathDlg()
{ //! get path from FileDialog and set into line edit "path"
 
  BD_lineEdit_path->setText(QFileDialog::getExistingDirectory(this, msg_head_pathdialog, "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks)  + "/");
}


void BatchDialog::retranslateUi()
{ //! retranslate Ui

  this->setWindowTitle(tr("Batch converter"));
	
  //ok Button
  BD_pushButton_ok->setText(tr("Ok"));
  BD_pushButton_ok->setStatusTip(tr("Accept inputs and close window"));
  BD_pushButton_ok->setToolTip(BD_pushButton_ok->statusTip());
  
  //close Button
  BD_pushButton_close->setText(tr("Close"));
  BD_pushButton_close->setStatusTip(tr("Cancel settings and close window"));
  BD_pushButton_close->setToolTip(BD_pushButton_close->statusTip());
  
  //label DIR
  BD_label_path->setText(tr("Result path:"));
  BD_label_path->setStatusTip(tr("Set a path for file storage"));
  BD_label_path->setToolTip(BD_label_path->statusTip());
  //line edit DIR
  BD_lineEdit_path->setStatusTip(BD_label_path->statusTip());
  BD_lineEdit_path->setToolTip(BD_lineEdit_path->statusTip());
  //push button DIR
  BD_pushButton_path->setStatusTip(BD_label_path->statusTip());
  BD_pushButton_path->setToolTip(BD_pushButton_path->statusTip());
  
  //label compatibility
  BD_label_compatibility->setText(tr("Compatibility:"));
  BD_label_compatibility->setStatusTip(tr("Select an extension for file conversion"));
  BD_label_compatibility->setToolTip(BD_label_compatibility->statusTip());
  //combobox compatibility
  BD_comboBox_compatibility->setStatusTip(BD_label_compatibility->statusTip());
  BD_comboBox_compatibility->setToolTip(BD_comboBox_compatibility->statusTip());  
  //combobox compatibility item texts
  BD_comboBox_compatibility->setItemText(0, tr("only copy"));
  BD_comboBox_compatibility->setItemText(1, tr("plain POU (.txt)"));
  BD_comboBox_compatibility->setItemText(2, tr("CoDeSys (.EXP)"));
  BD_comboBox_compatibility->setItemText(3, tr("MELSOFT (.asc)"));
  BD_comboBox_compatibility->setItemText(4, tr("MULTIPROG (.IL / .ST - keep language)"));
  BD_comboBox_compatibility->setItemText(5, tr("STEP 7 (.awl / .scl - keep language)"));
  BD_comboBox_compatibility->setItemText(6, tr("Sucosoft S40 (.poe)"));

  //label fb call conversion
  BD_label_callmodelconversion->setText(tr("Conversion of function block call model:"));
  BD_label_callmodelconversion->setStatusTip(tr("Select a conversion for the function block call model"));
  BD_label_callmodelconversion->setToolTip(BD_label_callmodelconversion->statusTip());
  //combobox conversion
  BD_comboBox_callmodelconversion->setStatusTip(BD_label_callmodelconversion->statusTip());
  BD_comboBox_callmodelconversion->setToolTip(BD_label_callmodelconversion->statusTip()); 
  //combobox conversion item texts
  BD_comboBox_callmodelconversion->setItemText(0, tr("all into selected compatibility call model"));
  BD_comboBox_callmodelconversion->setItemText(1, tr("do nothing"));
  BD_comboBox_callmodelconversion->setItemText(2, tr("outputs into simple IEC call model"));
  BD_comboBox_callmodelconversion->setItemText(3, tr("all into simple IEC call model"));
  
  //messages
  msg_head_pathdialog = tr("Open Directory");
}


QString BatchDialog::selectedCompatibilityStr()
{ //! get selected file compatibility from comboBox "compatibility" as string
	
  return(BD_comboBox_compatibility->currentText().simplified());
}


int BatchDialog::selectedCompatibility()
{ //! get selected file compatibility from comboBox "compatibility" as integer

  //get file extension as INT
  if (selectedCompatibilityStr().contains(".txt", Qt::CaseInsensitive))
    return(0); 
  if (selectedCompatibilityStr().contains(".poe", Qt::CaseInsensitive))
    return(1); 
  if (selectedCompatibilityStr().contains(".exp", Qt::CaseInsensitive))
    return(2); 
  if (selectedCompatibilityStr().contains("Siemens", Qt::CaseInsensitive))
    return(3); 
  if (selectedCompatibilityStr().contains("KW-Software", Qt::CaseInsensitive))
    return(5);  
  if (selectedCompatibilityStr().contains(".asc", Qt::CaseInsensitive))
    return(7);  
  if (selectedCompatibilityStr().contains("only copy", Qt::CaseInsensitive))
    return(99);
  
  return(0);
}


int BatchDialog::selectedConversion()
{ //! get selected function block conversion from comboBox "conversion" as integer

  return(BD_comboBox_callmodelconversion->currentIndex());
}


QString BatchDialog::selectedPath()
{ //! get selected path from line edit "path"

  return(QFileInfo(BD_lineEdit_path->text().simplified()).absolutePath() + "/");
}
