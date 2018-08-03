#ifndef INCLUDED_QBtConfigTextViewer_h
#define INCLUDED_QBtConfigTextViewer_h
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
 * File         : QBtConfigTextViewer.h
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 21.02.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include "QBiConfigWidget.h"

/*------- forward declarations:
-------------------------------------------------------------------*/
class QBtColorDemo;
class QPushButton;

/*------- class declaration:
-------------------------------------------------------------------*/
class QBtConfigTextViewer : public QBiConfigWidget
{
   Q_OBJECT

//******* CONSTRUCTION / DESTRUCTION *******
public:
   QBtConfigTextViewer( QWidget* = 0 );
private:
   QBtConfigTextViewer( const QBtConfigTextViewer& );
   QBtConfigTextViewer& operator=( const QBtConfigTextViewer& );

//******* CONSTANTS *******
private:
   static const char* const BROWSER;
   static const char* const BACKGROUND;
   static const char* const FONT;
   static const char* const COLOR;
   static const char* const HIGHLIGHT;
   static const char* const CHANGED;
   static const char* const ADDED;
   static const char* const DELETED;
   static const char* const LINE_CHG;

//******* MEMBERS *******
private:
   QBtColorDemo* const chg_demo_;
   QPushButton * const chg_background_btn_;
   
   QBtColorDemo* const add_demo_;
   QPushButton * const add_background_btn_;

   QBtColorDemo* const del_demo_;
   QPushButton * const del_background_btn_;

   QBtColorDemo* const text_demo_;
   QPushButton * const text_font_btn_;
   QPushButton * const text_color_btn_;
   QPushButton * const text_background_btn_;
   
   QBtColorDemo* const part_demo_;
   QPushButton * const part_font_btn_;
   QPushButton * const part_color_btn_;
   QPushButton * const part_background_btn_;

//******* METHODS *******
public:
   void apply          ();
private slots:
   void text_font      ();
   void text_color     ();
   void text_background();
   void part_font      ();
   void part_color     ();
   void part_background();
   void chg_background ();
   void add_background ();
   void del_background ();
};

#endif // INCLUDED_QBtConfigTextViewer_h
