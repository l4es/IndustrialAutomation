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
 * File         : QBtDiffInfo.h
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 10.02.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include "QBtDiffInfo.h"
#include <QStringList>

//*******************************************************************
// insert                                                     PUBLIC
//-------------------------------------------------------------------
// W postaci lancuchow tekstowych przyslane sa dwa zakresy:
// 'in_first' dla pliku w lewym oknie,
// 'in_second' dla pliku w prawym oknie.
// Zakres moze byc jedna liczba (np. 26), lub zakresem (np. 8,128).
//*******************************************************************
void QBtDiffInfo::insert(  const QString& in_first,
                           const int      in_oper,
                           const QString& in_second )
{
   if( in_first.isEmpty()  ) return;
   if( in_second.isEmpty() ) return;

   int nr1 = int();
   int nr2 = int();
   
   {  // parse range for first file
      const int idx = in_first.indexOf( ',' );
      if( -1 == idx ) {
         nr1 = nr2 = in_first.toInt();
      }
      else {
         nr1 = in_first.left( idx ).toInt();
         nr2 = in_first.mid( idx + 1 ).toInt();
      }
      first_.nr1 ( nr1 );
      first_.nr2 ( nr2 );
      first_.oper( in_oper );
   }

   {  // parse range for second file
      const int idx = in_second.indexOf( ',' );
      if( -1 == idx ) {
         nr1 = nr2 = in_second.toInt();
      }
      else {
         nr1 = in_second.left( idx ).toInt();
         nr2 = in_second.mid( idx + 1 ).toInt();
      }
      second_.nr1 ( nr1 );
      second_.nr2 ( nr2 );
      second_.oper( in_oper );
   }
}
// end of insert
