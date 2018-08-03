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
 * File         : QBtWorkspace.cpp
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 28.01.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include "QBtWorkspace.h"
#include "QBtBrowser.h"
#include "QBtIndicator.h"
#include "QBtSeparator.h"
#include "QBtSettings.h"
#include "QBtConfig.h"
#include "QBtShared.h"
#include "QBtDiffProcess.h"
#include "QBtEventsController.h"
#include "QBtOperator.h"
#include "QBtSaveQuestion.h"
#include "QBtLineData.h"
#include <QApplication>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QFileDialog>
#include <QScrollBar>
#include <QFile>
#include <QMessageBox>
#include <QIcon>
#include <QtDebug>

/*------- local constants:
-------------------------------------------------------------------*/
const char* const QBtWorkspace::NOT_FILE    = QT_TR_NOOP( "The file: %1,\nis not a regular file." );
const char* const QBtWorkspace::NO_SELECTED = QT_TR_NOOP( "Not selected" );
const char* const QBtWorkspace::FILES_EQUAL = QT_TR_NOOP( "Both files are the same." );


//*******************************************************************
// QBtWorkspace                                          CONSTRUCTOR
//*******************************************************************
QBtWorkspace::QBtWorkspace( QWidget* const in_parent ) : QWidget( in_parent )
, saveA_btn_   ( new QPushButton( QIcon( ":/img/floppy.png" ), QString() ) )
, saveB_btn_   ( new QPushButton( QIcon( ":/img/floppy.png" ), QString() ) )
, openA_btn_   ( new QPushButton( "..." ) )
, openB_btn_   ( new QPushButton( "..." ) )
, pathA_cbx_   ( new QComboBox )
, pathB_cbx_   ( new QComboBox )
, scrollA_bar_ ( new QScrollBar )
, scrollB_bar_ ( new QScrollBar )
, browserA_    ( new QBtBrowser  ( true , scrollA_bar_ ) )
, browserB_    ( new QBtBrowser  ( false, scrollB_bar_ ) )
, indicatorA_  ( new QBtIndicator( browserA_ ) )
, indicatorB_  ( new QBtIndicator( browserB_ ) )
, operatorA_   ( new QBtOperator ( browserA_, browserB_ ) )
, operatorB_   ( new QBtOperator ( browserB_, browserA_ ) )
, separator_   ( new QBtSeparator( browserA_, browserB_ ) )
, dirA_        ( QDir::homePath() )
, dirB_        ( dirA_ )
, can_scroll_  ( true )
{
   QBtDiffProcess::instance();
   QBtDiffProcess::instance()->setParent( this );

   saveA_btn_->setEnabled( false );
   saveB_btn_->setEnabled( false );
   pathA_cbx_->setDuplicatesEnabled( false );
   pathB_cbx_->setDuplicatesEnabled( false );
   
   const int min_w = 10 * QFontMetrics( font() ).width( 'X' );
   pathA_cbx_->setMinimumWidth( min_w );
   pathB_cbx_->setMinimumWidth( min_w );

   // Lewy browser ze scrollbarem.
   QHBoxLayout* const layoutA = new QHBoxLayout;
   layoutA->setSpacing( 0 );
   layoutA->setMargin( 0 );
   layoutA->addWidget( scrollA_bar_ );
   layoutA->addWidget( browserA_ );
   layoutA->addWidget( operatorA_ );
   QFrame* const containerA = new QFrame;
   containerA->setFrameShadow( QFrame::Sunken );
   containerA->setFrameShape( QFrame::StyledPanel );
   containerA->setLineWidth( 1 );
   containerA->setLayout( layoutA );

   // Prawy browser ze scrollbarem.
   QHBoxLayout* const layoutB = new QHBoxLayout;
   layoutB->setSpacing( 0 );
   layoutB->setMargin( 0 );
   layoutB->addWidget( operatorB_ );
   layoutB->addWidget( browserB_ );
   layoutB->addWidget( scrollB_bar_ );
   QFrame* const containerB = new QFrame;
   containerB->setFrameShadow( QFrame::Sunken );
   containerB->setFrameShape( QFrame::StyledPanel );
   containerB->setLineWidth( 1 );
   containerB->setLayout( layoutB );

   QGridLayout* const main_layout = new QGridLayout;
   main_layout->addWidget( pathA_cbx_ , 0, 1 );
   main_layout->addWidget( openA_btn_ , 0, 2 );
   main_layout->addWidget( saveA_btn_ , 0, 3 );
   main_layout->addWidget( pathB_cbx_ , 0, 5 );
   main_layout->addWidget( openB_btn_ , 0, 6 );
   main_layout->addWidget( saveB_btn_ , 0, 7 );
   main_layout->addWidget( indicatorA_, 1, 0 );
   main_layout->addWidget( containerA , 1, 1, 1, 3 );
   main_layout->addWidget( separator_ , 1, 4 );
   main_layout->addWidget( containerB , 1, 5, 1, 3 );
   main_layout->addWidget( indicatorB_, 1, 8 );

   main_layout->setColumnStretch( 1, 100 );
   main_layout->setColumnStretch( 5, 100 );
   main_layout->setMargin( 10 );
   main_layout->setSpacing( 0 );
   setLayout( main_layout );

   restore_config();

   connect( pathA_cbx_ , SIGNAL( activated ( const QString& ) ),
            this       , SLOT  ( lft_fpath_activated( const QString& ) ) );
   connect( pathB_cbx_ , SIGNAL( activated ( const QString& ) ),
            this       , SLOT  ( rgt_fpath_activated( const QString& ) ) );
   connect( openA_btn_ , SIGNAL( clicked             () ),
            this       , SLOT  ( lft_file_selection  () ) );
   connect( openB_btn_ , SIGNAL( clicked             () ),
            this       , SLOT  ( rgt_file_selection  () ) );
   connect( saveA_btn_ , SIGNAL( clicked             () ),
            this       , SLOT  ( save_A              () ) );
   connect( saveB_btn_ , SIGNAL( clicked             () ),
            this       , SLOT  ( save_B              () ) );
   connect( browserA_  , SIGNAL( scroll_request      ( int ) ),
            this       , SLOT  ( browser_A_scrolled( int ) ) );
   connect( browserB_  , SIGNAL( scroll_request      ( int ) ),
            this       , SLOT  ( browser_B_scrolled( int ) ) );
   connect( browserA_  , SIGNAL( update_request      () ),
            this       , SLOT  ( update_request      () ) );
   connect( browserB_  , SIGNAL( update_request      () ),
            this       , SLOT  ( update_request      () ) );

   QBtEventsController* const ec = QBtEventsController::instance();
   ec->append( this, QBtEvent::BROWSER_CFG_CHANGED );
   ec->append( this, QBtEvent::DIFF_CFG_CHANGED );
   ec->append( this, QBtEvent::REMOVE_FROM_LEFT );
   ec->append( this, QBtEvent::REMOVE_FROM_RIGHT );
   ec->append( this, QBtEvent::MOVE_FROM_LEFT );
   ec->append( this, QBtEvent::MOVE_FROM_RIGHT );
   ec->append( this, QBtEvent::CHANGE_FROM_LEFT );
   ec->append( this, QBtEvent::CHANGE_FROM_RIGHT );
   ec->append( this, QBtEvent::SYNC );
   ec->append( this, QBtEvent::DOCUMENT_CHANGED );


  //remarks for PLCEdit
   saveA_btn_->setVisible( false );
   saveB_btn_->setVisible( false );
}
// end of QBtWorkspace

