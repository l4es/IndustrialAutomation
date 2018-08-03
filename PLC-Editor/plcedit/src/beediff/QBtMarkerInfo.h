#ifndef INCLUDED_QBtMarkerInfo_h
#define INCLUDED_QBtMarkerInfo_h
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
 * File         : QBtMarkerInfo.h
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 28.02.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include "QBtShared.h"
#include <QRect>


/*------- class declaration:
-------------------------------------------------------------------*/
class QBtMarkerInfo
{
//******* CONSTRUCTION / DESTRUCTION *******
public:
   QBtMarkerInfo()
   : rect_ ( QRect() )
   , oper_ ( QBtShared::NO_OPER )
   , nr1_  ( -1 )
   {}
   QBtMarkerInfo( const QRect& in_rect, const int in_oper, const int in_nr1 )
   : rect_ ( in_rect )
   , oper_ ( in_oper )
   , nr1_  ( in_nr1  )
   {}
   QBtMarkerInfo( const QBtMarkerInfo& rhs )
   : rect_ ( rhs.rect_ )
   , oper_ ( rhs.oper_ )
   , nr1_  ( rhs.nr1_ )
   {}
   QBtMarkerInfo& operator=( const QBtMarkerInfo& rhs )
   {
      if( this != &rhs ) {
         rect_ = rhs.rect_;
         oper_ = rhs.oper_;
         nr1_  = rhs.nr1_;
      }
      return *this;
   }

//******* METHODS *******
public:
   const QRect& rect () const { return rect_; }
   int          oper () const { return oper_; }
   int          nr1  () const { return nr1_;  }
   
   void rect ( const QRect& in_value ) { rect_ = in_value; }
   void oper ( const int    in_value ) { oper_ = in_value; }
   void nr1  ( const int    in_value ) { nr1_  = in_value; }

   bool is_valid() const { return ( nr1_ != -1 ); }

//******* MEMBERS *******
private:
   QRect rect_;
   int   oper_;
   int   nr1_;
};

#endif // INCLUDED_QBtMarkerInfo_h
