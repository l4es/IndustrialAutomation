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
 * File         : QBtSettings.cpp
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 19.02.2008
 *******************************************************************/
 
/*------- include files:
-------------------------------------------------------------------*/
#include "QBtSettings.h"

//*******************************************************************
// save                                                       PUBLIC
//*******************************************************************
bool QBtSettings::save( const QString& in_ident, const QVariant& in_data )
{
   setValue( in_ident, in_data );
   return ( NoError == status() );
}
// end of save

//*******************************************************************
// read                                                PUBLIC static
//*******************************************************************
bool QBtSettings::read( const QString& in_ident, QVariant& out_data )
{
   bool retval = contains( in_ident );
   if( retval ) {
      out_data = value( in_ident );
      retval = ( NoError == status() );
   }
   return retval;
}
// end of read
