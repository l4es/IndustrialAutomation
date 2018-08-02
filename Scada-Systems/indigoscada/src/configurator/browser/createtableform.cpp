/****************************************************************************
** Form implementation generated from reading ui file 'createtableform.ui'
**
** Created: Sat May 5 20:10:29 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "createtableform.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qgroupbox.h>
#include <qheader.h>
#include <qlistview.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "qmessagebox.h"
#include "addfieldform.h"
#include "createtableform.ui.h"
/*
 *  Constructs a createTableForm as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
createTableForm::createTableForm( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "createTableForm" );
    createTableFormLayout = new QVBoxLayout( this, 11, 6, "createTableFormLayout"); 

    layout7 = new QHBoxLayout( 0, 0, 6, "layout7"); 

    textLabel1 = new QLabel( this, "textLabel1" );
    layout7->addWidget( textLabel1 );

    tablenameLineEdit = new QLineEdit( this, "tablenameLineEdit" );
    tablenameLineEdit->setFrameShape( QLineEdit::LineEditPanel );
    tablenameLineEdit->setFrameShadow( QLineEdit::Sunken );
    layout7->addWidget( tablenameLineEdit );
    createTableFormLayout->addLayout( layout7 );

    groupBox1 = new QGroupBox( this, "groupBox1" );
    groupBox1->setColumnLayout(0, Qt::Vertical );
    groupBox1->layout()->setSpacing( 6 );
    groupBox1->layout()->setMargin( 11 );
    groupBox1Layout = new QVBoxLayout( groupBox1->layout() );
    groupBox1Layout->setAlignment( Qt::AlignTop );

    fieldListView = new QListView( groupBox1, "fieldListView" );
    fieldListView->addColumn( tr( "Field name" ) );
    fieldListView->addColumn( tr( "Field type" ) );
    fieldListView->setResizePolicy( QScrollView::Manual );
    fieldListView->setResizeMode( QListView::AllColumns );
    groupBox1Layout->addWidget( fieldListView );

    layout6 = new QHBoxLayout( 0, 0, 6, "layout6"); 
    QSpacerItem* spacer = new QSpacerItem( 111, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout6->addItem( spacer );

    buttonAddField = new QPushButton( groupBox1, "buttonAddField" );
    layout6->addWidget( buttonAddField );

    buttonDeleteField = new QPushButton( groupBox1, "buttonDeleteField" );
    buttonDeleteField->setEnabled( FALSE );
    layout6->addWidget( buttonDeleteField );
    groupBox1Layout->addLayout( layout6 );
    createTableFormLayout->addWidget( groupBox1 );

    layout8 = new QHBoxLayout( 0, 0, 6, "layout8"); 
    QSpacerItem* spacer_2 = new QSpacerItem( 91, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout8->addItem( spacer_2 );

    buttonCreate = new QPushButton( this, "buttonCreate" );
    layout8->addWidget( buttonCreate );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setDefault( TRUE );
    layout8->addWidget( buttonCancel );
    createTableFormLayout->addLayout( layout8 );
    languageChange();
    resize( QSize(309, 320).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( buttonCreate, SIGNAL( clicked() ), this, SLOT( confirmCreate() ) );
    connect( buttonAddField, SIGNAL( clicked() ), this, SLOT( addField() ) );
    connect( buttonDeleteField, SIGNAL( clicked() ), this, SLOT( deleteField() ) );
    connect( fieldListView, SIGNAL( selectionChanged() ), this, SLOT( fieldSelectionChanged() ) );
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
createTableForm::~createTableForm()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void createTableForm::languageChange()
{
    setCaption( tr( "Create Table" ) );
    textLabel1->setText( tr( "Table name:" ) );
    tablenameLineEdit->setText( QString::null );
    QToolTip::add( tablenameLineEdit, tr( "Enter the name for the new table" ) );
    QWhatsThis::add( tablenameLineEdit, tr( "Use this control to enter the name of the table to be created." ) );
    groupBox1->setTitle( tr( "Define fields:" ) );
    fieldListView->header()->setLabel( 0, tr( "Field name" ) );
    fieldListView->header()->setLabel( 1, tr( "Field type" ) );
    fieldListView->clear();
    QListViewItem * item = new QListViewItem( fieldListView, 0 );
    item->setText( 0, tr( "New Item" ) );

    buttonAddField->setText( tr( "Add" ) );
    QToolTip::add( buttonAddField, tr( "Add a new field definition" ) );
    QWhatsThis::add( buttonAddField, tr( "This button is used to add a new field definition to your table" ) );
    buttonDeleteField->setText( tr( "Delete" ) );
    QToolTip::add( buttonDeleteField, tr( "Delete current field definition" ) );
    QWhatsThis::add( buttonDeleteField, tr( "This button is used to delete the currently selected field definition from your table" ) );
    buttonCreate->setText( tr( "Create" ) );
    QToolTip::add( buttonCreate, tr( "Create the table" ) );
    buttonCancel->setText( tr( "Cancel" ) );
    QToolTip::add( buttonCancel, tr( "Cancel and close dialog box" ) );
}

