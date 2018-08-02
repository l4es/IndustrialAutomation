/****************************************************************************
** Form interface generated from reading ui file 'findform.ui'
**
** Created: Sat May 5 20:10:28 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef FINDFORM_H
#define FINDFORM_H

#include <qvariant.h>
#include <qdialog.h>
#include "sqlitedb.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QComboBox;
class QLineEdit;
class QListView;
class QListViewItem;
class QLabel;

class findForm : public QDialog
{
    Q_OBJECT

public:
    findForm( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~findForm();

    QPushButton* searchButton;
    QComboBox* findFieldCombobox;
    QLineEdit* searchLine;
    QComboBox* findOperatorComboBox;
    QListView* findListView;
    QLabel* resultsLabel;

public slots:
    virtual void showResults( resultMap rmap );
    virtual void find();
    virtual void resetFields( QStringList fieldlist );
    virtual void resetResults();
    virtual void recordSelected( QListViewItem * witem );
    virtual void closeEvent( QCloseEvent * );

signals:
    void lookfor(const QString&, const QString&, const QString&);
    void showrecord(int);
    void goingAway();

protected:
    QVBoxLayout* findFormLayout;
    QGridLayout* layout5;
    QHBoxLayout* layout18;

protected slots:
    virtual void languageChange();

};

#endif // FINDFORM_H
