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
 * File         : QBtOperator.cpp
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 27.02.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include "QBtOperator.h"
#include "QBtBrowser.h"
#include "QBtLineData.h"
#include "QBtDiffProcess.h"
#include "QBtConfig.h"
#include "QBtEventsController.h"
#include <QPainter>
#include <QPaintEvent>
#include <QTextBlock>
#include <QScrollBar>
#include <QVector>
#include <QtDebug>

/*------- constants:
-------------------------------------------------------------------*/
const QString QBtOperator::TO_LFT_ICON = ":/img/to_left.png";
const QString QBtOperator::TO_RGT_ICON = ":/img/to_right.png";
const QString QBtOperator::REMOVE_ICON = ":/img/remove.png";


//*******************************************************************
// QBtOperator                                           CONSTRUCTOR
//*******************************************************************
QBtOperator::QBtOperator( QBtBrowser* const in_parent, QBtBrowser* const in_partner )
: QWidget             ( in_parent )
, browser_            ( in_parent )
, partner_browser_    ( in_partner )
, is_left_            ( in_parent->is_left() )
, icon_move_name_     ( is_left_ ? TO_RGT_ICON : TO_LFT_ICON )
, icon_remove_name_   ( REMOVE_ICON )
, icon_move_          ( QPixmap() )
, icon_remove_        ( QPixmap() )
, icon_move_width_    ( int() )
, icon_move_height_   ( int() )
, icon_remove_width_  ( int() )
, icon_remove_height_ ( int() )
, markers_            ( TMarkers() )
{
   setAutoFillBackground( true );
   update_geometry();
   update_looks();
   browser_->set_operator( this );
}
// end of QBtOperator

//*******************************************************************
// update_geometry                                            PUBLIC
//*******************************************************************
void QBtOperator::update_geometry()
{
   icon_move_ = QPixmap( icon_move_name_ );
   icon_remove_ = QPixmap( icon_remove_name_ );

   const int mark_height = browser_->fontMetrics().height() - 2;
   if( icon_move_.height() > mark_height ) {
      icon_move_ = icon_move_.scaled( mark_height, mark_height );
      icon_remove_ = icon_remove_.scaled( mark_height - 2, mark_height - 2 );
   }

   icon_move_width_    = icon_move_.width();
   icon_move_height_   = icon_move_.height();
   icon_remove_width_  = icon_remove_.width();
   icon_remove_height_ = icon_remove_.height();
   
   const int w = 2 + icon_remove_width_ + 1 + icon_move_width_ + 2;
   setMinimumWidth( w );
   setMaximumWidth( w );
}
// end of geometry

//*******************************************************************
// update_looks                                               PUBLIC
//*******************************************************************
void QBtOperator::update_looks()
{
   QPalette p = palette();
   p.setColor( backgroundRole(), QBtConfig::instance()->text_background() );
   setPalette( p );
}
// end of update_looks

//*******************************************************************
// painEvent                                       PRIVATE inherited
//*******************************************************************
void QBtOperator::paintEvent( QPaintEvent* const in_event )
{
   QWidget::paintEvent( in_event );
   QPainter p( this );

   const QBtConfig* const cfg = QBtConfig::instance();
   QBtDiffProcess* const diff = QBtDiffProcess::instance();
   QVector<QBtRange> data = QVector< QBtRange >();
   const bool ok = is_left_ ? diff->first_ranges( data ) : diff->second_ranges( data );
   
   if( ok ) {
      markers_.clear();
      const bool vcenter = QBtConfig::instance()->mark_vcenter();   
      const int view_yt  = browser_->verticalScrollBar()->value();
      const int check_yt = view_yt - 1;
      const int view_yb  = view_yt + browser_->viewport()->height() - 1;
      const int check_yb = view_yb + 1;
      const int w        = width();

      QVector< QBtRange >::const_iterator it = data.begin();
      const QVector< QBtRange >::const_iterator end = data.end();
      
      while( it != end ) {
         const int block_yb = it->yb();

         if( block_yb < check_yt ) {
            ++it;
            continue;
         }
         const int block_yt = it->yt();
         if( block_yt > check_yb ) break;

         const int oper = it->oper();
         QBrush brush = cfg->text_background_brush();
         bool only_line = false;
         if( oper != QBtShared::NO_OPER ) {
            switch( oper ) {
               case QBtShared::CHANGE:
                  brush = cfg->chg_background_brush();
                  break;   
               case QBtShared::DELETE:
                  brush = cfg->del_background_brush();
                  only_line = !is_left_;
                  break;
               case QBtShared::APPEND:
                  brush = cfg->add_background_brush();
                  only_line = is_left_;
                  break;
            }
         }
         //******* WSPOLRZEDNE *******
         const int block_h  = block_yb - block_yt + 1;
         const int h = only_line ? 2 : block_h;
         int yt = block_yt - view_yt - 1;
         if( only_line ) yt += ( block_h - 2 );
         const QRect r( 0, yt, w, h );
/*
         if( !is_left_ ) {
            qDebug() << r.x() << ", " << r.y();
         }
*/
         //******* MALOWANIE *******
         p.fillRect( r, brush );

         // ******* IKONY *******
         if( partner_browser_ ) {
            if( !only_line ) {
               if( vcenter ) {
                  yt += ( ( ( block_h - icon_move_height_ ) >> 1 ) - 1 );
               }
               switch( oper ) {
                  case QBtShared::CHANGE:
                     paint_one_icon( p, yt, it->nr1() );
                     break;
                  case QBtShared::DELETE:
                  case QBtShared::APPEND:
                     if( !only_line ) paint_two_icons( p, yt, it->nr1() );
                     break;
               }
            }
         }
         ++it;
      }
      markers_.squeeze();
   }
   
   p.end();
}
// end of paintEvent