//*******************************************************************
// ~QBtWorkspace                                          DESTRUCTOR
//*******************************************************************
QBtWorkspace::~QBtWorkspace()
{
   QBtEventsController::instance()->remove( this );
      
   remove_no_selected( pathA_cbx_ );
   remove_no_selected( pathB_cbx_ );
   //remarks for PLCEdit
   //save_config();
}
// end of ~QBtWorkspace

//*******************************************************************
// showEvent                                       PRIVATE inherited
//*******************************************************************
void QBtWorkspace::showEvent( QShowEvent* const in_event )
{
   const int h = pathA_cbx_->height();
   //----------------------------------
   openA_btn_->setMaximumHeight( h );
   openA_btn_->setMaximumWidth ( h );
   saveA_btn_->setMaximumHeight( h );
   saveA_btn_->setMaximumWidth ( h );
   //----------------------------------
   openB_btn_->setMaximumHeight( h );
   openB_btn_->setMaximumWidth ( h );
   saveB_btn_->setMaximumHeight( h );
   saveB_btn_->setMaximumWidth ( h );
   //----------------------------------
   QWidget::showEvent( in_event );
}
// end of resizeEvent

//*******************************************************************
// customEvent                                     PRIVATE inherited
//*******************************************************************
void QBtWorkspace::customEvent( QEvent* const in_event )
{
   QBtEvent* const event = dynamic_cast< QBtEvent* >( in_event );
   const int type = static_cast< int >( event->type() );

   switch( type ) {
      case QBtEvent::BROWSER_CFG_CHANGED:
         update_looks();
         break;
      case QBtEvent::DIFF_CFG_CHANGED:
         update_request();
         break;
      case QBtEvent::REMOVE_FROM_LEFT:
         remove_from_left( event->data( 0 ).toInt() );
         break;
      case QBtEvent::REMOVE_FROM_RIGHT:
         remove_from_right( event->data( 0 ).toInt() );
         break;
      case QBtEvent::MOVE_FROM_LEFT:
         move_from_left( event->data( 0 ).toInt() );
         break;
      case QBtEvent::MOVE_FROM_RIGHT:
         move_from_right( event->data( 0 ).toInt() );
         break;
      case QBtEvent::CHANGE_FROM_LEFT:
         change_from_left( event->data( 0 ).toInt() );
         break;
      case QBtEvent::CHANGE_FROM_RIGHT:
         change_from_right( event->data( 0 ).toInt() );
         break;
      case QBtEvent::SYNC:
         {
            const bool state = event->data( 0 ).toBool();
            QBtConfig::instance()->sync( state );
            QBtConfig::instance()->save_data();
         }
         break;
      case QBtEvent::DOCUMENT_CHANGED:
         document_changed( event->data( 0 ).toBool(), event->data( 1 ).toBool() );
         break;
   }
}
// end of customEvent

