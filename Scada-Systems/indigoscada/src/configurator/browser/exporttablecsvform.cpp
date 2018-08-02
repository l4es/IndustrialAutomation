/****************************************************************************
** Form implementation generated from reading ui file 'exporttablecsvform.ui'
**
** Created: Sat May 5 20:10:29 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "exporttablecsvform.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "exporttablecsvform.ui.h"
/*
 *  Constructs a exportTableCSVForm as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
exportTableCSVForm::exportTableCSVForm( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "exportTableCSVForm" );
    exportTableCSVFormLayout = new QVBoxLayout( this, 11, 6, "exportTableCSVFormLayout"); 

    layout21 = new QHBoxLayout( 0, 0, 6, "layout21"); 

    textLabel2 = new QLabel( this, "textLabel2" );
    layout21->addWidget( textLabel2 );

    comboOptions = new QComboBox( FALSE, this, "comboOptions" );
    layout21->addWidget( comboOptions );
    exportTableCSVFormLayout->addLayout( layout21 );
    QSpacerItem* spacer = new QSpacerItem( 20, 41, QSizePolicy::Minimum, QSizePolicy::Expanding );
    exportTableCSVFormLayout->addItem( spacer );

    layout69 = new QHBoxLayout( 0, 0, 6, "layout69"); 
    QSpacerItem* spacer_2 = new QSpacerItem( 29, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout69->addItem( spacer_2 );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setDefault( FALSE );
    layout69->addWidget( buttonCancel );

    buttonExport = new QPushButton( this, "buttonExport" );
    layout69->addWidget( buttonExport );
    exportTableCSVFormLayout->addLayout( layout69 );
    languageChange();
    resize( QSize(365, 150).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonExport, SIGNAL( clicked() ), this, SLOT( exportPressed() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
exportTableCSVForm::~exportTableCSVForm()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void exportTableCSVForm::languageChange()
{
    setCaption( tr( "Choose table to export as CSV text" ) );
    textLabel2->setText( tr( "Table name:" ) );
    QToolTip::add( comboOptions, tr( "Choose the table to delete" ) );
    QWhatsThis::add( comboOptions, tr( "Use this control to select the name of the table to be deleted" ) );
    buttonCancel->setText( tr( "Cancel" ) );
    QToolTip::add( buttonCancel, tr( "Cancel and close dialog box" ) );
    buttonExport->setText( tr( "Export" ) );
    QToolTip::add( buttonExport, tr( "Delete the selected table" ) );
}

