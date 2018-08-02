/****************************************************************************
** Form implementation generated from reading ui file 'choosetableform.ui'
**
** Created: Sat May 5 20:10:28 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "choosetableform.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "choosetableform.ui.h"
/*
 *  Constructs a chooseTableForm as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
chooseTableForm::chooseTableForm( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "chooseTableForm" );
    chooseTableFormLayout = new QVBoxLayout( this, 11, 6, "chooseTableFormLayout"); 

    layout21 = new QHBoxLayout( 0, 0, 6, "layout21"); 

    textLabel2 = new QLabel( this, "textLabel2" );
    layout21->addWidget( textLabel2 );

    comboOptions = new QComboBox( FALSE, this, "comboOptions" );
    layout21->addWidget( comboOptions );
    chooseTableFormLayout->addLayout( layout21 );
    QSpacerItem* spacer = new QSpacerItem( 20, 41, QSizePolicy::Minimum, QSizePolicy::Expanding );
    chooseTableFormLayout->addItem( spacer );

    layout19 = new QHBoxLayout( 0, 0, 6, "layout19"); 
    QSpacerItem* spacer_2 = new QSpacerItem( 31, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout19->addItem( spacer_2 );

    buttonEdit = new QPushButton( this, "buttonEdit" );
    layout19->addWidget( buttonEdit );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setDefault( TRUE );
    layout19->addWidget( buttonCancel );
    chooseTableFormLayout->addLayout( layout19 );
    languageChange();
    resize( QSize(256, 163).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( buttonEdit, SIGNAL( clicked() ), this, SLOT( editPressed() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
chooseTableForm::~chooseTableForm()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void chooseTableForm::languageChange()
{
    setCaption( tr( "Select table to edit" ) );
    textLabel2->setText( tr( "Table name:" ) );
    QToolTip::add( comboOptions, tr( "Choose the table to delete" ) );
    QWhatsThis::add( comboOptions, tr( "Use this control to select the name of the table to be edited" ) );
    buttonEdit->setText( tr( "Edit" ) );
    QToolTip::add( buttonEdit, tr( "Edit table" ) );
    buttonCancel->setText( tr( "Cancel" ) );
    QToolTip::add( buttonCancel, tr( "Cancel and close dialog box" ) );
}

