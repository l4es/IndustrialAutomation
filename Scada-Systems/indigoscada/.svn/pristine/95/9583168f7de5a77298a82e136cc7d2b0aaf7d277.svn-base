/****************************************************************************
** Form interface generated from reading ui file 'exporttablecsvform.ui'
**
** Created: Sat May 5 20:10:28 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef EXPORTTABLECSVFORM_H
#define EXPORTTABLECSVFORM_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QComboBox;
class QPushButton;

class exportTableCSVForm : public QDialog
{
    Q_OBJECT

public:
    exportTableCSVForm( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~exportTableCSVForm();

    QLabel* textLabel2;
    QComboBox* comboOptions;
    QPushButton* buttonCancel;
    QPushButton* buttonExport;

    QString option;

public slots:
    virtual void exportPressed();
    virtual void populateOptions( QStringList entries );

protected:
    QVBoxLayout* exportTableCSVFormLayout;
    QHBoxLayout* layout21;
    QHBoxLayout* layout69;

protected slots:
    virtual void languageChange();

};

#endif // EXPORTTABLECSVFORM_H
