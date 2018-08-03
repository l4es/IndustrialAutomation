#ifndef INCLUDED_QBtConfig_h
#define INCLUDED_QBtConfig_h
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
#include <QObject>
#include <QColor>
#include <QFont>
#include <QBrush>

/*------- class declaration:
-------------------------------------------------------------------*/
class QBtConfig : public QObject
{
   Q_OBJECT
   
// ******* CONSTRUCTION / DESTRUCTION *******
private:
   QBtConfig();
   QBtConfig( const QBtConfig& );
   QBtConfig& operator=( const QBtConfig& );

// ******* CONSTANTS *******
public:
   static const QString MAIN_WINDOW_GROUP;
   static const QString POS_KEY;
   static const QString SIZE_KEY;
   static const QString STATE_KEY;
   static const QString FPATH_HISTORY_SIZE_KEY;
   static const QString LFT_FPATH_HISTORY_KEY;
   static const QString RGT_FPATH_HISTORY_KEY;
   //--------------------------------------------
   static const QString BROWSER_GROUP;
   static const QString TEXT_FONT_KEY;
   static const QString TEXT_COLOR_KEY;
   static const QString TEXT_BACKGROUND_KEY;
   static const QString PART_FONT_KEY;
   static const QString PART_COLOR_KEY;
   static const QString PART_BACKGROUND_KEY;
   static const QString CHG_BACKGROUND_KEY;
   static const QString ADD_BACKGROUND_KEY;
   static const QString DEL_BACKGROUND_KEY;
   static const QString SYNC_KEY;
   static const QString NUMERATION_KEY;
   static const QString BEZIER_KEY;
   static const QString MARK_VCENTER_KEY;
   static const QString UTF8_KEY;
   //--------------------------------------------
   static const QString DIFF_GROUP;
   static const QString CASE_KEY;
   static const QString TAB_EXP_KEY;
   static const QString WSPACE_AMOUNT_KEY;
   static const QString WSPACE_ALL_KEY;
   static const QString BLANK_LINES_KEY;
   static const QString AS_TEXT_KEY;

private:
   static const int     DEFAULT_FPATH_HISTORY_SIZE;
   static const QColor  DEFAULT_BKG_COLOR;
   static const QColor  DEFAULT_FNT_COLOR;
   static const QString DEFAULT_FONT_STRING;
   static const QColor  DEFAULT_DEL_COLOR;
   static const QColor  DEFAULT_ADD_COLOR;
   static const QColor  DEFAULT_CHG_COLOR;

   
// ******* MEMBERS *******
private:
   int    fpath_history_size_;
   bool   sync_;
   bool   numeration_;
   bool   bezier_;
   bool   mark_vcenter_;
   bool   utf8_;

   bool   diff_case_;
   bool   diff_tab_exp_;
   bool   diff_wspace_amount_;
   bool   diff_wspace_all_;
   bool   diff_blank_lines_;
   bool   diff_as_text_;

   QFont  text_font_;
   QColor text_color_;
   QColor text_background_;
   QBrush text_background_brush_;
   //--------------------------------------------
   QFont  part_font_;
   QColor part_color_;
   QColor part_background_;
   QBrush part_background_brush_;
   //--------------------------------------------
   QColor chg_background_;
   QBrush chg_background_brush_;
   QColor add_background_;
   QBrush add_background_brush_;
   QColor del_background_;
   QBrush del_background_brush_;
   
// ******* METHODS *******
public:
   static QBtConfig* const instance();
   
   void          update();
   void          save_data();
   void          fpath_history_size ( const int in_value ) { fpath_history_size_ = in_value; }
   int           fpath_history_size () const { return fpath_history_size_; }

