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
 * File         : QBtConfigDiffProcess.h
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 21.02.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include "QBtConfigDiffProcess.h"
#include "QBtEventsController.h"
#include "QBtConfig.h"
#include <QCheckBox>
#include <QVBoxLayout>

/*------- constants:
-------------------------------------------------------------------*/
const char* const QBtConfigDiffProcess::IGNORE_CASE          = QT_TR_NOOP( "ignore case" );
const char* const QBtConfigDiffProcess::IGNORE_TAB_EXP       = QT_TR_NOOP( "ignore changes due to tab expansion" );
const char* const QBtConfigDiffProcess::IGNORE_WSPACE_AMOUNT = QT_TR_NOOP( "ignore changes in the amount of white space" );
const char* const QBtConfigDiffProcess::IGNORE_WSPACE_ALL    = QT_TR_NOOP( "ignore all white spaces" );
const char* const QBtConfigDiffProcess::IGNORE_BLANK_LINES   = QT_TR_NOOP( "ignore changes whose lines are all blank" );
const char* const QBtConfigDiffProcess::FILES_AS_TEXT        = QT_TR_NOOP( "treat files as text" );


//*******************************************************************
// QBtConfigDiffProcess                                   CONSTRUCTOR
//*******************************************************************
QBtConfigDiffProcess::QBtConfigDiffProcess( QWidget* const in_parent )
: QBiConfigWidget( in_parent )
, case_          ( new QCheckBox( tr( IGNORE_CASE          ) ) )
, tab_exp_       ( new QCheckBox( tr( IGNORE_TAB_EXP       ) ) )
, wspace_amount_ ( new QCheckBox( tr( IGNORE_WSPACE_AMOUNT ) ) )
, wspace_all_    ( new QCheckBox( tr( IGNORE_WSPACE_ALL    ) ) )
, blank_lines_   ( new QCheckBox( tr( IGNORE_BLANK_LINES   ) ) )
, as_text_       ( new QCheckBox( tr( FILES_AS_TEXT        ) ) )
{
   const QBtConfig* const cfg = QBtConfig::instance();
   case_         ->setChecked( cfg->diff_case()          );
   tab_exp_      ->setChecked( cfg->diff_tab_exp()       );
   wspace_amount_->setChecked( cfg->diff_wspace_amount() );
   wspace_all_   ->setChecked( cfg->diff_wspace_all()    );
   blank_lines_  ->setChecked( cfg->diff_blank_lines()   );
   as_text_      ->setChecked( cfg->diff_as_text()       );

   QVBoxLayout* const main_layout = new QVBoxLayout;
   main_layout->addWidget( case_ );
   main_layout->addWidget( tab_exp_ );
   main_layout->addWidget( wspace_amount_ );
   main_layout->addWidget( wspace_all_ );
   main_layout->addWidget( blank_lines_ );
   main_layout->addWidget( as_text_ );
   main_layout->addStretch();
   setLayout( main_layout );
}
// end of QBtConfigDiffProcess

//*******************************************************************
// apply                                            PUBLIC inherited
//*******************************************************************
void QBtConfigDiffProcess::apply()
{
   QBtConfig* const cfg = QBtConfig::instance();

   cfg->diff_case         ( case_         ->isChecked() );
   cfg->diff_tab_exp      ( tab_exp_      ->isChecked() );
   cfg->diff_wspace_amount( wspace_amount_->isChecked() );
   cfg->diff_wspace_all   ( wspace_all_   ->isChecked() );
   cfg->diff_blank_lines  ( blank_lines_  ->isChecked() );
   cfg->diff_as_text      ( as_text_      ->isChecked() );

   cfg->save_data();
   QBtEventsController::instance()->send_event( QBtEvent::DIFF_CFG_CHANGED );
}
// end of apply
