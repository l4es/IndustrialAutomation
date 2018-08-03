#ifndef INCLUDED_QBtDiffInfo_h
#define INCLUDED_QBtDiffInfo_h
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
 * File         : QBtDiffInfo.h
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 10.02.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include <QObject>
#include "QBtRange.h"

/*------- class declaration:
-------------------------------------------------------------------*/
class QBtDiffInfo
{
//******* CONSTRUCTION / DESTRUCTION *******
public:
   QBtDiffInfo( const QString& in_A    = QString(),
                int const      in_oper = QBtShared::NO_OPER,
                const QString& in_B    = QString() )
   : first_ ( QBtRange() )
   , second_( QBtRange() )
   {
      insert( in_A, in_oper, in_B );
   }
   QBtDiffInfo( const QBtDiffInfo& rhs )
   : first_ ( rhs.first_  )
   , second_( rhs.second_ )
   {}
   QBtDiffInfo& operator=( const QBtDiffInfo& rhs ) {
      if( this != &rhs ) {
         first_  = rhs.first_;
         second_ = rhs.second_;
      }
      return *this;
   }
//******* MEMBERS *******
private:
   QBtRange first_;
   QBtRange second_;

//******* METHODS *******
public:
   void insert ( const QString&, int, const QString& );
   //------------------------------------------------------
   QBtRange& first_range () {
      return first_;
   }
   const QBtRange& first_range() const {
      return first_;
   }
   QBtRange& second_range() {
      return second_;
   }
   const QBtRange& second_range() const {
      return second_;
   }
   bool is_valid() {
      return ( first_.is_valid() && second_.is_valid() );
   }
   bool is_valid() const {
      return ( first_.is_valid() && second_.is_valid() );
   }
};

#endif // INCLUDED_QBtDiffInfo_h
