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

#include "pouinfodialog.h"
#include "pagectrl.h"
#include "prototype.h"
#include "importexport.h"


POUInfoDialog::POUInfoDialog(QWidget *parent, PageCtrl *pageCtrl, Prototype *prototype, ImportExport *importExport)
    : QDialog(parent), m_PageCtrl(pageCtrl), m_Prototype(prototype), m_ImportExport(importExport) //member VARs of PrefDialog
{ //! creating function
  //! \param PageCtrl class, Prototype class, ImportExport class

  setWindowFlags(windowFlags()^Qt::WindowContextHelpButtonHint);
    
  setupUi(this);
  retranslateUi();
  
  //diverse connections
  connect(POUI_okButton, SIGNAL(clicked()), this, 		SLOT(getDataFromDialog()));	//call getDataFromDialog slot
}


POUInfoDialog::~POUInfoDialog()
{ //! virtual destructor
}


void POUInfoDialog::getDataFromDialog()
{ //! get dialog content by dectcting current file extension if 1 or 5/6 (Sucosoft or MULTIPROG)
  
  if((m_PageCtrl->currentFileExt() == 0) || (m_PageCtrl->currentFileExt() == 1)) 
	getSucosoftHeaderFromDialog();
	
  if((m_PageCtrl->currentFileExt() == 5) || (m_PageCtrl->currentFileExt() == 6)) 
  	getMULTIPROGDataFromDialog();

  m_PageCtrl->setPageModified(true);	//call page modified slot
}


void POUInfoDialog::getSucosoftHeaderFromDialog()
{ //! copy lineEdits & textEdit to current page
	
  int i=0;
  QDate date;
  QTime time;
  QString tmpStr="";
  QString SucosoftHeader = m_ImportExport->createSucosoftHeader(m_PageCtrl->currentSucosoftHeader());
  
  SucosoftHeader.remove(SucosoftHeader.indexOf("PROJECT = "), SucosoftHeader.length());

  SucosoftHeader.append("PROJECT = " + POUI_lineEdit_prj->text() + "\n");
  SucosoftHeader.append("OEMNAME = " + POUI_lineEdit_developer->text() + "\n");
  SucosoftHeader.append("OEMDATE = " + POUI_lineEdit_date->text() + "\n");
  SucosoftHeader.append("COMPANY = " + POUI_lineEdit_name->text() + "\n");
  SucosoftHeader.append("VERSION = " + POUI_lineEdit_version->text() + "\n");
  SucosoftHeader.append("SPS_TYPE = "+ POUI_lineEdit_plc->text() + "\n");
  
  tmpStr = POUI_textEdit_comment->toPlainText();
  QStringList list1 = tmpStr.split("\n");
  SucosoftHeader.append("POETXTSIZE = " + QString::number(list1.size()) + "\n");
  for (i  =0; i < list1.size(); i++)
    SucosoftHeader.append("POETXT = " + list1.value(i) + "\n");
  
  SucosoftHeader.append("$END_KM_HEADER"); 
  
  m_PageCtrl->setCurrentSucosoftHeader(SucosoftHeader);
  
  //set header comment if selected
  if (POUI_checkBox_Comment->isChecked())
  { //not ready now!
  	QString tmpStr = POUI_textEdit_comment->toPlainText();
  	m_PageCtrl->insertText("Ins", "\n(*" + tmpStr + "*)\n");
  }
}


void POUInfoDialog::getMULTIPROGDataFromDialog()
{ //! copy lineEdits & textEdit to current page

  //m_PageCtrl->setCurrentMULTUPROGWorkSheetName(POUI_lineEdit_prj->text());
  m_PageCtrl->setCurrentMULTIPROGDescription(POUI_textEdit_comment->toPlainText());
}


void POUInfoDialog::setDialog()
{ //! set dialog items by detecting current file extension if 1 or 5/6 (Moeller or KW-Software)
  /*! If file type is not plain POU, Moeller POU or KW-Software POU the input fields of the info section are disabled.*/
  /*! This function also calls Moeller header data if the file type has changed to any other, excluding KW-Software decription inputs. If the file type was changed from KW-Software to any other, the KW description data are there but not shown.*/
  
  setInterfaceItemsEnabled(); //clear and enanble/disable interface items
  
  if((m_PageCtrl->currentFileExt() == 5) || (m_PageCtrl->currentFileExt() == 6)) 
  	setDialogFromMULTIPROGData(); //call function for MULTIPROG POU
  else
    setDialogFromSucosoftHeader();//call Sucosoft header function (if the file type was changed, old header data are there!)
}


