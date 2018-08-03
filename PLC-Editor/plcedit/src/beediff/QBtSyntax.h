#ifndef INCLUDED_QBtSyntax_H
#define INCLUDED_QBtSyntax_H
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
 * File         : QBtSyntax.h
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 26.02.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include <QSyntaxHighlighter>
#include <QFont>

/*------- forward declarations:
-------------------------------------------------------------------*/
class QTextDocument;

/*------- class declaration:
-------------------------------------------------------------------*/
class QBtSyntax : public QSyntaxHighlighter
{
   Q_OBJECT

//******* CONSTRUCTION / DESTRUCTION *******
public:
   QBtSyntax( QTextDocument* );
private:
   QBtSyntax( const QBtSyntax& );
   QBtSyntax& operator=( const QBtSyntax& );

//******* MEMBERS *******
private:
   QTextCharFormat normal_format_;
   QTextCharFormat part_format_;

//******* METHODS *******
public:
   void renew();
private:
   void highlightBlock( const QString& );
};

#endif // INCLUDED_QBtSyntax_H