//*******************************************************************
// document_changed                                          PRIVATE
//*******************************************************************
void QBtWorkspace::document_changed( const bool in_isA, const bool in_is_changed )
{
   if( in_isA ) saveA_btn_->setEnabled( in_is_changed );
   else         saveB_btn_->setEnabled( in_is_changed );

   if( browserA_->changed() || browserB_->changed() ) {
      QBtEventsController::instance()->send_event( QBtEvent::FILES_CHANGED );
   }
   else {
      QBtEventsController::instance()->send_event( QBtEvent::FILES_UNCHANGED );
   }
}
// end of document_changed

//*******************************************************************
// save_config                                               PRIVATE
//*******************************************************************
void QBtWorkspace::save_config() const
{
   QBtSettings stt;
   {
      QStringList data = QStringList();
      const int n = pathA_cbx_->count();
      for( int i = 0; i < n; ++i ) {
         data << pathA_cbx_->itemText( i );
      }
      stt.save( QBtConfig::MAIN_WINDOW_GROUP + QBtConfig::LFT_FPATH_HISTORY_KEY, data );
   }
   {
      QStringList data = QStringList();
      const int n = pathB_cbx_->count();
      for( int i = 0; i < n; ++i ) {
         data << pathB_cbx_->itemText( i );
      }
      stt.save( QBtConfig::MAIN_WINDOW_GROUP + QBtConfig::RGT_FPATH_HISTORY_KEY, data );
   }
}
// end of save_config

//*******************************************************************
// restore_config                                            PRIVATE
//*******************************************************************
void QBtWorkspace::restore_config()
{
   QStringList dataA = QStringList();
   QStringList dataB = QStringList();
   
   QBtSettings stt;
   QVariant data;
   
   if( stt.read( QBtConfig::MAIN_WINDOW_GROUP + QBtConfig::LFT_FPATH_HISTORY_KEY, data ) ) {
      dataA = data.toStringList();
   }
   if( stt.read( QBtConfig::MAIN_WINDOW_GROUP + QBtConfig::RGT_FPATH_HISTORY_KEY, data ) ) {
      dataB = data.toStringList();
   }

   pathA_cbx_->addItems( dataA );
   pathB_cbx_->addItems( dataB );
   
   pathA_cbx_->insertItem( 0, tr( NO_SELECTED ) );
   pathA_cbx_->setCurrentIndex( 0 );
   pathB_cbx_->insertItem( 0, tr( NO_SELECTED ) );
   pathB_cbx_->setCurrentIndex( 0 );
}
// end of restore_config

//*******************************************************************
// lft_fpath_activated                                  PRIVATE slot
//-------------------------------------------------------------------
// Wybrano nowa pozycje z listy wczesniej uzywanych plikow w lewym
// combobox.
//*******************************************************************
void QBtWorkspace::lft_fpath_activated( const QString& in_fpath )
{
   if( in_fpath != tr( NO_SELECTED ) ) {
      if( in_fpath != browserA_->fpath() ) {
         if( browserA_->read_file( in_fpath ) ) {
            remove_no_selected( pathA_cbx_ );
         }
         else {
            update_lft_cbox( false );
         }
      }
      update_request();
   }
}
// end of lft_fpath_activated

//*******************************************************************
// rgt_fpath_activated                                  PRIVATE slot
//-------------------------------------------------------------------
// Wybrano nowa pozycje z listy wczesniej uzywanych plikow w prawym
// combobox.
//*******************************************************************
void QBtWorkspace::rgt_fpath_activated( const QString& in_fpath )
{
   if( in_fpath != tr( NO_SELECTED ) ) {
      if( in_fpath != browserB_->fpath() ) {
         if( browserB_->read_file( in_fpath ) ) {
            remove_no_selected( pathB_cbx_ );
         }
         else {
            update_rgt_cbox( false );
         }
      }
      update_request();
   }
}
// end of rgt_fpath_activated


//*******************************************************************
// clear both browsers                                   PUBLIC slot
//*******************************************************************
//remarks for PLCEdit
void QBtWorkspace::clearBrowserA()
{
  browserA_->clearBrowser();
  pathA_cbx_->clear();
  update_looks();
}

void QBtWorkspace::clearBrowserB()
{
  browserB_->clearBrowser();
  pathB_cbx_->clear();
  update_looks();
}

