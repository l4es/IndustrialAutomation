/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */
#include "dbtable.h"
#include <qdir.h>
#include <qstringlist.h>
#include <qheader.h>
#include <qcombobox.h>
#include <stdlib.h>


DbTable::DbTable( int numRows, int numCols, QWidget *parent, const char *name)
    : QTable(numRows, numCols, parent, name)
{
    setSorting( TRUE );
	
//    adjustColumn( 0 );
}


void DbTable::sortColumn( int col, bool ascending, bool  )
{
    // sum row should not be sorted, so get rid of it for now
//    clearCell( numRows() - 1, 0 );
//    clearCell( numRows() - 1, 1 );
    // do sort
    QTable::sortColumn( col, ascending, TRUE );
    // re-insert sum row
//    recalcSum( 0, 1 );
}

QString DbTable::getFieldIndexedName()
{
  return FieldIndexedName;
}

QString DbTable::getFieldIndexedType()
{
  return FieldIndexedType;
}

int DbTable::getFieldIndexedColumn()
{
	return FieldIndexedColumn;
}

void DbTable::setFieldIndexedName(QString& fieldIndexedName)
{
	FieldIndexedName = fieldIndexedName;
}

void DbTable::setFieldIndexedType(QString& fieldIndexedType)
{
	FieldIndexedType = fieldIndexedType;
}

void DbTable::setFieldIndexedColumn(int col)
{
	FieldIndexedColumn = col;
}

void DbTable::setDatabase(QObject* db)
{
	database = db;
}

QObject* DbTable::getDatabase()
{
	return database;
}

void DbTableItem::paint( QPainter *p, const QColorGroup &cg, const QRect &cr, bool selected )
{
    QColorGroup g( cg );
    // last row is the sum row - we want to make it better visible by
    // using a red background
    if ( row() == table()->numRows() - 1 )
	g.setColor( QColorGroup::Base, red );
    QTableItem::paint( p, g, cr, selected );
}




DbComboItem::DbComboItem( QTable *t, EditType et )
    : QTableItem( t, et, "Yes" ), cb( 0 )
{
    // we do not want that this item can be replaced
    setReplaceable( FALSE );
}

QWidget *DbComboItem::createEditor() const
{
    // create an editor - a combobox in our case
    ( (DbComboItem*)this )->cb = new QComboBox( table()->viewport() );
    cb->insertItem( "Yes" );
    cb->insertItem( "No" );
    // and initialize it
    cb->setCurrentItem( text() == "No" ? 1 : 0 );
    return cb;
}

void DbComboItem::setContentFromEditor( QWidget *w )
{
    // the user changed the value of the combobox, so syncronize the
    // value of the item (text), with the value of the combobox
    if ( w->inherits( "QComboBox" ) )
	setText( ( (QComboBox*)w )->currentText() );
    else
	QTableItem::setContentFromEditor( w );
}	

void DbComboItem::setText( const QString &s )
{
    if ( cb ) {
	// initialize the combobox from the text
	if ( s == "No" )
	    cb->setCurrentItem( 1 );
	else
	    cb->setCurrentItem( 0 );
    }	
    QTableItem::setText( s );
}
