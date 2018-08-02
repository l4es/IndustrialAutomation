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
#ifndef DBTABLE_H_
#define DBTABLE_H_

#include <qtable.h>
#include <qcombobox.h>

class DbTableItem : public QTableItem
{
public:
    DbTableItem( QTable *t, EditType et, const QString &txt ) : QTableItem( t, et, txt ) {}
    void paint( QPainter *p, const QColorGroup &cg, const QRect &cr, bool selected );
};

class DbComboItem : public QTableItem
{
public:
    DbComboItem( QTable *t, EditType et );
    QWidget *createEditor() const;
    void setContentFromEditor( QWidget *w );
    void setText( const QString &s );
    
private:
    QComboBox *cb;

};

class DbTable : public QTable
{
    Q_OBJECT

	QObject* database;
	QString FieldIndexedName;
	QString FieldIndexedType;
	int FieldIndexedColumn;

public:
    DbTable( int numRows, int numCols, QWidget *parent = 0, const char *name = 0);
    void sortColumn( int col, bool ascending, bool wholeRows );

	QString getFieldIndexedName();
	QString getFieldIndexedType();
	int getFieldIndexedColumn();
	void setFieldIndexedName(QString& fieldIndexedName);
	void setFieldIndexedType(QString& fieldIndexedType);
	void setFieldIndexedColumn(int col);
	void setDatabase(QObject* db);
	QObject* getDatabase();
};

#endif //DBTABLE_H_
