/****************************************************************************
** Form implementation generated from reading ui file 'findform.ui'
**
** Created: Sat May 5 20:10:29 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "findform.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qheader.h>
#include <qlistview.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "findform.ui.h"
/*
 *  Constructs a findForm as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
findForm::findForm( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "findForm" );
    findFormLayout = new QVBoxLayout( this, 11, 6, "findFormLayout"); 

    layout5 = new QGridLayout( 0, 1, 1, 0, 6, "layout5"); 

    searchButton = new QPushButton( this, "searchButton" );

    layout5->addWidget( searchButton, 1, 2 );

    findFieldCombobox = new QComboBox( FALSE, this, "findFieldCombobox" );

    layout5->addWidget( findFieldCombobox, 0, 0 );

    searchLine = new QLineEdit( this, "searchLine" );
    searchLine->setFrameShape( QLineEdit::LineEditPanel );
    searchLine->setFrameShadow( QLineEdit::Sunken );

    layout5->addMultiCellWidget( searchLine, 1, 1, 0, 1 );

    findOperatorComboBox = new QComboBox( FALSE, this, "findOperatorComboBox" );

    layout5->addMultiCellWidget( findOperatorComboBox, 0, 0, 1, 2 );
    findFormLayout->addLayout( layout5 );

    findListView = new QListView( this, "findListView" );
    findListView->addColumn( tr( "Record" ) );
    findListView->addColumn( tr( "Data" ) );
    findListView->setMidLineWidth( 30 );
    findListView->setResizePolicy( QScrollView::Manual );
    findListView->setResizeMode( QListView::LastColumn );
    findFormLayout->addWidget( findListView );

    layout18 = new QHBoxLayout( 0, 0, 6, "layout18"); 

    resultsLabel = new QLabel( this, "resultsLabel" );
    resultsLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)5, 0, 0, resultsLabel->sizePolicy().hasHeightForWidth() ) );
    layout18->addWidget( resultsLabel );
    QSpacerItem* spacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout18->addItem( spacer );
    findFormLayout->addLayout( layout18 );
    languageChange();
    resize( QSize(239, 319).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( searchButton, SIGNAL( clicked() ), this, SLOT( find() ) );
    connect( findListView, SIGNAL( clicked(QListViewItem*) ), this, SLOT( recordSelected(QListViewItem*) ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
findForm::~findForm()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void findForm::languageChange()
{
    setCaption( tr( "Find" ) );
    searchButton->setText( tr( "Search" ) );
    QToolTip::add( searchButton, tr( "Perform the search" ) );
    QWhatsThis::add( searchButton, tr( "This button starts the search process" ) );
    findFieldCombobox->clear();
    findFieldCombobox->insertItem( tr( "user" ) );
    QToolTip::add( findFieldCombobox, tr( "Field to be searched" ) );
    QWhatsThis::add( findFieldCombobox, tr( "Use this control to select the field to be searched in the current table" ) );
    QToolTip::add( searchLine, tr( "Enter values or words to search" ) );
    QWhatsThis::add( searchLine, tr( "This is a place to enter the word or number to be searched in the database" ) );
    findOperatorComboBox->clear();
    findOperatorComboBox->insertItem( tr( "=" ) );
    findOperatorComboBox->insertItem( tr( "contains" ) );
    findOperatorComboBox->insertItem( tr( ">" ) );
    findOperatorComboBox->insertItem( tr( ">=" ) );
    findOperatorComboBox->insertItem( tr( "<=" ) );
    findOperatorComboBox->insertItem( tr( "<" ) );
    QToolTip::add( findOperatorComboBox, tr( "Search criteria: use 'contains' for partial matches" ) );
    QWhatsThis::add( findOperatorComboBox, tr( "This control is used to select the search criteria used to look for the search term in the database. Use '=' or 'contains' to find words, and the comparison symbols to filter numeric data." ) );
    findListView->header()->setLabel( 0, tr( "Record" ) );
    findListView->header()->setLabel( 1, tr( "Data" ) );
    QWhatsThis::add( findListView, tr( "Results of the search will appear in this area. Click on a result to select the corresponding record in the database" ) );
    resultsLabel->setText( tr( "Found:" ) );
}

