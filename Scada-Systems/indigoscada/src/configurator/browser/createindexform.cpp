/****************************************************************************
** Form implementation generated from reading ui file 'createindexform.ui'
**
** Created: Sat May 5 20:10:29 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "createindexform.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qgroupbox.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "qmessagebox.h"
#include "createindexform.ui.h"
/*
 *  Constructs a createIndexForm as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
createIndexForm::createIndexForm( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "createIndexForm" );
    createIndexFormLayout = new QVBoxLayout( this, 11, 6, "createIndexFormLayout"); 

    layout15 = new QHBoxLayout( 0, 0, 6, "layout15"); 

    textLabel2 = new QLabel( this, "textLabel2" );
    layout15->addWidget( textLabel2 );

    indexLineEdit = new QLineEdit( this, "indexLineEdit" );
    indexLineEdit->setFrameShape( QLineEdit::LineEditPanel );
    indexLineEdit->setFrameShadow( QLineEdit::Sunken );
    layout15->addWidget( indexLineEdit );
    createIndexFormLayout->addLayout( layout15 );

    groupBox2 = new QGroupBox( this, "groupBox2" );
    groupBox2->setColumnLayout(0, Qt::Vertical );
    groupBox2->layout()->setSpacing( 6 );
    groupBox2->layout()->setMargin( 11 );
    groupBox2Layout = new QGridLayout( groupBox2->layout() );
    groupBox2Layout->setAlignment( Qt::AlignTop );

    layout12 = new QVBoxLayout( 0, 0, 6, "layout12"); 

    comboTables = new QComboBox( FALSE, groupBox2, "comboTables" );
    layout12->addWidget( comboTables );

    comboFields = new QComboBox( FALSE, groupBox2, "comboFields" );
    layout12->addWidget( comboFields );

    comboOrder = new QComboBox( FALSE, groupBox2, "comboOrder" );
    layout12->addWidget( comboOrder );

    comboUnique = new QComboBox( FALSE, groupBox2, "comboUnique" );
    layout12->addWidget( comboUnique );

    groupBox2Layout->addLayout( layout12, 0, 1 );

    layout13 = new QVBoxLayout( 0, 0, 6, "layout13"); 

    textLabel3 = new QLabel( groupBox2, "textLabel3" );
    textLabel3->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    layout13->addWidget( textLabel3 );

    textLabel4 = new QLabel( groupBox2, "textLabel4" );
    textLabel4->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    layout13->addWidget( textLabel4 );

    textLabel5 = new QLabel( groupBox2, "textLabel5" );
    textLabel5->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    layout13->addWidget( textLabel5 );

    textLabel6 = new QLabel( groupBox2, "textLabel6" );
    textLabel6->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    layout13->addWidget( textLabel6 );

    groupBox2Layout->addLayout( layout13, 0, 0 );
    createIndexFormLayout->addWidget( groupBox2 );

    layout16 = new QHBoxLayout( 0, 0, 6, "layout16"); 
    QSpacerItem* spacer = new QSpacerItem( 51, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout16->addItem( spacer );

    buttonCreate = new QPushButton( this, "buttonCreate" );
    layout16->addWidget( buttonCreate );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setDefault( TRUE );
    layout16->addWidget( buttonCancel );
    createIndexFormLayout->addLayout( layout16 );
    languageChange();
    resize( QSize(300, 258).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonCreate, SIGNAL( clicked() ), this, SLOT( confirmCreate() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( comboTables, SIGNAL( activated(const QString&) ), this, SLOT( tableSelected(const QString&) ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
createIndexForm::~createIndexForm()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void createIndexForm::languageChange()
{
    setCaption( tr( "Create Index" ) );
    textLabel2->setText( tr( "Index name:" ) );
    indexLineEdit->setText( QString::null );
    QToolTip::add( indexLineEdit, tr( "Enter the name for the new index" ) );
    QWhatsThis::add( indexLineEdit, tr( "This area contains the name of the index to be created" ) );
    groupBox2->setTitle( tr( "Define properties:" ) );
    QToolTip::add( comboTables, tr( "Choose the table to index" ) );
    QWhatsThis::add( comboTables, tr( "This control is used to select the table to be indexed. Changing the selected table will automatically update the fields available in the control below" ) );
    QToolTip::add( comboFields, tr( "Choose the field to be indexed" ) );
    QWhatsThis::add( comboFields, tr( "This control specifies the field to be used as an index" ) );
    comboOrder->clear();
    comboOrder->insertItem( tr( "Ascending" ) );
    comboOrder->insertItem( tr( "Descending" ) );
    QToolTip::add( comboOrder, tr( "Choose the index order" ) );
    QWhatsThis::add( comboOrder, tr( "This option controls the ordering of the index. Ascending is the recommended ordering" ) );
    comboUnique->clear();
    comboUnique->insertItem( tr( "Allowed" ) );
    comboUnique->insertItem( tr( "Not allowed" ) );
    QToolTip::add( comboUnique, tr( "Allow duplicate values in the index field" ) );
    QWhatsThis::add( comboUnique, tr( "This control determines if the indexed field allows duplicate values to be inserted into the database. Attempting to insert a duplicate value in an indexed fiield that does not allow this option will generate an error" ) );
    textLabel3->setText( tr( "Table to index:" ) );
    textLabel4->setText( tr( "Field to index:" ) );
    textLabel5->setText( tr( "Indexing order:" ) );
    textLabel6->setText( tr( "Duplicate values:" ) );
    buttonCreate->setText( tr( "Create" ) );
    QToolTip::add( buttonCreate, tr( "Create Index" ) );
    buttonCancel->setText( tr( "Cancel" ) );
    QToolTip::add( buttonCancel, tr( "Cancel and close dialog box" ) );
}