void QBtWorkspace::setFontAndTabWidthBrowserA(QFont font, int tabStopWidth)
{
  browserA_->setFont(font);
  browserA_->setTabStopWidth(tabStopWidth);
}

void QBtWorkspace::setFontAndTabWidthBrowserB(QFont font, int tabStopWidth)
{
  browserB_->setFont(font);
  browserB_->setTabStopWidth(tabStopWidth);
}


//*******************************************************************
// lft_file_selection                                   PUBLIC slot
//-------------------------------------------------------------------
// Uzytkownik nacisnal lewy kropki-przycisk.
// Czyli chce za pomoca dialogu wskazac, ktory plik chce wczytac
// do lewego okna.
//*******************************************************************
//remarks for PLCEdit
void QBtWorkspace::lft_file_selection()
{
   const QString fpath = QFileDialog::getOpenFileName( this, QString(), dirA_ );
   if( !fpath.isEmpty() ) {
      dirA_ = QFileInfo( fpath ).absolutePath();
      lft_read_file( fpath );
   }
}
// end of lft_file_selection

//*******************************************************************
// lft_read_file                                              PUBLIC
//-------------------------------------------------------------------
// Uzytkownik wybral plik w dialog.
//*******************************************************************
//remarks for PLCEdit
void QBtWorkspace::lft_read_file( const QString& in_fpath )
{
   if( !in_fpath.isEmpty() ) {
      if( !browserA_->read_file( in_fpath ) ) {
         update_lft_cbox( false );
      }
   }
   update_request();
}
// end of lft_read_file

//*******************************************************************
// rgt_file_selection                                   PUBLIC slot
//-------------------------------------------------------------------
// Uzytkownik nacisnal prawy kropki-przycisk.
// Czyli chce za pomoca dialogu wskazac, ktory plik chce wczytac
// do prawego okna.
//*******************************************************************
//remarks for PLCEdit
void QBtWorkspace::rgt_file_selection()
{
   const QString fpath = QFileDialog::getOpenFileName( this, QString(), dirB_ );
   if( !fpath.isEmpty() ) {
      dirB_ = QFileInfo( fpath ).absolutePath();
      rgt_read_file( fpath );
   }
}
// end of rgt_file_selection

//*******************************************************************
// save_A                                               PRIVATE slot
//*******************************************************************
void QBtWorkspace::save_A()
{
   browserA_->save_file();
}
// end of save_A

//*******************************************************************
// save_B                                               PRIVATE slot
//*******************************************************************
void QBtWorkspace::save_B()
{
   browserB_->save_file();
}
// end of save_B

//*******************************************************************
// rgt_read_file                                              PUBLIC
//-------------------------------------------------------------------
// Uzytkownik wybral plik w dialogu.
//*******************************************************************
//remarks for PLCEdit
void QBtWorkspace::rgt_read_file( const QString& in_fpath )
{
   if( !in_fpath.isEmpty() ) {
      if( !browserB_->read_file( in_fpath ) ) {
         update_rgt_cbox( false );
      }
   }
   update_request();
}
// rgt_read_file

//*******************************************************************
// update_lft_cbox                                           PRIVATE
//*******************************************************************
void QBtWorkspace::update_lft_cbox( const bool in_ok )
{
   remove_no_selected( pathA_cbx_ );
   const int idx = pathA_cbx_->findText( browserA_->fpath() );
   if( idx != -1 ) pathA_cbx_->removeItem( idx );
   
   if( in_ok ) {
      pathA_cbx_->insertItem( 0, browserA_->fpath() );
      pathA_cbx_->setCurrentIndex( 0 );
      check_fpath_history( pathA_cbx_ );
   }
   else {
      check_fpath_history( pathA_cbx_ );
      pathA_cbx_->insertItem( 0, tr( NO_SELECTED ) );
      pathA_cbx_->setCurrentIndex( 0 );
   }
}
// end of update_lft_cbox

//*******************************************************************
// update_rgt_cbox                                           PRIVATE
//*******************************************************************
void QBtWorkspace::update_rgt_cbox( const bool in_ok )
{
   remove_no_selected( pathB_cbx_ );
   const int idx = pathB_cbx_->findText( browserB_->fpath() );
   if( idx != -1 ) pathB_cbx_->removeItem( idx );   
   
   if( in_ok ) {      
      pathB_cbx_->insertItem( 0, browserB_->fpath() );
      pathB_cbx_->setCurrentIndex( 0 );
      check_fpath_history( pathB_cbx_ );
   }
   else {
      check_fpath_history( pathB_cbx_ );
      pathB_cbx_->insertItem( 0, tr( NO_SELECTED ) );
      pathB_cbx_->setCurrentIndex( 0 );
   }
}
// end of update_rgt_cbox

