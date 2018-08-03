#ifndef INCLUDED_QBtConfigDialog_h
#define INCLUDED_QBtConfigDialog_h
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
 * File         : QBtConfigDialog.h
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 21.02.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include <QDialog>

/*------- forward declarations:
-------------------------------------------------------------------*/
class QTabBar;
class QStackedWidget;
class QPushButton;
class QBtConfigTextViewer;
class QBtConfigDiffProcess;
class QBtConfigOthers;


/*------- class declaration:
-------------------------------------------------------------------*/
class QBtConfigDialog : public QDialog
{
   Q_OBJECT

//******* CONSTRUCTION / DESTRUCTION *******
public:
   QBtConfigDialog( QWidget* = 0 );
private:
   QBtConfigDialog( const QBtConfigDialog& );
   QBtConfigDialog& operator=( const QBtConfigDialog& );

//******* TYPES *******
private:
   enum {
      TEXT_VIEWER_IDX = 0,
      DIFF_PROCESS_IDX,
      OTHERS_IDX,
   };
//******* CONSTANTS *******
private:
   static const char* const CAPTION;
   static const char* const APPLY;
   static const char* const EXIT;
   static const char* const TEXT_VIEWER;
   static const char* const DIFF_PROCESS;
   static const char* const OTHERS;

//******* MEMBERS *******
private:
   QTabBar              * const tbar_;
   QStackedWidget       * const wstack_;
   QPushButton          * const apply_;
   QPushButton          * const exit_;
   QBtConfigTextViewer  * const tv_config_;
   QBtConfigDiffProcess * const dp_config_;
   QBtConfigOthers      * const others_config_;
   
//******* METHODS *******
private slots:
   void apply ();
};

#endif // INCLUDED_QBtConfigDialog_h
