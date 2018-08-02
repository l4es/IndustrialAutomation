/****************************************************************************
** Form interface generated from reading ui file 'addfieldtypeform.ui'
**
** Created: Sat May 5 20:10:27 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef ADDFIELDTYPEFORM_H
#define ADDFIELDTYPEFORM_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLineEdit;
class QPushButton;

class addFieldTypeForm : public QDialog
{
    Q_OBJECT

public:
    addFieldTypeForm( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~addFieldTypeForm();

    QLineEdit* typeNameEdit;
    QPushButton* cancelButton;
    QPushButton* okButton;

protected:
    QGridLayout* addFieldTypeFormLayout;
    QHBoxLayout* layout11;

protected slots:
    virtual void languageChange();

};

#endif // ADDFIELDTYPEFORM_H
