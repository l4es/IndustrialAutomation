/****************************************************************************
** Form interface generated from reading ui file 'addfieldform.ui'
**
** Created: Sat May 5 20:10:27 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef ADDFIELDFORM_H
#define ADDFIELDFORM_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QLabel;
class QLineEdit;
class QComboBox;
class QToolButton;

class addFieldForm : public QDialog
{
    Q_OBJECT

public:
    addFieldForm( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~addFieldForm();

    QPushButton* cancelButton;
    QPushButton* createButton;
    QLabel* textLabel1;
    QLabel* textLabel2;
    QLineEdit* nameLineEdit;
    QComboBox* typeBox;
    QToolButton* typeButton;

    QString fname;
    QString ftype;

public slots:
    virtual void setInitialValues( QString name, QString type );
    virtual void confirmAddField();
    virtual void getCustomType();

protected:
    QGridLayout* addFieldFormLayout;
    QHBoxLayout* layout21;
    QVBoxLayout* layout23;
    QVBoxLayout* layout7;
    QHBoxLayout* layout6;

protected slots:
    virtual void languageChange();

};

#endif // ADDFIELDFORM_H
