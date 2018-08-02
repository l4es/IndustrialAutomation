/****************************************************************************
** Form implementation generated from reading ui file 'edittableform.ui'
**
** Created: Sat May 5 20:10:29 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "edittableform.h"

#include <qvariant.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qheader.h>
#include <qlistview.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "renametableform.h"
#include "addfieldform.h"
#include "editfieldform.h"
#include "edittableform.ui.h"
/*
 *  Constructs a editTableForm as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
editTableForm::editTableForm( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "editTableForm" );
    editTableFormLayout = new QGridLayout( this, 1, 1, 11, 6, "editTableFormLayout"); 

    layout50 = new QVBoxLayout( 0, 0, 6, "layout50"); 

    tableLine = new QLineEdit( this, "tableLine" );
    tableLine->setFocusPolicy( QLineEdit::NoFocus );
    tableLine->setReadOnly( TRUE );
    layout50->addWidget( tableLine );

    fieldListView = new QListView( this, "fieldListView" );
    fieldListView->addColumn( tr( "Field name" ) );
    fieldListView->addColumn( tr( "Field type" ) );
    fieldListView->setResizePolicy( QScrollView::Manual );
    fieldListView->setResizeMode( QListView::AllColumns );
    layout50->addWidget( fieldListView );

    editTableFormLayout->addLayout( layout50, 0, 0 );

    layout42 = new QVBoxLayout( 0, 0, 6, "layout42"); 

    renameTableButton = new QPushButton( this, "renameTableButton" );
    layout42->addWidget( renameTableButton );
    QSpacerItem* spacer = new QSpacerItem( 20, 23, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout42->addItem( spacer );

    renameFieldButton = new QPushButton( this, "renameFieldButton" );
    renameFieldButton->setEnabled( FALSE );
    layout42->addWidget( renameFieldButton );

    removeFieldButton = new QPushButton( this, "removeFieldButton" );
    removeFieldButton->setEnabled( FALSE );
    layout42->addWidget( removeFieldButton );

    addFieldButton = new QPushButton( this, "addFieldButton" );
    layout42->addWidget( addFieldButton );

    editTableFormLayout->addLayout( layout42, 0, 1 );

    layout41 = new QHBoxLayout( 0, 0, 6, "layout41"); 
    QSpacerItem* spacer_2 = new QSpacerItem( 161, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout41->addItem( spacer_2 );

    closeButton = new QPushButton( this, "closeButton" );
    layout41->addWidget( closeButton );

    editTableFormLayout->addMultiCellLayout( layout41, 1, 1, 0, 1 );
    languageChange();
    resize( QSize(428, 266).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( closeButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( renameTableButton, SIGNAL( clicked() ), this, SLOT( renameTable() ) );
    connect( removeFieldButton, SIGNAL( clicked() ), this, SLOT( removeField() ) );
    connect( addFieldButton, SIGNAL( clicked() ), this, SLOT( addField() ) );
    connect( renameFieldButton, SIGNAL( clicked() ), this, SLOT( editField() ) );
    connect( fieldListView, SIGNAL( selectionChanged() ), this, SLOT( fieldSelectionChanged() ) );
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
editTableForm::~editTableForm()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void editTableForm::languageChange()
{
    setCaption( tr( "Edit table definition" ) );
    fieldListView->header()->setLabel( 0, tr( "Field name" ) );
    fieldListView->header()->setLabel( 1, tr( "Field type" ) );
    fieldListView->clear();
    QListViewItem * item = new QListViewItem( fieldListView, 0 );
    item->setText( 0, tr( "New Item" ) );

    renameTableButton->setText( tr( "Rename table" ) );
    renameFieldButton->setText( tr( "Edit field" ) );
    removeFieldButton->setText( tr( "Remove field" ) );
    addFieldButton->setText( tr( "Add field" ) );
    closeButton->setText( tr( "Close" ) );
}

