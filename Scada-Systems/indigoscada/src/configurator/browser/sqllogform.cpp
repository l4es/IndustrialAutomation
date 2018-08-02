/****************************************************************************
** Form implementation generated from reading ui file 'sqllogform.ui'
**
** Created: Sat May 5 20:10:30 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "sqllogform.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qwidgetstack.h>
#include <qwidget.h>
#include <qtextedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "sqlitedb.h"
#include "sqllogform.ui.h"
/*
 *  Constructs a sqlLogForm as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
sqlLogForm::sqlLogForm( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "sqlLogForm" );
    sqlLogFormLayout = new QVBoxLayout( this, 11, 6, "sqlLogFormLayout"); 

    layout6 = new QHBoxLayout( 0, 0, 6, "layout6"); 

    textLabel1 = new QLabel( this, "textLabel1" );
    layout6->addWidget( textLabel1 );

    comboBox3 = new QComboBox( FALSE, this, "comboBox3" );
    layout6->addWidget( comboBox3 );
    QSpacerItem* spacer = new QSpacerItem( 150, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout6->addItem( spacer );

    clearButton = new QPushButton( this, "clearButton" );
    layout6->addWidget( clearButton );
    sqlLogFormLayout->addLayout( layout6 );

    logStack = new QWidgetStack( this, "logStack" );

    WStackPage = new QWidget( logStack, "WStackPage" );
    WStackPageLayout = new QGridLayout( WStackPage, 1, 1, 11, 6, "WStackPageLayout"); 

    userLogText = new QTextEdit( WStackPage, "userLogText" );
    userLogText->setTextFormat( QTextEdit::PlainText );
    userLogText->setReadOnly( TRUE );

    WStackPageLayout->addWidget( userLogText, 0, 0 );
    logStack->addWidget( WStackPage, 0 );

    WStackPage_2 = new QWidget( logStack, "WStackPage_2" );
    WStackPageLayout_2 = new QVBoxLayout( WStackPage_2, 11, 6, "WStackPageLayout_2"); 

    appLogText = new QTextEdit( WStackPage_2, "appLogText" );
    appLogText->setTextFormat( QTextEdit::PlainText );
    appLogText->setReadOnly( TRUE );
    WStackPageLayout_2->addWidget( appLogText );
    logStack->addWidget( WStackPage_2, 1 );
    sqlLogFormLayout->addWidget( logStack );
    languageChange();
    resize( QSize(547, 203).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( clearButton, SIGNAL( clicked() ), this, SLOT( clearLog() ) );
    connect( comboBox3, SIGNAL( activated(int) ), logStack, SLOT( raiseWidget(int) ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
sqlLogForm::~sqlLogForm()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void sqlLogForm::languageChange()
{
    setCaption( tr( "SQL Log" ) );
    textLabel1->setText( tr( "Show SQL submitted by:" ) );
    comboBox3->clear();
    comboBox3->insertItem( tr( "User" ) );
    comboBox3->insertItem( tr( "Application" ) );
    clearButton->setText( tr( "Clear" ) );
}

