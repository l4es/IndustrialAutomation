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
 * File         : QBtBrowser.cpp
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 04.02.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include "QBtBrowser.h"
#include "QBtLineData.h"
#include "QBtDiffProcess.h"
#include "QBtConfig.h"
#include "QBtShared.h"
#include "QBtEventsController.h"
#include "QBtOperator.h"
#include "QBtNumeration.h"
#include <QFile>
#include <QFileInfo>
#include <QFont>
#include <QTextStream>
#include <QApplication>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextLayout>
#include <QPainter>
#include <QMessageBox>
#include <QUrl>
#include <QtDebug>

/*------- constnats:
-------------------------------------------------------------------*/
const char* const QBtBrowser::READING     = QT_TR_NOOP( "File reading" );
const char* const QBtBrowser::NOT_FILE    = QT_TR_NOOP( "This is not a file:\n%1" );
const char* const QBtBrowser::NOT_EXISTS  = QT_TR_NOOP( "This file not exists:\n%1" );
const char* const QBtBrowser::READ_ERROR  = QT_TR_NOOP( "Error on read the file:\n%1\nError: %2" );
const char* const QBtBrowser::SAVING      = QT_TR_NOOP( "File saving" );
const char* const QBtBrowser::READ_ONLY   = QT_TR_NOOP( "This file is read only:\n%1" );
const char* const QBtBrowser::SAVE_ERROR  = QT_TR_NOOP( "Error on save the file:\n%1\nError: %2" );


//*******************************************************************
// QBtBrowser                                            CONSTRUCTOR
//*******************************************************************
QBtBrowser::QBtBrowser( const int         in_is_left,
                        QScrollBar* const in_scroll,
                        QWidget* const    in_parent )
: QTextBrowser ( in_parent )
, is_left_     ( in_is_left )
, scroll_      ( in_scroll )
, syntax_      ( new QBtSyntax( document() ) )
, operator_    ( 0 )
, numeration_  ( new QBtNumeration( this ) )
, fpath_       ( QString() )
, drop_fpath_  ( QString() )
, is_writable_ ( true )
, saved_nr1_   ( -1 )
, saved_nr2_   ( -1 )
, is_changed_  ( false )
{
   setAcceptDrops( true );
   setAcceptRichText( false );
   
   setFrameShadow( Plain );
   setFrameShape( NoFrame );
   setLineWidth( 0 );
   
   setLineWrapMode( NoWrap );
   setAutoFillBackground( true );
   update_looks();
   
   init_scrollbar();

   set_numeration( QBtConfig::instance()->numeration() );

   QBtEventsController* const ec = QBtEventsController::instance();
   ec->append( this, QBtEvent::NUMERATION );
   ec->append( this, QBtEvent::DOCUMENT_CHANGED );
}
// end of QBtBrowser

//*******************************************************************
// ~QBtBrowser                                            DESTRUCTOR
//*******************************************************************
QBtBrowser::~QBtBrowser()
{
   QBtEventsController::instance()->remove( this );
}
// end of ~QBtBrowser

//*******************************************************************
// updates_enable                                             PUBLIC
//*******************************************************************
void QBtBrowser::updates_enable( const bool in_stat )
{
   setUpdatesEnabled( in_stat );
   scroll_->setUpdatesEnabled( in_stat );
}
// end of updates_enable

//*******************************************************************
// resizeEvent                                     PRIVATE inherited
//*******************************************************************
void QBtBrowser::resizeEvent( QResizeEvent* const in_event )
{
   QTextBrowser::resizeEvent( in_event );
   update_numeration();   
}
// end of resizeEvent

//*******************************************************************
// customEvent                                     PRIVATE inherited
//*******************************************************************
void QBtBrowser::customEvent( QEvent* const in_event )
{
   QBtEvent* const event = dynamic_cast< QBtEvent* >( in_event );
   const int type = static_cast< int >( event->type() );

   switch( type ) {
      case QBtEvent::NUMERATION:
         {
            const bool state = event->data( 0 ).toBool();
            QBtConfig::instance()->numeration( state );
            QBtConfig::instance()->save_data();
            set_numeration( state );
         }
         break;
   }
}
// end of customEvent

