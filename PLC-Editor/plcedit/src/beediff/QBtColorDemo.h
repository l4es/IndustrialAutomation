#ifndef INCLUDED_QBtColorDemo_h
#define INCLUDED_QBtColorDemo_h
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
 * File         : QBtColorDemo.cpp
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 25.02.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include <QLineEdit>

/*------- forward declarations:
-------------------------------------------------------------------*/
class QColor;
class QFont;

/*------- class declaration:
-------------------------------------------------------------------*/
class QBtColorDemo : public QLineEdit
{
   Q_OBJECT

//******* CONSTRUCTION / DESTRUCTION *******
public:
   QBtColorDemo( bool = false, QWidget* = 0 );
private:
   QBtColorDemo( const QBtColorDemo& );
   QBtColorDemo& operator=( const QBtColorDemo& );

//******* CONSTANTS *******
private:
   static const int HEIGHT;
   static const char* const TEXT;

//******* METHODS *******
public:
   void   background ( const QColor& );
   QColor background () const;
   void   color      ( const QColor& );
   QColor color      () const;
   void   fonts      ( const QFont& );
   QFont  fonts      () const;
};
 
#endif // INCLUDED_QBtColorDemo_h
