/****************************************************************************
** Form interface generated from reading ui file 'renametableform.ui'
**
** Created: Sat May 5 20:10:28 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef RENAMETABLEFORM_H
#define RENAMETABLEFORM_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLineEdit;
class QPushButton;

class renameTableForm : public QDialog
{
    Q_OBJECT

public:
    renameTableForm( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~renameTableForm();

    QLineEdit* tablenameLineEdit;
    QPushButton* closeButton;
    QPushButton* renameButton;

    virtual QString getTableName();

public slots:
    virtual void renameClicked();
    virtual void setTableName( QString name );

protected:
    QVBoxLayout* renameTableFormLayout;
    QHBoxLayout* layout34;

protected slots:
    virtual void languageChange();

};

#endif // RENAMETABLEFORM_H
