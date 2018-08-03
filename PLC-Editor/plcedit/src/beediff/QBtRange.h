#ifndef INCLUDED_QBtRange_h
#define INCLUDED_QBtRange_h
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
 * File         : QBtRange.h
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 14.02.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include "QBtShared.h"
#include "BtToken.h"
#include <QVector>

/*------- class declaration:
-------------------------------------------------------------------*/
class QBtRange
{
//******* CONSTRUCTION / DESTRUCTION *******
public:
   QBtRange(   const int      in_nr1    = -1,
               const int      in_oper   = QBtShared::NO_OPER,
               const int      in_nr2    = -1,
               const VTokens& in_tokens = VTokens() )
   : nr1_    ( in_nr1  )
   , nr2_    ( in_nr2  )
   , oper_   ( in_oper )
   , yt_     ( -1 )
   , yb_     ( -1 )
   , tokens_ ( in_tokens )
   {}

//******* MEMBERS *******
private:
   int     nr1_;   // first line number (in range)
   int     nr2_;   // last line number (in range)
   int     oper_;  // operation
   int     yt_;    // upper y block coordinate
   int     yb_;    // bottom y block coordinate
   VTokens tokens_;

//******* METHODS *******
public:
   void inc( const int in_value ) {
      nr1_ += in_value;
      nr2_ += in_value;
   }
   void dec( const int in_value ) {
      nr1_ -= in_value;
      nr2_ -= in_value;
   }
   void nr1    ( const int      in_value ) { nr1_    = in_value; }
   void nr2    ( const int      in_value ) { nr2_    = in_value; }
   void oper   ( const int      in_value ) { oper_   = in_value; }
   void yt     ( const int      in_value ) { yt_     = in_value; }
   void yb     ( const int      in_value ) { yb_     = in_value; }
   void tokens ( const VTokens& in_value ) { tokens_ = in_value; }

   int            nr1    () const { return nr1_;    }
   int            nr2    () const { return nr2_;    }
   int            oper   () const { return oper_;   }
   int            yt     () const { return yt_;     }
   int            yb     () const { return yb_;     }
   const VTokens& tokens () const { return tokens_; }

   bool is_valid() const
      { return ( ( nr1_ != -1 ) && ( nr2_ != -1 ) ); }
   bool is_nr_in_range ( const int in_nr ) const
      { return ( ( in_nr >= nr1_ ) && ( in_nr <= nr2_ ) ); }
   bool is_y_in_range ( const int in_y ) const
      { return ( ( in_y >= yt_ ) && ( in_y <= yb_ ) ); }
};

#endif // INCLUDED_QBtRange_h