void POUInfoDialog::setDialogFromSucosoftHeader()
{ //! collect Sucosoft header and fill lineEdits & textEdit 
	
  QString SucosoftHeader = m_ImportExport->createSucosoftHeader(m_PageCtrl->currentSucosoftHeader());
  QString tmpStr;
  QStringList list1 = SucosoftHeader.split("\n");
  int i=0, POETXT_start=0, POETXT_end=0;
  
  //get PRJ/NAME/DATE/COMPANY/VERS/SPS
  foreach (QString tmpStr, list1) 
  {
    if (tmpStr.contains("PROJECT ="))	
  	{
      tmpStr = tmpStr.remove(0, tmpStr.indexOf("=")+2);
      POUI_lineEdit_prj->setText(tmpStr);
    }
    if (tmpStr.contains("OEMNAME ="))	
  	{
      tmpStr = tmpStr.remove(0, tmpStr.indexOf("=")+2);
      POUI_lineEdit_developer->setText(tmpStr);
    }
    if (tmpStr.contains("OEMDATE ="))	
  	{
      tmpStr = tmpStr.remove(0, tmpStr.indexOf("=")+2);
      POUI_lineEdit_date->setText(tmpStr);
    }
    if (tmpStr.contains("COMPANY ="))	
  	{
      tmpStr = tmpStr.remove(0, tmpStr.indexOf("=")+2);
      POUI_lineEdit_name->setText(tmpStr);
    }
    if (tmpStr.contains("VERSION ="))	
  	{
      tmpStr = tmpStr.remove(0, tmpStr.indexOf("=")+2);
      POUI_lineEdit_version->setText(tmpStr);
    }
    if (tmpStr.contains("SPS_TYPE ="))	
  	{
      tmpStr = tmpStr.remove(0, tmpStr.indexOf("=")+2);
      POUI_lineEdit_plc->setText(tmpStr);
    }
   }
  
  //set POUI_textEdit_comment
  POUI_textEdit_comment->clear();
  
  for (i=24; i<list1.size(); i++)
  { //get index of POETXT start and end points
  	tmpStr = list1.value(i);
  	if (tmpStr.contains("POETXTSIZE ="))	
      POETXT_start = i+1;
    if (tmpStr.contains("$END_KM_HEADER"))	
      POETXT_end = i;
  }
  for (i = POETXT_start; i < POETXT_end; i++)
  { //get single lines and strip 'POETXT = ' from it 
  	tmpStr = list1.value(i).simplified();
  	POUI_textEdit_comment->append(tmpStr.remove(0, 9));
  }
}


void POUInfoDialog::setDialogFromMULTIPROGData()
{ //! collect MULTIPROG description and fill lineEdits & textEdit 
	
  //POUI_lineEdit_prj->setText(m_PageCtrl->currentMULTIPROGWorkSheetName());
  POUI_textEdit_comment->append(m_PageCtrl->currentMULTIPROGDescription());
}


void POUInfoDialog::setInterfaceItemsEnabled()
{ //! set interface items enabled by detecting the file extension

  bool enable_full=false, enable_kw=false;

  if((m_PageCtrl->currentFileExt() == 0) || (m_PageCtrl->currentFileExt() == 1)) 
	enable_full=true; //full enable if plain POU or Moeller POU
	
  if((m_PageCtrl->currentFileExt() == 5) || (m_PageCtrl->currentFileExt() == 6)) 
  	enable_kw=true; //reduced enable if KW-Software POU
  
  POUI_lineEdit_prj->setEnabled(enable_full);
  POUI_lineEdit_prj->clear();
  POUI_lineEdit_version->setEnabled(enable_full);
  POUI_lineEdit_version->clear();
  POUI_lineEdit_name->setEnabled(enable_full);
  POUI_lineEdit_name->clear();
  POUI_lineEdit_plc->setEnabled(enable_full);
  POUI_lineEdit_plc->clear();
  POUI_lineEdit_developer->setEnabled(enable_full);
  POUI_lineEdit_developer->clear();
  POUI_lineEdit_date->setEnabled(enable_full);
  POUI_lineEdit_date->clear();
  POUI_textEdit_comment->setEnabled(enable_full || enable_kw);
  POUI_textEdit_comment->clear();
  POUI_checkBox_Comment->setEnabled(enable_full || enable_kw);
}


