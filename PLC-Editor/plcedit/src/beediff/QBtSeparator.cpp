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
 * File         : QBtSeparator.cpp
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 04.02.2008
 *-------------------------------------------------------------------
 * 06.04.2008 - bazier curve from Thomas Schweizer.
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include "QBtSeparator.h"
#include "QBtBrowser.h"
#include "QBtDiffProcess.h"
#include "QBtConfig.h"
#include <QPainter>
#include <QPalette>
#include <QScrollBar>
#include <QtDebug>

/*------- local constants:
-------------------------------------------------------------------*/
const int QBtSeparator::WIDTH = 40;
const int QBtSeparator::X_MIN = 0;
const int QBtSeparator::X_MAX = WIDTH - 1;
const int QBtSeparator::X_OFF = 4;
const int QBtSeparator::X_LFT = X_MIN + X_OFF;
const int QBtSeparator::X_RGT = X_MAX - X_OFF + 1;

//*******************************************************************
// QBtSeparator                                          CONSTRUCTOR
//*******************************************************************
QBtSeparator::QBtSeparator( const QBtBrowser* const in_lft_browser,
                            const QBtBrowser* const in_rgt_browser,
                            QWidget* const          in_parent )
: QWidget      ( in_parent )
, lft_browser_ ( in_lft_browser )
, rgt_browser_ ( in_rgt_browser )
, lft_brace_   ( TPoints() )
, rgt_brace_   ( TPoints() )
, connector_   ( TPoints() )
{
   setAutoFillBackground( true );
   setMinimumWidth( WIDTH );
   setMaximumWidth( WIDTH );
}
// end of QBtSeparator

//*******************************************************************
// painEvent                                       PRIVATE inherited
//*******************************************************************
void QBtSeparator::paintEvent( QPaintEvent* const in_event )
{
   QWidget::paintEvent( in_event );
   
   QPainter p( this );
   paint( p );
   p.end();
}
// end of paintEvent

//*******************************************************************
// paint                                                     PRIVATE
//*******************************************************************
void QBtSeparator::paint( QPainter& in_p )
{
   QBtDiffProcess* const dp = QBtDiffProcess::instance();
   if( !dp->is_valid() ) return;

   const QBtConfig* const cfg = QBtConfig::instance();
   chg_color_ = cfg->chg_background().darker( 120 );
   add_color_ = cfg->add_background().darker( 120 );
   del_color_ = cfg->del_background().darker( 120 );
   bezier_    = cfg->bezier();

   in_p.setRenderHint( QPainter::Antialiasing );
   QPen my_pen = in_p.pen();
   my_pen.setWidth( 2 );
   my_pen.setJoinStyle( Qt::RoundJoin );
   in_p.setPen( my_pen );

   int lft_nr1 = lft_browser_->first_visible() - 1;
   int lft_nr2 = lft_browser_->last_visible () + 1;
   int rgt_nr1 = rgt_browser_->first_visible() - 1;
   int rgt_nr2 = rgt_browser_->last_visible () + 1;

   // Jako punkt odniesienia do rysowania polaczen
   // wybralismy linie wyswietlane w lewym browserze.
   // Jednak zakres linii, ktore nas interesuja musimy
   // zmodyfikowac o linie, do ktorych odwoluja sie linie
   // z prawej strony. Chodzi o to, aby z prawej strony
   // nie pozostaly 'osierocone' linie.

   for( int i = lft_nr1; i <= lft_nr2; ++i ) {
      const QBtDiffInfo info = dp->info_for_nr_in_first( i );
      if( info.is_valid() ) {
         const int nr1 = info.second_range().nr1();
         const int nr2 = info.second_range().nr2();
         if( nr1 < rgt_nr1 ) rgt_nr1 = nr1;
         if( nr2 > rgt_nr2 ) rgt_nr2 = nr2;
      }
   }

   for( int i = rgt_nr1; i <= rgt_nr2; ++i ) {
      const QBtDiffInfo info = dp->info_for_nr_in_second( i );
      if( info.is_valid() ) {
         const int nr1 = info.first_range().nr1();
         const int nr2 = info.first_range().nr2();
         if( nr1 < lft_nr1 ) lft_nr1 = nr1;
         if( nr2 > lft_nr2 ) lft_nr2 = nr1;
      }
   }

   int nr = lft_nr1;
   while( nr <= lft_nr2 ) {
      const QBtDiffInfo info = dp->info_for_nr_in_first( nr );
      if( info.is_valid() ) {
         paint_info( in_p, info.first_range(), info.second_range() );
         nr = info.first_range().nr2();
      }
      ++nr;
   }
}
// end of paint

//*******************************************************************
// paint_info                                                PRIVATE
//*******************************************************************
void QBtSeparator::paint_info( QPainter&       in_p,
                               const QBtRange& in_A_range,
                               const QBtRange& in_B_range )
{
   QPen my_pen = in_p.pen();
   switch( in_A_range.oper() ) {
      case QBtShared::CHANGE:
         my_pen.setColor( chg_color_ );
         break;
      case QBtShared::APPEND:
         my_pen.setColor( add_color_ );
         break;
      case QBtShared::DELETE:
         my_pen.setColor( del_color_ );
         break;
   }
   in_p.setPen( my_pen );
   in_p.setBrush( my_pen.color() );

   bezier_ ? bezier( in_p, in_A_range, in_B_range )
           : normal( in_p, in_A_range, in_B_range );
}
// end of paint_info

