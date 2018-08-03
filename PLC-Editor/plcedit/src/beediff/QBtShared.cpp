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
 * File         : QBtShared.cpp
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 27.01.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include "QBtShared.h"
#include <QSize>
#include <QCursor>
#include <QApplication>
#include <QDesktopWidget>

/*------- constants:
--------------------------------------------------------------------*/
const char* const QBtShared::ORGANISATION       = "beesoft.at";
const char* const QBtShared::PROGRAM_NAME       = QT_TR_NOOP( "Beesoft Differ" );
const char* const QBtShared::VERSION            = QT_TR_NOOP( ", ver. " );     
const char* const QBtShared::VERNUM             = QT_TR_NOOP( "1.9" );

/*------- local variables:
-------------------------------------------------------------------*/
QString QBtShared::buffer_ = QString();

//*******************************************************************
// program_name
//*******************************************************************
const QString& QBtShared::program_name()
{
   buffer_  = tr( PROGRAM_NAME );
   buffer_ += tr( VERSION );
   buffer_ += tr( VERNUM );

   return buffer_;
}
// end of program_name

//*******************************************************************
// resize
//*******************************************************************
void QBtShared::resize( QWidget* const in_widget,
                        const int      in_width,
                        const int      in_height )
{
   const int   screen_width = qApp->desktop()->availableGeometry().width();
   const int   screen_height = qApp->desktop()->availableGeometry().height();
   const float width_percent = static_cast<float>( in_width ) / 100.0;
   const float height_percent = static_cast<float>( in_height ) / 100.0;    
   const int   dx = static_cast<int>( screen_width * width_percent );
   const int   dy = static_cast<int>( screen_height * height_percent );

   resize( in_widget, QSize( dx, dy ));
}
void QBtShared::resize( QWidget* const in_widget, const QSize& in_size )
{
    const int screen_width  = qApp->desktop()->availableGeometry().width();
    const int screen_height = qApp->desktop()->availableGeometry().height();
    const int x = ( screen_width  - in_size.width()  + 1 ) >> 1;
    const int y = ( screen_height - in_size.height() + 1 ) >>  1;
    
    in_widget->move( x, y );
    in_widget->resize( in_size.width(), in_size.height() );
}
void QBtShared::resize( QWidget* const in_widget, const int in_width )
{
    const int   screen_width  = qApp->desktop()->screenGeometry().width();
    const int   screen_height = qApp->desktop()->screenGeometry().height();
    const float width_percent = ( static_cast<float>( in_width ) ) / 100.0;
    
    QSize new_size = in_widget->size();
    new_size.setWidth( static_cast<int>( screen_width * width_percent ) );
    in_widget->resize( new_size );
    
    const int x  = ( screen_width  - new_size.width()  + 1 ) >> 1;
    const int y  = ( screen_height - new_size.height() + 1 ) >>  1;
    in_widget->move( x, y );
}
// end of resize

//*******************************************************************
// set_cursor
//*******************************************************************
void QBtShared::set_cursor( const QCursor& in_cursor )
{
   if( !QApplication::overrideCursor() || ( QApplication::overrideCursor()->shape() != in_cursor.shape() ) ) {
      QApplication::setOverrideCursor( in_cursor );
   }
}
// end of set_cursor

//*******************************************************************
// restore_cursor
//*******************************************************************
void QBtShared::restore_cursor()
{
   QApplication::restoreOverrideCursor();
}
// end of restore_cursor
