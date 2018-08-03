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
 * File         : QBtConfig.cpp
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 19.02.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include "QBtConfig.h"
#include "QBtSettings.h"
#include <QApplication>


/*------- public constants:
-------------------------------------------------------------------*/
const QString QBtConfig::MAIN_WINDOW_GROUP      = "main_window/";
const QString QBtConfig::POS_KEY                = "position";
const QString QBtConfig::SIZE_KEY               = "size";
const QString QBtConfig::STATE_KEY              = "state";
const QString QBtConfig::FPATH_HISTORY_SIZE_KEY = "fpath_history_size";
const QString QBtConfig::LFT_FPATH_HISTORY_KEY  = "lft_fpath_history";
const QString QBtConfig::RGT_FPATH_HISTORY_KEY  = "rgt_fpath_history";
//-------------------------------------------------------------------
const QString QBtConfig::BROWSER_GROUP       = "browser/";
const QString QBtConfig::TEXT_FONT_KEY       = "font";
const QString QBtConfig::TEXT_COLOR_KEY      = "font_color";
const QString QBtConfig::TEXT_BACKGROUND_KEY = "color";
const QString QBtConfig::PART_FONT_KEY       = "part_font";
const QString QBtConfig::PART_COLOR_KEY      = "part_font_color";
const QString QBtConfig::PART_BACKGROUND_KEY = "part_bkg_color";
const QString QBtConfig::CHG_BACKGROUND_KEY  = "chg_color";
const QString QBtConfig::ADD_BACKGROUND_KEY  = "add_color";
const QString QBtConfig::DEL_BACKGROUND_KEY  = "del_color";
const QString QBtConfig::SYNC_KEY            = "sync";
const QString QBtConfig::NUMERATION_KEY      = "numeration";
const QString QBtConfig::BEZIER_KEY          = "use_bezier";
const QString QBtConfig::MARK_VCENTER_KEY    = "oper_mark_vcenter";
const QString QBtConfig::UTF8_KEY            = "use_utf8";
//-------------------------------------------------------------------
const QString QBtConfig::DIFF_GROUP             = "diff/";
const QString QBtConfig::CASE_KEY               = "case";
const QString QBtConfig::TAB_EXP_KEY            = "tab_exp";
const QString QBtConfig::WSPACE_AMOUNT_KEY      = "wspace_amount";
const QString QBtConfig::WSPACE_ALL_KEY         = "wspace_all";
const QString QBtConfig::BLANK_LINES_KEY        = "blank_lines";
const QString QBtConfig::AS_TEXT_KEY            = "as_text";

/*------- private constants:
-------------------------------------------------------------------*/
const int     QBtConfig::DEFAULT_FPATH_HISTORY_SIZE = 5;
const QColor  QBtConfig::DEFAULT_BKG_COLOR   = qRgb( 255, 255, 255 );
const QColor  QBtConfig::DEFAULT_FNT_COLOR   = qRgb(   0,   0,   0 );
const QString QBtConfig::DEFAULT_FONT_STRING = "Monospace,9,-1,5,50,0,0,0,0,0";
const QColor  QBtConfig::DEFAULT_DEL_COLOR   = qRgb( 255, 146, 157 );
const QColor  QBtConfig::DEFAULT_ADD_COLOR   = qRgb( 190, 208, 255 );
const QColor  QBtConfig::DEFAULT_CHG_COLOR   = qRgb( 232, 232, 182 );


//*******************************************************************
// QBtConfig                                     CONSTRUCTOR private
//*******************************************************************
QBtConfig::QBtConfig() : QObject()
, text_background_brush_( QBrush( DEFAULT_BKG_COLOR ) )
, part_background_brush_( QBrush( DEFAULT_CHG_COLOR ) )
, chg_background_brush_ ( QBrush( DEFAULT_CHG_COLOR ) )
, add_background_brush_ ( QBrush( DEFAULT_ADD_COLOR ) )
, del_background_brush_ ( QBrush( DEFAULT_DEL_COLOR ) )
{
   setParent( qApp );
   read_data();
}
// end of QBtConfig

//*******************************************************************
// instance                                            PUBLIC static
//*******************************************************************
QBtConfig* const QBtConfig::instance()
{
   static QBtConfig* ptr = 0;
   if( !ptr ) ptr = new QBtConfig;
   return ptr;
}
// end of instance

//*******************************************************************
// update                                                     PUBLIC
//*******************************************************************
void QBtConfig::update()
{
   save_data();
   read_data();
}
// end of update

//*******************************************************************
// save_data                                                  PUBLIC
//*******************************************************************
void QBtConfig::save_data()
{
   QBtSettings stt;
   stt.save( MAIN_WINDOW_GROUP + FPATH_HISTORY_SIZE_KEY, fpath_history_size_ );
   //------------------------------------------------------
   stt.save( BROWSER_GROUP + TEXT_FONT_KEY      , text_font_.toString  () );
   stt.save( BROWSER_GROUP + TEXT_COLOR_KEY     , text_color_.name     () );
   stt.save( BROWSER_GROUP + TEXT_BACKGROUND_KEY, text_background_.name() );
   stt.save( BROWSER_GROUP + PART_FONT_KEY      , part_font_.toString  () );
   stt.save( BROWSER_GROUP + PART_COLOR_KEY     , part_color_.name     () );
   stt.save( BROWSER_GROUP + PART_BACKGROUND_KEY, part_background_.name() );
   stt.save( BROWSER_GROUP + CHG_BACKGROUND_KEY , chg_background_.name () );
   stt.save( BROWSER_GROUP + ADD_BACKGROUND_KEY , add_background_.name () );
   stt.save( BROWSER_GROUP + DEL_BACKGROUND_KEY , del_background_.name () );
   
   stt.save( BROWSER_GROUP + SYNC_KEY        , sync_         );
   stt.save( BROWSER_GROUP + NUMERATION_KEY  , numeration_   );
   stt.save( BROWSER_GROUP + BEZIER_KEY      , bezier_       );
   stt.save( BROWSER_GROUP + MARK_VCENTER_KEY, mark_vcenter_ );
   stt.save( BROWSER_GROUP + UTF8_KEY        , utf8_         );
   //------------------------------------------------------
   stt.save( DIFF_GROUP + CASE_KEY         , diff_case_          );
   stt.save( DIFF_GROUP + TAB_EXP_KEY      , diff_tab_exp_       );
   stt.save( DIFF_GROUP + WSPACE_AMOUNT_KEY, diff_wspace_amount_ );
   stt.save( DIFF_GROUP + WSPACE_ALL_KEY   , diff_wspace_all_    );
   stt.save( DIFF_GROUP + BLANK_LINES_KEY  , diff_blank_lines_   );
   stt.save( DIFF_GROUP + AS_TEXT_KEY      , diff_as_text_       );
}
// end of save_data

