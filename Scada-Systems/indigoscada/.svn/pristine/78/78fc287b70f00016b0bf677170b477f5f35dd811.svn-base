/****************************************************************************
** Form interface generated from reading ui file 'importcsvform.ui'
**
** Created: Sat May 5 20:10:28 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef IMPORTCSVFORM_H
#define IMPORTCSVFORM_H

#include <qvariant.h>
#include <qdialog.h>
#include <qstringlist.h>
#include <qstring.h>
#include "sqlitedb.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QLineEdit;
class QCheckBox;
class QComboBox;
class QTable;
class QPushButton;

class importCSVForm : public QDialog
{
    Q_OBJECT

public:
    importCSVForm( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~importCSVForm();

    QLabel* textLabel3;
    QLineEdit* tableNameEdit;
    QCheckBox* extractFieldNamesCheckbox;
    QLabel* textLabel1;
    QLabel* textLabel2;
    QComboBox* fieldBox;
    QComboBox* quoteBox;
    QTable* previewTable;
    QPushButton* cancelButton;
    QPushButton* createButton;

public slots:
    virtual void initialize( QString & csvfile, DBBrowserDB * db );
    virtual void createButtonPressed();
    virtual void preview();
    virtual void fieldSeparatorChanged();
    virtual void textQuoteChanged();
    virtual void extractFieldNamesChanged( bool enabled );

protected:
    QStringList curList;
    char quote;
    char sep;
    int numfields;
    QString csvfilename;
    DBBrowserDB * pdb;

    QVBoxLayout* importCSVFormLayout;
    QGridLayout* layout27;
    QHBoxLayout* layout25;
    QVBoxLayout* layout23;
    QVBoxLayout* layout22;
    QHBoxLayout* layout20;

protected slots:
    virtual void languageChange();

private:
    void init();

};

#endif // IMPORTCSVFORM_H
