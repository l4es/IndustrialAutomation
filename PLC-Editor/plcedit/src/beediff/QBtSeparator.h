#ifndef INCLUDED_QBtSeparator_h
#define INCLUDED_QBtSeparator_h
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
 * File         : QBtSeparator.h
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 04.02.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include <QWidget>
#include <QColor>
#include <QPoint>
#include <QVector>

/*------- forward declarations:
-------------------------------------------------------------------*/
class QBtBrowser;
class QPainter;
class QBtRange;

/*------- class declaration:
-------------------------------------------------------------------*/
class QBtSeparator : public QWidget
{
   Q_OBJECT

//******* CONSTRUCTION / DESTRUCTION *******
public:
   QBtSeparator( const QBtBrowser*, const QBtBrowser*, QWidget* = 0 );
private:
   QBtSeparator( const QBtSeparator& );
   QBtSeparator& operator=( const QBtSeparator& );

//******* TYPES *******
private:
   typedef QVector<QPoint> TPoints;

//******* CONSTANTS *******
private:
   static const int WIDTH;
   static const int X_MIN;
   static const int X_MAX;
   static const int X_OFF;
   static const int X_LFT;
   static const int X_RGT;

//******* MEMBERS *******
private:
   const QBtBrowser* const lft_browser_;
   const QBtBrowser* const rgt_browser_;
   TPoints                 lft_brace_;
   TPoints                 rgt_brace_;
   TPoints                 connector_;
   QColor                  chg_color_;
   QColor                  add_color_;
   QColor                  del_color_;
   bool                    bezier_;

//******* METHODS *******
private:
   void paintEvent( QPaintEvent* );
   void paint( QPainter& );
   void paint_info( QPainter&, const QBtRange&, const QBtRange& );
   void normal( QPainter&, const QBtRange&, const QBtRange& ) ;
   void bezier( QPainter&, const QBtRange&, const QBtRange& );
};

#endif // INCLUDED_QBtSeparator_h
