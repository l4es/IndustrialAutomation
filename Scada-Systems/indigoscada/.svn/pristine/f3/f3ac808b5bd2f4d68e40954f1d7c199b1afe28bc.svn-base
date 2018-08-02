/****************************************************************************
** Form implementation generated from reading ui file 'addfieldtypeform.ui'
**
** Created: Sat May 5 20:10:28 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "addfieldtypeform.h"

#include <qvariant.h>
#include <qmessagebox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "sqlitedb.h"
/*
 *  Constructs a addFieldTypeForm as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
addFieldTypeForm::addFieldTypeForm( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "addFieldTypeForm" );
    addFieldTypeFormLayout = new QGridLayout( this, 1, 1, 11, 6, "addFieldTypeFormLayout"); 

    typeNameEdit = new QLineEdit( this, "typeNameEdit" );

    addFieldTypeFormLayout->addWidget( typeNameEdit, 0, 0 );

    layout11 = new QHBoxLayout( 0, 0, 6, "layout11"); 
    QSpacerItem* spacer = new QSpacerItem( 31, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout11->addItem( spacer );

    cancelButton = new QPushButton( this, "cancelButton" );
    layout11->addWidget( cancelButton );

    okButton = new QPushButton( this, "okButton" );
    okButton->setDefault( TRUE );
    layout11->addWidget( okButton );

    addFieldTypeFormLayout->addLayout( layout11, 1, 0 );
    languageChange();
    resize( QSize(294, 98).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( okButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
addFieldTypeForm::~addFieldTypeForm()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void addFieldTypeForm::languageChange()
{
    setCaption( tr( "Enter field type" ) );
    cancelButton->setText( tr( "Cancel" ) );
    okButton->setText( tr( "OK" ) );
}

