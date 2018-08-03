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
 * File         : QBtDiffProcess.cpp
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 08.02.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include "QBtDiffProcess.h"
#include "QBtBrowser.h"
#include "QBtConfig.h"
#include "QBtEventsController.h"
#include "BtStringTool.h"
#include <QtDebug>
using namespace std;


//*******************************************************************
// QBtDiffProcess                                CONSTRUCTOR private
//*******************************************************************
QBtDiffProcess::QBtDiffProcess( QObject* in_parent ) : QObject( in_parent )
, process_    ( new QProcess( this ) )
, cin_buffer_ ( QString() )
, err_buffer_ ( QString() )
, ranges_     ( QVector< QBtDiffInfo >() )
, nchg_       ( int() )
, nadd_       ( int() )
, ndel_       ( int() )
{
   connect( process_, SIGNAL( error                  ( QProcess::ProcessError ) ),
            this    , SLOT  ( error_slot             ( QProcess::ProcessError ) ) );
   connect( process_, SIGNAL( finished               ( int, QProcess::ExitStatus ) ),
            this    , SLOT  ( finished_slot          ( int, QProcess::ExitStatus ) ) );
   connect( process_, SIGNAL( readyReadStandardError () ),
            this    , SLOT  ( read_stderr            () ) );
   connect( process_, SIGNAL( readyReadStandardOutput() ),
            this    , SLOT  ( read_stdcin            () ) );
   connect( process_, SIGNAL( started                () ),
            this    , SLOT  ( started_slot           () ) );
}
// end of QBtDiffProcess

//*******************************************************************
// ~QBtDiffProcess                                DESTRUCTOR private
//*******************************************************************
QBtDiffProcess::~QBtDiffProcess()
{
   process_->close();
}
// end of ~QBtDiffProcess

//*******************************************************************
// instance                                                   PUBLIC
//*******************************************************************
QBtDiffProcess* const QBtDiffProcess::instance()
{
   static QBtDiffProcess* ptr = 0;
   if( !ptr ) ptr = new QBtDiffProcess;
   return ptr;
}
// end of instance

//*******************************************************************
// diff                                                       PUBLIC
//*******************************************************************
void QBtDiffProcess::diff( const QString& in_first, const QString& in_second )
{
   if( process_->state() != QProcess::Running ) {
      connect( this, SIGNAL( ready() ), parent(), SLOT( ready() ) );

      ranges_.clear();
      cin_buffer_.clear();
      err_buffer_.clear();
      nchg_ = nadd_ = ndel_ = int();

      const QBtConfig* const cfg = QBtConfig::instance();
      
      QString pars = QString();
      if( cfg->diff_case()          ) pars += "-i ";
      if( cfg->diff_tab_exp()       ) pars += "-E ";
      if( cfg->diff_wspace_amount() ) pars += "-b ";
      if( cfg->diff_wspace_all()    ) pars += "-w ";
      if( cfg->diff_blank_lines()   ) pars += "-B ";
      if( cfg->diff_as_text()       ) pars += "-a ";

      static const QString cmd = "diff %1 \"%2\" \"%3\"";
      process_->start( cmd.arg( pars ).arg( in_first ).arg( in_second ), QIODevice::ReadOnly );
   }
}
// end of diff

//*******************************************************************
// error_slot                                           PRIVATE slot
//*******************************************************************
void QBtDiffProcess::error_slot( QProcess::ProcessError )
{}
// end of error_slot

//*******************************************************************
// started_slot                                         PRIVATE slot
//*******************************************************************
void QBtDiffProcess::started_slot()
{}
// end of started_slot

//*******************************************************************
// finished                                             PRIVATE slot
//*******************************************************************
void QBtDiffProcess::finished_slot( int, QProcess::ExitStatus ) 
{
   parse();
   emit ready();
   disconnect( this, SIGNAL( ready() ), parent(), SLOT( ready() ) );
}
// end of finished

//*******************************************************************
// read_stderr                                          PRIVATE slot
//*******************************************************************
void QBtDiffProcess::read_stderr()
{
   err_buffer_ += process_->readAllStandardError();
}
// end of read_stderr

//*******************************************************************
// read_stdcin                                          PRIVATE slot
//*******************************************************************
void QBtDiffProcess::read_stdcin()
{
   cin_buffer_ += process_->readAllStandardOutput();
}
// end of read_stdcin

