/****************************************************************************
** Form implementation generated from reading ui file 'addfieldform.ui'
**
** Created: Sat May 5 20:10:28 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "addfieldform.h"

#include <qvariant.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "sqlitedb.h"
#include "addfieldtypeform.h"
#include "addfieldform.ui.h"
/*
 *  Constructs a addFieldForm as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
addFieldForm::addFieldForm( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "addFieldForm" );
    addFieldFormLayout = new QGridLayout( this, 1, 1, 11, 6, "addFieldFormLayout"); 
    QSpacerItem* spacer = new QSpacerItem( 20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding );
    addFieldFormLayout->addItem( spacer, 1, 1 );

    layout21 = new QHBoxLayout( 0, 0, 6, "layout21"); 
    QSpacerItem* spacer_2 = new QSpacerItem( 41, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout21->addItem( spacer_2 );

    cancelButton = new QPushButton( this, "cancelButton" );
    layout21->addWidget( cancelButton );

    createButton = new QPushButton( this, "createButton" );
    createButton->setDefault( TRUE );
    layout21->addWidget( createButton );

    addFieldFormLayout->addMultiCellLayout( layout21, 2, 2, 0, 1 );

    layout23 = new QVBoxLayout( 0, 0, 6, "layout23"); 

    textLabel1 = new QLabel( this, "textLabel1" );
    layout23->addWidget( textLabel1 );

    textLabel2 = new QLabel( this, "textLabel2" );
    layout23->addWidget( textLabel2 );

    addFieldFormLayout->addLayout( layout23, 0, 0 );

    layout7 = new QVBoxLayout( 0, 0, 6, "layout7"); 

    nameLineEdit = new QLineEdit( this, "nameLineEdit" );
    layout7->addWidget( nameLineEdit );

    layout6 = new QHBoxLayout( 0, 0, 6, "layout6"); 

    typeBox = new QComboBox( FALSE, this, "typeBox" );
    layout6->addWidget( typeBox );

    typeButton = new QToolButton( this, "typeButton" );
    typeButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, typeButton->sizePolicy().hasHeightForWidth() ) );
    layout6->addWidget( typeButton );
    layout7->addLayout( layout6 );

    addFieldFormLayout->addLayout( layout7, 0, 1 );
    languageChange();
    resize( QSize(338, 136).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( createButton, SIGNAL( clicked() ), this, SLOT( confirmAddField() ) );
    connect( typeButton, SIGNAL( clicked() ), this, SLOT( getCustomType() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
addFieldForm::~addFieldForm()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void addFieldForm::languageChange()
{
    setCaption( tr( "Add database field" ) );
    cancelButton->setText( tr( "Cancel" ) );
    createButton->setText( tr( "Create" ) );
    textLabel1->setText( tr( "Field name:" ) );
    textLabel2->setText( tr( "Field type:" ) );
    typeBox->clear();
    typeBox->insertItem( QString::null );
    typeBox->insertItem( tr( "text" ) );
    typeBox->insertItem( tr( "integer" ) );
    typeBox->insertItem( tr( "blob" ) );
    typeButton->setText( tr( "..." ) );
    QToolTip::add( typeButton, tr( "Custom type" ) );
    QWhatsThis::add( typeButton, tr( "Click this button to enter a new type for the field" ) );
}

