/****************************************************************************
** Form implementation generated from reading ui file 'iec61850driver_configuration.ui'
**
** Created: Sun Apr 7 14:40:22 2013
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "iec61850driverConfigurationData.h"

#include <qvariant.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a Iec61850driverConfigurationData as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
Iec61850driverConfigurationData::Iec61850driverConfigurationData( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "Form1" );

    Name = new QLineEdit( this, "Name" );
    Name->setEnabled( FALSE );
    Name->setGeometry( QRect( 170, 30, 200, 20 ) );
    Name->setFrameShape( QLineEdit::LineEditPanel );
    Name->setFrameShadow( QLineEdit::Sunken );

    textLabel1 = new QLabel( this, "textLabel1" );
    textLabel1->setGeometry( QRect( 40, 30, 49, 20 ) );

    textLabel4 = new QLabel( this, "textLabel4" );
    textLabel4->setGeometry( QRect( 40, 110, 90, 20 ) );

    textLabel5 = new QLabel( this, "textLabel5" );
    textLabel5->setGeometry( QRect( 40, 150, 49, 20 ) );

    textLabel3 = new QLabel( this, "textLabel3" );
    textLabel3->setGeometry( QRect( 40, 70, 100, 20 ) );

    CancelButton = new QPushButton( this, "CancelButton" );
    CancelButton->setGeometry( QRect( 290, 230, 82, 26 ) );

    OkButton = new QPushButton( this, "OkButton" );
    OkButton->setGeometry( QRect( 50, 230, 82, 26 ) );

    HelpButton = new QPushButton( this, "HelpButton" );
    HelpButton->setGeometry( QRect( 170, 230, 82, 26 ) );

    IEC61850ServerIPAddressText = new QLineEdit( this, "IEC61850ServerIPAddressText" );
    IEC61850ServerIPAddressText->setGeometry( QRect( 170, 70, 200, 20 ) );

    IEC61850ServerIPPortText = new QLineEdit( this, "IEC61850ServerIPPortText" );
    IEC61850ServerIPPortText->setGeometry( QRect( 170, 110, 200, 20 ) );

    NItems = new QSpinBox( this, "NItems" );
    NItems->setGeometry( QRect( 170, 150, 100, 20 ) );
    NItems->setMaxValue( 1000 );

    PollInterval = new QSpinBox( this, "PollInterval" );
    PollInterval->setGeometry( QRect( 170, 180, 100, 20 ) );
    PollInterval->setMaxValue( 10000 );

    textLabel6 = new QLabel( this, "textLabel6" );
    textLabel6->setGeometry( QRect( 40, 180, 80, 20 ) );
    languageChange();
    resize( QSize(429, 299).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( OkButton, SIGNAL( clicked() ), this, SLOT( OkClicked() ) );
    connect( CancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( HelpButton, SIGNAL( clicked() ), this, SLOT( Help() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Iec61850driverConfigurationData::~Iec61850driverConfigurationData()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Iec61850driverConfigurationData::languageChange()
{
    setCaption( tr( "Form1" ) );
    textLabel1->setText( tr( "Name" ) );
    textLabel4->setText( tr( "TCP port" ) );
    textLabel5->setText( tr( "N Items" ) );
    textLabel3->setText( tr( "Slave IP address" ) );
    CancelButton->setText( tr( "Cancel" ) );
    OkButton->setText( tr( "Ok" ) );
    HelpButton->setText( tr( "Help" ) );
    textLabel6->setText( tr( "Poll interval" ) );
}

void Iec61850driverConfigurationData::Help()
{
    qWarning( "Iec61850driverConfigurationData::Help(): Not implemented yet" );
}

void Iec61850driverConfigurationData::OkClicked()
{
    qWarning( "Iec61850driverConfigurationData::OkClicked(): Not implemented yet" );
}