//*******************************************************************
// parse                                                     PRIVATE
//*******************************************************************
void QBtDiffProcess::parse()
{
   const QStringList lista = cin_buffer_.split( '\n', QString::SkipEmptyParts );
   QStringList::const_iterator it = lista.begin();
   const QStringList::const_iterator end = lista.end();
   while( it != end ) {
      if( it->at( 0 ).isNumber() ) {
              if( it->contains( 'c' ) ) insert_c( *it );
         else if( it->contains( 'a' ) ) insert_a( *it );
         else if( it->contains( 'd' ) ) insert_d( *it );
      }
      ++it;
   }
}
// end of parse

//*******************************************************************
// in_first                                                   PUBLIC
//*******************************************************************
int QBtDiffProcess::in_first( const int in_nr )
{
   QVector<QBtDiffInfo>::iterator it = ranges_.begin();
   const QVector<QBtDiffInfo>::iterator end = ranges_.end();
   while( it != end ) {
      if( it->first_range().is_nr_in_range( in_nr ) ) {
         return it->first_range().oper();
      }
      ++it;
   }
   return QBtShared::NO_OPER;
}
// end of in_first

//*******************************************************************
// in_second                                                  PUBLIC
//*******************************************************************
int QBtDiffProcess::in_second( const int in_nr )
{
   QVector<QBtDiffInfo>::iterator it = ranges_.begin();
   const QVector<QBtDiffInfo>::iterator end = ranges_.end();
   while( it != end ) {
      if( it->second_range().is_nr_in_range( in_nr ) ) {
         return it->second_range().oper();
      }
      ++it;
   }
   return QBtShared::NO_OPER;
}
// end of in_first

//*******************************************************************
// first_to_second                                            PUBLIC
//*******************************************************************
int QBtDiffProcess::first_to_second( const int in_nr )
{
   QVector< QBtDiffInfo >::iterator it = ranges_.begin();
   const QVector< QBtDiffInfo >::iterator end = ranges_.end();
   while( it != end ) {
      if( it->first_range().is_nr_in_range( in_nr ) ) {
         const int idx = in_nr - it->first_range().nr1();
         int retval = it->second_range().nr1() + idx;
         if( retval > it->second_range().nr2() ) {
            retval = it->second_range().nr2();
         }
         return retval;
      }
      ++it;
   }
   return -1;
}
// end of first_to_second

//*******************************************************************
// second_to_first                                            PUBLIC
//*******************************************************************
int QBtDiffProcess::second_to_first( const int in_nr )
{
   QVector< QBtDiffInfo >::iterator it = ranges_.begin();
   const QVector< QBtDiffInfo >::iterator end = ranges_.end();
   while( it != end ) {
      if( it->second_range().is_nr_in_range( in_nr ) ) {
         const int idx = in_nr - it->second_range().nr1();
         int retval = it->first_range().nr1() + idx;
         if( retval > it->first_range().nr2() ) {
            retval = it->first_range().nr2();
         }
         return retval;
      }
      ++it;
   }
   return -1;
}
// end of second_to_first

//*******************************************************************
// insert_c                                                  PRIVATE
//*******************************************************************
void QBtDiffProcess::insert_c( const QString& in_range )
{
   const int idx = in_range.indexOf( 'c' );
   const QString first = in_range.left( idx );
   const QString second = in_range.mid( idx + 1 );
   ranges_ << QBtDiffInfo( first, QBtShared::CHANGE, second );
   ++nchg_;
}
// end of insert_c

//*******************************************************************
// insert_a                                                  PRIVATE
//*******************************************************************
void QBtDiffProcess::insert_a( const QString& in_range )
{
   const int idx = in_range.indexOf( 'a' );
   const QString first = in_range.left( idx );
   const QString second = in_range.mid( idx + 1 );
   ranges_ << QBtDiffInfo( first, QBtShared::APPEND, second );
   ++nadd_;
}
// end of insert_a

//*******************************************************************
// insert_d                                                  PRIVATE
//*******************************************************************
void QBtDiffProcess::insert_d( const QString& in_range )
{
   const int idx = in_range.indexOf( 'd' );
   const QString first = in_range.left( idx );
   const QString second = in_range.mid( idx + 1 );
   ranges_ << QBtDiffInfo( first, QBtShared::DELETE, second );
   ++ndel_;
}
// end of insert_d

