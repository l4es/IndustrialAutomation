/****************************************************************************
** Form interface generated from reading ui file 'deleteindexform.ui'
**
** Created: Sat May 5 20:10:28 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef DELETEINDEXFORM_H
#define DELETEINDEXFORM_H

#include <qvariant.h>
#include <qdialog.h>
#include "qstring.h"
#include "qstringlist.h"
#include "sqlitedb.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QComboBox;
class QPushButton;

class deleteIndexForm : public QDialog
{
    Q_OBJECT

public:
    deleteIndexForm( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~deleteIndexForm();

    QLabel* textLabel2;
    QComboBox* comboOptions;
    QPushButton* buttonDelete;
    QPushButton* buttonCancel;

    QString option;

public slots:
    virtual void confirmDelete();
    virtual void populateOptions( QStringList entries );

protected:
    QVBoxLayout* deleteIndexFormLayout;
    QHBoxLayout* layout21;
    QHBoxLayout* layout19;

protected slots:
    virtual void languageChange();

};

#endif // DELETEINDEXFORM_H
