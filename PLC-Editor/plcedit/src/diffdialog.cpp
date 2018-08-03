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

#include "diffdialog.h"
#include "pagectrl.h"

#include "beediff/QBtWorkspace.h"
#include "beediff/QBtShared.h"
#include "beediff/QBtConfig.h"
#include "beediff/QBtConfigDialog.h"
#include "beediff/QBtEventsController.h"
#include "beediff/QBtSettings.h"


#include <QAction>
#include <QLabel>
#include <QMessageBox>


DiffDialog::DiffDialog(QWidget *parent, PageCtrl *pageCtrl)
    : QDialog(parent), m_PageCtrl(pageCtrl)
{ //! contructor
	
  this->setWindowTitle("Diff");
  this->setWindowFlags(Qt::Window);
  this->resize(640, 600);

  //create layout
  QHBoxLayout *statusLayout = new QHBoxLayout;
  QVBoxLayout *mainLayout = new QVBoxLayout;

  //create workspace
  m_workspace = new QBtWorkspace(this);

  retranslateUi();
  
  //create status labels
  st_total = new QLabel(msg_st_total);
  st_total->setAlignment(Qt::AlignHCenter);
  st_total->setMinimumSize(40,10);
  st_total->setFrameShape(QFrame::StyledPanel);
  st_total->setFrameShadow(QFrame::Sunken);
  
  st_chg = new QLabel(msg_st_chg);
  st_chg->setAlignment(Qt::AlignHCenter);
  st_chg->setMinimumSize(40,10);
  st_chg->setFrameShape(QFrame::StyledPanel);
  st_chg->setFrameShadow(QFrame::Sunken);
  
  st_add = new QLabel(msg_st_add);
  st_add->setAlignment(Qt::AlignHCenter);
  st_add->setMinimumSize(40,10);
  st_add->setFrameShape(QFrame::StyledPanel);
  st_add->setFrameShadow(QFrame::Sunken);

  st_del = new QLabel(msg_st_del);
  st_del->setAlignment(Qt::AlignHCenter);
  st_del->setMinimumSize(40,10);
  st_del->setFrameShape(QFrame::StyledPanel);
  st_del->setFrameShadow(QFrame::Sunken);

  statusLayout->addStretch(0);
  statusLayout->addWidget(st_total);
  statusLayout->addWidget(st_chg);
  statusLayout->addWidget(st_add);
  statusLayout->addWidget(st_del);
  statusLayout->addSpacing(10);
  statusLayout->setSpacing(2);
  
  mainLayout->addWidget(m_workspace);
  mainLayout->addLayout(statusLayout);
  mainLayout->setContentsMargins(0,0,0,10);
  mainLayout->setSpacing(0);
  this->setLayout(mainLayout);

  stat_total(0);
  stat_chg(0);
  stat_add(0);
  stat_del(0);

  connect(m_workspace, SIGNAL(stat_total(int)),	this, SLOT(stat_total(int)));
  connect(m_workspace, SIGNAL(stat_chg(int)), 	this, SLOT(stat_chg(int)));
  connect(m_workspace, SIGNAL(stat_add(int)), 	this, SLOT(stat_add(int)));
  connect(m_workspace, SIGNAL(stat_del(int)), 	this, SLOT(stat_del(int)));

}


DiffDialog::~DiffDialog()
{  //! virtaul destructor
}


//=============================================================================
// - - - public SLOTs - - -
void DiffDialog::readLeftFile(QString path)
{ //! read left file

  configureBrowserA();
  m_workspace->lft_read_file(path);
}


void DiffDialog::readRightFile(QString path)
{ //! read right file

  configureBrowserB();
  m_workspace->rgt_read_file(path);
}


void DiffDialog::retranslateUi()
{ //! retranslate Ui

  msg_st_total	= tr("Total:");
  msg_st_chg	= tr("Changed:");
  msg_st_add	= tr("Added:");
  msg_st_del 	= tr("Deleted:");
}


void DiffDialog::selectLeftFile()
{ //! call file open dialog of workspace

  configureBrowserA();
  m_workspace->lft_file_selection();
}


void DiffDialog::selectRightFile()
{ //! call file open dialog of workspace

  configureBrowserB();
  m_workspace->rgt_file_selection();
}



//=============================================================================
// - - - private SLOTs - - -
void DiffDialog::configureBrowserA()
{ //! configure brower A

  m_workspace->setFontAndTabWidthBrowserA(m_PageCtrl->font, m_PageCtrl->currentTabStopWidth());
  m_workspace->clearBrowserA();
}


void DiffDialog::configureBrowserB()
{ //! configure brower B

  m_workspace->setFontAndTabWidthBrowserB(m_PageCtrl->font, m_PageCtrl->currentTabStopWidth());
  m_workspace->clearBrowserB();
}


void DiffDialog::stat_total(int value)
{ //! set text total number of differences
	
  st_total->setText(" " + msg_st_total + " " + (QString::number(value))+ " ");
}


void DiffDialog::stat_chg(int value)
{ //! set text number of changes
	
  st_chg->setText(" " + msg_st_chg + " " + (QString::number(value))+ " ");
}


void DiffDialog::stat_add(int value)
{ //! set text number of added items
	
  st_add->setText(" " + msg_st_add + " " + (QString::number(value))+ " ");
}


void DiffDialog::stat_del(int value)
{ //! set text number of deleted items
	
  st_del->setText(" " + msg_st_del + " " + (QString::number(value))+ " ");
}
