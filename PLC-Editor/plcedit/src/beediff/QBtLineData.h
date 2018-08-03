#ifndef INCLUDED_QBtLineData_h
#define INCLUDED_QBtLineData_h
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
 * File         : QBtLineData.h
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 12.02.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include <QTextBlockUserData>
#include "QBtShared.h"

/*------- class declaration:
-------------------------------------------------------------------*/
class QBtLineData : public QTextBlockUserData
{
//******* CONSTRUCTION / DESTRUCTION *******
public:
   QBtLineData( const int  in_number = int(),
                const int  in_status = QBtShared::NO_OPER,
                const bool in_is_left = true )
   : number_  ( in_number )
   , status_  ( in_status )
   , is_left_ ( in_is_left )
   {}
private:
   QBtLineData( const QBtLineData& );
   QBtLineData& operator=( const QBtLineData& );

//******* MEMBERS *******
private:
   int  number_;
   int  status_;
   bool is_left_;

//******* METHODS *******
public:
   int  get_number () const { return number_;  }
   int  get_status () const { return status_;  }
   bool is_left    () const { return is_left_; }
   void set_data   ( const int in_number, const int in_status, const bool in_is_left )
   {
      number_  = in_number;
      status_  = in_status;
      is_left_ = in_is_left;
   }
};

#endif // INCLUDED_QBtLineData_h