//*******************************************************************
// paint_one_icon                                            PRIVATE
//*******************************************************************
void QBtOperator::paint_one_icon( QPainter& in_p,
                                  const int in_yt,
                                  const int in_nr1 )
{
   if( !partner_browser_->is_writable() ) return;
   
   const int x = is_left_ ? ( width() - icon_move_width_ - 2 ) : 2;
   const QRect r( x, in_yt, icon_move_width_, icon_move_height_ );
   in_p.drawPixmap( r, icon_move_ );
   markers_.push_back( QBtMarkerInfo( r, QBtShared::CHANGE, in_nr1 ) );
}
// end of paint_one_icon

//*******************************************************************
// paint_two_icons                                           PRIVATE
//*******************************************************************
void QBtOperator::paint_two_icons( QPainter&    in_p, 
                                   const int in_yt,
                                   const int in_nr1 )
{
   int x = 2;
         
   if( is_left_ ) {
      if( browser_->is_writable() ) {
         const QRect r( x, in_yt + 2, icon_remove_width_, icon_remove_height_ );
         in_p.drawPixmap( r, icon_remove_ );
         markers_.push_back( QBtMarkerInfo( r, QBtShared::DELETE, in_nr1 ) );
      }
      if( partner_browser_->is_writable() ) {
         x += ( icon_remove_width_ + 1 );
         const QRect r( x, in_yt, icon_move_width_, icon_move_height_ );
         in_p.drawPixmap( r, icon_move_ );
         markers_.push_back( QBtMarkerInfo( r, QBtShared::MOVE, in_nr1 ) );
      }
   }
   else {
      if( partner_browser_->is_writable() ) {
         const QRect r( x, in_yt, icon_move_width_, icon_move_height_ );
         in_p.drawPixmap( r, icon_move_ );
         markers_.push_back( QBtMarkerInfo( r, QBtShared::MOVE, in_nr1 ) );
         x += ( icon_move_width_ + 1 );
      }
      if( browser_->is_writable() ) {
         const QRect r( x, in_yt + 2, icon_remove_width_, icon_remove_height_ );
         in_p.drawPixmap( r, icon_remove_ );
         markers_.push_back( QBtMarkerInfo( r, QBtShared::DELETE, in_nr1 ) );
      }
   }
}
// end of paint_two_icons

//*******************************************************************
// mousePressEvent                                 PRIVATE inherited
//*******************************************************************
void QBtOperator::mousePressEvent( QMouseEvent* const in_event )
{
   QWidget::mousePressEvent( in_event );

   int nr = -1;
   int oper = QBtShared::NO_OPER;
   
   TMarkers::const_iterator it = markers_.begin();
   const TMarkers::const_iterator end = markers_.end();
   while( it != end ) {
      if( it->rect().contains( in_event->pos(), true ) ) {
         nr = it->nr1();
         oper = it->oper();
         break;
      }
      ++it;
   }

   if( -1 == nr ) return;
   if( QBtShared::NO_OPER == oper ) return;
   
   QBtEventsController* const ec = QBtEventsController::instance();
   switch( oper ) {
      case QBtShared::DELETE:
         if( is_left_ ) ec->send_event( QBtEvent::REMOVE_FROM_LEFT , nr );
         else           ec->send_event( QBtEvent::REMOVE_FROM_RIGHT, nr );
         break;
      case QBtShared::MOVE:
         if( is_left_ ) ec->send_event( QBtEvent::MOVE_FROM_LEFT , nr );
         else           ec->send_event( QBtEvent::MOVE_FROM_RIGHT, nr );
         break;
      case QBtShared::CHANGE:
         if( is_left_ ) ec->send_event( QBtEvent::CHANGE_FROM_LEFT , nr );
         else           ec->send_event( QBtEvent::CHANGE_FROM_RIGHT, nr );
         break;
   }
}
// end of mousePressEvent
