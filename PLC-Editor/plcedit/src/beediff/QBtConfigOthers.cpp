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
 * File         : QBtConfigOthers.cpp
 * Author       : Piotr Pszczolkowski
 * Contact      : piotr@beesoft.org
 * Creation date: 06.04.2008
 *******************************************************************/

/*------- include files:
-------------------------------------------------------------------*/
#include "QBtConfigOthers.h"
#include "QBtConfig.h"
#include <QCheckBox>
#include <QVBoxLayout>

/*------- local constants:
-------------------------------------------------------------------*/
const char* const QBtConfigOthers::BezierCurve = QT_TR_NOOP( "&connector as Bezier curves" );
const char* const QBtConfigOthers::MarkVCenter = QT_TR_NOOP( "&operations mark vertical centered" );
const char* const QBtConfigOthers::Utf8        = QT_TR_NOOP( "text treat as &UTF-8" );

 
//*******************************************************************
// QBtConfigOthers                                       CONSTRUCTOR
//*******************************************************************
QBtConfigOthers::QBtConfigOthers( QWidget* const in_parent )
: QBiConfigWidget( in_parent )
, bezier_cbx_ ( new QCheckBox( tr( BezierCurve ) ) )
, center_cbx_ ( new QCheckBox( tr( MarkVCenter ) ) )
, utf8_cbx_   ( new QCheckBox( tr( Utf8        ) ) )
{
   const QBtConfig* const cfg = QBtConfig::instance();
   bezier_cbx_->setChecked( cfg->bezier()       );
   center_cbx_->setChecked( cfg->mark_vcenter() );
   utf8_cbx_  ->setChecked( cfg->utf8()         );
   
   QVBoxLayout* const main_layout = new QVBoxLayout;
   main_layout->addWidget( bezier_cbx_ );
   main_layout->addWidget( center_cbx_ );
   main_layout->addWidget( utf8_cbx_   );
   main_layout->addStretch();
   setLayout( main_layout );
}
// end of QBtConfigOthers

//*******************************************************************
// apply                                                      PUBLIC
//*******************************************************************
void QBtConfigOthers::apply()
{
   QBtConfig* const cfg = QBtConfig::instance();
   cfg->bezier      ( bezier_cbx_->isChecked() );
   cfg->mark_vcenter( center_cbx_->isChecked() );
   cfg->utf8        ( utf8_cbx_  ->isChecked() );

   cfg->save_data();
}
// end of apply
