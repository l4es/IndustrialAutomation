#ifndef INCLUDED_QBtSaveQuestion_h
#define INCLUDED_QBtSaveQuestion_h
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
 * File         : QBtSaveQuestion.h
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 10.03.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include <QDialog>

/*------- forward declaration:
-------------------------------------------------------------------*/
class QPushButton;


/*------- class declaration:
-------------------------------------------------------------------*/
class QBtSaveQuestion : public QDialog
{
   Q_OBJECT

//******* CONSTRUCTION / DESTRUCTION *******
public:
   QBtSaveQuestion( bool, bool, QWidget* = 0 );
private:
   QBtSaveQuestion( const QBtSaveQuestion& );
   QBtSaveQuestion& operator=( const QBtSaveQuestion& );

//******* CONSTANTS *******
private:
   static const char* const CAPTION;
   static const char* const MESSAGE;
   static const char* const SAVE;
   static const char* const CANCEL;
   static const char* const EXIT;
   static const char* const SAVE_LFT;
   static const char* const SAVE_RGT;

//******* MEMBERS *******
private:
   QPushButton* const save_;
   QPushButton* const cancel_;
   QPushButton* const exit_;
   QPushButton* const save_lft_;
   QPushButton* const save_rgt_;

//******* METHODS *******
private slots:
   void cancel   ();
   void exit     ();
   void save     ();
   void save_lft ();
   void save_rgt ();
};

#endif // QBtSaveQuestion
