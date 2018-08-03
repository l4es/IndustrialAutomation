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

#include "fncdialog.h"

FncDialog::FncDialog(QWidget *parent)
    : QDialog(parent) 
{ //! creating function

  setWindowFlags(windowFlags()^Qt::WindowContextHelpButtonHint);
  
  setupUi(this);
  retranslateUi();
   
  connect(Fnc_treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(setokButtonState()));
  connect(Fnc_treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(resetUserDefinedType()));
  connect(Fnc_lineEdit_userDefinedType, SIGNAL(textEdited(const QString)), this, SLOT(resetTreeWidget(const QString)));
  connect(Fnc_okButton, SIGNAL(clicked()), this, SLOT(okButtonState())); 
}


FncDialog::~FncDialog()
{ //! virtual destructor
}


void FncDialog::okButtonState()
{ //! return okButton state

  if (Fnc_okButton->isEnabled())
  { emit accept(); close(); }
}


void FncDialog::setokButtonState()
{ //! set ok button by a tree item selection

  if (((Fnc_treeWidget->indexOfTopLevelItem(Fnc_treeWidget->currentItem()) == -1)  && (Fnc_treeWidget->hasFocus())) || (!Fnc_lineEdit_userDefinedType->text().isEmpty())) 
    Fnc_okButton->setEnabled(true);
  else
    Fnc_okButton->setDisabled(true);
}


void FncDialog::resetTreeWidget(const QString &text)
{ //! reset tree widget if the text of the user defined line edit was changed
  //! \param &text is only used to signal/slot handling 

  Fnc_treeWidget->clearSelection();
  setokButtonState();
}


void FncDialog::resetUserDefinedType()
{ //! reset user defined type input by a tree item selection

  if ((Fnc_treeWidget->indexOfTopLevelItem(Fnc_treeWidget->currentItem()) == -1) && (Fnc_treeWidget->hasFocus())) 
    Fnc_lineEdit_userDefinedType->clear();
}


QString FncDialog::currentName()
{ //! get actual item with ": " add on as function type string

  if (Fnc_lineEdit_userDefinedType->text().isEmpty())
    return((": " + this->Fnc_treeWidget->currentItem()->text(0)));
  else
    return((": " + this->Fnc_lineEdit_userDefinedType->text()));
}


void FncDialog::retranslateUi()
{ //! retranslate Ui

  this->setWindowTitle(tr("Function Type Selection")); 
  
  //ok Button
  Fnc_okButton->setText(tr("Ok"));
  Fnc_okButton->setStatusTip(tr("Accept selection and close window"));
  Fnc_okButton->setToolTip(Fnc_okButton->statusTip());
  
  //cancel Button
  Fnc_cancelButton->setText(tr("Cancel"));
  Fnc_cancelButton->setStatusTip(tr("Cancel selection and close window"));
  Fnc_cancelButton->setToolTip(Fnc_cancelButton->statusTip());
 
  //label user defined type
  Fnc_label_userDefinedType->setText(tr("User defined Type:"));
  Fnc_label_userDefinedType->setStatusTip(tr("Insert a user defined function return type"));
  Fnc_label_userDefinedType->setToolTip(Fnc_cancelButton->statusTip());
 
  //line edit user defined type
  Fnc_lineEdit_userDefinedType->setStatusTip(Fnc_cancelButton->statusTip());
  Fnc_lineEdit_userDefinedType->setToolTip(Fnc_cancelButton->statusTip());
}