//*******************************************************************
// update                                                     PUBLIC
//*******************************************************************
void QBtDiffProcess::update(  QBtBrowser* const in_left_browser,
                              QBtBrowser* const in_right_browser )
{
   int yt = -1;
   int yb = -1;

   QVector< QBtDiffInfo >::iterator it = ranges_.begin();
   const QVector< QBtDiffInfo >::iterator end = ranges_.end();
   while( it != end ) {
      QBtRange& range_on_left  = it->first_range();
      const int nr1_1 = range_on_left.nr1();
      const int nr1_2 = range_on_left.nr2();
      const QString txt1 = in_left_browser->get_y( nr1_1, nr1_2, yt, yb );
      range_on_left.yt( yt );
      range_on_left.yb( yb );

      QBtRange& range_on_right = it->second_range();
      const int nr2_1 = range_on_right.nr1();
      const int nr2_2 = range_on_right.nr2();
      const QString txt2 = in_right_browser->get_y( nr2_1, nr2_2, yt, yb );
      range_on_right.yt( yt );
      range_on_right.yb( yb );

      if( QBtShared::CHANGE == range_on_left.oper() ) {
         if( ( nr1_1 == nr1_2 ) && ( nr2_1 == nr2_2 ) ) {
            VTokens all_tokens_1, all_tokens_2;
            VTokens tokens_1, tokens_2, tokens_3, tokens_4;

            create_tokens( txt1.toStdString(), all_tokens_1 );
            create_tokens( txt2.toStdString(), all_tokens_2 );
            compare_words( all_tokens_1, all_tokens_2, tokens_1, tokens_2 );
            remove_dups( tokens_1, tokens_2 );
            compare_words( all_tokens_2, all_tokens_1, tokens_4, tokens_3 );
            append_tokens( tokens_1, tokens_3 );
            append_tokens( tokens_2, tokens_4 );

            range_on_left.tokens ( tokens_1 );
            range_on_right.tokens( tokens_2 );
         }
      }
      ++it;
   }
}
// end of update

//*******************************************************************
// first_ranges                                               PUBLIC
//*******************************************************************
bool QBtDiffProcess::first_ranges( QVector<QBtRange>& out_ranges )
{
   out_ranges.clear();
   
         QVector<QBtDiffInfo>::iterator it  = ranges_.begin();
   const QVector<QBtDiffInfo>::iterator end = ranges_.end();
   while( it != end ) {
      out_ranges << it->first_range();
      ++it;
   }   

   out_ranges.squeeze();
   return ( false == out_ranges.empty() );
}
// end of first_ranges

//*******************************************************************
// second_ranges                                              PUBLIC
//*******************************************************************
bool QBtDiffProcess::second_ranges( QVector<QBtRange>& out_ranges )
{
   out_ranges.clear();
   
         QVector<QBtDiffInfo>::iterator it  = ranges_.begin();
   const QVector<QBtDiffInfo>::iterator end = ranges_.end();
   while( it != end ) {
      out_ranges << it->second_range();
      ++it;
   }   

   out_ranges.squeeze();
   return ( false == out_ranges.empty() );
}
// end of second_ranges

//*******************************************************************
// info_for_nr_in_first                                       PUBLIC
//*******************************************************************
QBtDiffInfo QBtDiffProcess::info_for_nr_in_first( const int  in_nr,
                                                  const bool in_delete )

{
   QBtDiffInfo retval = QBtDiffInfo();
   
         QVector<QBtDiffInfo>::iterator it  = ranges_.begin();
   const QVector<QBtDiffInfo>::iterator end = ranges_.end();
   while( it != end ) {
      if( it->first_range().is_nr_in_range( in_nr ) ) {
         retval = *it;
         if( in_delete ) {
            info_deleted( *it );
            ranges_.erase( it );
            QBtEventsController::instance()->send_event( QBtEvent::FILES_CHANGED );
            if( ranges_.empty() ) {
               QBtEventsController::instance()->send_event( QBtEvent::CANT_MERGE );
            }
         }
         break;
      }
      ++it;
   }
   return retval;
}
// end of info_for_nr_in_first

//*******************************************************************
// info_for_nr_in_second                                      PUBLIC
//*******************************************************************
QBtDiffInfo QBtDiffProcess::info_for_nr_in_second( const int  in_nr,
                                                   const bool in_delete )
{
   QBtDiffInfo retval = QBtDiffInfo();
   
         QVector<QBtDiffInfo>::iterator it  = ranges_.begin();
   const QVector<QBtDiffInfo>::iterator end = ranges_.end();
   while( it != end ) {
      if( it->second_range().is_nr_in_range( in_nr ) ) {
         retval = *it;
         if( in_delete ) {
            info_deleted( *it );
            ranges_.erase( it );
            QBtEventsController::instance()->send_event( QBtEvent::FILES_CHANGED );
            if( ranges_.empty() ) {
               QBtEventsController::instance()->send_event( QBtEvent::CANT_MERGE );
            }
         }
      }
      ++it;
   }
   return retval;
}
// end of info_for_nr_in_second