//*******************************************************************
// set_numeration                                            PRIVATE
//*******************************************************************
void QBtBrowser::set_numeration( const bool in_visible )
{
   numeration_->setEnabled( in_visible );
   numeration_->setVisible( in_visible );
   update_numeration();
   update();
}
// end of set_numeration

//*******************************************************************
// update_looks                                              PRIVATE
//*******************************************************************
void QBtBrowser::update_looks()
{
   const QBtConfig* const cfg = QBtConfig::instance();
   
   {
      QPalette p = viewport()->palette();
      p.setColor( viewport()->backgroundRole(), cfg->text_background() );
      viewport()->setPalette( p );
   }
   {
      QPalette p = palette();
      p.setColor( backgroundRole(), cfg->text_background() );
      setPalette( p );
   }  

   syntax_->renew();
   renew();
   
}
// end of update_looks

//*******************************************************************
// update_numeration                                         PRIVATE
//*******************************************************************
void QBtBrowser::update_numeration()
{
   numeration_->update_looks();
   numeration_->update_geometry();
   setViewportMargins( numeration_->width(), 0, 0, 0 );
}
// end of update_numeration

//*******************************************************************
// read_file                                                  PUBLIC
//*******************************************************************
bool QBtBrowser::read_file( const QString& in_fpath, const bool in_is_drag )
{
   const QFileInfo fi( in_fpath );
   const QString fpath = fi.absoluteFilePath();

   if( !fi.exists() ) {
      QMessageBox::critical( this, tr( READING ), tr( NOT_EXISTS ).arg( fpath ) );
      return false;
   }
   if( !fi.isFile() ) {
      QMessageBox::critical( this, tr( READING ), tr( NOT_FILE ).arg( fpath ) );
      return false;
   }

   if( !fi.isWritable() ) {
      QMessageBox::information( this, tr( SAVING ), tr( READ_ONLY ).arg( fpath ) );
   }

   QFile file( fpath );
   if( file.open( QIODevice::ReadOnly ) ) {
      QBtShared::set_cursor( Qt::WaitCursor );
      clear();
      const QByteArray data = file.readAll();
      if( QBtConfig::instance()->utf8() ) {
         setPlainText( QString::fromUtf8( data ) );
      }
      else {
         setPlainText( QString::fromLocal8Bit( data ) );
      }

       file.close();
      fpath_ = fpath;
      is_writable_ = fi.isWritable();
      changed( false );
      
      renew();
      if( in_is_drag ) emit update_request();
      document()->adjustSize();
      QBtShared::restore_cursor();
      return true;
   }

   QMessageBox::critical( this, tr( READING ), tr( READ_ERROR ).arg( fpath ).arg( file.errorString() ) );
   return false;
}
// end of read_file

//*******************************************************************
// save_file                                                  PUBLIC
//*******************************************************************
bool QBtBrowser::save_file()
{
   if( fpath_.isEmpty() ) return false;
   if( !is_changed_ ) return true;
   
   const QFileInfo fi( fpath_ );
   if( !fi.isWritable() ) return false;

   // Tworzymy kopie zapasowa starego pliku.
   const QString tmp_fpath = fpath_ + ".bak";
   QFile::remove( tmp_fpath );
   QFile::rename( fpath_, tmp_fpath );

   // Zapisujemy plik na dysk.
   QFile file( fpath_ );
   if( file.open( QIODevice::Truncate | QIODevice::WriteOnly ) ) {
      QApplication::setOverrideCursor( Qt::WaitCursor );
      file.write( toPlainText().toLocal8Bit() );
      QApplication::restoreOverrideCursor();
      file.close();
      changed( false );
      return true;
   }
   
   QMessageBox::critical( this, tr( SAVING ), tr( SAVE_ERROR ).arg( fpath_ ).arg( file.errorString() ) );
   return false;
}
// end of save_file

