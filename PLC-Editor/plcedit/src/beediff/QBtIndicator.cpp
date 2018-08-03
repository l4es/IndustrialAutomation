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
 * File         : QBtIndicator.cpp
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 13.02.2008
 *------------------------------------------------------------------
 * 02.04.2008 - changes (mouse behaviour) from Thomas Schweitzer
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include "QBtIndicator.h"
#include "QBtBrowser.h"
#include "QBtDiffInfo.h"
#include "QBtDiffProcess.h"
#include "QBtConfig.h"
#include <QPainter>
#include <QVector>
#include <QMouseEvent>
//#include <QtDebug>

/*------- local constants:
-------------------------------------------------------------------*/
const int QBtIndicator::WIDTH      = 11;
const int QBtIndicator::IND_MARGIN = 3;
const int QBtIndicator::IND_WIDTH  = WIDTH - 2 * IND_MARGIN;


//*******************************************************************
// QBtIndicator                                          CONSTRUCTOR
//*******************************************************************
QBtIndicator::QBtIndicator( QBtBrowser* const in_browser, QWidget* const in_parent )
: QWidget       ( in_parent  )
, browser_      ( in_browser )
, bkg_color_    ( QColor()   )
, border_color_ ( QColor()   )
, H_            ( int()   )
, h_            ( int()   )
, is_left_      ( browser_->is_left() )
{
   setAutoFillBackground( true );
    
   const QPalette p = palette();
   bkg_color_ = p.color( QPalette::Window );
   border_color_ = QColor( 91, 91, 91 );

   setMinimumWidth( WIDTH );
   setMaximumWidth( WIDTH );
   
   setMouseTracking( true );
}
// end of QBtIndicator

//*******************************************************************
// renew                                                      PUBLIC
//*******************************************************************
void QBtIndicator::renew()
{
   repaint();
}
// end of renew

//*******************************************************************
// paintEvent                                      PRIVATE inherited
//*******************************************************************
void QBtIndicator::paintEvent( QPaintEvent* const in_event )
{  
   QWidget::paintEvent( in_event );
    
   h_ = height();
   H_ = static_cast<int>( browser_->document()->size().height() );
   if( h_ > H_ ) h_ = H_;
      
   QPainter p( this );
   p.setPen( border_color_ );
   paint( p );
   p.end();
}
// end of paintEvent

//*******************************************************************
// paint                                                     PRIVATE
//*******************************************************************
void QBtIndicator::paint( QPainter& in_p )
{
   QBtDiffProcess* const dp = QBtDiffProcess::instance();

   if( is_left_ ) dp->first_ranges ( ranges_ );
   else           dp->second_ranges( ranges_ );
   
   QVector< QBtRange >::iterator it = ranges_.begin();
   const QVector< QBtRange>::iterator end = ranges_.end();
   while( it != end ) {
      indicator( in_p,  *it );
      ++it;
   }
}
// end of paint

//*******************************************************************
// indicator                                                 PRIVATE
//********************************************************************
void QBtIndicator::indicator( QPainter& in_p, QBtRange& in_range )
{
   const QBtConfig* const cfg = QBtConfig::instance();
   const float  prop          = float( h_ ) / float( H_ );
   const int    yt            = int( in_range.yt() * prop );
   const int    yb            = int( in_range.yb() * prop );

   bool only_line = false;
   QBrush brush = cfg->text_background_brush();
   switch( in_range.oper() ) {
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
   if( only_line ) {
      const QRect r( IND_MARGIN, yb -1, IND_WIDTH, 4 );
      in_range.yt( r.top() );
      in_range.yb( r.bottom() );
      
      in_p.fillRect( r, brush );
      in_p.drawRect( r );
   }
   else {
      const QRect r( IND_MARGIN, yt, IND_WIDTH, yb - yt + 2 );
      in_range.yt( r.top() );
      in_range.yb( r.bottom() );

      in_p.fillRect( r, brush );
      in_p.drawRect( r );
   }
}
// end of indicator

//*******************************************************************
// line_number                                               PRIVATE
//*******************************************************************
int QBtIndicator::line_number( const int in_ypos ) const
{
   QVector<QBtRange>::const_iterator it = ranges_.begin();
   const QVector<QBtRange>::const_iterator end = ranges_.end();
   while( it != end ) {
      if( it->yb() < in_ypos ) {
         ++it;
         continue;
      }
      if( it->yt() > in_ypos ) {
         break;
      }
      return it->nr1();
   }
   return -1;
}
// end of line_number

//*******************************************************************
// mouseReleaseEvent                               PRIVATE inherited
//*******************************************************************
void QBtIndicator::mouseReleaseEvent( QMouseEvent* const in_event )
{
   QWidget::mouseReleaseEvent( in_event );
   if( ranges_.empty() ) return;

   const int nr = line_number( in_event->y() );
   if( nr >= 0 ) {
      browser_->goto_line( ( nr > 0 ) ? nr : 1 );
   }
}
// end of mouseReleaseEvent

//*******************************************************************
// mouseMoveEvent                                  PRIVATE inherited
//*******************************************************************
void QBtIndicator::mouseMoveEvent( QMouseEvent* const in_event )
{
   QWidget::mouseMoveEvent( in_event );
   if( ranges_.empty() ) return;

   const int nr = line_number( in_event->y() );
   if( nr != -1 ) QBtShared::set_cursor( QCursor( Qt::PointingHandCursor ) );
   else           QBtShared::restore_cursor();
}
// end of mouseReleaseEvent

//*******************************************************************
// leaveEvent                                      PRIVATE inherited
//*******************************************************************
void QBtIndicator::leaveEvent( QEvent* const in_event )
{
   QWidget::leaveEvent( in_event );
   QBtShared::restore_cursor();
}
// end of leaveEvent
