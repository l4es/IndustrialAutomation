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
 * File         : QBtBrowser.cpp
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 30.03.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include "QBtNumeration.h"
#include "QBtBrowser.h"
#include "QBtLineData.h"
#include "QBtConfig.h"
#include "QBtDiffProcess.h"
#include <QPainter>
#include <QTextBlock>
#include <QScrollBar>
#include <QTextLayout>
#include <QColor>
#include <QPalette>
#include <QtDebug>


//*******************************************************************
// QBtNumeration                                         CONSTRUCTOR
//*******************************************************************
QBtNumeration::QBtNumeration( QWidget* const in_parent )
: QWidget( in_parent )
, width_ ( int() )
{
   setAutoFillBackground( true );
   update_looks();
}
// end of QBtNumeration

//*******************************************************************
// update_geometry                                            PUBLIC
//*******************************************************************
void QBtNumeration::update_geometry()
{
   const QBtBrowser* const browser = dynamic_cast<QBtBrowser*>( parent() );
   if( !browser ) return;
   
   setGeometry( QRect( 0, 0, width(), browser->viewport()->height() ) );
}
// end of update_geometry

//*******************************************************************
// update_looks                                               PUBLIC
//*******************************************************************
void QBtNumeration::update_looks()
{
   const QBtBrowser* const browser = dynamic_cast<QBtBrowser*>( parent() );
   if( !browser ) return;

   const QBtConfig* const cfg = QBtConfig::instance();
   
   QPalette p = palette();
   p.setColor( backgroundRole(), cfg->text_background() );
   p.setColor( foregroundRole(), cfg->text_color() );
   setPalette( p );

   //setFont( cfg->text_font() );//remarks for PLCEdit
}
// end of update_looks

//*******************************************************************
// width                                                      PUBLIC
//-------------------------------------------------------------------
// Oblicza aktualna szerokosc marginesu.
// Szerokosc moze zmieniac sie dynamicznie uwzgledniajac liczbe
// linii w dokumencie.
//*******************************************************************
qint32 QBtNumeration::width()
{
   width_ = int();
   if( !isVisible() ) return width_;

   const QBtBrowser* const browser = dynamic_cast<QBtBrowser*>( parent() );
   if( !browser ) return width_;
   
   QTextBlock block = browser->document()->end().previous();
   if( !block.isValid() ) return width_;

   const QBtLineData* const info = dynamic_cast<QBtLineData*>( block.userData() );
   if( !info ) return width_;

   const int n = QString::number( info->get_number() ).length() + 1;
   return ( width_ = n * browser->fontMetrics().width( "99", 1 ) );
}
// end of width

//*******************************************************************
// on_screen                                                 PRIVATE
//*******************************************************************
bool QBtNumeration::on_screen( const int in_top_y, const int in_bottom_y, const int in_y ) const
{
   return ( ( in_y > in_top_y ) && ( in_y < in_bottom_y ) );
}
// end of on_screen

//*******************************************************************
// paintEvent                                      PRIVATE inherited
//*******************************************************************
void QBtNumeration::paintEvent( QPaintEvent* const in_event )
{
   QWidget::paintEvent( in_event );
   if( !isVisible() ) return;

   const QBtBrowser* const browser = dynamic_cast<QBtBrowser*>( parent() );
   if( !browser ) return;

   if( browser->document()->isEmpty() ) return;

   QPainter p( this );
   const int  view_yt = browser->verticalScrollBar()->value();
   const int  view_yb = view_yt + browser->viewport()->height() - 1;
   const bool is_left = browser->is_left();
   const QBtConfig* const cfg = QBtConfig::instance();
   QBrush brush = cfg->text_background_brush();

   QTextBlock block = browser->document()->begin();
   while( block.isValid() ) {
      const int block_yt = qRound( block.layout()->position().y() );
      const int block_h  = block.layout()->boundingRect().toRect().height();
      const int block_yb = block_yt + block_h - 1;

      if( block_yb < view_yt ) {
         block = block.next();
         continue;
      }
      if( block_yt > view_yb ) break;

      const QBtLineData* const info = dynamic_cast<QBtLineData*>( block.userData() );
      if( info ) {
         if( info->get_status() ) {
            bool only_line = false;
            switch( info->get_status() ) {
               case QBtShared::CHANGE:
                  brush = cfg->chg_background_brush();
                  break;   
               case QBtShared::DELETE:
                  brush = cfg->del_background_brush();
                  only_line = !is_left;
                  break;
               case QBtShared::APPEND:
                  brush = cfg->add_background_brush();
                  only_line = is_left;
                  break;
            }
            if( only_line ) {
               const int yt = block_yt + block_h - 3;
               const int h = 2;
               p.fillRect( QRect( 0, yt - view_yt, width_, h ), brush );
            }
            else {
               const int yt = block_yt - 1;
               const int h = block_h;
               p.fillRect( QRect( 0, yt - view_yt, width_, h ), brush );
            }
         }
         const int nr = info->get_number();
         const QRect rect( 2, block_yt - view_yt , width_, block_h  );
         p.drawText( rect, Qt::AlignLeft | Qt::AlignBottom, QString::number( nr ) );
      }
      block = block.next();
   }
   p.end();
}
// end of paintEvent
