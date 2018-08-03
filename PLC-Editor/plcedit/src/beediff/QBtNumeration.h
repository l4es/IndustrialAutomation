#ifndef INCLUDED_QBtNumeration_h
#define INCLUDED_QBtNumeration_h
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
 * Creation date: 04.02.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include <QWidget>

/*------- class declaration:
-------------------------------------------------------------------*/
class QBtNumeration : public QWidget
{
   Q_OBJECT
public:
   QBtNumeration( QWidget* );
private:
   QBtNumeration( const QBtNumeration& );
   QBtNumeration& operator=( const QBtNumeration& );

// ******* MEMBERS *******
private:
   int width_;

// ******* METHODS *******
public:
   void update_geometry ();
   void update_looks    ();
   int  width           ();
private:
   void  paintEvent     ( QPaintEvent* );
   bool  on_screen      ( int, int, int ) const;
};

#endif // INCLUDED_QBtNumeration_h