//*******************************************************************
// renew                                                      PUBLIC
//*******************************************************************
void QBtBrowser::renew( const bool in_repaint )
{
   QBtDiffProcess* const dp = QBtDiffProcess::instance();
   int n = 1;
   
   QTextBlock block = document()->begin();
   while( block.isValid() ) {
      const int status = is_left_ ? dp->in_first( n ) : dp->in_second( n );

      if( block.userData() ) {
         dynamic_cast<QBtLineData*>( block.userData() )->set_data( n, status, is_left_ );
      }
      else {
         block.setUserData( new QBtLineData( n, status, is_left_ ) );
      }
      
      block = block.next();
      ++n;
   }

   update_numeration();
   if( in_repaint ) {
      viewport()->repaint();
      numeration_->repaint();
   }
}
// end of renew

//*******************************************************************
// changed                                                   PRIVATE
//*******************************************************************
void QBtBrowser::changed( const bool in_is_changed )
{
   is_changed_ = in_is_changed;
   QBtEventsController* const ec = QBtEventsController::instance();
   ec->send_event( QBtEvent::DOCUMENT_CHANGED, is_left_, is_changed_ );
}
// end of changed

//*******************************************************************
// fragment_position                                         PRIVATE
//*******************************************************************
void QBtBrowser::fragment_position( const int  in_nr1,
                                    const int  in_nr2,
                                          int& out_pos1,
                                          int& out_pos2 ) const
{
   QTextBlock block = document()->begin();
   while( block.isValid() ) {
      const QBtLineData* const info = dynamic_cast<QBtLineData*>( block.userData() );
      const int nr = info->get_number();
      if( nr < in_nr1 ) {
         block = block.next();
         continue;
      }
      if( nr == in_nr1 ) {
         out_pos1 = block.position();
      }
      if( nr == in_nr2 ) {
         out_pos2 = block.position() + block.length();
         break;
      }
      block = block.next();
   }
}
// end of fragment_position


//*******************************************************************
// clear browser                                              PUBLIC
//*******************************************************************
//remarks for PLCEdit
void QBtBrowser::clearBrowser()
{
  clear();
}


//*******************************************************************
// remove_range                                               PUBLIC
//*******************************************************************
void QBtBrowser::remove_range( const int  in_nr1,
                               const int  in_nr2,
                               const bool in_update )
{
   if( in_update ) save_pos();
   
   int pos1 = -1;
   int pos2 = -1;
   fragment_position( in_nr1, in_nr2, pos1, pos2 );

   if( ( pos1 != -1 ) && ( pos2 != -1 ) ) {
      QTextCursor cursor = textCursor();
      cursor.beginEditBlock();
      cursor.setPosition( pos1 );
      cursor.setPosition( pos2, QTextCursor::KeepAnchor );
      cursor.removeSelectedText();
      cursor.endEditBlock();
      changed( true );
   }
   if( in_update ) {
      document()->adjustSize();
      restore_pos();
   }
}
// end of remove_range

//*******************************************************************
// get_fragment                                               PUBLIC
//*******************************************************************
QString QBtBrowser::get_fragment( const int in_nr1, const int in_nr2 )
{
   QString retval = QString();
   
   int pos1 = -1;
   int pos2 = -1;
   fragment_position( in_nr1, in_nr2, pos1, pos2 );

   if( ( pos1 != -1 ) && ( pos2 != -1 ) ) {
      QTextCursor cursor = textCursor();
      cursor.setPosition( pos1 );
      cursor.setPosition( pos2, QTextCursor::KeepAnchor );
      retval = cursor.selectedText();
   }
   return retval;
}
// end of get_fragment

