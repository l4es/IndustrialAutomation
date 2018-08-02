/****************************************************************************
** Form interface generated from reading ui file 'createindexform.ui'
**
** Created: Sat May 5 20:10:28 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef CREATEINDEXFORM_H
#define CREATEINDEXFORM_H

#include <qvariant.h>
#include <qdialog.h>
#include "sqlitedb.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QLineEdit;
class QGroupBox;
class QComboBox;
class QPushButton;

class createIndexForm : public QDialog
{
    Q_OBJECT

public:
    createIndexForm( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~createIndexForm();

    QLabel* textLabel2;
    QLineEdit* indexLineEdit;
    QGroupBox* groupBox2;
    QComboBox* comboTables;
    QComboBox* comboFields;
    QComboBox* comboOrder;
    QComboBox* comboUnique;
    QLabel* textLabel3;
    QLabel* textLabel4;
    QLabel* textLabel5;
    QLabel* textLabel6;
    QPushButton* buttonCreate;
    QPushButton* buttonCancel;

    tableMap mtablemap;
    QString createStatement;

public slots:
    virtual void tableSelected( const QString & entry );
    virtual void confirmCreate();
    virtual void populateTable( tableMap rmap );

protected:
    QVBoxLayout* createIndexFormLayout;
    QHBoxLayout* layout15;
    QGridLayout* groupBox2Layout;
    QVBoxLayout* layout12;
    QVBoxLayout* layout13;
    QHBoxLayout* layout16;

protected slots:
    virtual void languageChange();

};

#endif // CREATEINDEXFORM_H
