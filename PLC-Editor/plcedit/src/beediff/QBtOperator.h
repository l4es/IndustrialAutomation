#ifndef INCLUDED_QBtOperator_h
#define INCLUDED_QBtOperator_h
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
 * File         : QBtOperator.h
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 27.02.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include "QBtMarkerInfo.h"
#include <QWidget>
#include <QPixmap>
#include <QVector>

/*------- forward declarations:
-------------------------------------------------------------------*/
class QBtBrowser;
class QBtRange;

/*------- class declaration:
-------------------------------------------------------------------*/
class QBtOperator : public QWidget
{
   Q_OBJECT

//******* CONSTRUCTION / DESTRUCTION *******
public:
   QBtOperator( QBtBrowser*, QBtBrowser* );
private:
   QBtOperator( const QBtOperator& );
   QBtOperator& operator=( const QBtOperator& );

//******* TYPES *******
private:
   typedef QVector<QBtMarkerInfo> TMarkers;

//******* CONSTANTS *******
private:
   static const QString TO_LFT_ICON;
   static const QString TO_RGT_ICON;
   static const QString REMOVE_ICON;

//******* MEMBERS *******
private:
   QBtBrowser* const browser_;
   QBtBrowser* const partner_browser_;
   const bool        is_left_;
   const QString     icon_move_name_;
   const QString     icon_remove_name_;
   QPixmap           icon_move_;
   QPixmap           icon_remove_;
   int               icon_move_width_;
   int               icon_move_height_;
   int               icon_remove_width_;
   int               icon_remove_height_;
   TMarkers          markers_;

//******* METHODS *******
public:
   void   update_geometry ();
   void   update_looks    ();
   void   update_marks    ();
private:
   void paintEvent     ( QPaintEvent* );
   void paint_one_icon ( QPainter&, int, int );
   void paint_two_icons( QPainter&, int, int );
   void mousePressEvent( QMouseEvent* );
};

#endif // INCLUDED_QBtOperator_h
