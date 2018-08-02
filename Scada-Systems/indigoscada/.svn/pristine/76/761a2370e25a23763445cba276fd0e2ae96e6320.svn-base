/****************************************************************************
** Form implementation generated from reading ui file 'modbus_driver_configuration.ui'
**
** Created: lun 7. apr 17:09:40 2014
**      by: The User Interface Compiler ($Id: main.cpp 2051 2007-02-21 10:04:20Z chehrlic $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "modbus_driverConfigurationData.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a Modbus_driverConfigurationData as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
Modbus_driverConfigurationData::Modbus_driverConfigurationData( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "Form1" );

    textLabel1 = new QLabel( this, "textLabel1" );
    textLabel1->setGeometry( QRect( 40, 110, 49, 20 ) );

    textLabel4 = new QLabel( this, "textLabel4" );
    textLabel4->setGeometry( QRect( 40, 230, 90, 20 ) );

    textLabel7 = new QLabel( this, "textLabel7" );
    textLabel7->setGeometry( QRect( 40, 270, 90, 20 ) );

    textLabel8 = new QLabel( this, "textLabel8" );
    textLabel8->setGeometry( QRect( 40, 310, 49, 20 ) );

    textLabel9 = new QLabel( this, "textLabel9" );
    textLabel9->setGeometry( QRect( 40, 350, 90, 20 ) );

    textLabel10 = new QLabel( this, "textLabel10" );
    textLabel10->setGeometry( QRect( 40, 390, 90, 20 ) );

    textLabel11 = new QLabel( this, "textLabel11" );
    textLabel11->setGeometry( QRect( 40, 430, 55, 20 ) );

    SerialDevice = new QLineEdit( this, "SerialDevice" );
    SerialDevice->setGeometry( QRect( 170, 270, 200, 20 ) );

    MODBUSServerIPPortText = new QLineEdit( this, "MODBUSServerIPPortText" );
    MODBUSServerIPPortText->setGeometry( QRect( 170, 230, 200, 20 ) );

    MODBUSServerIPAddressText = new QLineEdit( this, "MODBUSServerIPAddressText" );
    MODBUSServerIPAddressText->setGeometry( QRect( 170, 190, 200, 20 ) );

    Name = new QLineEdit( this, "Name" );
    Name->setEnabled( FALSE );
    Name->setGeometry( QRect( 170, 110, 200, 20 ) );
    Name->setFrameShape( QLineEdit::LineEditPanel );
    Name->setFrameShadow( QLineEdit::Sunken );

    textLabel2 = new QLabel( this, "textLabel2" );
    textLabel2->setGeometry( QRect( 40, 150, 90, 20 ) );

    Baud = new QSpinBox( this, "Baud" );
    Baud->setGeometry( QRect( 170, 310, 200, 20 ) );
    Baud->setMaxValue( 200000 );

    DataBits = new QSpinBox( this, "DataBits" );
    DataBits->setGeometry( QRect( 170, 350, 200, 20 ) );
    DataBits->setMaxValue( 8 );

    buttonGroup1 = new QButtonGroup( this, "buttonGroup1" );
    buttonGroup1->setGeometry( QRect( 30, 30, 360, 51 ) );
    buttonGroup1->setExclusive( TRUE );

    TCPButton = new QRadioButton( buttonGroup1, "TCPButton" );
    TCPButton->setGeometry( QRect( 40, 20, 82, 20 ) );

    RTUButton = new QRadioButton( buttonGroup1, "RTUButton" );
    RTUButton->setGeometry( QRect( 250, 20, 82, 20 ) );

    textLabel3 = new QLabel( this, "textLabel3" );
    textLabel3->setGeometry( QRect( 40, 190, 100, 20 ) );

    HelpButton = new QPushButton( this, "HelpButton" );
    HelpButton->setGeometry( QRect( 170, 550, 82, 26 ) );

    CancelButton = new QPushButton( this, "CancelButton" );
    CancelButton->setGeometry( QRect( 290, 550, 82, 26 ) );

    OkButton = new QPushButton( this, "OkButton" );
    OkButton->setGeometry( QRect( 50, 550, 82, 26 ) );

    StopBit = new QSpinBox( this, "StopBit" );
    StopBit->setGeometry( QRect( 170, 390, 200, 20 ) );
    StopBit->setMaxValue( 2 );

    PollInterval = new QSpinBox( this, "PollInterval" );
    PollInterval->setGeometry( QRect( 230, 510, 100, 20 ) );
    PollInterval->setMaxValue( 60000 );

    NItems = new QSpinBox( this, "NItems" );
    NItems->setGeometry( QRect( 230, 480, 100, 20 ) );
    NItems->setMaxValue( 1000 );

    textLabel5 = new QLabel( this, "textLabel5" );
    textLabel5->setGeometry( QRect( 40, 480, 49, 20 ) );

    textLabel6 = new QLabel( this, "textLabel6" );
    textLabel6->setGeometry( QRect( 40, 510, 110, 20 ) );

    Parity = new QComboBox( FALSE, this, "Parity" );
    Parity->setGeometry( QRect( 170, 430, 200, 20 ) );
    languageChange();
    resize( QSize(429, 613).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( OkButton, SIGNAL( clicked() ), this, SLOT( OkClicked() ) );
    connect( CancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( HelpButton, SIGNAL( clicked() ), this, SLOT( Help() ) );
    connect( RTUButton, SIGNAL( toggled(bool) ), this, SLOT( RTUContextActive(bool) ) );
    connect( TCPButton, SIGNAL( toggled(bool) ), this, SLOT( TCPContextActive(bool) ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Modbus_driverConfigurationData::~Modbus_driverConfigurationData()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Modbus_driverConfigurationData::languageChange()
{
    setCaption( tr( "Form1" ) );
    textLabel1->setText( tr( "Name" ) );
    textLabel4->setText( tr( "TCP port" ) );
    textLabel7->setText( tr( "Serial device" ) );
    textLabel8->setText( tr( "Baud" ) );
    textLabel9->setText( tr( "Data bits" ) );
    textLabel10->setText( tr( "Stop bit" ) );
    textLabel11->setText( tr( "Parity" ) );
    buttonGroup1->setTitle( tr( "MODBUS Context" ) );
    TCPButton->setText( tr( "TCP" ) );
    RTUButton->setText( tr( "RTU" ) );
    textLabel3->setText( tr( "Slave IP address" ) );
    HelpButton->setText( tr( "Help" ) );
    CancelButton->setText( tr( "Cancel" ) );
    OkButton->setText( tr( "Ok" ) );
    textLabel5->setText( tr( "N Items" ) );
    textLabel6->setText( tr( "Poll interval (ms)" ) );
}

void Modbus_driverConfigurationData::Help()
{
    qWarning( "Modbus_driverConfigurationData::Help(): Not implemented yet" );
}

void Modbus_driverConfigurationData::OkClicked()
{
    qWarning( "Modbus_driverConfigurationData::OkClicked(): Not implemented yet" );
}

void Modbus_driverConfigurationData::RTUContextActive(bool)
{
    qWarning( "Modbus_driverConfigurationData::RTUContextActive(bool): Not implemented yet" );
}

void Modbus_driverConfigurationData::TCPContextActive(bool)
{
    qWarning( "Modbus_driverConfigurationData::TCPContextActive(bool): Not implemented yet" );
}