//*******************************************************************
// set_fragment                                               PUBLIC
//*******************************************************************
void QBtBrowser::set_fragment( const int      in_nr,
                               const QString& in_text,
                               const bool     in_update )
{
   if( in_update ) save_pos();
   
   int pos1 = -1;
   int pos2 = -1;

   if( 0 == in_nr ) {
      // Jest to sytuacja specjalna.
      // Normalnie nie ma linii 0. Linia 0 to pseudo-linia.
      // Przyslanie numeru zero oznacza, ze tekst nalezy wcisnac
      // przed 1-sza linia. Na poczatek tekstu.
      pos1 = pos2 = 0;
   }
   else {
      // To jest sytuacja normalna.
      // Przyslana 'porzadny' numer linii.
      fragment_position( in_nr, in_nr, pos1, pos2 );
   }

   if( pos2 != -1 ) {
      QTextCursor cursor = textCursor();
      cursor.beginEditBlock();
      cursor.setPosition( pos2 );
      cursor.insertText( in_text );
      cursor.endEditBlock();
      changed( true );
   }
   if( in_update ) {
       document()->adjustSize();
      restore_pos();
   }
}
// end of set_fragment

//*******************************************************************
// replace_fragment                                           PUBLIC
//*******************************************************************
void QBtBrowser::replace_fragment( const int      in_nr1,
                                   const int      in_nr2,
                                   const QString& in_text,
                                   const bool     in_update )
{
   if( in_update ) save_pos();
   
   int pos1 = -1;
   int pos2 = -1;
   
   fragment_position( in_nr1, in_nr2, pos1, pos2 );

   if( ( pos1 != -1 ) && ( pos2 != -1 ) ) {
      QTextCursor cursor = textCursor();
      cursor.beginEditBlock();
      cursor.setPosition( pos1 );
      cursor.setPosition( pos2, QTextCursor::KeepAnchor );
      cursor.insertText( in_text );
      cursor.endEditBlock();
      changed( true );
   }
   if( in_update ) {
      document()->adjustSize();
      restore_pos();
   }
}
// end of replace_fragment

//*******************************************************************
// get_y                                                      PUBLIC
//*******************************************************************
QString QBtBrowser::get_y( const int in_nr1,
                           const int in_nr2,
                           int&      out_yt,
                           int&      out_yb ) const
{
 
   QString retval = QString();
   
   QTextBlock block = document()->begin();
   while( block.isValid() ) {
      const QBtLineData* info = dynamic_cast< QBtLineData* >( block.userData() );
      const int nr = info->get_number();

      if( nr < in_nr1 ) {
         block = block.next();
         continue;
      }
      if( nr > in_nr2 ) {
         break;
      }

      if( in_nr1 == nr ) {
         out_yt = qRound( block.layout()->position().y() );
         retval = block.text();
      }
      if( in_nr2 == nr ) {
         const int block_yt = qRound( block.layout()->position().y() );
         const int block_h  = block.layout()->boundingRect().toRect().height();
         out_yb = block_yt + block_h - 1;
         break;
      }
      block = block.next();
   }
   if( 0 == in_nr1 ) out_yt = 2;
   if( 0 == in_nr2 ) out_yb = 2;

   return retval;
}
// end of get_y