//*******************************************************************
// normal                                                    PRIVATE
//*******************************************************************
void QBtSeparator::normal( QPainter&       in_p      ,
                           const QBtRange& in_A_range,
                           const QBtRange& in_B_range )
{
   const int max_y = height() - 1;
   {
      const bool only_line = ( QBtShared::APPEND == in_A_range.oper() );
      const int  offset    = lft_browser_->verticalScrollBar()->value() - 1;

      int yb = in_A_range.yb() - offset;
      int yt = only_line ? yb : ( in_A_range.yt() - offset + 1 );
      
      if( only_line ) {
         lft_brace_ << QPoint( X_MIN, yb ) << QPoint( X_LFT, yb );
         connector_ << QPoint( X_LFT, yb );
      }
      else {
         if( ( yt >= 0 ) && ( yb <= max_y ) ) {
            lft_brace_ << QPoint( X_MIN, yt ) << QPoint( X_LFT, yt )
                       << QPoint( X_LFT, yt ) << QPoint( X_LFT, yb )
                       << QPoint( X_LFT, yb ) << QPoint( X_MIN, yb );
         }
         else if( ( yt < 0 ) && ( yb > max_y ) ) {
            lft_brace_ << QPoint( X_LFT, 0 ) << QPoint( X_LFT, max_y );
         }
         else if( ( yt < 0 ) && ( yb >= 0 ) && ( yb <= max_y ) ) {
            lft_brace_ << QPoint( X_LFT,  0 ) << QPoint( X_LFT, yb )
                       << QPoint( X_LFT, yb ) << QPoint( X_MIN, yb );
         }
         else if( ( yb > max_y ) && ( yt >= 0 ) && ( yt <= max_y ) ) {
            lft_brace_ << QPoint( X_MIN, yt ) << QPoint( X_LFT, yt )
                       << QPoint( X_LFT, yt ) << QPoint( X_LFT, max_y );
         }

         const int y = yt + ( ( yb - yt ) >> 1 );
         connector_ << QPoint( X_LFT, y );
      }
   }
   {
      const bool only_line = ( QBtShared::DELETE == in_B_range.oper() );
      const int  offset    = rgt_browser_->verticalScrollBar()->value() - 1;
      
      int yb = in_B_range.yb() - offset;
      int yt = only_line ? yb : ( in_B_range.yt() - offset + 1 );

      if( only_line ) {
         connector_ << QPoint( X_RGT, yb );
         rgt_brace_ << QPoint( X_RGT, yb ) << QPoint( X_MAX, yb );
      }
      else {
         if( ( yt >= 0 ) && ( yb <= max_y ) ) {
            rgt_brace_ << QPoint( X_MAX, yt ) << QPoint( X_RGT, yt )
                       << QPoint( X_RGT, yt ) << QPoint( X_RGT, yb )
                       << QPoint( X_RGT, yb ) << QPoint( X_MAX, yb );
         }
         else if( ( yt < 0 ) && ( yb > max_y ) ) {
            rgt_brace_ << QPoint( X_RGT, 0 ) << QPoint( X_RGT, max_y );
         }
         else if( ( yt < 0 ) && ( yb >= 0 ) && ( yb <= max_y ) ) {
            rgt_brace_ << QPoint( X_RGT,  0 ) << QPoint( X_RGT, yb )
                       << QPoint( X_RGT, yb ) << QPoint( X_MAX, yb );
         }
         else if( ( yb > max_y ) && ( yt >= 0 ) && ( yt <= max_y ) ) {
            rgt_brace_ << QPoint( X_MAX, yt ) << QPoint( X_RGT, yt )
                       << QPoint( X_RGT, yt ) << QPoint( X_RGT, max_y );
         }
         
         const int y = yt + ( ( yb - yt ) >> 1 );
         connector_ << QPoint( X_RGT, y );
      }
   }
   in_p.drawLines( lft_brace_ );
   in_p.drawLines( rgt_brace_ );
   in_p.drawLines( connector_ );

   lft_brace_.clear();
   rgt_brace_.clear();
   connector_.clear();
}
// end of normal

//*******************************************************************
// bezier                                                    PRIVATE
//-------------------------------------------------------------------
// From Thomas Schweizer
//*******************************************************************
void QBtSeparator::bezier( QPainter&       in_p,
                           const QBtRange& in_A_range,
                           const QBtRange& in_B_range )
{

   bool only_line = ( QBtShared::APPEND == in_A_range.oper() );
   int  offset    = lft_browser_->verticalScrollBar()->value() - 1;
#ifdef Q_OS_WIN32
   // Under Windows the scrollbar value has a difference of one pixel compared to other os.
   offset++;
#endif
   int yLeftBottom = in_A_range.yb() - offset;
   int yLeftTop = only_line ? yLeftBottom : ( in_A_range.yt() - offset + 1 );

   only_line = ( QBtShared::DELETE == in_B_range.oper() );
   offset    = rgt_browser_->verticalScrollBar()->value() - 1;
#ifdef Q_OS_WIN32
   // Under Windows the scrollbar value has a difference of one pixel compared to other os.
   offset++;
#endif
   int yRightBottom = in_B_range.yb() - offset;
   int yRightTop = only_line ? yRightBottom : ( in_B_range.yt() - offset + 1 );

   QPainterPath bezierPath;
   int xMiddle = X_MIN + (X_MAX - X_MIN) / 2;
   bezierPath.moveTo(X_MIN, yLeftTop);
   bezierPath.cubicTo(xMiddle, yLeftTop, xMiddle, yRightTop, X_MAX, yRightTop);
   bezierPath.lineTo( X_MAX, yRightBottom );
   bezierPath.cubicTo(xMiddle, yRightBottom, xMiddle, yLeftBottom, X_MIN, yLeftBottom);
   in_p.drawPath( bezierPath );
}
// end of bezier
