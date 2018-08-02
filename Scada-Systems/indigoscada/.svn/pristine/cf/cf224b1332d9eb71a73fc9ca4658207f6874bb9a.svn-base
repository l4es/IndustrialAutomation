/****************************************************************************
** Form implementation generated from reading ui file 'editform.ui'
**
** Created: Sat May 5 20:10:29 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "editform.h"

#include <qvariant.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qwidgetstack.h>
#include <qwidget.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qframe.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "sqlitedb.h"
#include "editform.ui.h"
/*
 *  Constructs a editForm as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
editForm::editForm( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "editForm" );
    setModal( TRUE );
    editFormLayout = new QVBoxLayout( this, 11, 6, "editFormLayout"); 

    layout11 = new QHBoxLayout( 0, 0, 6, "layout11"); 

    importButton = new QToolButton( this, "importButton" );
    importButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, importButton->sizePolicy().hasHeightForWidth() ) );
    layout11->addWidget( importButton );

    exportButton = new QToolButton( this, "exportButton" );
    exportButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, exportButton->sizePolicy().hasHeightForWidth() ) );
    layout11->addWidget( exportButton );
    QSpacerItem* spacer = new QSpacerItem( 81, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout11->addItem( spacer );

    clearButton = new QToolButton( this, "clearButton" );
    clearButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, clearButton->sizePolicy().hasHeightForWidth() ) );
    layout11->addWidget( clearButton );
    editFormLayout->addLayout( layout11 );

    editWidgetStack = new QWidgetStack( this, "editWidgetStack" );

    WStackPage = new QWidget( editWidgetStack, "WStackPage" );
    WStackPageLayout = new QGridLayout( WStackPage, 1, 1, 11, 6, "WStackPageLayout"); 

    textEditor = new QTextEdit( WStackPage, "textEditor" );

    WStackPageLayout->addWidget( textEditor, 0, 0 );
    editWidgetStack->addWidget( WStackPage, 0 );

    WStackPage_2 = new QWidget( editWidgetStack, "WStackPage_2" );
    WStackPageLayout_2 = new QGridLayout( WStackPage_2, 1, 1, 11, 6, "WStackPageLayout_2"); 

    editPixmap = new QLabel( WStackPage_2, "editPixmap" );
    editPixmap->setMaximumSize( QSize( 320, 240 ) );
    editPixmap->setScaledContents( TRUE );
    editPixmap->setAlignment( int( QLabel::AlignCenter ) );

    WStackPageLayout_2->addWidget( editPixmap, 0, 0 );
    editWidgetStack->addWidget( WStackPage_2, 1 );
    editFormLayout->addWidget( editWidgetStack );

    frame9 = new QFrame( this, "frame9" );
    frame9->setFrameShape( QFrame::Box );
    frame9->setFrameShadow( QFrame::Sunken );
    frame9Layout = new QVBoxLayout( frame9, 11, 6, "frame9Layout"); 

    currentTypeLabel = new QLabel( frame9, "currentTypeLabel" );
    currentTypeLabel->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignLeft ) );
    frame9Layout->addWidget( currentTypeLabel );

    currentDataInfo = new QLabel( frame9, "currentDataInfo" );
    currentDataInfo->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter | QLabel::AlignLeft ) );
    frame9Layout->addWidget( currentDataInfo );
    editFormLayout->addWidget( frame9 );

    layout3 = new QHBoxLayout( 0, 0, 6, "layout3"); 
    QSpacerItem* spacer_2 = new QSpacerItem( 90, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout3->addItem( spacer_2 );

    closeButton = new QPushButton( this, "closeButton" );
    closeButton->setDefault( TRUE );
    layout3->addWidget( closeButton );

    saveChangesButton = new QPushButton( this, "saveChangesButton" );
    saveChangesButton->setEnabled( FALSE );
    saveChangesButton->setDefault( FALSE );
    layout3->addWidget( saveChangesButton );
    editFormLayout->addLayout( layout3 );
    languageChange();
    resize( QSize(366, 431).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( importButton, SIGNAL( clicked() ), this, SLOT( importData() ) );
    connect( exportButton, SIGNAL( clicked() ), this, SLOT( exportData() ) );
    connect( saveChangesButton, SIGNAL( clicked() ), this, SLOT( saveChanges() ) );
    connect( clearButton, SIGNAL( clicked() ), this, SLOT( clearData() ) );
    connect( textEditor, SIGNAL( textChanged() ), this, SLOT( editTextChanged() ) );
    connect( closeButton, SIGNAL( clicked() ), this, SLOT( close() ) );
    connect( textEditor, SIGNAL( clicked(int,int) ), this, SLOT( editorClicked() ) );
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
editForm::~editForm()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void editForm::languageChange()
{
    setCaption( tr( "Edit database cell" ) );
    importButton->setText( tr( "Import" ) );
    QToolTip::add( importButton, tr( "Import text" ) );
    QWhatsThis::add( importButton, tr( "Opens a file dialog used to import text to this database cell." ) );
    exportButton->setText( tr( "Export" ) );
    QToolTip::add( exportButton, tr( "Export text" ) );
    QWhatsThis::add( exportButton, tr( "Opens a file dialog used to export the contents of this database cell to a text file." ) );
    clearButton->setText( tr( "Clear" ) );
    QToolTip::add( clearButton, tr( "Clear cell data" ) );
    QWhatsThis::add( clearButton, tr( "Erases the contents of the cell" ) );
    QToolTip::add( frame9, QString::null );
    QWhatsThis::add( frame9, tr( "This area displays information about the data present in this database cell" ) );
    currentTypeLabel->setText( tr( "Type of data currently in cell: Empty" ) );
    currentDataInfo->setText( tr( "Data information" ) );
    closeButton->setText( tr( "Close" ) );
    QToolTip::add( closeButton, tr( "Close" ) );
    QWhatsThis::add( closeButton, tr( "Close the window without saving changes" ) );
    saveChangesButton->setText( tr( "Apply Changes" ) );
    QToolTip::add( saveChangesButton, tr( "Save changes to database" ) );
    QWhatsThis::add( saveChangesButton, tr( "Close this window saving changes to the database" ) );
}