//*******************************************************************
// paintEvent                                                PRIVATE
//*******************************************************************
void QBtBrowser::paintEvent( QPaintEvent* const in_event )
{
   const QBtConfig* const cfg = QBtConfig::instance();
   
   setTextColor( cfg->text_color() );
   QPainter p( viewport() );
   
   const int view_yt    = verticalScrollBar()->value();
   const int check_yt   = view_yt - 1;
   const int view_yb    = view_yt + viewport()->height() - 1;
   const int check_yb   = view_yb + 1;
   const int width      = viewport()->width();

   QTextBlock block = document()->begin();
   while( block.isValid() ) {
      const int block_yt = qRound( block.layout()->position().y() );
      const int block_h  = block.layout()->boundingRect().toRect().height();
      const int block_yb = block_yt + block_h - 1;
      
      if( block_yb < check_yt ) {
         block = block.next();
         continue;
      }
      if( block_yt > check_yb ) break;
      

      QBtLineData* const info = dynamic_cast<QBtLineData*>( block.userData() );
      if( info ) {
         const int block_yt = qRound( block.layout()->position().y() );
         const int block_h  = block.layout()->boundingRect().toRect().height();
         QBrush brush       = cfg->text_background_brush();

         if( info->get_status() ) {
            bool only_line = false;
            switch( info->get_status() ) {
               case QBtShared::CHANGE:
                  brush = cfg->chg_background_brush();
                  break;   
               case QBtShared::DELETE:
                  brush = cfg->del_background_brush();
                  only_line = !is_left_;
                  break;
               case QBtShared::APPEND:
                  brush = cfg->add_background_brush();
                  only_line = is_left_;
                  break;
            }
            if( only_line ) {
               const int yt = block_yt + block_h - 3;
               const int h = 2;
               p.fillRect( QRect( 0, yt - view_yt, width, h ), brush );
            }
            else {
               const int yt = block_yt - 1;
               const int h = block_h;
               p.fillRect( QRect( 0, yt - view_yt, width, h ), brush );
            }
         }
         else {
            const int yt = block_yt - 1;
            const int h = block_h;
            p.fillRect( QRect( 0, yt - view_yt, width, h ), brush );
         }

         if( 1 == info->get_number() ) {
            // Poniewaz kreske malujemy wzdluz gornej krawedzi linii nr. 1
            // malujemy ja tylko i wylacznie wtedy, gdy cala linia
            // jest juz widoczna.
            if( view_yt < 2 ) {
               QBtDiffProcess* const dp = QBtDiffProcess::instance();
               QBtDiffInfo di = is_left_ ? dp->info_for_nr_in_first( 0 )
                                         : dp->info_for_nr_in_second( 0 );
               if( di.is_valid() ) {
                  const QBrush brush = is_left_
                                     ? cfg->add_background_brush()
                                     : cfg->del_background_brush();
                  p.fillRect( QRect( 0, 0, width, 2 ), brush );
               }
            }
         }
      }
      block = block.next();
   }
   p.end();
   QTextBrowser::paintEvent( in_event );
}
// end of paintEvent

//*******************************************************************
// goto_line                                                  PUBLIC
//*******************************************************************
void QBtBrowser::goto_line( const int in_nr )
{
   QTextBlock block = document()->begin();
   while( block.isValid() ) {
      const QBtLineData* const info = dynamic_cast< QBtLineData* >( block.userData() );
      if( info ) {
         if( info->get_number() == in_nr  ) {
            set_cursor_position( block.position() );
            if( 1 == in_nr ) {
               verticalScrollBar()->setValue( 0 );
            }
            return;
         }
      }
      block = block.next();
   }
}
// end of goto_line

//*******************************************************************
// set_cursor_position                                       PRIVATE
//*******************************************************************
void QBtBrowser::set_cursor_position( const int in_position )
{
   QTextCursor cursor = textCursor();
   cursor.setPosition( in_position );
   setTextCursor( cursor );
   ensureCursorVisible();
}
// end of set_cursor_position

//*******************************************************************
// first_selected                                             PUBLIC
//-------------------------------------------------------------------
// Zwraca PIERWSZA WYBRANA linie WIDOCZNA W CALOSCI a ekranie.
// Wybrana tzn. taka ktora jest wskazana przez diff'a.
//*******************************************************************
int QBtBrowser::first_selected() const
{
   const int view_yt = verticalScrollBar()->value();
   const int view_yb = view_yt + viewport()->height() - 1;
   
   QTextBlock block = document()->begin();
   while( block.isValid() ) {
      const QBtLineData* const info = dynamic_cast<QBtLineData*>( block.userData() );
      if( !info ) return -1;
      
      const int block_yt = qRound( block.layout()->position().y() );
      const int block_h  = block.layout()->boundingRect().toRect().height();
      const int block_yb = block_yt + block_h - 1;

      if( ( block_yt > view_yt ) && (  block_yb < view_yb ) ) {
         if( info->get_status() != QBtShared::NO_OPER ) {
            return info->get_number();
         }
      }
      block = block.next();
   }
   return -1;
}
// end of first_selected

