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
 * File         : QBtEventsController.cpp
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 24.08.2007
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include "QBtEventsController.h"
#include <QApplication>
#include <QMutexLocker>
using namespace std;

//*******************************************************************
// QBtEventsController                           CONSTRUCTOR private
//*******************************************************************
QBtEventsController::QBtEventsController() : QObject()
{
   setParent( qApp );
}
// end of EventsControler

//*******************************************************************
// ~EventsContoller                               DESTRUCTOR private
//*******************************************************************
QBtEventsController::~QBtEventsController()
{
   QMutexLocker locker( &mutex_ );
   evreg_.clear();
}
// end of ~QBtEventsController

//*******************************************************************
// instance                                                   PUBLIC
//*******************************************************************
QBtEventsController* QBtEventsController::instance()
{
   static QBtEventsController* instance = 0;
   if( !instance ) instance = new QBtEventsController;
   return instance;
}
// end of instance

//*******************************************************************
// append                                                     PUBLIC
//-------------------------------------------------------------------
// Zarejestrowanie chetnego do odbierania wskazanego sygnalu.
//*******************************************************************
bool QBtEventsController::append( QObject* const in_receiver, const int in_event_id )
{
   QMutexLocker locker( &mutex_ );
   bool retval = true;

   // Jesli taka pozycja juz istnieje to nie zapamietujemy jej ponownie.
   if( exists( in_receiver, in_event_id ) ) {
      retval = false;
   }
   // Jesli nie ma takiej pozycji to ja zapamietujemy w rejestrze.
   else {
      EventsRegister::iterator it = evreg_.find( in_event_id );
      if( it != evreg_.end() ) {
         retval = it->second.insert( in_receiver ).second;
      }
   }
   return retval;
}
// end of append

//*******************************************************************
// remove                                                     PUBLIC
//*******************************************************************
void QBtEventsController::remove( QObject* const in_receiver )
{
   QMutexLocker locker( &mutex_ );

   if( !evreg_.empty() ) {
      EventsRegister::iterator it = evreg_.begin();
      const EventsRegister::iterator end = evreg_.end();
      while( it != end ) {
         it->second.erase( in_receiver );
         ++it;
      }
   }
}
// end of remove

//*******************************************************************
// exists                                                    PRIVATE
//*******************************************************************
bool QBtEventsController::exists( QObject* const in_receiver, const int in_event_id )
{
   bool retval = false;

   const EventsRegister::const_iterator it = evreg_.find( in_event_id );
   if( it != evreg_.end() ) {
      if( it->second.find( in_receiver ) != it->second.end() ) {
         retval = true;
      }
   }
   else {
      // Dopisuje do rejestru TYLKO identyfikator sygnalu (nie obiekt).
      evreg_.insert( EventsRegister::value_type( in_event_id, Objects() ) );
   }
   return retval;
}
// end of exists

//*******************************************************************
// send_event                                                 PUBLIC
//*******************************************************************

// WERSJA BEZ DANYCH
void QBtEventsController::send_event( const int in_event_id )
{
   QMutexLocker locker( &mutex_ );

   const EventsRegister::const_iterator rit = evreg_.find( in_event_id );
   if( rit != evreg_.end() ) {
      Objects::const_iterator it = rit->second.begin();
      const Objects::const_iterator end = rit->second.end();
      while( it != end ) {
         QApplication::postEvent( *it, new QBtEvent( in_event_id ) );
         ++it;
      }
   }
}
// WERSJA Z 1 DANA
void QBtEventsController::send_event(  const int in_event_id,
                                       const QVariant in_val )
{
   QMutexLocker locker( &mutex_ );

   const EventsRegister::const_iterator rit = evreg_.find( in_event_id );
   if( rit != evreg_.end() ) {
      Objects::const_iterator it = rit->second.begin();
      const Objects::const_iterator end = rit->second.end();
      while( it != end ) {
         QApplication::postEvent( *it, new QBtEvent( in_event_id, in_val ) );
         ++it;
      }
   }
}
// WERSJA Z 2 DANYMI
void QBtEventsController::send_event(  const int in_event_id,
                                       const QVariant in_val1,
                                       const QVariant in_val2 )
{
   QMutexLocker locker( &mutex_ );

   const EventsRegister::const_iterator rit = evreg_.find( in_event_id );
   if( rit != evreg_.end() ) {
      Objects::const_iterator it = rit->second.begin();
      const Objects::const_iterator end = rit->second.end();
      while( it != end ) {
         QApplication::postEvent( *it, new QBtEvent( in_event_id, in_val1, in_val2 ) );
         ++it;
      }
   }
}
// WERSJA Z 3 DANYMI
void QBtEventsController::send_event(  const int in_event_id,
                                       const QVariant in_val1,
                                       const QVariant in_val2,
                                       const QVariant in_val3 )
{
   QMutexLocker locker( &mutex_ );

   const EventsRegister::const_iterator rit = evreg_.find( in_event_id );
   if( rit != evreg_.end() ) {
      Objects::const_iterator it = rit->second.begin();
      const Objects::const_iterator end = rit->second.end();
      while( it != end ) {
         QApplication::postEvent( *it, new QBtEvent( in_event_id, in_val1, in_val2, in_val3 ) );
         ++it;
      }
   }
}
// WERSJA Z 4 DANYMI
void QBtEventsController::send_event(  const int in_event_id,
                                       const QVariant in_val1,
                                       const QVariant in_val2,
                                       const QVariant in_val3,
                                       const QVariant in_val4 )
{
   QMutexLocker locker( &mutex_ );

   const EventsRegister::const_iterator rit = evreg_.find( in_event_id );
   if( rit != evreg_.end() ) {
      Objects::const_iterator it = rit->second.begin();
      const Objects::const_iterator end = rit->second.end();
      while( it != end ) {
         QApplication::postEvent( *it, new QBtEvent( in_event_id, in_val1, in_val2, in_val3, in_val4 ) );
         ++it;
      }
   }
}
// end of send_event