   //------- text ---------------------------------------------------
   void          text_font( const QFont& in_font ) { text_font_ = in_font; }
   const QFont&  text_font() const { return text_font_; }
   void          text_color( const QColor& in_color ) { text_color_ = in_color; }
   const QColor  text_color() const { return text_color_; }
   void          text_background( const QColor& in_color ) {
                     text_background_ = in_color;
                     text_background_brush_.setColor( text_background_ );
                 }
   void          text_background( const QString& in_color ) {
                     text_background_.setNamedColor( in_color );
                     text_background_brush_.setColor( text_background_ );
                 }
   const QColor& text_background() const { return text_background_; }
   const QBrush& text_background_brush() const { return text_background_brush_; }
   //------- part ---------------------------------------------------
   void          part_font( const QFont& in_font ) { part_font_ = in_font; }
   const QFont&  part_font() const { return part_font_; }
   void          part_color( const QColor& in_color ) { part_color_ = in_color; }
   const QColor  part_color() const { return part_color_; }
   void          part_background( const QColor& in_color ) {
                     part_background_ = in_color;
                     part_background_brush_.setColor( part_background_ );
                 }
   void          part_background( const QString& in_color ) {
                     part_background_.setNamedColor( in_color );
                     part_background_brush_.setColor( part_background_ );
                 }
   const QColor& part_background() const { return part_background_; }
   const QBrush& part_background_brush() const { return part_background_brush_; }
   //------- chg ----------------------------------------------------
   void          chg_background( const QColor& in_color ) { 
                     chg_background_ = in_color;
                     chg_background_brush_.setColor( chg_background_ );
                 }
   void          chg_background( const QString& in_text ) {
                     chg_background_.setNamedColor( in_text );
                     chg_background_brush_.setColor( chg_background_ );
                 }
   const QColor& chg_background() const { return chg_background_; }
   const QBrush& chg_background_brush() const { return chg_background_brush_; }
   //------- add ----------------------------------------------------
   void          add_background( const QColor& in_color ) {
                     add_background_ = in_color;
                     add_background_brush_.setColor( add_background_ );
                 }
   void          add_background( const QString& in_text ) {
                     add_background_.setNamedColor( in_text );
                     add_background_brush_.setColor( add_background_ );
                 }
   const QColor& add_background() const { return add_background_; }
   const QBrush& add_background_brush() const { return add_background_brush_; }
   //------- del ----------------------------------------------------
   void          del_background( const QColor& in_color ) {
                     del_background_ = in_color;
                     del_background_brush_.setColor( del_background_ );
                 }
   void          del_background( const QString& in_text ) {
                     del_background_.setNamedColor( in_text );
                     del_background_brush_.setColor( del_background_ );
                 }
   const QColor& del_background() const { return del_background_; }
   const QBrush& del_background_brush() const { return del_background_brush_; }
   
   
   void diff_case( const bool in_value ) { diff_case_ = in_value; }
   bool diff_case() const { return diff_case_; }
   void diff_tab_exp( const bool in_value ) { diff_tab_exp_ = in_value; }
   bool diff_tab_exp() const { return diff_tab_exp_; }
   void diff_wspace_amount( const bool in_value ) { diff_wspace_amount_ = in_value; }
   bool diff_wspace_amount() const { return diff_wspace_amount_; }
   void diff_wspace_all( const bool in_value ) { diff_wspace_all_ = in_value; }
   bool diff_wspace_all() const { return diff_wspace_all_; }
   void diff_blank_lines( const bool in_value ) { diff_blank_lines_ = in_value; }
   bool diff_blank_lines() const { return diff_blank_lines_; }
   void diff_as_text( const bool in_value ) { diff_as_text_ = in_value; }
   bool diff_as_text() const { return diff_as_text_; }

   bool sync() const { return sync_; }
   void sync( const bool in_value ) { sync_ = in_value; }
   bool numeration() const { return numeration_; }
   void numeration( const bool in_value ) { numeration_ = in_value; } 
   
   bool bezier() const { return bezier_; }
   void bezier( const bool in_use_bezier ) { bezier_ = in_use_bezier; }
   bool mark_vcenter() const { return mark_vcenter_; }
   void mark_vcenter( const bool in_vcenter ) { mark_vcenter_ = in_vcenter; }
   bool utf8() const { return utf8_; }
   void utf8( const bool in_utf8 ) { utf8_ = in_utf8; }
private:
   void read_data();
};

#endif // INCLUDED_QBtConfig_h