//*******************************************************************
// last_selected                                              PUBLIC
//-------------------------------------------------------------------
// Zwraca OSTATNIA WYBRANA linie WIDOCZNA W CALOSCI na ekranie.
// Wybrana tzn. taka ktora jest wskazana przez diff'a.
//*******************************************************************
int QBtBrowser::last_selected() const
{
   const int view_yt = verticalScrollBar()->value();
   const int view_yb = view_yt + viewport()->height() - 1;
   
   QTextBlock block = document()->end().previous();
   while( block.isValid() ) {
      const QBtLineData* const info = dynamic_cast<QBtLineData*>( block.userData() );
      if( !info ) return -1;
      
      const int block_yt = qRound( block.layout()->position().y() );
      const int block_h  = block.layout()->boundingRect().toRect().height();
      const int block_yb = block_yt + block_h - 1;
      
      if( ( block_yt > view_yt ) && ( block_yb < view_yb ) ) {
         if( info->get_status() != QBtShared::NO_OPER ) {
            return info->get_number();
         }
      }
      block = block.previous();
   }
   return -1;
}
// end of last_selected

//*******************************************************************
// first_visible                                              PUBLIC
//-------------------------------------------------------------------
// Zwaraca PIERWSZA WIDOCZNA W CALOSCI linie.
// Czy jest zaznaczona, czy nie, nie ma tu nic do rzeczy.
//*******************************************************************
int QBtBrowser::first_visible() const
{
   const int view_yt = verticalScrollBar()->value();
   
   QTextBlock block = document()->begin();
   while( block.isValid() ) {
      const QBtLineData* const info = dynamic_cast<QBtLineData*>( block.userData() );
      if( !info ) return -1;

      const int block_yt = qRound( block.layout()->position().y() );
      if( block_yt >= view_yt ) {
         return info->get_number();
      }
      block = block.next();
   }
   return -1;
}
// end of first_visible

//*******************************************************************
// last_visible                                               PUBLIC
//-------------------------------------------------------------------
// Zwraca OSTATNIA WIDOCZNA W CALOSCI linie.
// Czy jest zaznaczona, czy nie, nie ma tu nic do rzeczy.
//*******************************************************************
int QBtBrowser::last_visible() const
{
   const int view_yt = verticalScrollBar()->value();
   const int view_yb = view_yt + viewport()->height() - 1;
   
   QTextBlock block = document()->end().previous();
   while( block.isValid() ) {
      const QBtLineData* const info = dynamic_cast< QBtLineData* >( block.userData() );
      if( !info ) return -1;
      
      const int block_yt = qRound( block.layout()->position().y() );
      const int block_h  = block.layout()->boundingRect().toRect().height();
      const int block_yb = block_yt + block_h - 1;
      if( block_yb <= view_yb ) {
         return info->get_number();
      }
      block = block.previous();
   }
   return -1;
}
// end of last_visible

//*******************************************************************
// numeration_update                                          PUBLIC
//*******************************************************************
void QBtBrowser::numeration_update()
{
   numeration_->update();
}
// end of numeration_update


//###################################################################
//#                                                                 #
//#                       MY SCROLL BAR                             #
//#                                                                 #
//###################################################################


//*******************************************************************
// init_scrollbar                                            PRIVATE
//*******************************************************************
void QBtBrowser::init_scrollbar()
{
   scroll_->setMinimum ( verticalScrollBar()->minimum() );
   scroll_->setMaximum ( verticalScrollBar()->maximum() );
   scroll_->setPageStep( verticalScrollBar()->pageStep() );
   scroll_->setValue   ( verticalScrollBar()->value() );
      
   connect( verticalScrollBar(), SIGNAL( rangeChanged( int, int ) ),
            this               , SLOT  ( set_range   ( int, int ) ) );
   connect( scroll_            , SIGNAL( valueChanged( int ) ),
            this               , SLOT  ( set_value   ( int ) ) );
   connect( scroll_            , SIGNAL( sliderMoved ( int ) ),
            this               , SLOT  ( set_value   ( int ) ) );
               
   setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
   scroll_->setVisible( scroll_->maximum() != 0 );
}
// end of init_scrollbar

