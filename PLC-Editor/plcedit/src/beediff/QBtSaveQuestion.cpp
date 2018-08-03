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
 * File         : QBtSaveQuestion.h
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 10.03.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include "QBtSaveQuestion.h"
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QHBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

/*------- constants:
-------------------------------------------------------------------*/
const char* const QBtSaveQuestion::CAPTION  = QT_TR_NOOP( "Saving" );
const char* const QBtSaveQuestion::MESSAGE  = QT_TR_NOOP( "You made changes. Would you like to save them?" );
const char* const QBtSaveQuestion::SAVE     = QT_TR_NOOP( "&Save" );
const char* const QBtSaveQuestion::CANCEL   = QT_TR_NOOP( "&Cancel" );
const char* const QBtSaveQuestion::EXIT     = QT_TR_NOOP( "&E&xit" );
const char* const QBtSaveQuestion::SAVE_LFT = QT_TR_NOOP( "Save &left" );
const char* const QBtSaveQuestion::SAVE_RGT = QT_TR_NOOP( "Save &right" );


//*******************************************************************
// QBtSaveQuestion                                       CONSTRUCTOR
//*******************************************************************
QBtSaveQuestion::QBtSaveQuestion( const bool in_lft, const bool in_rgt, QWidget* const in_parent )
: QDialog   ( in_parent )
, save_     ( new QPushButton( tr( SAVE     ) ) )
, cancel_   ( new QPushButton( tr( CANCEL   ) ) )
, exit_     ( new QPushButton( tr( EXIT     ) ) )
, save_lft_ ( new QPushButton( tr( SAVE_LFT ) ) )
, save_rgt_ ( new QPushButton( tr( SAVE_RGT ) ) )
{
   setWindowTitle( tr( CAPTION ) );
   
   QHBoxLayout* const info_layout = new QHBoxLayout;
   QLabel* const icon_label = new QLabel();
   icon_label->setPixmap( QPixmap( ":/img/question.png" ) );
   QLabel* const msg_label = new QLabel( tr( MESSAGE ) );
   info_layout->addWidget( icon_label );
   info_layout->addWidget( msg_label );

   QGridLayout* const btn_layout = new QGridLayout;
   btn_layout->addWidget( save_lft_, 0, 1 );
   btn_layout->addWidget( save_rgt_, 0, 2 );
   btn_layout->addWidget( save_    , 0, 3 );
   btn_layout->addWidget( exit_    , 1, 2 );
   btn_layout->addWidget( cancel_  , 1, 3 );
   btn_layout->setColumnStretch( 0, 100 );
   
   QGridLayout* const main_layout = new QGridLayout;
   main_layout->addLayout( info_layout, 0, 0 );
   main_layout->addLayout( btn_layout , 1, 0 );
   setLayout( main_layout );

   if( !( in_lft & in_rgt ) ) {
      save_lft_->setVisible( false );
      save_rgt_->setVisible( false );
   }

   connect( save_    , SIGNAL( clicked() ), this, SLOT( save    () ) );
   connect( cancel_  , SIGNAL( clicked() ), this, SLOT( cancel  () ) );
   connect( exit_    , SIGNAL( clicked() ), this, SLOT( exit    () ) );
   connect( save_lft_, SIGNAL( clicked() ), this, SLOT( save_lft() ) );
   connect( save_rgt_, SIGNAL( clicked() ), this, SLOT( save_rgt() ) );
}
// end of QBtSaveQuestion

//*******************************************************************
// cancel                                               PRIVATE slot
//*******************************************************************
void QBtSaveQuestion::cancel()
{
   done( 0 );
}
// end of cancel

//*******************************************************************
// save_lft                                             PRIVATE slot
//*******************************************************************
void QBtSaveQuestion::save_lft()
{
   done( 1 );
}
// end of save_lft

//*******************************************************************
// save_rgt                                             PRIVATE slot
//*******************************************************************
void QBtSaveQuestion::save_rgt()
{
   done( 2 );
}
// end of save_rgt

//*******************************************************************
// save                                                 PRIVATE slot
//*******************************************************************
void QBtSaveQuestion::save()
{
   done( 3 );
}
// end of save

//*******************************************************************
// exit                                                 PRIVATE slot
//*******************************************************************
void QBtSaveQuestion::exit()
{
   done( 4 );
}
// end of exit
