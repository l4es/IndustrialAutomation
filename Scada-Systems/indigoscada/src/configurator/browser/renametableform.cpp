/****************************************************************************
** Form implementation generated from reading ui file 'renametableform.ui'
**
** Created: Sat May 5 20:10:30 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "renametableform.h"

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
#include "renametableform.ui.h"
/*
 *  Constructs a renameTableForm as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
renameTableForm::renameTableForm( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "renameTableForm" );
    renameTableFormLayout = new QVBoxLayout( this, 11, 6, "renameTableFormLayout"); 

    tablenameLineEdit = new QLineEdit( this, "tablenameLineEdit" );
    renameTableFormLayout->addWidget( tablenameLineEdit );
    QSpacerItem* spacer = new QSpacerItem( 20, 21, QSizePolicy::Minimum, QSizePolicy::Expanding );
    renameTableFormLayout->addItem( spacer );

    layout34 = new QHBoxLayout( 0, 0, 6, "layout34"); 
    QSpacerItem* spacer_2 = new QSpacerItem( 31, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout34->addItem( spacer_2 );

    closeButton = new QPushButton( this, "closeButton" );
    layout34->addWidget( closeButton );

    renameButton = new QPushButton( this, "renameButton" );
    renameButton->setDefault( TRUE );
    layout34->addWidget( renameButton );
    renameTableFormLayout->addLayout( layout34 );
    languageChange();
    resize( QSize(313, 101).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( closeButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( renameButton, SIGNAL( clicked() ), this, SLOT( renameClicked() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
renameTableForm::~renameTableForm()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void renameTableForm::languageChange()
{
    setCaption( tr( "Rename table" ) );
    closeButton->setText( tr( "Cancel" ) );
    renameButton->setText( tr( "Rename" ) );
}