//*******************************************************************
// check_fpath_history                                       PRIVATE
//-------------------------------------------------------------------
// Funkcja kontroluje liczbe plikow na liscie wskazanego combobox'a.
// Nadmiarowe pliki sa usuwane (od konca).
//*******************************************************************
void QBtWorkspace::check_fpath_history( QComboBox* const inout_cbox )
{
   const int max = QBtConfig::instance()->fpath_history_size();
   int n = inout_cbox->count();
   if( n <= max ) return;
   
   while( n > max ) {
      inout_cbox->removeItem( n - 1 );
      n = inout_cbox->count();
   }
}
// end of check_fpath_history

//*******************************************************************
// save                                                       PUBLIC
//*******************************************************************
void QBtWorkspace::save()
{
   browserA_->save_file();
   browserB_->save_file();
}
// end of save

//*******************************************************************
// save_on_exit                                              PRIVATE
//*******************************************************************
bool QBtWorkspace::save_on_exit()
{
   if( browserA_->changed() || browserB_->changed() ) {
      QBtSaveQuestion dialog( browserA_->changed(), browserB_->changed(), this );
      const int answer = dialog.exec();
      if( answer ) {
         switch( answer ) {
            case 1:
               browserA_->save_file();
               break;
            case 2:
               browserB_->save_file();
               break;
            case 3:
               browserA_->save_file();
               browserB_->save_file();
               break;
         }
         return true;
      }
      return false;
   }
   return true; 
}
// end of save_on_exit

//*******************************************************************
// remove_no_selected                                        PRIVATE
//------------------------------------------------------------------
// Z listy zawartej combobox'ie usuwamy pozycje NO_SELECTED,
// ktora uzywamy tylko po starcie programu, przed wczytaniem
// pierwszego pliku.
//*******************************************************************
void QBtWorkspace::remove_no_selected( QComboBox* const inout_cbox )
{
   const QString text = tr( NO_SELECTED );
   int idx = inout_cbox->findText( text );

   while( idx != -1 ) {
      inout_cbox->removeItem( idx );
      idx = inout_cbox->findText( text );
   }
}
// end of remove_no_selected

//*******************************************************************
// update_request                                            PRIVATE
//*******************************************************************
void QBtWorkspace::update_request()
{
   if( !browserA_->fpath().isEmpty() && !browserB_->fpath().isEmpty() ) {
      QBtDiffProcess::instance()->diff( browserA_->fpath(), browserB_->fpath() );
   }
}
// end of update_request

//*******************************************************************
// ready                                                PRIVATE slot
//*******************************************************************
void QBtWorkspace::ready()
{
   update_lft_cbox();
   update_rgt_cbox();
   update_all();
   browserA_->rehighlight();
   browserB_->rehighlight();

   are_the_same();
}
// end of ready

//*******************************************************************
// update_looks                                              PRIVATE         
//*******************************************************************
void QBtWorkspace::update_looks()
{
   browserA_->update_looks();
   browserB_->update_looks();
   operatorA_->update_looks();
   operatorB_->update_looks();
   update_all();   
}
// end of update_looks

//*******************************************************************
// browser_A_scrolled                                 PRIVATE slot
//*******************************************************************
void QBtWorkspace::browser_A_scrolled( const int in_dy )
{
   if( QBtConfig::instance()->sync() && can_scroll_ ) {
      can_scroll_ = false;
      browserB_->scroll_by( in_dy );
      const int nr  = ( in_dy >= 0 ) ? browserA_->first_selected()
                                     : browserA_->last_selected();
      if( nr != -1 ) {
         const int snr = QBtDiffProcess::instance()->first_to_second( nr );
         if( snr != -1 ) browserB_->goto_line( snr ? snr : 1 );
      }
      can_scroll_ = true;
   }
   browserA_->numeration_update();
   browserB_->numeration_update();
   operatorA_->update();
   separator_->update();
}
// end of browser_A_scrolled

//*******************************************************************
// browser_B_scrolled                                 PRIVATE slot
//*******************************************************************
void QBtWorkspace::browser_B_scrolled( const int in_dy )
{
   if( QBtConfig::instance()->sync() && can_scroll_ ) {
      can_scroll_ = false;
      browserA_->scroll_by( in_dy );
      const int nr  = ( in_dy >= 0 ) ? browserB_->first_selected()
                                     : browserB_->last_selected();
      if( nr != -1 ) {
         const int fnr = QBtDiffProcess::instance()->second_to_first( nr );
         if( fnr != -1 ) browserA_->goto_line( fnr ? fnr : 1 );
      }
      can_scroll_ = true;
   }
   browserA_->numeration_update();
   browserB_->numeration_update();
   operatorB_->update();
   separator_->update();
}
// end of browser_B_scrolled

//*******************************************************************
// befor_automation                                          PRIVATE
//*******************************************************************
void QBtWorkspace::befor_automation()
{
   updates_enable( false );
   browserA_->save_pos();
   browserB_->save_pos();
}
// end of befor_automation

//*******************************************************************
// after_automation                                          PRIVATE
//*******************************************************************
void QBtWorkspace::after_automation()
{
   browserA_->rehighlight();
   browserB_->rehighlight();
   browserA_->adjust_size();
   browserB_->adjust_size();
   update_all();

   browserA_->restore_pos();
   browserB_->restore_pos();
   updates_enable( true );
}
// end of after_automation

