#ifndef INCLUDED_QBtEventsController_h
#define INCLUDED_QBtEventsController_h
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
 * File         : QBtEventsController.h
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 25.02.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include "QBtEvent.h"
#include <QVariant>
#include <QObject>
#include <QMutex>
#include <map>
#include <set>

/*------- class declaration:
-------------------------------------------------------------------*/
class QBtEventsController : public QObject
{
   Q_OBJECT

// ******* CONSTRUCTION/DESTRUCTION *******
private:
   QBtEventsController();
   ~QBtEventsController();
   QBtEventsController( const QBtEventsController& );
   QBtEventsController& operator=( const QBtEventsController& );

// ******* TYPES *******
private:
   typedef std::set<QObject*> Objects;
   typedef std::map<int, Objects> EventsRegister;

// ******* MEMBERS *******
private:
   EventsRegister evreg_;
   QMutex         mutex_;

// ******* METHODS *******
public:
   static QBtEventsController* instance();
   
   bool append    ( QObject* in_receiver, int in_event_id );
   void remove    ( QObject* in_receiver );
   void send_event( int      in_event_id );
   void send_event( int      in_event_id,
                    QVariant in_val );
   void send_event( int      in_event_id,
                    QVariant in_val1,
                    QVariant in_val2 );
   void send_event( int      in_event_id,
                    QVariant in_val1,
                    QVariant in_val2,
                    QVariant in_val3 );
   void send_event( int      in_event_id,
                    QVariant in_val1,
                    QVariant in_val2,
                    QVariant in_val3,
                    QVariant in_val4 );
private:
   bool exists( QObject* in_receiver, int in_event_id );
};

#endif // INCLUDED_QBtEventsController_h
