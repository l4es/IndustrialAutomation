/****************************************************************************
** Form implementation generated from reading ui file 'importcsvform.ui'
**
** Created: Sat May 5 20:10:30 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "importcsvform.h"

#include <qvariant.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qtable.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "importcsvform.ui.h"
/*
 *  Constructs a importCSVForm as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
importCSVForm::importCSVForm( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "importCSVForm" );
    setModal( TRUE );
    importCSVFormLayout = new QVBoxLayout( this, 11, 6, "importCSVFormLayout"); 

    layout27 = new QGridLayout( 0, 1, 1, 0, 6, "layout27"); 

    textLabel3 = new QLabel( this, "textLabel3" );

    layout27->addWidget( textLabel3, 0, 0 );

    tableNameEdit = new QLineEdit( this, "tableNameEdit" );

    layout27->addWidget( tableNameEdit, 0, 1 );

    extractFieldNamesCheckbox = new QCheckBox( this, "extractFieldNamesCheckbox" );

    layout27->addMultiCellWidget( extractFieldNamesCheckbox, 2, 2, 0, 1 );

    layout25 = new QHBoxLayout( 0, 0, 6, "layout25"); 

    layout23 = new QVBoxLayout( 0, 0, 6, "layout23"); 

    textLabel1 = new QLabel( this, "textLabel1" );
    layout23->addWidget( textLabel1 );

    textLabel2 = new QLabel( this, "textLabel2" );
    layout23->addWidget( textLabel2 );
    layout25->addLayout( layout23 );
    QSpacerItem* spacer = new QSpacerItem( 81, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout25->addItem( spacer );

    layout22 = new QVBoxLayout( 0, 0, 6, "layout22"); 

    fieldBox = new QComboBox( FALSE, this, "fieldBox" );
    layout22->addWidget( fieldBox );

    quoteBox = new QComboBox( FALSE, this, "quoteBox" );
    layout22->addWidget( quoteBox );
    layout25->addLayout( layout22 );

    layout27->addMultiCellLayout( layout25, 1, 1, 0, 1 );
    importCSVFormLayout->addLayout( layout27 );
    QSpacerItem* spacer_2 = new QSpacerItem( 138, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    importCSVFormLayout->addItem( spacer_2 );

    previewTable = new QTable( this, "previewTable" );
    previewTable->setNumRows( 0 );
    previewTable->setNumCols( 0 );
    previewTable->setReadOnly( TRUE );
    previewTable->setSelectionMode( QTable::NoSelection );
    importCSVFormLayout->addWidget( previewTable );

    layout20 = new QHBoxLayout( 0, 0, 6, "layout20"); 
    QSpacerItem* spacer_3 = new QSpacerItem( 41, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout20->addItem( spacer_3 );

    cancelButton = new QPushButton( this, "cancelButton" );
    layout20->addWidget( cancelButton );

    createButton = new QPushButton( this, "createButton" );
    layout20->addWidget( createButton );
    importCSVFormLayout->addLayout( layout20 );
    languageChange();
    resize( QSize(372, 382).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( createButton, SIGNAL( clicked() ), this, SLOT( createButtonPressed() ) );
    connect( fieldBox, SIGNAL( activated(int) ), this, SLOT( fieldSeparatorChanged() ) );
    connect( quoteBox, SIGNAL( activated(int) ), this, SLOT( textQuoteChanged() ) );
    connect( extractFieldNamesCheckbox, SIGNAL( toggled(bool) ), this, SLOT( extractFieldNamesChanged(bool) ) );
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
importCSVForm::~importCSVForm()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void importCSVForm::languageChange()
{
    setCaption( tr( "Create table from CSV file" ) );
    textLabel3->setText( tr( "New table name:" ) );
    extractFieldNamesCheckbox->setText( tr( "Extract field names from first line" ) );
    textLabel1->setText( tr( "Field separator:" ) );
    textLabel2->setText( tr( "Text quote character:" ) );
    fieldBox->clear();
    fieldBox->insertItem( tr( "," ) );
    fieldBox->insertItem( tr( ";" ) );
    fieldBox->insertItem( tr( "TAB" ) );
    quoteBox->clear();
    quoteBox->insertItem( tr( "\"" ) );
    quoteBox->insertItem( tr( "'" ) );
    quoteBox->insertItem( tr( "\\" ) );
    quoteBox->insertItem( QString::null );
    cancelButton->setText( tr( "Cancel" ) );
    createButton->setText( tr( "Create" ) );
}