//*******************************************************************
// del_on_A                                                   PUBLIC
//*******************************************************************
void QBtWorkspace::del_on_A()
{
   befor_automation();

   QTextBlock block = browserA_->document()->begin();
   if( block.isValid() && block.userData() ) {
      QBtDiffProcess* const dp = QBtDiffProcess::instance();
      while( block.isValid() ) {
         const QBtLineData* const info = dynamic_cast<QBtLineData*>( block.userData() );
         const int opc = info->get_status();
         if( QBtShared::DELETE == opc ) {
            QBtDiffInfo di = dp->info_for_nr_in_first( info->get_number(), true );
            if( di.is_valid() ) {
               const QTextBlock prv_block = block.previous();
               const int nr1 = di.first_range().nr1();
               const int nr2 = di.first_range().nr2();
               dp->decrement_left_after( nr1, nr2 - nr1 + 1 );
               browserA_->remove_range( nr1, nr2, false );
               browserA_->renew( false );
               
               block = ( prv_block.isValid() ) ? prv_block.next() : browserA_->document()->begin();
               continue;
            }
         }
         block = block.next();
      }
   }
   after_automation();
   are_the_same();
}
// end of del_on_A

//*******************************************************************
// del_on_B                                                   PUBLIC
//*******************************************************************
void QBtWorkspace::del_on_B()
{
   befor_automation();

   QTextBlock block = browserB_->document()->begin();
   if( block.isValid() && block.userData() ) {
      QBtDiffProcess* const dp = QBtDiffProcess::instance();
      while( block.isValid() ) {
         const QBtLineData* const info = dynamic_cast<QBtLineData*>( block.userData() );
         const int opc = info->get_status();
         if( QBtShared::APPEND == opc ) {
            QBtDiffInfo di = dp->info_for_nr_in_second( info->get_number(), true );
            if( di.is_valid() ) {
               const QTextBlock prv_block = block.previous();
               const int nr1 = di.second_range().nr1();
               const int nr2 = di.second_range().nr2();
               dp->decrement_right_after( nr1, nr2 - nr1 + 1 );
               browserB_->remove_range( nr1, nr2, false );
               browserB_->renew( false );
               
               block = ( prv_block.isValid() ) ? prv_block.next() : browserB_->document()->begin();
               continue;
            }
         }
         block = block.next();
      }
   }
   after_automation();
   are_the_same();
}
// end of del_on_B

//*******************************************************************
// merge_to_A                                                 PUBLIC
//*******************************************************************
void QBtWorkspace::merge_to_A()
{
   befor_automation();
   
   QTextBlock block = browserB_->document()->begin();
   if( block.isValid() && block.userData() ) {
      QBtDiffProcess* const dp = QBtDiffProcess::instance();
      while( block.isValid() ) {
         const QBtLineData* const info = dynamic_cast<QBtLineData*>( block.userData() );
         const int opc = info->get_status();
         if( ( QBtShared::CHANGE == opc ) || ( QBtShared::APPEND == opc ) ) {
            QBtDiffInfo di = dp->info_for_nr_in_second( info->get_number(), true );
            if( di.is_valid() ) {
               const int nr1 = di.second_range().nr1();
               const int nr2 = di.second_range().nr2();
               const int nr3 = di.first_range().nr1();
               const int nr4 = di.first_range().nr2();
               const int d1  = nr2 - nr1 + 1;
               const int d2  = nr4 - nr3 + 1;
               const QString fragment = browserB_->get_fragment( nr1, nr2 );
               if( QBtShared::CHANGE == opc ) {
                  dp->increment_left_after( nr3, d1 - d2 );
                  browserA_->replace_fragment( nr3, nr4, fragment, false );
               }
               else {
                  dp->increment_left_after( nr3, d1 );
                  browserA_->set_fragment( nr3, fragment, false );
               }
               browserA_->renew( false );
            }
         }
         block = block.next();
      }
   }

   after_automation();
   are_the_same();
}
// end of merge_to_A

