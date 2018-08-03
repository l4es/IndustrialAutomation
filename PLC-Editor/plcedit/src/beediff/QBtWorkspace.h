#ifndef INCLUDED_QBtWorkspace_h
#define INCLUDED_QBtWorkspace_h
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
 * File         : QBtWorkspace.h
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 28.01.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include <QWidget>

/*------- forward declarations:
-------------------------------------------------------------------*/
class QComboBox;
class QPushButton;
class QBtSeparator;
class QBtBrowser;
class QScrollBar;
class QBtIndicator;
class QBtOperator;

/*------- class declaration:
-------------------------------------------------------------------*/
class QBtWorkspace : public QWidget
{
   Q_OBJECT

//******* CONSTRUCTION / DESTRUCTION *******
public:
   QBtWorkspace( QWidget* = 0 );
   ~QBtWorkspace();
private:
   QBtWorkspace( const QBtWorkspace& );
   QBtWorkspace& operator=( const QBtWorkspace& );

//******* CONSTANTS *******
private:
   static const char* const NOT_FILE;
   static const char* const NO_SELECTED;
   static const char* const FILES_EQUAL;

//******* MEMBERS *******
private:
   QPushButton*  const saveA_btn_;
   QPushButton*  const saveB_btn_;
   QPushButton*  const openA_btn_;
   QPushButton*  const openB_btn_;
   QComboBox*    const pathA_cbx_;
   QComboBox*    const pathB_cbx_;
   QScrollBar*   const scrollA_bar_;
   QScrollBar*   const scrollB_bar_;
   QBtBrowser*   const browserA_;
   QBtBrowser*   const browserB_;
   QBtIndicator* const indicatorA_;
   QBtIndicator* const indicatorB_;
   QBtOperator*  const operatorA_;
   QBtOperator*  const operatorB_;
   QBtSeparator* const separator_;
   QString             dirA_;
   QString             dirB_;
   bool                can_scroll_;

//******* METHODS *******
public:
   void lft_read_file  ( const QString& );
   void rgt_read_file  ( const QString& );
   void save           ();
   bool save_on_exit   ();
   void del_on_A       ();
   void del_on_B       ();
   void merge_to_A     ();
   void merge_to_B     ();

public slots:
   void clearBrowserA();        //remarks for PLCEdit
   void clearBrowserB();        //remarks for PLCEdit
   void lft_file_selection();   //remarks for PLCEdit
   void rgt_file_selection();   //remarks for PLCEdit
   void setFontAndTabWidthBrowserA(QFont font, int tabStopWidth); //remarks for PLCEdit
   void setFontAndTabWidthBrowserB(QFont font, int tabStopWidth); //remarks for PLCEdit

private:
   void updates_enable       ( bool );
   void document_changed     ( bool, bool );
   void showEvent            ( QShowEvent* );
   void customEvent          ( QEvent* );
   void update_looks         ();
   void update_outside_looks ();
   void save_config          () const;
   void restore_config       ();
   void check_fpath_history  ( QComboBox* );
   void remove_no_selected   ( QComboBox* );
   void remove_from_left     ( int, bool = true );
   void remove_from_right    ( int, bool = true );
   void move_from_left       ( int, bool = true );
   void move_from_right      ( int, bool = true );
   void change_from_left     ( int, bool = true );
   void change_from_right    ( int, bool = true );
   void update_all           ();
   void update_lft_cbox      ( bool = true );
   void update_rgt_cbox      ( bool = true );
   void befor_automation     ();
   void after_automation     ();
   void are_the_same         ();
private slots:
   void update_request();
   void lft_fpath_activated( const QString& );
   void rgt_fpath_activated( const QString& );
   void ready();
   void browser_A_scrolled( int );
   void browser_B_scrolled( int );
   void save_A();
   void save_B();
signals:
   void stat_total( int );
   void stat_chg  ( int );
   void stat_add  ( int );
   void stat_del  ( int );
};

#endif // INCLUDED_QBtWorkspace_h
