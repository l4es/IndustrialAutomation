/****************************************************************************
** Form implementation generated from reading ui file 'dnp3_driver_configuration.ui'
**
** Created: Thu Mar 28 15:22:18 2013
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "dnp3driverConfigurationData.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a Dnp3driverConfigurationData as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
Dnp3driverConfigurationData::Dnp3driverConfigurationData( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "Form1" );

    textLabel5 = new QLabel( this, "textLabel5" );
    textLabel5->setGeometry( QRect( 40, 480, 49, 20 ) );

    textLabel6 = new QLabel( this, "textLabel6" );
    textLabel6->setGeometry( QRect( 40, 510, 80, 20 ) );

    HelpButton = new QPushButton( this, "HelpButton" );
    HelpButton->setGeometry( QRect( 170, 550, 82, 26 ) );

    CancelButton = new QPushButton( this, "CancelButton" );
    CancelButton->setGeometry( QRect( 290, 550, 82, 26 ) );

    OkButton = new QPushButton( this, "OkButton" );
    OkButton->setGeometry( QRect( 50, 550, 82, 26 ) );

    IOA_AO = new QSpinBox( this, "IOA_AO" );
    IOA_AO->setGeometry( QRect( 170, 430, 200, 20 ) );
    IOA_AO->setMaxValue( 1000 );

    IOA_BO = new QSpinBox( this, "IOA_BO" );
    IOA_BO->setGeometry( QRect( 170, 390, 200, 20 ) );
    IOA_BO->setMaxValue( 1000 );

    PollInterval = new QSpinBox( this, "PollInterval" );
    PollInterval->setGeometry( QRect( 170, 510, 100, 20 ) );
    PollInterval->setMaxValue( 10000 );

    NItems = new QSpinBox( this, "NItems" );
    NItems->setGeometry( QRect( 170, 480, 100, 20 ) );
    NItems->setMaxValue( 1000 );

    DNP3ServerIPPortText = new QLineEdit( this, "DNP3ServerIPPortText" );
    DNP3ServerIPPortText->setGeometry( QRect( 170, 230, 200, 20 ) );

    DNP3ServerIPAddressText = new QLineEdit( this, "DNP3ServerIPAddressText" );
    DNP3ServerIPAddressText->setGeometry( QRect( 170, 190, 200, 20 ) );

    Name = new QLineEdit( this, "Name" );
    Name->setEnabled( FALSE );
    Name->setGeometry( QRect( 170, 110, 200, 20 ) );
    Name->setFrameShape( QLineEdit::LineEditPanel );
    Name->setFrameShadow( QLineEdit::Sunken );

    textLabel4 = new QLabel( this, "textLabel4" );
    textLabel4->setGeometry( QRect( 40, 230, 100, 20 ) );

    textLabel3 = new QLabel( this, "textLabel3" );
    textLabel3->setGeometry( QRect( 40, 190, 100, 20 ) );

    textLabel2 = new QLabel( this, "textLabel2" );
    textLabel2->setGeometry( QRect( 40, 150, 90, 20 ) );

    textLabel1 = new QLabel( this, "textLabel1" );
    textLabel1->setGeometry( QRect( 40, 110, 49, 20 ) );

    IOA_BI = new QSpinBox( this, "IOA_BI" );
    IOA_BI->setGeometry( QRect( 170, 310, 200, 20 ) );
    IOA_BI->setMaxValue( 1000 );

    IOA_CI = new QSpinBox( this, "IOA_CI" );
    IOA_CI->setGeometry( QRect( 170, 350, 200, 20 ) );
    IOA_CI->setMaxValue( 1000 );

    textLabel9 = new QLabel( this, "textLabel9" );
    textLabel9->setGeometry( QRect( 30, 310, 90, 20 ) );

    textLabel10 = new QLabel( this, "textLabel10" );
    textLabel10->setGeometry( QRect( 30, 350, 90, 20 ) );

    textLabel11 = new QLabel( this, "textLabel11" );
    textLabel11->setGeometry( QRect( 30, 390, 100, 20 ) );

    textLabel11_2 = new QLabel( this, "textLabel11_2" );
    textLabel11_2->setGeometry( QRect( 30, 430, 100, 20 ) );

    textLabel8 = new QLabel( this, "textLabel8" );
    textLabel8->setGeometry( QRect( 29, 270, 90, 20 ) );

    IOA_AI = new QSpinBox( this, "IOA_AI" );
    IOA_AI->setGeometry( QRect( 170, 270, 200, 20 ) );
    IOA_AI->setMaxValue( 1000 );

    ServerID = new QSpinBox( this, "ServerID" );
    ServerID->setGeometry( QRect( 170, 150, 200, 20 ) );
    ServerID->setMaxValue( 1000 );
    languageChange();
    resize( QSize(429, 619).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( OkButton, SIGNAL( clicked() ), this, SLOT( OkClicked() ) );
    connect( CancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( HelpButton, SIGNAL( clicked() ), this, SLOT( Help() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Dnp3driverConfigurationData::~Dnp3driverConfigurationData()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Dnp3driverConfigurationData::languageChange()
{
    setCaption( tr( "Form1" ) );
    textLabel5->setText( tr( "N Items" ) );
    textLabel6->setText( tr( "Poll interval" ) );
    HelpButton->setText( tr( "Help" ) );
    CancelButton->setText( tr( "Cancel" ) );
    OkButton->setText( tr( "Ok" ) );
    textLabel4->setText( tr( "Slave TCP port" ) );
    textLabel3->setText( tr( "Slave IP address" ) );
    textLabel2->setText( tr( "Server ID" ) );
    textLabel1->setText( tr( "Name" ) );
    textLabel9->setText( tr( "First IOA for BI" ) );
    textLabel10->setText( tr( "First IOA for CI" ) );
    textLabel11->setText( tr( "First IOA for BO" ) );
    textLabel11_2->setText( tr( "First IOA for AO" ) );
    textLabel8->setText( tr( "First IOA for AI" ) );
}

void Dnp3driverConfigurationData::Help()
{
    qWarning( "Dnp3driverConfigurationData::Help(): Not implemented yet" );
}

void Dnp3driverConfigurationData::OkClicked()
{
    qWarning( "Dnp3driverConfigurationData::OkClicked(): Not implemented yet" );
}

