#ifndef INCLUDED_QBtShared_h
#define INCLUDED_QBtShared_h
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
 * File         : QBtShared.h
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 27.01.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include <QObject>
#include <QString>
#include <QColor>

/*------- forward declarations:
-------------------------------------------------------------------*/
class QSize;
class QCursor;

/*------- class declaration:
-------------------------------------------------------------------*/
class QBtShared : public QObject
{
//******** TYPES *******
public:
   enum {
      NO_OPER = 0,
      CHANGE,
      APPEND,
      DELETE,
      MOVE,
      SCROLL_UP = 0,
      SCROLL_DOWN,
      
   };

//******* CONSTANTS *******
public:
   static const char* const ORGANISATION;
   static const char* const PROGRAM_NAME;
   static const char* const VERSION;
   static const char* const VERNUM;

//******* MEMBERS *******
private:
   static QString buffer_;

//******* METHODS *******
public:
   static const QString& program_name   ();
   static void           resize         ( QWidget*, int, int );
   static void           resize         ( QWidget*, const QSize& );
   static void           resize         ( QWidget*, int );
   static void           set_cursor     ( const QCursor& );
   static void           restore_cursor ();
};

#endif // INCLUDED_QBtShared_h