void POUInfoDialog::setPrototype()
{ //! set prototype of current page
	
  QStringList *tmpList = new QStringList();        	//create new StringList
  int i=0;
  
  m_Prototype->currentPrototype(tmpList);			//get curent prototype
  
  POUI_textEdit_Prototype->clear();
  for (i=0; i<tmpList->count(); i++)
    POUI_textEdit_Prototype->append(tmpList->value(i)); //needs to be simplified, becc. it contains '\n'
  
  delete tmpList;
}


void POUInfoDialog::retranslateUi()
{ //! retranslate Ui

  this->setWindowTitle(tr("POU Info")); 

  //ok Button
  POUI_okButton->setText(tr("Ok"));
  POUI_okButton->setStatusTip(tr("Accept inputs and close window"));
  POUI_okButton->setToolTip(POUI_okButton->statusTip());
  
  //cancel Button
  POUI_cancelButton->setText(tr("Cancel"));
  POUI_cancelButton->setStatusTip(tr("Cancel inputs and close window"));
  POUI_cancelButton->setToolTip(POUI_cancelButton->statusTip());

  //textedit pou info comment
  POUI_textEdit_comment->setStatusTip(tr("POU description"));
  POUI_textEdit_comment->setToolTip(POUI_textEdit_comment->statusTip());
  
  //checkbox pou info comment
  POUI_checkBox_Comment->setText(tr("copy edit field into instruction section"));
  POUI_checkBox_Comment->setStatusTip(tr("copy edit field into instruction section of current window"));
  POUI_checkBox_Comment->setToolTip(POUI_checkBox_Comment->statusTip());

  //label prj
  POUI_label_Prj->setText(tr("Project:"));
  POUI_label_Prj->setStatusTip(tr("Insert a project name"));
  POUI_label_Prj->setToolTip(POUI_label_Prj->statusTip());
  //lineedit prj
  POUI_lineEdit_prj->setStatusTip(POUI_label_Prj->statusTip());
  POUI_lineEdit_prj->setToolTip(POUI_lineEdit_prj->statusTip());

  //label version
  POUI_label_Version->setText(tr("Version:"));
  POUI_label_Version->setStatusTip(tr("Insert a version number"));
  POUI_label_Version->setToolTip(POUI_label_Version->statusTip());
  //lineedit version
  POUI_lineEdit_version->setStatusTip(POUI_label_Version->statusTip());
  POUI_lineEdit_version->setToolTip(POUI_lineEdit_version->statusTip());

  //label company name
  POUI_label_Name->setText(tr("Company:"));
  POUI_label_Name->setStatusTip(tr("Insert a company name"));
  POUI_label_Name->setToolTip(POUI_label_Name->statusTip());
  //lineedit company name
  POUI_lineEdit_name->setStatusTip(POUI_label_Name->statusTip());
  POUI_lineEdit_name->setToolTip(POUI_lineEdit_name->statusTip());

  //label plc
  POUI_label_PLC->setText(tr("PLC:"));
  POUI_label_PLC->setStatusTip(tr("Insert a PLC name/notice"));
  POUI_label_PLC->setToolTip(POUI_label_PLC->statusTip());
  //lineedit plc
  POUI_lineEdit_plc->setStatusTip(POUI_label_PLC->statusTip());
  POUI_lineEdit_plc->setToolTip(POUI_lineEdit_plc->statusTip());

  //label developer
  POUI_label_Developer->setText(tr("Developer:"));
  POUI_label_Developer->setStatusTip(tr("Insert a developer name"));
  POUI_label_Developer->setToolTip(POUI_label_Developer->statusTip());
  //lineedit developer
  POUI_lineEdit_developer->setStatusTip(POUI_label_Developer->statusTip());
  POUI_lineEdit_developer->setToolTip(POUI_lineEdit_developer->statusTip());

  //label date
  POUI_label_Date->setText(tr("Date:"));
  POUI_label_Date->setStatusTip(tr("Insert the date of release"));
  POUI_label_Date->setToolTip(POUI_label_Date->statusTip());
  //lineedit date
  POUI_lineEdit_date->setStatusTip(POUI_label_Date->statusTip());
  POUI_lineEdit_date->setToolTip(POUI_lineEdit_date->statusTip());
 
}
