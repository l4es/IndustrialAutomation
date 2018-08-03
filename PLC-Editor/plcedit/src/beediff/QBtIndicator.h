#ifndef INCLUDED_QBtIndicator_h
#define INCLUDED_QBtIndicator_h
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
 * File         : QBtIndicator.h
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 13.02.2008
 *------------------------------------------------------------------
 * 02.04.2008 - changes (mouse behaviour) from Thomas Schweitzer
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include <QWidget>
#include "QBtRange.h"
#include <QColor>
#include <QVector>
#include <QApplication>

/*------- forward declarations:
-------------------------------------------------------------------*/
class QBtBrowser;
class QPainter;

/*------- class declaration:
-------------------------------------------------------------------*/
class QBtIndicator : public QWidget
{
   Q_OBJECT
   
//******* CONSTRUCTION / DESTRUCTION *******
public:
   QBtIndicator( QBtBrowser*, QWidget* = 0 );
private:
   QBtIndicator( const QBtIndicator& );
   QBtIndicator& operator=( const QBtIndicator& );

//******* CONSTANTS *******
private:
   static const int WIDTH;
   static const int IND_MARGIN;
   static const int IND_WIDTH;

//******* MEMBERS *******
private:
   QBtBrowser* const browser_;
   QColor            bkg_color_;
   QColor            border_color_;
   int               H_;
   int               h_;
   bool              is_left_;
   QVector<QBtRange> ranges_;
   
//******* METHODS *******
private:
   void paintEvent            ( QPaintEvent* );
   void paint                 ( QPainter&    );
   void indicator             ( QPainter&, QBtRange& );
   int  line_number           ( int ) const;
   void mouseReleaseEvent     ( QMouseEvent* );
   void mouseMoveEvent        ( QMouseEvent* );
   void leaveEvent            ( QEvent* );
public:
   void renew     ();
};

#endif // INCLUDED_QBtIndicator_h
