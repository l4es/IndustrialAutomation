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
 * File         : QBtSyntax.cpp
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 26.02.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include "QBtSyntax.h"
#include "QBtConfig.h"
#include "QBtLineData.h"
#include "QBtDiffProcess.h"
#include <QTextDocument>
#include <QTextBrowser>
#include <QtDebug>


//*******************************************************************
// QBtSyntax                                             CONSTRUCTOR
//*******************************************************************
QBtSyntax::QBtSyntax( QTextDocument* const in_doc )
: QSyntaxHighlighter ( in_doc )
, normal_format_     ( QTextCharFormat() )
, part_format_       ( QTextCharFormat() )
{
   renew();
}
// end of QBtSyntax

//*******************************************************************
// renew                                                      PUBLIC
//*******************************************************************
void QBtSyntax::renew()
{
   const QBtConfig* const cfg = QBtConfig::instance();
   
   //normal_format_.setFont( cfg->text_font() ); //remarks for PLCEdit
   normal_format_.setForeground( cfg->text_color() );

   //part_format_.setFont( cfg->part_font() ); //remarks for PLCEdit
   part_format_.setForeground( cfg->part_color() );
   part_format_.setBackground( cfg->part_background() );

   rehighlight();
}
// end of renew

//*******************************************************************
// highlightBlock                                  PRIVATE inherited
//*******************************************************************
void QBtSyntax::highlightBlock( const QString& in_text )
{
   setFormat( 0, in_text.length(), normal_format_ );

   const QBtLineData* const info = dynamic_cast<QBtLineData*>( currentBlockUserData() );
   if( !info ) return;

   QBtDiffProcess* const dp = QBtDiffProcess::instance();
   const int nr = info->get_number();
   const QBtRange range = info->is_left()
                        ? dp->info_for_nr_in_first( nr ).first_range()
                        : dp->info_for_nr_in_second( nr ).second_range();
   const VTokens& tokens = range.tokens();

   if( QBtShared::CHANGE == range.oper() ) {
      VTokens::const_iterator it = tokens.begin();
      const VTokens::const_iterator end = tokens.end();
      while( it != end ) {
         setFormat( it->position(), it->length(), part_format_ );
         ++it;
      }
   }
}
// end of highlightBlock