//*******************************************************************
// info_deleted                                              PRIVATE
//*******************************************************************
void QBtDiffProcess::info_deleted( QBtDiffInfo& in_info )
{
   switch( in_info.first_range().oper() ) {
      case QBtShared::CHANGE:
         if( nchg_ ) --nchg_;
         break;
      case QBtShared::DELETE:
         if( ndel_ ) --ndel_;
         break;
      case QBtShared::APPEND:
         if( nadd_ ) --nadd_;
         break;
   }
}
// end of info_deleted

//*******************************************************************
// decrement_left_after                                       PUBLIC
//*******************************************************************
void QBtDiffProcess::decrement_left_after( const int in_nr,
                                           const int in_delta )
{
   if( in_delta < 0 ) return;
   
   QVector< QBtDiffInfo >::iterator it = ranges_.begin();
   const QVector< QBtDiffInfo >::iterator end = ranges_.end();
   while( it != end ) {
      if( it->first_range().nr1() > in_nr ) {
         it->first_range().dec( in_delta );
      }
      ++it;
   }
}
// end of decrement_left_after

//*******************************************************************
// increment_left_after                                       PUBLIC
//*******************************************************************
void QBtDiffProcess::increment_left_after( const int in_nr,
                                           const int in_delta )
{
   if( in_delta < 0 ) return;
   
   QVector< QBtDiffInfo >::iterator it = ranges_.begin();
   const QVector< QBtDiffInfo >::iterator end = ranges_.end();
   while( it != end ) {
      if( it->first_range().nr1() > in_nr ) {
         it->first_range().inc( in_delta );
      }
      ++it;
   }
}
// end of increment_left_after

//*******************************************************************
// decrement_right_after                                      PUBLIC
//*******************************************************************
void QBtDiffProcess::decrement_right_after( const int in_nr,
                                            const int in_delta )
{
   if( in_delta < 0 ) return;
   
   QVector< QBtDiffInfo >::iterator it = ranges_.begin();
   const QVector< QBtDiffInfo >::iterator end = ranges_.end();
   while( it != end ) {
      if( it->second_range().nr1() > in_nr ) {
         it->second_range().dec( in_delta );
      }
      ++it;
   }
}
// end of decrement_right_after

//*******************************************************************
// increment_right_after                                      PUBLIC
//*******************************************************************
void QBtDiffProcess::increment_right_after( const int in_nr,
                                            const int in_delta )
{
   if( in_delta < 0 ) return;
   
   QVector< QBtDiffInfo >::iterator it = ranges_.begin();
   const QVector< QBtDiffInfo >::iterator end = ranges_.end();
   while( it != end ) {
      if( it->second_range().nr1() > in_nr ) {
         it->second_range().inc( in_delta );
      }
      ++it;
   }
}
// end of increment_right_after

//*******************************************************************
// is_valid                                                   PUBLIC
//*******************************************************************
bool QBtDiffProcess::is_valid() const
{
   return !ranges_.empty();
}
// end if is_valid

//*******************************************************************
// statistic                                                  PUBLIC
//*******************************************************************
void QBtDiffProcess::statistic( int& out_chg, int& out_add, int& out_del ) const
{
   out_chg = nchg_;
   out_add = nadd_;
   out_del = ndel_;
}
// end of statistic


//###################################################################
//#                                                                 #
//#                C O M P A R E   W O R D S                        #
//#                                                                 #
//###################################################################

//*******************************************************************
// create_tokens                                             PRIVATE
//-------------------------------------------------------------------
// Dokunuje podzialu przyslanego tekstu na poszczegolne slowa.
//*******************************************************************
void QBtDiffProcess::create_tokens( const string& in_text,
                                    VTokens&      out_data ) const
{
   BtStringTool st;

   list<string> lista = list<string>();
   st.split( in_text, lista );
   
   list<string>::const_iterator       it  = lista.begin();
   const list<string>::const_iterator end = lista.end();
   unsigned int                       pos = 0;
   
   while( it != end ) {
      const unsigned int n = it->length();
      pos = in_text.find( *it, pos );
      out_data.push_back( BtToken( pos, n, *it  ) );
      pos += n;
      ++it;
   }
   VTokens( out_data ).swap( out_data );
}
// end of create_tokens

