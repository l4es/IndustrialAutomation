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
 * File         : QBtConfigTextViewer.h
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 21.02.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include "QBtConfigTextViewer.h"
#include "QBtColorDemo.h"
#include "QBtConfig.h"
#include "QBtSettings.h"
#include "QBtEventsController.h"
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QColorDialog>
#include <QFontDialog>

/*------- constants:
-------------------------------------------------------------------*/
const char* const QBtConfigTextViewer::BROWSER    = QT_TR_NOOP( "Browser" );
const char* const QBtConfigTextViewer::BACKGROUND = QT_TR_NOOP( "&Background" );
const char* const QBtConfigTextViewer::FONT       = QT_TR_NOOP( "&Font" );
const char* const QBtConfigTextViewer::COLOR      = QT_TR_NOOP( "C&olor" );
const char* const QBtConfigTextViewer::HIGHLIGHT  = QT_TR_NOOP( "Highlighting" );
const char* const QBtConfigTextViewer::CHANGED    = QT_TR_NOOP( "Cha&nged:" );
const char* const QBtConfigTextViewer::ADDED      = QT_TR_NOOP( "Add&ed:" );
const char* const QBtConfigTextViewer::DELETED    = QT_TR_NOOP( "&Deleted:" );
const char* const QBtConfigTextViewer::LINE_CHG   = QT_TR_NOOP( "Changes inside a line" );