//*******************************************************************
// merge_to_B                                                 PUBLIC
//*******************************************************************
void QBtWorkspace::merge_to_B()
{
   befor_automation();
   
   QTextBlock block = browserA_->document()->begin();
   if( block.isValid() && block.userData() ) {
      QBtDiffProcess* const dp  = QBtDiffProcess::instance();
      while( block.isValid() ) {
         const QBtLineData* const info = dynamic_cast<QBtLineData*>( block.userData() );
         const int opc = info->get_status();
         if( ( QBtShared::CHANGE == opc ) || ( QBtShared::DELETE == opc ) ) {
            QBtDiffInfo  di  = dp->info_for_nr_in_first( info->get_number(), true );
            if( di.is_valid() ) {
               const int nr1 = di.first_range().nr1();
               const int nr2 = di.first_range().nr2();
               const int nr3 = di.second_range().nr1();
               const int nr4 = di.second_range().nr2();
               const int d1  = nr2 - nr1 + 1;
               const int d2  = nr4 - nr3 + 1;

               const QString fragment = browserA_->get_fragment( nr1, nr2 );
               if( QBtShared::CHANGE == opc ) {
                  dp->increment_right_after( nr3, d1 - d2 );
                  browserB_->replace_fragment( nr3, nr4, fragment, false );
               }
               else {
                  browserB_->set_fragment( nr3, fragment, false );
                  dp->increment_right_after( nr3, d1 );
               }
               browserB_->renew( false );
            }
         }
         block = block.next();
      }
   }

   after_automation();
   are_the_same();
}
// end of merge_to_B

//*******************************************************************
// remove_from_left                                          PRIVATE
//*******************************************************************
void QBtWorkspace::remove_from_left( const int in_nr, const bool in_update )
{
   if( in_update ) {
      updates_enable( false );
      browserA_->save_pos();
   }
   
   QBtDiffProcess* const dp  = QBtDiffProcess::instance();
   QBtDiffInfo           di  = dp->info_for_nr_in_first( in_nr, true );
   const int             nr1 = di.first_range().nr1();
   const int             nr2 = di.first_range().nr2();
   
   dp->decrement_left_after( nr1, nr2 - nr1 + 1 );
   browserA_->remove_range( nr1, nr2, in_update );
   
   if( in_update ) {
      update_all();
      browserA_->restore_pos();
      updates_enable( true );
      are_the_same();
   }
}
// end of remove_from_left

//*******************************************************************
// remove_from_right                                         PRIVATE
//*******************************************************************
void QBtWorkspace::remove_from_right( const int in_nr, const bool in_update )
{
   if( in_update ) {
      updates_enable( false );
      browserB_->save_pos();
   }
   
   QBtDiffProcess* const dp  = QBtDiffProcess::instance();
   QBtDiffInfo           di  = dp->info_for_nr_in_second( in_nr, true );
   const int             nr1 = di.second_range().nr1();
   const int             nr2 = di.second_range().nr2();
   
   dp->decrement_right_after( nr1, nr2 - nr1 + 1 );
   browserB_->remove_range( nr1, nr2, in_update );
   
   if( in_update ) {
      update_all();
      browserB_->restore_pos();
      updates_enable( true );
      are_the_same();
   }
}
// end of remove_from_right

//*******************************************************************
// move_from_left                                            PRIVATE
//-------------------------------------------------------------------
// Lewy browser posiada kod, ktorego nie ma w prawym browserze.
// Kopiujemy z lewej na prawo.
//*******************************************************************
void QBtWorkspace::move_from_left( const int in_nr, const bool in_update )
{
   if( in_update ) {
      updates_enable( false );
      browserB_->save_pos();
   }

   QBtDiffProcess* const dp  = QBtDiffProcess::instance();
   QBtDiffInfo           di  = dp->info_for_nr_in_first( in_nr, true );
   const int             nr1 = di.first_range().nr1();
   const int             nr2 = di.first_range().nr2();
   const int             nr  = di.second_range().nr1();
   
   dp->increment_right_after( nr, nr2 - nr1 + 1 );
   const QString fragment = browserA_->get_fragment( nr1, nr2 );
   browserB_->set_fragment( nr, fragment, in_update );
   
   if( in_update ) {
      update_all();
      browserB_->restore_pos();
      updates_enable( true );
      are_the_same();
   }
}
// end of move_from_left

//*******************************************************************
// move_from_rigth                                           PRIVATE
//*******************************************************************
void QBtWorkspace::move_from_right( const int in_nr, const bool in_update )
{
   if( in_update ) {
      updates_enable( false );
      browserA_->save_pos();
   }
   
   QBtDiffProcess* const dp  = QBtDiffProcess::instance();
   QBtDiffInfo           di  = dp->info_for_nr_in_second( in_nr, true );
   const int             nr1 = di.second_range().nr1();
   const int             nr2 = di.second_range().nr2();
   const int             nr  = di.first_range().nr2();
   
   dp->increment_left_after( nr, nr2 - nr1 + 1 );
   const QString fragment = browserB_->get_fragment( nr1, nr2 );
   browserA_->set_fragment( nr, fragment, in_update );
   
   if( in_update ) {
      update_all();
      browserA_->restore_pos();
      updates_enable( true );
      are_the_same();
   }
}
// end of move_from_right