//*******************************************************************
// compare_words                                             PRIVATE
//-------------------------------------------------------------------
// Szukamy roznic pomiedzy zbiorami tokens1 i tokens2.
// Slowa ktore sa w tokens1 a nie ma ich w tokens2 zapamietujemy
// w data1.
// Slowa ktore sa w tokens2 a nie ma ich w tokens1 zapamietujemy
// w data2.
//*******************************************************************
void QBtDiffProcess::compare_words( const VTokens& in_tokens1,
                                    const VTokens& in_tokens2,
                                          VTokens& out_data1 ,
                                          VTokens& out_data2 ) const
{
   VTokens().swap( out_data1 );
   VTokens().swap( out_data2 );

         VTokens::const_iterator it1  = in_tokens1.begin();
         VTokens::const_iterator it2  = in_tokens2.begin();
         VTokens::const_iterator cit  = in_tokens1.begin();
   const VTokens::const_iterator end1 = in_tokens1.end();
   const VTokens::const_iterator end2 = in_tokens2.end();

   while( it2 != end2 ) {
      // sprawdzmy czy slowoB znajduje sie w zbiorzeA
      VTokens::const_iterator i = it1;         // tymczasowy iterator po zbiorzeA
      while( i != end1 ) {                     // szukamy slowaB w zbiorzeA
         if( i->hash() == it2->hash() ) break; // znaleziono slowoB jest w zbiorzeA
         ++i;
      }
      // slowoB NIE ZNAJDUJE sie w zbiorzeA
      if( i == end1 ) {
         out_data2.push_back( *it2 );
         ++it2;                                // sprawdzmy nastepne slowoB
         continue;
      }
      // slowoB ZNAJDUJE sie w zbiorzeA
      VTokens::const_iterator j = cit;         // kopiujemy zbior A poczawszy od
      while( j != i ) {                        // poprzednio znalezionego slowa
         out_data1.push_back( *j );            // do wlasnie znalezionego
         ++j;
      }
      cit = it1 = ++i;
      ++it2;
   }

   // Dodaj resztke.
   out_data1.insert( out_data1.end(), it1, end1 );

   // Koncowe porzadki.
   VTokens( out_data1 ).swap( out_data1 );
   VTokens( out_data2 ).swap( out_data2 );
}
// end of compare_words

//*******************************************************************
// remove_dups                                               PRIVATE
//*******************************************************************
void QBtDiffProcess::remove_dups( VTokens& inout_data1,
                                  VTokens& inout_data2 ) const
{
   VTokens result1 = VTokens();
   VTokens result2 = VTokens();

   {  // Usuwamy duplikaty z pierwszego zbioru.
      VTokens::const_iterator it1 = inout_data1.begin();
      const VTokens::const_iterator end1 = inout_data1.end();

      while( it1 != end1 ) {
         //const string item1 = it1->text();
         const unsigned int hash1 = it1->hash();
         VTokens::const_iterator it2 = inout_data2.begin();
         const VTokens::const_iterator end2 = inout_data2.end();
         while( it2 != end2 ) {
            if( hash1 == it2->hash() ) break;
            ++it2;
         }
         if( it2 == end2 ) result1.push_back( *it1 );
         ++it1;
      }
   }
   {  // Usuwamy duplikaty z drugiego zbioru.
      VTokens::const_iterator it2 = inout_data2.begin();
      const VTokens::const_iterator end2 = inout_data2.end();

      while( it2 != end2 ) {
         const unsigned int hash2 = it2->hash();
         VTokens::const_iterator it1 = inout_data1.begin();
         const VTokens::const_iterator end1 = inout_data1.end();
         while( it1 != end1 ) {
            if( it1->hash() == hash2 ) break;
            ++it1;
         }
         if( it1 == end1 ) result2.push_back( *it2 );
         ++it2;
      }
   }
   // Koncowe porzadki.
   inout_data1.swap( result1 );
   inout_data2.swap( result2 );
}
// end of remove_dups

//*******************************************************************
// append_tokens                                             PRIVATE
//*******************************************************************
void QBtDiffProcess::append_tokens(       VTokens& inout_data1,
                                    const VTokens& in_data2   ) const
{
   VTokens result = VTokens();

   VTokens::const_iterator       it1  = inout_data1.begin();
   VTokens::const_iterator       it2  = in_data2.begin();
   const VTokens::const_iterator end1 = inout_data1.end();
   const VTokens::const_iterator end2 = in_data2.end();

   while( ( it1 != end1 ) && ( it2 != end2 ) ) {
      const int pos1 = it1->position();
      const int pos2 = it2->position();

      if( pos1 < pos2 ) {
         result.push_back( *it1 );
         ++it1;
         continue;
      }
      if( pos2 < pos1 ) {
         result.push_back( *it2 );
         ++it2;
         continue;
      }
      result.push_back( *it1 );
      ++it1;
      ++it2;
   }

   // Dodaj resztki.
   result.insert( result.end(), it1, end1 );
   result.insert( result.end(), it2, end2 );
   // Koncowe porzadki.
   inout_data1.swap( result );
}
// end of append_tokens
