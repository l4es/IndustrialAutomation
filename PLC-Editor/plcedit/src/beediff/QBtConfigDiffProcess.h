#ifndef INCLUDED_QBtConfigDiffProcess_h
#define INCLUDED_QBtConfigDiffProcess_h
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
 * File         : QBtConfigDiffProcess.h
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 21.02.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include "QBiConfigWidget.h"

/*------- forward declarations:
-------------------------------------------------------------------*/
class QCheckBox;

/*------- class declaration:
-------------------------------------------------------------------*/
class QBtConfigDiffProcess : public QBiConfigWidget
{
   Q_OBJECT

//******* CONSTRUCTION / DESTRUCTION *******
public:
   QBtConfigDiffProcess( QWidget* = 0 );
private:
   QBtConfigDiffProcess( const QBtConfigDiffProcess& );
   QBtConfigDiffProcess& operator=( const QBtConfigDiffProcess& );

//******* CONSTANTS *******
private:
   static const char* const IGNORE_CASE;
   static const char* const IGNORE_TAB_EXP;
   static const char* const IGNORE_WSPACE_AMOUNT;
   static const char* const IGNORE_WSPACE_ALL;
   static const char* const IGNORE_BLANK_LINES;
   static const char* const FILES_AS_TEXT;

//******* MEMBERS *******
private:
   QCheckBox* const case_;
   QCheckBox* const tab_exp_;
   QCheckBox* const wspace_amount_;
   QCheckBox* const wspace_all_;
   QCheckBox* const blank_lines_;
   QCheckBox* const as_text_;

//******* METHODS *******
public:
   void apply();
};

#endif // INCLUDED_QBtConfigDiffProcess_h