//*******************************************************************
// read_data                                                 PRIVATE
//*******************************************************************
void QBtConfig::read_data()
{
   QBtSettings stt;
   QVariant data;

   fpath_history_size_ = DEFAULT_FPATH_HISTORY_SIZE;
   if( stt.read( MAIN_WINDOW_GROUP + FPATH_HISTORY_SIZE_KEY, data ) ) {
      fpath_history_size_ = data.toInt();
   }
   
   //------------------------------------------------------
   // chg
   chg_background( DEFAULT_CHG_COLOR );
   if( stt.read( BROWSER_GROUP + CHG_BACKGROUND_KEY, data ) ) {
      chg_background( data.toString() );
   }
   // add
   add_background( DEFAULT_ADD_COLOR );
   if( stt.read( BROWSER_GROUP + ADD_BACKGROUND_KEY, data ) ) {
      add_background( data.toString() );
   }
   // del
   del_background( DEFAULT_DEL_COLOR );
   if( stt.read( BROWSER_GROUP + DEL_BACKGROUND_KEY, data ) ) {
      del_background( data.toString() );
   }
   // text font
   text_font_.fromString( DEFAULT_FONT_STRING );
   if( stt.read( BROWSER_GROUP + TEXT_FONT_KEY, data ) ) {
      text_font_.fromString( data.toString() );
   }
   // text color
   text_color_ = DEFAULT_FNT_COLOR;
   if( stt.read( BROWSER_GROUP + TEXT_COLOR_KEY, data ) ) {
      text_color_.setNamedColor( data.toString() );
   }
   // text background
   text_background( DEFAULT_BKG_COLOR );
   if( stt.read( BROWSER_GROUP + TEXT_BACKGROUND_KEY, data ) ) {
      text_background( data.toString() );
   }
   // part font
   part_font_ = text_font_;
   if( stt.read( BROWSER_GROUP + PART_FONT_KEY, data ) ) {
      part_font_.fromString( data.toString() );
   }
   // part color
   part_color( text_color_ );
   if( stt.read( BROWSER_GROUP + PART_COLOR_KEY, data ) ) {
      part_color( data.toString() );
   }
   // part background
   part_background( chg_background_.lighter( 120 ) );
   if( stt.read( BROWSER_GROUP + PART_BACKGROUND_KEY, data ) ) {
      part_background( data.toString() );
   }

   
   sync_ = true; //remarks for PLCEdit
   /*if( stt.read( BROWSER_GROUP + SYNC_KEY, data ) ) {
      sync_ = data.toBool();
   }*/
   numeration_ = true; //remarks for PLCEdit
   //remarks for PLCEdit
   /*if( stt.read( BROWSER_GROUP + NUMERATION_KEY, data ) ) {
      numeration_ = data.toBool();
   }*/
   bezier_ = true; //remarks for PLCEdit
   /*if( stt.read( BROWSER_GROUP + BEZIER_KEY, data ) ) {
      bezier_ = data.toBool();
   }*/
   mark_vcenter_ = false;
   /*if( stt.read( BROWSER_GROUP + MARK_VCENTER_KEY, data ) ) {
      mark_vcenter_ = data.toBool();
   }*/
   utf8_ = false;
   if( stt.read( BROWSER_GROUP + UTF8_KEY, data ) ) {
      utf8_ = data.toBool();
   }
   

   // Paramtry fragmentow w zmienioniej linii.
   
   //------------------------------------------------------
   diff_case_          = 
   diff_tab_exp_       = 
   diff_wspace_amount_ = 
   diff_wspace_all_    =
   diff_blank_lines_   = 
   diff_as_text_       = false;
   
   if( stt.read( DIFF_GROUP + CASE_KEY, data ) ) {
      diff_case_ = data.toBool();
   }
   if( stt.read( DIFF_GROUP + TAB_EXP_KEY, data ) ) {
      diff_tab_exp_ = data.toBool();
   }
   if( stt.read( DIFF_GROUP + WSPACE_AMOUNT_KEY, data ) ) {
      diff_wspace_amount_ = data.toBool();
   }
   if( stt.read( DIFF_GROUP + WSPACE_ALL_KEY, data ) ) {
      diff_wspace_all_ = data.toBool();
   }
   if( stt.read( DIFF_GROUP + BLANK_LINES_KEY, data ) ) {
      diff_blank_lines_ = data.toBool();
   }
   if( stt.read( DIFF_GROUP + AS_TEXT_KEY, data ) ) {
      diff_as_text_ = data.toBool();
   }
}
// end of read_data
