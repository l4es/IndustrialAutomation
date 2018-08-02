/****************************************************************************
** Form interface generated from reading ui file 'sqllogform.ui'
**
** Created: Sat May 5 20:10:28 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef SQLLOGFORM_H
#define SQLLOGFORM_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QComboBox;
class QPushButton;
class QWidgetStack;
class QWidget;
class QTextEdit;

class sqlLogForm : public QDialog
{
    Q_OBJECT

public:
    sqlLogForm( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~sqlLogForm();

    QLabel* textLabel1;
    QComboBox* comboBox3;
    QPushButton* clearButton;
    QWidgetStack* logStack;
    QWidget* WStackPage;
    QTextEdit* userLogText;
    QWidget* WStackPage_2;
    QTextEdit* appLogText;

public slots:
    virtual void closeEvent( QCloseEvent * );
    virtual void log( QString & statement, int msgtype );
    virtual void msgDBDirtyState( bool dirty );
    virtual void clearLog();

signals:
    void goingAway();
    void dbState(bool dirty);

protected:
    QVBoxLayout* sqlLogFormLayout;
    QHBoxLayout* layout6;
    QGridLayout* WStackPageLayout;
    QVBoxLayout* WStackPageLayout_2;

protected slots:
    virtual void languageChange();

};

#endif // SQLLOGFORM_H
