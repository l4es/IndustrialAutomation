#ifndef INCLUDED_QBtEvent_h
#define INCLUDED_QBtEvent_h
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
 * File         : QBtEvent.h
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 25.02.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include <QEvent>
#include <QVariant>
#include <vector>

/*------- class declaration:
-------------------------------------------------------------------*/
class QBtEvent : public QEvent
{
// ******* TYPES *******
public:
   enum {
      UNKNOWN = QEvent::User,
      BROWSER_CFG_CHANGED,
      DOCUMENT_CHANGED,
      DIFF_CFG_CHANGED,
      BROWSER_UPDATED,
      MOVE_FROM_LEFT,
      MOVE_FROM_RIGHT,
      REMOVE_FROM_LEFT,
      REMOVE_FROM_RIGHT,
      CHANGE_FROM_LEFT,
      CHANGE_FROM_RIGHT,
      FILES_CHANGED,
      FILES_UNCHANGED,
      READY_TO_MERGE,
      CANT_MERGE,
      SYNC,
      NUMERATION,
   };

public:
   // ------- 0 -------------------------------------------
   explicit QBtEvent( int const in_event_id )
   : QEvent( static_cast<QEvent::Type>( in_event_id ) )
   {
      data_.reserve( 0 );
   }
   //------- 1 --------------------------------------------
   explicit QBtEvent(   const int      in_event_id,
                        const QVariant in_val )
   : QEvent( static_cast<QEvent::Type>( in_event_id ) )
   {
      data_.reserve( 1 );
      data_.push_back( in_val );
   }
   //------- 2 --------------------------------------------
   explicit QBtEvent(   const int      in_event_id,
                        const QVariant in_val1,
                        const QVariant in_val2 )
   : QEvent( static_cast<QEvent::Type>( in_event_id ) )
   {
      data_.reserve( 2 );
      data_.push_back( in_val1 );
      data_.push_back( in_val2 );
   }
   //------- 3 --------------------------------------------
   explicit QBtEvent(   const int      in_event_id,
                        const QVariant in_val1,
                        const QVariant in_val2,
                        const QVariant in_val3 )
   : QEvent( static_cast<QEvent::Type>( in_event_id ) )
   {
      data_.reserve( 3 );
      data_.push_back( in_val1 );
      data_.push_back( in_val2 );
      data_.push_back( in_val3 );
   }
   //------- 4 --------------------------------------------
   explicit QBtEvent(   const int      in_event_id,
                        const QVariant in_val1,
                        const QVariant in_val2,
                        const QVariant in_val3,
                        const QVariant in_val4 )
   : QEvent( static_cast<QEvent::Type>( in_event_id ) )
   {
      data_.reserve( 4 );
      data_.push_back( in_val1 );
      data_.push_back( in_val2 );
      data_.push_back( in_val3 );
      data_.push_back( in_val4 );
   }
private:
   std::vector<QVariant> data_;
public:
   const QVariant& data( const int in_idx ) const { return data_.at( in_idx ); }
private:
   QBtEvent( const QBtEvent& );
   QBtEvent& operator=( const QBtEvent& );
};

#endif // INCLUDED_QBtEvent_h
