/****************************************************************************
** Form implementation generated from reading ui file 'editfieldform.ui'
**
** Created: Sat May 5 20:10:29 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "editfieldform.h"

#include <qvariant.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qtoolbutton.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "sqlitedb.h"
#include "addfieldtypeform.h"
#include "editfieldform.ui.h"
/*
 *  Constructs a editFieldForm as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
editFieldForm::editFieldForm( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "editFieldForm" );
    editFieldFormLayout = new QGridLayout( this, 1, 1, 11, 6, "editFieldFormLayout"); 

    layout23 = new QVBoxLayout( 0, 0, 6, "layout23"); 

    textLabel1 = new QLabel( this, "textLabel1" );
    layout23->addWidget( textLabel1 );

    textLabel2 = new QLabel( this, "textLabel2" );
    layout23->addWidget( textLabel2 );

    editFieldFormLayout->addLayout( layout23, 0, 0 );
    QSpacerItem* spacer = new QSpacerItem( 20, 14, QSizePolicy::Minimum, QSizePolicy::Expanding );
    editFieldFormLayout->addItem( spacer, 1, 1 );

    layout21 = new QHBoxLayout( 0, 0, 6, "layout21"); 
    QSpacerItem* spacer_2 = new QSpacerItem( 41, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout21->addItem( spacer_2 );

    cancelButton = new QPushButton( this, "cancelButton" );
    layout21->addWidget( cancelButton );

    saveButton = new QPushButton( this, "saveButton" );
    saveButton->setEnabled( FALSE );
    saveButton->setDefault( TRUE );
    layout21->addWidget( saveButton );

    editFieldFormLayout->addMultiCellLayout( layout21, 2, 2, 0, 1 );

    layout26 = new QGridLayout( 0, 1, 1, 0, 6, "layout26"); 

    layout25 = new QHBoxLayout( 0, 0, 6, "layout25"); 

    typeBox = new QComboBox( FALSE, this, "typeBox" );
    layout25->addWidget( typeBox );

    typeButton = new QToolButton( this, "typeButton" );
    typeButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, typeButton->sizePolicy().hasHeightForWidth() ) );
    layout25->addWidget( typeButton );

    layout26->addLayout( layout25, 1, 0 );

    nameLineEdit = new QLineEdit( this, "nameLineEdit" );

    layout26->addWidget( nameLineEdit, 0, 0 );

    editFieldFormLayout->addLayout( layout26, 0, 1 );
    languageChange();
    resize( QSize(352, 140).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( saveButton, SIGNAL( clicked() ), this, SLOT( confirmEdit() ) );
    connect( nameLineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( enableSave() ) );
    connect( typeButton, SIGNAL( clicked() ), this, SLOT( getCustomType() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
editFieldForm::~editFieldForm()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void editFieldForm::languageChange()
{
    setCaption( tr( "Edit field name and type" ) );
    textLabel1->setText( tr( "Field name:" ) );
    textLabel2->setText( tr( "Field type:" ) );
    cancelButton->setText( tr( "Cancel" ) );
    saveButton->setText( tr( "Apply Changes" ) );
    typeBox->clear();
    typeBox->insertItem( QString::null );
    typeBox->insertItem( tr( "text" ) );
    typeBox->insertItem( tr( "integer" ) );
    typeBox->insertItem( tr( "blob" ) );
    typeButton->setText( tr( "..." ) );
    QToolTip::add( typeButton, tr( "Custom type" ) );
    QWhatsThis::add( typeButton, tr( "Click this button to enter a new type for the field" ) );
}

