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

#include "fileviewdialog.h"
#include "pagectrl.h"
#include  "importexport.h"


FileViewDialog::FileViewDialog(QWidget *parent, PageCtrl *pageCtrl, ImportExport *importExport) 
	: QDialog(parent), m_PageCtrl(pageCtrl), m_ImportExport(importExport)  //m_ImportExport as member VAR of PrefDialog
{ //! creating function
  //! \param ImportExport class
  
  setWindowFlags(windowFlags()^Qt::WindowContextHelpButtonHint);
    
  setupUi(this);
  retranslateUi();
    
  //setup dialog comboBox with -1
  setFileExt(-1);
  
  //diverse connections
  connect(FileView_comboBox_compatibility, SIGNAL(currentIndexChanged(int)), this, SLOT(compatibilityIndexChanged(int)));	//notice comboBox index changed 
}


FileViewDialog::~FileViewDialog()
{ //! virtual destructor
}


void FileViewDialog::setFileExt(int Ext)
{ //! set file extension

  QString ExtStr = m_PageCtrl->strFromDefinedPOUExtListAt(Ext);
  
  FileView_comboBox_compatibility->setCurrentIndex(FileView_comboBox_compatibility->findText(ExtStr, Qt::MatchContains));
  fillTextEdit(Ext); //needed, if SLOT was called external the connection above will not exec.
}


void FileViewDialog::compatibilityIndexChanged(int index)
{ //! handle compatibility index changed signal and call fillTextEdit with correct extension

  QString tmpStr="";
  int i=0;
  
  for (i; i < m_PageCtrl->definedPOUExtList.size(); i++)
  {
    tmpStr = FileView_comboBox_compatibility->currentText();
    if (tmpStr.contains(m_PageCtrl->definedPOUExtList.at(i)))
      break;
  }
  fillTextEdit(i); //call fill textedit
}


void FileViewDialog::fillTextEdit(int Ext)
{ //! fill text edit with current page content and additional data, depending on current file extension
  QStringList *tmpList = new QStringList();
  QString Str="";
  
  //call savePrepare fcn.
  m_ImportExport->SavePrepare(m_PageCtrl->currentTabIndex(), m_PageCtrl->currentFileName(), tmpList, Ext, m_ImportExport->conversionOption, 0);
  //clear FileView_textEdit_File
  FileView_textEdit_File->clear();

  //fill FileView_textEdit_File
  FileView_textEdit_File->setPlainText(tmpList->join("\n"));

  //delete tmpList
  delete tmpList;
}


void FileViewDialog::retranslateUi()
{ //! retranslate Ui

  this->setWindowTitle(tr("File View")); 
  
  //label compatibility/file extension
  FileView_label_compatibility->setText(tr("Compatibility:"));
  FileView_label_compatibility->setStatusTip(tr("Select an extension to show complete file content"));
  FileView_label_compatibility->setToolTip(FileView_label_compatibility->statusTip());
  //combobox compatibility/file extension
  FileView_comboBox_compatibility->setItemText(0, tr("plain POU (.txt)"));
  FileView_comboBox_compatibility->setItemText(1, tr("CoDeSys (.EXP)"));
  FileView_comboBox_compatibility->setItemText(2, tr("MELSOFT (.asc)"));
  FileView_comboBox_compatibility->setItemText(3, tr("MULTIPROG IL (.IL)"));
  FileView_comboBox_compatibility->setItemText(4, tr("MULTIPROG ST (.ST)"));
  FileView_comboBox_compatibility->setItemText(5, tr("STEP 7 AWL (.awl)"));
  FileView_comboBox_compatibility->setItemText(6, tr("STEP 7 SCL (.scl)"));
  FileView_comboBox_compatibility->setItemText(7, tr("Sucosoft S40 (.poe)"));
  FileView_comboBox_compatibility->setStatusTip(FileView_label_compatibility->statusTip());
  FileView_comboBox_compatibility->setToolTip(FileView_comboBox_compatibility->statusTip());

}
