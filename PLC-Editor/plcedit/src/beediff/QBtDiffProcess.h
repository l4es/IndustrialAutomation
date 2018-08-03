#ifndef INCLUDED_QBtDiffProcess_h
#define INCLUDED_QBtDiffProcess_h
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
 * File         : QBtDiffProcess.h
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 08.02.2008
 *******************************************************************/

/*-------include files:
-------------------------------------------------------------------*/
#include <QObject>
#include "QBtRange.h"
#include "QBtDiffInfo.h"
#include "BtToken.h"
#include <QProcess>
#include <QVector>
#include <string>

/*------- forward declarations:
-------------------------------------------------------------------*/
class QProcess;
class QBtBrowser;

/*------- class declaration:
-------------------------------------------------------------------*/
class QBtDiffProcess : public QObject
{
   Q_OBJECT

//******* CONSTRUCTION / DESTRUCTION *******
private:
   QBtDiffProcess( QObject* = 0 );
   ~QBtDiffProcess();
   QBtDiffProcess( const QBtDiffProcess& );
   QBtDiffProcess& operator=( const QBtDiffProcess& );

//******* MEMBERS *******
private:
   QProcess* const      process_;
   QString              cin_buffer_;
   QString              err_buffer_;
   QVector<QBtDiffInfo> ranges_;
   int                  nchg_;
   int                  nadd_;
   int                  ndel_;

//******* METHODS *******
public:
   static QBtDiffProcess* const instance();
   
   void diff                  ( const QString&, const QString& );
   int  in_first              ( int );
   int  in_second             ( int );
   void update                ( QBtBrowser*, QBtBrowser* );
   bool first_ranges          ( QVector< QBtRange >& );
   bool second_ranges         ( QVector< QBtRange >& );
   int  first_to_second       ( int );
   int  second_to_first       ( int );
   bool is_valid              () const;
   void statistic             ( int&, int&, int& ) const;
   void decrement_left_after  ( int, int );
   void increment_left_after  ( int, int );
   void decrement_right_after ( int, int );
   void increment_right_after ( int, int );

   QBtDiffInfo info_for_nr_in_first  ( int, bool = false );
   QBtDiffInfo info_for_nr_in_second ( int, bool = false );
   const QVector<QBtDiffInfo>& ranges() const { return ranges_; }
private:
   void parse         ();
   void insert_c      ( const QString& );
   void insert_a      ( const QString& );
   void insert_d      ( const QString& );
   void create_tokens ( const std::string&, VTokens& ) const;
   void compare_words ( const VTokens&, const VTokens&, VTokens&, VTokens& ) const;
   void remove_dups   ( VTokens&, VTokens& ) const;
   void append_tokens ( VTokens&, const VTokens& ) const;
   void info_deleted  ( QBtDiffInfo& );
      
private slots:
   void error_slot    ( QProcess::ProcessError );
   void finished_slot ( int, QProcess::ExitStatus );
   void read_stderr   ();
   void read_stdcin   ();
   void started_slot  ();
signals:
   void ready         ();
};

#endif // INCLUDED_QBtDiffProcess_h
