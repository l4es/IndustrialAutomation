/****************************************************************************
** Form interface generated from reading ui file 'modbus_driver_configuration.ui'
**
** Created: lun 7. apr 17:08:45 2014
**      by: The User Interface Compiler ($Id: main.cpp 2051 2007-02-21 10:04:20Z chehrlic $)
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
class QSpacerItem;
class QLabel;
class QLineEdit;
class QSpinBox;
class QButtonGroup;
class QRadioButton;
class QPushButton;
class QComboBox;

class Modbus_driverConfigurationData : public QDialog
{
    Q_OBJECT

public:
    Modbus_driverConfigurationData( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~Modbus_driverConfigurationData();

    QLabel* textLabel1;
    QLabel* textLabel4;
    QLabel* textLabel7;
    QLabel* textLabel8;
    QLabel* textLabel9;
    QLabel* textLabel10;
    QLabel* textLabel11;
    QLineEdit* SerialDevice;
    QLineEdit* MODBUSServerIPPortText;
    QLineEdit* MODBUSServerIPAddressText;
    QLineEdit* Name;
    QLabel* textLabel2;
    QSpinBox* Baud;
    QSpinBox* DataBits;
    QButtonGroup* buttonGroup1;
    QRadioButton* TCPButton;
    QRadioButton* RTUButton;
    QLabel* textLabel3;
    QPushButton* HelpButton;
    QPushButton* CancelButton;
    QPushButton* OkButton;
    QSpinBox* StopBit;
    QSpinBox* PollInterval;
    QSpinBox* NItems;
    QLabel* textLabel5;
    QLabel* textLabel6;
    QComboBox* Parity;

public slots:
    virtual void Help();
    virtual void OkClicked();
    virtual void RTUContextActive(bool);
    virtual void TCPContextActive(bool);

protected:

protected slots:
    virtual void languageChange();

};

#endif // FORM1_H