//*******************************************************************
// change_from_left                                          PRIVATE
//-------------------------------------------------------------------
// Blok tekstu z lewego browsera ma zastapic odpowiedni blok
// tekstu w prawym browserze.
//-------------------------------------
// in_nr : numer pierwszej linii bloku zrodlowego w lewym browserze.
//*******************************************************************
void QBtWorkspace::change_from_left( const int in_nr, const bool in_update )
{
   if( in_update ) {
      updates_enable( false );
      browserB_->save_pos();
   }
   
   QBtDiffProcess* const dp  = QBtDiffProcess::instance();
   QBtDiffInfo           di  = dp->info_for_nr_in_first( in_nr, true );
   const int             nr1 = di.first_range().nr1();
   const int             nr2 = di.first_range().nr2();
   const int             nr3 = di.second_range().nr1();
   const int             nr4 = di.second_range().nr2();
   const int             d1  = nr2 - nr1 + 1;
   const int             d2  = nr4 - nr3 + 1;
   
   dp->increment_right_after( nr3, d1 - d2 );
   const QString fragment = browserA_->get_fragment( nr1, nr2 );
   browserB_->replace_fragment( nr3, nr4, fragment, in_update );

   if( in_update ) {
      if( ( nr1 == nr2 ) && ( nr3 == nr4 ) ) {
         browserA_->rehighlight();
         browserB_->rehighlight();
      }
      update_all();
      browserB_->restore_pos();
      updates_enable( true );
      are_the_same();
   }
}
// end of change_from_left

//*******************************************************************
// change_from_rigth                                         PRIVATE
//-------------------------------------------------------------------
// Blok tekstu z prawego browsera ma zastapic odpowiedni blok
// tekstu w lewym browserze.
//-------------------------------------
// in_nr : numer pierwszej linii bloku zrodlowego w prawym browserze.
//*******************************************************************
void QBtWorkspace::change_from_right( const int in_nr, const bool in_update )
{
   if( in_update ) {
      updates_enable( false );
      browserA_->save_pos();
   }
  
   QBtDiffProcess* const dp = QBtDiffProcess::instance();
   QBtDiffInfo           di = dp->info_for_nr_in_second( in_nr, true );
   const int             nr1 = di.second_range().nr1();
   const int             nr2 = di.second_range().nr2();
   const int             nr3 = di.first_range().nr1();
   const int             nr4 = di.first_range().nr2();
   const int             d1  = nr2 - nr1 + 1;
   const int             d2  = nr4 - nr3 + 1;
   
   dp->increment_right_after( nr3, d1 - d2 );
   const QString fragment = browserB_->get_fragment( nr1, nr2 );
   browserA_->replace_fragment( nr3, nr4, fragment, in_update );

   if( in_update ) {
      if( ( nr1 == nr2 ) && ( nr3 == nr4 ) ) {
         browserB_->rehighlight();
         browserA_->rehighlight();
      }
      update_all();
      browserA_->restore_pos();
      updates_enable( true );
      are_the_same();
   }
}
// end of change_from_right

//*******************************************************************
// updates_enable                                            PRIVATE
//*******************************************************************
void QBtWorkspace::updates_enable( const bool in_stat )
{
   browserA_->updates_enable( in_stat );
   browserB_->updates_enable( in_stat );
   operatorA_->setUpdatesEnabled( in_stat );
   operatorB_->setUpdatesEnabled( in_stat );
   indicatorA_->setUpdatesEnabled( in_stat );
   indicatorB_->setUpdatesEnabled( in_stat );
   separator_->setUpdatesEnabled( in_stat );
   
   if( in_stat ) {
      QBtShared::restore_cursor();
      QApplication::processEvents();
   }
   else {
      QBtShared::set_cursor( Qt::WaitCursor );
   }
}
// end of updates_enable

//*******************************************************************
// update_all                                                PRIVATE
//*******************************************************************
void QBtWorkspace::update_all()
{
   QBtDiffProcess* const dp = QBtDiffProcess::instance();

   browserA_->renew();
   browserB_->renew();
   
   dp->update( browserA_, browserB_ );
   
   browserA_->update();
   browserA_->numeration_update();
   operatorA_->update();
   indicatorA_->update();
   
   browserB_->update();
   browserB_->numeration_update();
   operatorB_->update();
   indicatorB_->update();

   separator_->update();

   int nchg, nadd, ndel;
   dp->statistic( nchg, nadd, ndel );
   emit stat_total( nchg + nadd + ndel );
   emit stat_chg( nchg );
   emit stat_add( nadd );
   emit stat_del( ndel );

   QApplication::processEvents();
}
// end of update_all

//*******************************************************************
// are_the_same                                              PRIVATE
//*******************************************************************
void QBtWorkspace::are_the_same()
{
   QApplication::processEvents();
   
   int nchg, nadd, ndel;
   QBtDiffProcess::instance()->statistic( nchg, nadd, ndel );

   if( ( 0 == nchg ) && ( 0 == nadd ) && ( 0 == ndel ) ) {
      QBtEventsController::instance()->send_event( QBtEvent::CANT_MERGE );
      QMessageBox::information( this, QBtShared::program_name(), tr( FILES_EQUAL ) );
      return;
   }
   QBtEventsController::instance()->send_event( QBtEvent::READY_TO_MERGE );
   return;
}
// end of are_the_same
