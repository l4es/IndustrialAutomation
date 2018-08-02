/****************************************************************************
** Form implementation generated from reading ui file 'aboutform.ui'
**
** Created: Sat May 5 20:10:28 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "aboutform.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "sqlitedb.h"
#include "aboutform.ui.h"
/*
 *  Constructs a aboutForm as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
aboutForm::aboutForm( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "aboutForm" );
    aboutFormLayout = new QVBoxLayout( this, 11, 6, "aboutFormLayout"); 

    textLabel1 = new QLabel( this, "textLabel1" );
    textLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, textLabel1->sizePolicy().hasHeightForWidth() ) );
    textLabel1->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
    aboutFormLayout->addWidget( textLabel1 );

    layout55 = new QHBoxLayout( 0, 0, 6, "layout55"); 
    QSpacerItem* spacer = new QSpacerItem( 141, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout55->addItem( spacer );

    okButton = new QPushButton( this, "okButton" );
    okButton->setDefault( TRUE );
    layout55->addWidget( okButton );
    aboutFormLayout->addLayout( layout55 );
    languageChange();
    resize( QSize(514, 191).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( okButton, SIGNAL( clicked() ), this, SLOT( close() ) );
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
aboutForm::~aboutForm()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void aboutForm::languageChange()
{
    setCaption( tr( "About" ) );
    textLabel1->setText( tr( "IndigoSCADA\n"
"\n"
"protocol configurator built with version 3.7.11 of the SQLite engine.\n"
"\n") );
    okButton->setText( tr( "Close" ) );
}

