/****************************************************************************
** Form interface generated from reading ui file 'editfieldform.ui'
**
** Created: Sat May 5 20:10:28 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef EDITFIELDFORM_H
#define EDITFIELDFORM_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QPushButton;
class QComboBox;
class QToolButton;
class QLineEdit;

class editFieldForm : public QDialog
{
    Q_OBJECT

public:
    editFieldForm( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~editFieldForm();

    QLabel* textLabel1;
    QLabel* textLabel2;
    QPushButton* cancelButton;
    QPushButton* saveButton;
    QComboBox* typeBox;
    QToolButton* typeButton;
    QLineEdit* nameLineEdit;

    QString name;
    QString type;

public slots:
    virtual void setInitialValues( QString name, QString type );
    virtual void confirmEdit();
    virtual void enableSave();
    virtual void getCustomType();

protected:
    QGridLayout* editFieldFormLayout;
    QVBoxLayout* layout23;
    QHBoxLayout* layout21;
    QGridLayout* layout26;
    QHBoxLayout* layout25;

protected slots:
    virtual void languageChange();

};

#endif // EDITFIELDFORM_H