//*******************************************************************
// scrollContentsBy                                PRIVATE inherited
//-------------------------------------------------------------------
// Funkcje dziedziczymy od QTextBrowser po to, aby moc uaktualniac
// nasz scrollbar. Dotyczy to sytuacji gdy skrolowana jest
// zawartosc edytora przez uzycie kolka myszy lub klawiszy strzalek.
//*******************************************************************
void QBtBrowser::scrollContentsBy( int in_dx, int in_dy )
{
   QTextEdit::scrollContentsBy( in_dx, in_dy );
   scroll_->setValue( verticalScrollBar()->value() );
   if( in_dy ) emit scroll_request( in_dy );
}
// end of scrollContentsBy

//*******************************************************************
// scroll_by                                                  PUBLIC
//*******************************************************************
void QBtBrowser::scroll_by( const int in_dy ) const
{
   int pos = verticalScrollBar()->value() - in_dy;
   verticalScrollBar()->setValue( pos );
}
// end of scroll_by

//*******************************************************************
// set_range                                            PRIVATE slot
//*******************************************************************
void QBtBrowser::set_range( int in_min, int in_max ) const
{
   scroll_->setRange( in_min, in_max );
   scroll_->setPageStep( verticalScrollBar()->pageStep() );
   scroll_->setVisible( in_max );
}
// end of set_range

//*******************************************************************
// set_value                                            PRIVATE slot
//*******************************************************************
void QBtBrowser::set_value( const int in_value ) const
{
   verticalScrollBar()->setValue( in_value );
}
// end of set_value

//*******************************************************************
// dragEnterEvent                                  PRIVATE inherited
//*******************************************************************
void QBtBrowser::dragEnterEvent( QDragEnterEvent* const in_event )
{
   if( !in_event ) return;
   
   const QMimeData* const md = in_event->mimeData();
   if( md ) if( md->hasUrls() ) in_event->accept();
}
// end of dragEnterEvent

//*******************************************************************
// dragMoveEvent                                   PRIVATE inherited
//*******************************************************************
void QBtBrowser::dragMoveEvent( QDragMoveEvent* const in_event )
{
   if( !in_event ) return;
   
   const QMimeData* const md = in_event->mimeData();
   if( md ) if( md->hasUrls() ) in_event->accept();
}
// end of dragMoveEvent

//*******************************************************************
// dropEvent                                       PRIVATE inherited
//*******************************************************************
void QBtBrowser::dropEvent( QDropEvent* const in_event )
{
   if( !in_event ) return;
   
   const QMimeData* const md = in_event->mimeData();
   if( !md ) return;
   if( !md->hasUrls() ) return;

   const QFileInfo fi( md->urls().first().toLocalFile() );
   if( fi.isFile() ) read_file( drop_fpath_ = fi.absoluteFilePath(), true );
}
// end of dropEvent

//*******************************************************************
// save_pos                                                  PRIVATE
//*******************************************************************
void QBtBrowser::save_pos()
{
   saved_nr1_ = first_visible();
   saved_nr2_ = last_visible();
   //qDebug() << "save_pos: " << saved_nr1_ << ", " << saved_nr2_;
}   
// end of save_pos

//*******************************************************************
// restore_pos                                               PRIVATE
//*******************************************************************
void QBtBrowser::restore_pos()
{
   int first_nr = -1;
   int last_nr  = -1;

   {
      QTextBlock block = document()->begin();
      if( block.isValid() ) {
         const QBtLineData* const info = dynamic_cast< QBtLineData* >( block.userData() );
         if( info ) first_nr = info->get_number();
      }
   }
   {   
      QTextBlock block = document()->end().previous();
      if( block.isValid() ) {
         const QBtLineData* const info = dynamic_cast< QBtLineData* >( block.userData() );
         if( info ) last_nr = info->get_number();
      }
   }

   first_nr = ( first_nr != -1 ) ? qMin( last_nr, saved_nr1_ ) : saved_nr1_;
   last_nr  = ( last_nr  != -1 ) ? qMin( last_nr , saved_nr2_ ) : saved_nr2_;

   //qDebug() << "restore_pos: " << first_nr << ", " << last_nr;

   goto_line( first_nr );
   goto_line( last_nr  );
}
// end of restore_pos