//*******************************************************************
// QBtConfigTextViewer                                   CONSTRUCTOR
//*******************************************************************
QBtConfigTextViewer::QBtConfigTextViewer( QWidget* const in_parent )
: QBiConfigWidget     ( in_parent )
, chg_demo_           ( new QBtColorDemo( true ) )
, chg_background_btn_ ( new QPushButton( QIcon( ":/img/colors.png" ), QString() ) )
, add_demo_           ( new QBtColorDemo( true ) )
, add_background_btn_ ( new QPushButton( QIcon( ":/img/colors.png" ), QString() ) )
, del_demo_           ( new QBtColorDemo( true ) )
, del_background_btn_ ( new QPushButton( QIcon( ":/img/colors.png" ), QString() ) )
, text_demo_          ( new QBtColorDemo( true ) )
, text_font_btn_      ( new QPushButton( QIcon( ":/img/fonts.png" ) , tr( FONT ) ) )
, text_color_btn_     ( new QPushButton( QIcon( ":/img/colors.png" ), tr( COLOR ) ) )
, text_background_btn_( new QPushButton( QIcon( ":/img/colors.png" ), tr( BACKGROUND ) ) )
, part_demo_          ( new QBtColorDemo( true ) )
, part_font_btn_      ( new QPushButton( QIcon( ":/img/fonts.png" ) , tr( FONT ) ) )
, part_color_btn_     ( new QPushButton( QIcon( ":/img/colors.png" ), tr( COLOR ) ) )
, part_background_btn_( new QPushButton( QIcon( ":/img/colors.png" ), tr( BACKGROUND ) ) )
{
   // Parametry wyswietlania niewybranych linii w browserze.
   QHBoxLayout* const text_btn_layout = new QHBoxLayout;
   text_btn_layout->addStretch();
   text_btn_layout->addWidget( text_background_btn_ );
   text_btn_layout->addWidget( text_font_btn_ );
   text_btn_layout->addWidget( text_color_btn_ );
   text_btn_layout->addStretch();
   QGroupBox* const text_gbox = new QGroupBox( tr( BROWSER ) );
   QVBoxLayout* const text_layout = new QVBoxLayout( text_gbox );
   text_layout->addWidget( text_demo_ );
   text_layout->addLayout( text_btn_layout );

   // Parametry wyswietalania zmian w obrebie linii.
   QHBoxLayout* const part_btn_layout = new QHBoxLayout;
   part_btn_layout->addStretch();
   part_btn_layout->addWidget( part_background_btn_ );
   part_btn_layout->addWidget( part_font_btn_ );
   part_btn_layout->addWidget( part_color_btn_ );
   part_btn_layout->addStretch();
   QGroupBox* const part_gbox = new QGroupBox( tr( LINE_CHG ) );
   QVBoxLayout* const part_layout = new QVBoxLayout( part_gbox );
   part_layout->addWidget( part_demo_ );
   part_layout->addLayout( part_btn_layout );

   // Parametry sposobu wyswietlania wybranych linii (wskazanych przez diff).   
   QLabel* const chg_lbl = new QLabel( tr( CHANGED ) );
   QLabel* const add_lbl = new QLabel( tr( ADDED ) );
   QLabel* const del_lbl = new QLabel( tr( DELETED ) );
   chg_lbl->setBuddy( chg_background_btn_ );
   add_lbl->setBuddy( add_background_btn_ );
   del_lbl->setBuddy( del_background_btn_ );
   QGroupBox* const hlight_gbox = new QGroupBox( tr( HIGHLIGHT ) );
   QGridLayout* const hlight_layout = new QGridLayout( hlight_gbox );
   hlight_layout->addWidget( chg_lbl            , 0, 0 );
   hlight_layout->addWidget( add_lbl            , 1, 0 );
   hlight_layout->addWidget( del_lbl            , 2, 0 );
   hlight_layout->addWidget( chg_demo_          , 0, 1 );
   hlight_layout->addWidget( add_demo_          , 1, 1 );
   hlight_layout->addWidget( del_demo_          , 2, 1 );
   hlight_layout->addWidget( chg_background_btn_, 0, 2 );
   hlight_layout->addWidget( add_background_btn_, 1, 2 );
   hlight_layout->addWidget( del_background_btn_, 2, 2 );
   
   QVBoxLayout* const main_layout = new QVBoxLayout;
   main_layout->addWidget( text_gbox );
   main_layout->addWidget( hlight_gbox );
   main_layout->addWidget( part_gbox );
   setLayout( main_layout );

   const QBtConfig* const cfg = QBtConfig::instance();
   
   text_demo_->background( cfg->text_background() );
   part_demo_->background( cfg->part_background() );
   chg_demo_ ->background( cfg->chg_background () );
   add_demo_ ->background( cfg->add_background () );
   del_demo_ ->background( cfg->del_background () );
   
   text_demo_ ->color( cfg->text_color() );
   part_demo_ ->color( cfg->part_color() );
   chg_demo_  ->color( cfg->text_color() );
   add_demo_  ->color( cfg->text_color() );
   del_demo_  ->color( cfg->text_color() );
   
   text_demo_ ->fonts( cfg->text_font() );
   part_demo_ ->fonts( cfg->part_font() );
   chg_demo_  ->fonts( cfg->text_font() );
   add_demo_  ->fonts( cfg->text_font() );
   del_demo_  ->fonts( cfg->text_font() );
   
   connect( text_font_btn_      , SIGNAL( clicked() ), this, SLOT( text_font      () ) );
   connect( text_color_btn_     , SIGNAL( clicked() ), this, SLOT( text_color     () ) );
   connect( text_background_btn_, SIGNAL( clicked() ), this, SLOT( text_background() ) );
   connect( part_font_btn_      , SIGNAL( clicked() ), this, SLOT( part_font      () ) );
   connect( part_color_btn_     , SIGNAL( clicked() ), this, SLOT( part_color      () ) );
   connect( part_background_btn_, SIGNAL( clicked() ), this, SLOT( part_background() ) );
   connect( chg_background_btn_ , SIGNAL( clicked() ), this, SLOT( chg_background () ) );
   connect( add_background_btn_ , SIGNAL( clicked() ), this, SLOT( add_background () ) );
   connect( del_background_btn_ , SIGNAL( clicked() ), this, SLOT( del_background () ) );
}
// end of QBtConfigTextViewer

//*******************************************************************
// text_font                                            PRIVATE slot
//*******************************************************************
void QBtConfigTextViewer::text_font()
{
   bool ok = bool();
   const QFont old_font = text_demo_->fonts();
   const QFont new_font = QFontDialog::getFont( &ok, old_font, this );
   if( ok ) {
      if( old_font != new_font ) {
         text_demo_->fonts( new_font );
         chg_demo_ ->fonts( new_font );
         add_demo_ ->fonts( new_font );
         del_demo_ ->fonts( new_font );
      }
   }
}
// end of text_font

