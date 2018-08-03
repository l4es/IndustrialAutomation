#ifndef INCLUDED_QBtBrowser_h
#define INCLUDED_QBtBrowser_h
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
 * File         : QBtBrowser.h
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 04.02.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include "QBtSyntax.h"
#include <QTextBrowser>
#include <QScrollBar>
#include <QTextDocumentFragment>

/*------- forward declaration:
-------------------------------------------------------------------*/
class QScrollBar;
class QBtSyntax;
class QBtOperator;
class QBtNumeration;

/*------- class declaration:
-------------------------------------------------------------------*/
class QBtBrowser : public QTextBrowser
{
   Q_OBJECT

//******* CONSTRUCTION / DESTRUCTION *******
public:
   QBtBrowser( int, QScrollBar*, QWidget* = 0 );
   ~QBtBrowser();
private:
   QBtBrowser( const QBtBrowser& );
   QBtBrowser& operator=( const QBtBrowser& );

//******* CONSTANTS *******
private:
   static const char* const READING;
   static const char* const NOT_EXISTS;
   static const char* const NOT_FILE;
   static const char* const READ_ERROR;
   static const char* const SAVING;
   static const char* const READ_ONLY;
   static const char* const SAVE_ERROR;

//******* MEMBERS *******
private:
   const bool           is_left_;
   QScrollBar *   const scroll_;
   QBtSyntax  *   const syntax_;
   QBtOperator*         operator_;
   QBtNumeration* const numeration_;
   QString              fpath_;
   QString              drop_fpath_;
   bool                 is_writable_;
   int                  saved_sc_;
   int                  saved_nr1_;
   int                  saved_nr2_;
   bool                 is_changed_;

//******* METHODS *******
public:
   void			  clearBrowser    (); //remarks for PLCEdit
   void           remove_range    ( int, int, bool = true );
   QString        get_fragment    ( int, int );
   void           set_fragment    ( int, const QString&, bool = true );
   void           replace_fragment( int, int, const QString&, bool = true );
   const QString& fpath           () const { return fpath_; }

   void    scroll_by           ( int ) const;
   bool    is_left             () const { return is_left_; }
   bool    is_writable         () const { return is_writable_; }
   void    rehighlight         () { syntax_->rehighlight(); }
   void    save_pos            ();
   void    restore_pos         ();
   void    adjust_size         () { document()->adjustSize(); }

   void    numeration_update   ();
   int     changed             () const { return is_changed_; }
   bool    read_file           ( const QString&, bool = false );
   bool    save_file           ();
   void    renew               ( bool = true );
   QString get_y               ( int, int, int&, int& ) const;
   void    goto_line           ( int );
   void    set_cursor_position ( int );
   int     first_selected      () const;
   int     last_selected       () const;
   int     first_visible       () const;
   int     last_visible        () const;
   void    update_looks        ();
   void    set_operator        ( QBtOperator* in_ptr ) { operator_ = in_ptr; }
   void    updates_enable      ( bool );
private:
   void resizeEvent       ( QResizeEvent* );
   void customEvent       ( QEvent* );
   void set_numeration    ( bool );
   void dragEnterEvent    ( QDragEnterEvent* );
   void dragMoveEvent     ( QDragMoveEvent* );
   void dropEvent         ( QDropEvent* );
   void update_numeration ();
   void fragment_position ( int, int, int&, int& ) const;
   void paintEvent        ( QPaintEvent* );
   void init_scrollbar    ();
   void scrollContentsBy  ( int, int );
   void changed           ( bool );
private slots:
   void set_range         ( int, int ) const;
   void set_value         ( int ) const;
signals:
   void update_request    ();
   void scroll_request    ( int );
};

#endif // INCLUDED_QBtBrowser
