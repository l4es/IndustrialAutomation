/****************************************************************************
** Form interface generated from reading ui file 'createtableform.ui'
**
** Created: Sat May 5 20:10:28 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef CREATETABLEFORM_H
#define CREATETABLEFORM_H

#include <qvariant.h>
#include <qdialog.h>
#include "sqlitedb.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QLineEdit;
class QGroupBox;
class QListView;
class QListViewItem;
class QPushButton;

class createTableForm : public QDialog
{
    Q_OBJECT

public:
    createTableForm( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~createTableForm();

    QLabel* textLabel1;
    QLineEdit* tablenameLineEdit;
    QGroupBox* groupBox1;
    QListView* fieldListView;
    QPushButton* buttonAddField;
    QPushButton* buttonDeleteField;
    QPushButton* buttonCreate;
    QPushButton* buttonCancel;

    QString createStatement;

public slots:
    virtual void confirmCreate();
    virtual void addField();
    virtual void deleteField();
    virtual void fieldSelectionChanged();

protected:
    QVBoxLayout* createTableFormLayout;
    QHBoxLayout* layout7;
    QVBoxLayout* groupBox1Layout;
    QHBoxLayout* layout6;
    QHBoxLayout* layout8;

protected slots:
    virtual void languageChange();

private:
    void init();

};

#endif // CREATETABLEFORM_H