//*******************************************************************
// text_color                                           PRIVATE slot
//*******************************************************************
void QBtConfigTextViewer::text_color()
{
   const QColor old_color = text_demo_->color();
   const QColor new_color = QColorDialog::getColor( old_color, this );
   if( new_color.isValid() ) {
      if( new_color != old_color ) {
         text_demo_->color( new_color );
         chg_demo_ ->color( new_color );
         add_demo_ ->color( new_color );
         del_demo_ ->color( new_color );
      }
   }
}
// end of text_color

//*******************************************************************
// text_background                                      PRIVATE slot
//*******************************************************************
void QBtConfigTextViewer::text_background()
{
   const QColor old_color = text_demo_->background();
   const QColor new_color = QColorDialog::getColor( old_color, this );
   if( new_color.isValid() ) {
      if( new_color != old_color ) {
         text_demo_->background( new_color );
      }
   }
}
// end of text_background

//*******************************************************************
// part_font                                            PRIVATE slot
//*******************************************************************
void QBtConfigTextViewer::part_font()
{
   bool ok = bool();
   const QFont old_font = part_demo_->fonts();
   const QFont new_font = QFontDialog::getFont( &ok, old_font, this );
   if( ok ) {
      if( old_font != new_font ) {
         part_demo_->fonts( new_font );
      }
   }
}
// end of part_font

//*******************************************************************
// part_color                                           PRIVATE slot
//*******************************************************************
void QBtConfigTextViewer::part_color()
{
   const QColor old_color = part_demo_->color();
   const QColor new_color = QColorDialog::getColor( old_color, this );
   if( new_color.isValid() ) {
      if( new_color != old_color ) {
         part_demo_->color( new_color );
      }
   }
}
// end of part_color

//*******************************************************************
// part_background                                      PRIVATE slot
//*******************************************************************
void QBtConfigTextViewer::part_background()
{
   const QColor old_color = part_demo_->background();
   const QColor new_color = QColorDialog::getColor( old_color, this );
   if( new_color.isValid() ) {
      if( new_color != old_color ) {
         part_demo_->background( new_color );
      }
   }
}
// end of text_background

//*******************************************************************
// chg_background                                       PRIVATE slot
//*******************************************************************
void QBtConfigTextViewer::chg_background()
{
   const QColor old_color = chg_demo_->background();
   const QColor new_color = QColorDialog::getColor( old_color, this );
   if( new_color.isValid() ) {
      if( new_color != old_color ) {
         chg_demo_->background( new_color );
      }
   }
}
// end of chg_background

//*******************************************************************
// add_background                                       PRIVATE slot
//*******************************************************************
void QBtConfigTextViewer::add_background()
{
   const QColor old_color = add_demo_->background();
   const QColor new_color = QColorDialog::getColor( old_color, this );
   if( new_color.isValid() ) {
      if( new_color != old_color ) {
         add_demo_->background( new_color );
      }
   }
}
// end of add_background

//*******************************************************************
// del_background                                       PRIVATE slot
//*******************************************************************
void QBtConfigTextViewer::del_background()
{
   const QColor old_color = del_demo_->background();
   const QColor new_color = QColorDialog::getColor( old_color, this );
   if( new_color.isValid() ) {
      if( new_color != old_color ) {
         del_demo_->background( new_color );
      }
   }
}
// end of del_background

//*******************************************************************
// apply                                            PUBLIC inherited
//*******************************************************************
void QBtConfigTextViewer::apply()
{
   QBtConfig* const cfg = QBtConfig::instance();
   
   cfg->text_font      ( text_demo_->font      () );
   cfg->text_color     ( text_demo_->color     () );
   cfg->text_background( text_demo_->background() );

   cfg->part_font      ( part_demo_->font      () );
   cfg->part_color     ( part_demo_->color     () );
   cfg->part_background ( part_demo_->background() );

   cfg->chg_background ( chg_demo_->background () );
   cfg->add_background ( add_demo_->background () );
   cfg->del_background ( del_demo_->background () );
   
   cfg->save_data();
   QBtEventsController::instance()->send_event( QBtEvent::BROWSER_CFG_CHANGED );
}
// end of apply
