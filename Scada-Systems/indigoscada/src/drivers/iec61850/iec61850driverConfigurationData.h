/****************************************************************************
** Form interface generated from reading ui file 'iec61850driver_configuration.ui'
**
** Created: Sun Apr 7 14:40:20 2013
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef FORM1_H
#define FORM1_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLineEdit;
class QLabel;
class QPushButton;
class QSpinBox;

class Iec61850driverConfigurationData : public QDialog
{
    Q_OBJECT

public:
    Iec61850driverConfigurationData( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~Iec61850driverConfigurationData();

    QLineEdit* Name;
    QLabel* textLabel1;
    QLabel* textLabel4;
    QLabel* textLabel5;
    QLabel* textLabel3;
    QPushButton* CancelButton;
    QPushButton* OkButton;
    QPushButton* HelpButton;
    QLineEdit* IEC61850ServerIPAddressText;
    QLineEdit* IEC61850ServerIPPortText;
    QSpinBox* NItems;
    QSpinBox* PollInterval;
    QLabel* textLabel6;

public slots:
    virtual void Help();
    virtual void OkClicked();

protected:

protected slots:
    virtual void languageChange();

};

#endif // FORM1_H
