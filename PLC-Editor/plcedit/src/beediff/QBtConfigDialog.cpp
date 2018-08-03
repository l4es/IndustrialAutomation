/********************************************************************
 * Copyright (C) Piotr Pszczolkowski
 *-------------------------------------------------------------------
 * This file is part of Beesoft Differ.
 *
 * Beesoft Differ is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * Beesoft Differ is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Beesoft Differ; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 *-------------------------------------------------------------------
 * Project      : Beesoft Differ
 * File         : QBtConfigDialog.cpp
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 21.02.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include "QBtConfigDialog.h"
#include "QBtConfigTextViewer.h"
#include "QBtConfigDiffProcess.h"
#include "QBtConfigOthers.h"
#include <QTabBar>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

/*------- local constants:
-------------------------------------------------------------------*/
const char* const QBtConfigDialog::CAPTION      = QT_TR_NOOP( "Configure" );
const char* const QBtConfigDialog::APPLY        = QT_TR_NOOP( "&Apply"    );
const char* const QBtConfigDialog::EXIT         = QT_TR_NOOP( "E&xit"   );
const char* const QBtConfigDialog::TEXT_VIEWER  = QT_TR_NOOP( "Text viewer" );
const char* const QBtConfigDialog::DIFF_PROCESS = QT_TR_NOOP( "Diff process" );
const char* const QBtConfigDialog::OTHERS       = QT_TR_NOOP( "Others" );


//*******************************************************************
// QBtConfigDialog                                       CONSTRUCTOR
//*******************************************************************
QBtConfigDialog::QBtConfigDialog( QWidget* const in_parent )
: QDialog        ( in_parent )
, tbar_          ( new QTabBar )
, wstack_        ( new QStackedWidget )
, apply_         ( new QPushButton( tr( APPLY ) ) )
, exit_          ( new QPushButton( tr( EXIT ) ) )
, tv_config_     ( new QBtConfigTextViewer )
, dp_config_     ( new QBtConfigDiffProcess )
, others_config_ ( new QBtConfigOthers )
{
   setWindowTitle( tr( CAPTION ) );

   tbar_->insertTab( TEXT_VIEWER_IDX, tr( TEXT_VIEWER ) );
   tbar_->insertTab( DIFF_PROCESS_IDX, tr( DIFF_PROCESS ) );
   tbar_->insertTab( OTHERS_IDX, tr( OTHERS ) );

   wstack_->insertWidget( TEXT_VIEWER_IDX, tv_config_ );
   wstack_->insertWidget( DIFF_PROCESS_IDX, dp_config_ );
   wstack_->insertWidget( OTHERS_IDX, others_config_ );

   QHBoxLayout* const btn_layout = new QHBoxLayout;
   btn_layout->addStretch();
   btn_layout->addWidget( exit_ );
   btn_layout->addWidget( apply_ );

   QVBoxLayout* const main_layout = new QVBoxLayout;
   main_layout->addWidget( tbar_ );
   main_layout->addWidget( wstack_ );
   main_layout->addLayout( btn_layout );
   setLayout( main_layout );

   connect( exit_  , SIGNAL( clicked() ),
            this   , SLOT  ( reject() ) );
   connect( apply_ , SIGNAL( clicked() ),
            this   , SLOT  ( apply() ) );
   connect( tbar_  , SIGNAL( currentChanged(int) ),
            wstack_, SLOT  ( setCurrentIndex(int) ) );
}
// end of QBtConfigDialog

//*******************************************************************
// apply                                                PRIVATE slot
//*******************************************************************
void QBtConfigDialog::apply()
{
   tv_config_->apply();
   dp_config_->apply();
   others_config_->apply();
}
// end of apply
