#ifndef INCLUDED_QBtConfigOthers_h
#define INCLUDED_QBtConfigOthers_h
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
 * File         : QBtConfigOthers.h
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 06.04.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include "QBiConfigWidget.h"

/*------- forward declarations:
-------------------------------------------------------------------*/
class QCheckBox;

/*------- class declaration:
-------------------------------------------------------------------*/
class QBtConfigOthers : public QBiConfigWidget
{
   Q_OBJECT

//******* CONSTRUCTION / DESTRUCTION ********
public:
   QBtConfigOthers( QWidget* = 0 );
private:
   QBtConfigOthers( const QBtConfigOthers& );
   QBtConfigOthers& operator=( const QBtConfigOthers& );

//******* CONSTANTS *******
private:
   static const char* const BezierCurve;
   static const char* const MarkVCenter;
   static const char* const Utf8;

//******* MEMBERS *******
private:
   QCheckBox* const bezier_cbx_;
   QCheckBox* const center_cbx_;
   QCheckBox* const utf8_cbx_;

//******* METHODS *******
public:
   void apply();
};

#endif // INCLUDED_QBtConfigOthers_h
