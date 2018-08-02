/****************************************************************************
** Form interface generated from reading ui file 'edittableform.ui'
**
** Created: Sat May 5 20:10:28 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef EDITTABLEFORM_H
#define EDITTABLEFORM_H

#include <qvariant.h>
#include <qdialog.h>
#include "sqlitedb.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLineEdit;
class QListView;
class QListViewItem;
class QPushButton;

class editTableForm : public QDialog
{
    Q_OBJECT

public:
    editTableForm( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~editTableForm();

    QLineEdit* tableLine;
    QListView* fieldListView;
    QPushButton* renameTableButton;
    QPushButton* renameFieldButton;
    QPushButton* removeFieldButton;
    QPushButton* addFieldButton;
    QPushButton* closeButton;

    bool modified;
    QString curTable;

public slots:
    virtual void setActiveTable( DBBrowserDB * thedb, QString tableName );
    virtual void populateFields();
    virtual void renameTable();
    virtual void editField();
    virtual void addField();
    virtual void removeField();
    virtual void fieldSelectionChanged();

protected:
    QStringList types;
    QStringList fields;
    DBBrowserDB * pdb;

    QGridLayout* editTableFormLayout;
    QVBoxLayout* layout50;
    QVBoxLayout* layout42;
    QHBoxLayout* layout41;

protected slots:
    virtual void languageChange();

private:
    void init();

};

#endif // EDITTABLEFORM_H
