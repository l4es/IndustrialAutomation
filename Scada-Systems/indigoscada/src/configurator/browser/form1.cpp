/****************************************************************************
** Form implementation generated from reading ui file 'form1.ui'
**
** Created: Sat May 5 20:10:30 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "form1.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qheader.h>
#include <qlistview.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qtable.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qimage.h>
#include <qpixmap.h>

#include "qmessagebox.h"
#include "qfiledialog.h"
#include "qfile.h"
#include "qapplication.h"
#include "createtableform.h"
#include "createindexform.h"
#include "deletetableform.h"
#include "deleteindexform.h"
#include "aboutform.h"
#include "choosetableform.h"
#include "edittableform.h"
#include "importcsvform.h"
#include "exporttablecsvform.h"
#include "form1.ui.h"
/*
 *  Constructs a mainForm as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
mainForm::mainForm( QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
{
    (void)statusBar();
    if ( !name )
	setName( "mainForm" );
    setCentralWidget( new QWidget( this, "qt_central_widget" ) );
    mainFormLayout = new QVBoxLayout( centralWidget(), 11, 6, "mainFormLayout"); 

    mainTab = new QTabWidget( centralWidget(), "mainTab" );

    structure = new QWidget( mainTab, "structure" );
    structureLayout = new QVBoxLayout( structure, 11, 6, "structureLayout"); 

    dblistView = new QListView( structure, "dblistView" );
    dblistView->addColumn( tr( "Name" ) );
    dblistView->header()->setClickEnabled( FALSE, dblistView->header()->count() - 1 );
    dblistView->addColumn( tr( "Object" ) );
    dblistView->header()->setClickEnabled( FALSE, dblistView->header()->count() - 1 );
    dblistView->addColumn( tr( "Type" ) );
    dblistView->header()->setClickEnabled( FALSE, dblistView->header()->count() - 1 );
    dblistView->addColumn( tr( "Schema" ) );
    dblistView->header()->setClickEnabled( FALSE, dblistView->header()->count() - 1 );
    dblistView->setResizePolicy( QScrollView::Manual );
    dblistView->setSelectionMode( QListView::NoSelection );
    dblistView->setRootIsDecorated( TRUE );
    dblistView->setResizeMode( QListView::LastColumn );
    structureLayout->addWidget( dblistView );
    mainTab->insertTab( structure, QString("") );

    browser = new QWidget( mainTab, "browser" );
    browserLayout = new QVBoxLayout( browser, 11, 6, "browserLayout"); 

    layout2 = new QHBoxLayout( 0, 0, 6, "layout2"); 

    textLabel1 = new QLabel( browser, "textLabel1" );
    layout2->addWidget( textLabel1 );

    comboBrowseTable = new QComboBox( FALSE, browser, "comboBrowseTable" );
    comboBrowseTable->setMinimumSize( QSize( 115, 0 ) );
    layout2->addWidget( comboBrowseTable );

    buttonFind = new QPushButton( browser, "buttonFind" );
    buttonFind->setPixmap( QPixmap::fromMimeSource( "searchfind.png" ) );
    buttonFind->setToggleButton( TRUE );
    layout2->addWidget( buttonFind );
    QSpacerItem* spacer = new QSpacerItem( 51, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout2->addItem( spacer );

    buttonNewRecord = new QPushButton( browser, "buttonNewRecord" );
    layout2->addWidget( buttonNewRecord );

    buttonDeleteRecord = new QPushButton( browser, "buttonDeleteRecord" );
    layout2->addWidget( buttonDeleteRecord );
    browserLayout->addLayout( layout2 );

    dataTable = new QTable( browser, "dataTable" );
    dataTable->setAcceptDrops( TRUE );
    dataTable->setResizePolicy( QTable::Default );
    dataTable->setVScrollBarMode( QTable::Auto );
    dataTable->setNumRows( 0 );
    dataTable->setNumCols( 0 );
    dataTable->setReadOnly( TRUE );
    dataTable->setSelectionMode( QTable::Single );
    dataTable->setFocusStyle( QTable::FollowStyle );
    browserLayout->addWidget( dataTable );

    layout9 = new QHBoxLayout( 0, 0, 6, "layout9"); 

    buttonPrevious = new QPushButton( browser, "buttonPrevious" );
    buttonPrevious->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, buttonPrevious->sizePolicy().hasHeightForWidth() ) );
    layout9->addWidget( buttonPrevious );

    labelRecordset = new QLabel( browser, "labelRecordset" );
    layout9->addWidget( labelRecordset );

    buttonNext = new QPushButton( browser, "buttonNext" );
    buttonNext->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, buttonNext->sizePolicy().hasHeightForWidth() ) );
    layout9->addWidget( buttonNext );
    QSpacerItem* spacer_2 = new QSpacerItem( 50, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout9->addItem( spacer_2 );

    buttonGoto = new QPushButton( browser, "buttonGoto" );
    layout9->addWidget( buttonGoto );

    editGoto = new QLineEdit( browser, "editGoto" );
    editGoto->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, editGoto->sizePolicy().hasHeightForWidth() ) );
    editGoto->setFrameShape( QLineEdit::LineEditPanel );
    editGoto->setFrameShadow( QLineEdit::Sunken );
    layout9->addWidget( editGoto );
    browserLayout->addLayout( layout9 );
    mainTab->insertTab( browser, QString("") );

    query = new QWidget( mainTab, "query" );
    queryLayout = new QVBoxLayout( query, 11, 6, "queryLayout"); 

    textLabel1_2 = new QLabel( query, "textLabel1_2" );
    queryLayout->addWidget( textLabel1_2 );

    sqlTextEdit = new QTextEdit( query, "sqlTextEdit" );
    sqlTextEdit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)5, 0, 0, sqlTextEdit->sizePolicy().hasHeightForWidth() ) );
    queryLayout->addWidget( sqlTextEdit );

    layout5 = new QHBoxLayout( 0, 0, 6, "layout5"); 

    executeQueryButton = new QPushButton( query, "executeQueryButton" );
    layout5->addWidget( executeQueryButton );
    QSpacerItem* spacer_3 = new QSpacerItem( 325, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout5->addItem( spacer_3 );
    queryLayout->addLayout( layout5 );

    textLabel2 = new QLabel( query, "textLabel2" );
    queryLayout->addWidget( textLabel2 );

    queryErrorLineEdit = new QLineEdit( query, "queryErrorLineEdit" );
    queryErrorLineEdit->setReadOnly( TRUE );
    queryLayout->addWidget( queryErrorLineEdit );

    textLabel3 = new QLabel( query, "textLabel3" );
    queryLayout->addWidget( textLabel3 );

    queryResultListView = new QListView( query, "queryResultListView" );
    queryResultListView->setResizePolicy( QListView::Default );
    queryResultListView->setSelectionMode( QListView::NoSelection );
    queryResultListView->setResizeMode( QListView::AllColumns );
    queryLayout->addWidget( queryResultListView );
    mainTab->insertTab( query, QString("") );
    mainFormLayout->addWidget( mainTab );

    // actions
    fileNewAction = new QAction( this, "fileNewAction" );
    fileNewAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "new.png" ) ) );
    fileOpenAction = new QAction( this, "fileOpenAction" );
    fileOpenAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "open.png" ) ) );
    fileExitAction = new QAction( this, "fileExitAction" );
    editCopyAction = new QAction( this, "editCopyAction" );
    editPasteAction = new QAction( this, "editPasteAction" );
    editFindAction = new QAction( this, "editFindAction" );
    editFindAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "searchfind.png" ) ) );
    helpContentsAction = new QAction( this, "helpContentsAction" );
    helpIndexAction = new QAction( this, "helpIndexAction" );
    helpAboutAction = new QAction( this, "helpAboutAction" );
    fileCloseAction = new QAction( this, "fileCloseAction" );
    fileCloseAction->setEnabled( FALSE );
    newRecordAction = new QAction( this, "newRecordAction" );
    fileCompactAction = new QAction( this, "fileCompactAction" );
    fileCompactAction->setEnabled( FALSE );
    helpWhatsThisAction = new QAction( this, "helpWhatsThisAction" );
    helpWhatsThisAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "whatis.png" ) ) );
    sqlLogAction = new QAction( this, "sqlLogAction" );
    sqlLogAction->setToggleAction( TRUE );
    sqlLogAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "log.png" ) ) );
    fileImportCSVAction = new QAction( this, "fileImportCSVAction" );
    fileExportCSVAction = new QAction( this, "fileExportCSVAction" );
    fileSaveAction = new QAction( this, "fileSaveAction" );
    fileSaveAction->setEnabled( FALSE );
    fileSaveAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "save.png" ) ) );
    fileRevertAction = new QAction( this, "fileRevertAction" );
    fileRevertAction->setEnabled( FALSE );
    fileRevertAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "revert.png" ) ) );
    fileImportAction = new QAction( this, "fileImportAction" );
    fileExportAction = new QAction( this, "fileExportAction" );
    editCreateTableAction = new QAction( this, "editCreateTableAction" );
    editCreateTableAction->setEnabled( FALSE );
    editCreateTableAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "create_table.png" ) ) );
    editDeleteTableAction = new QAction( this, "editDeleteTableAction" );
    editDeleteTableAction->setEnabled( FALSE );
    editDeleteTableAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "delete_table.png" ) ) );
    editModifyTableAction = new QAction( this, "editModifyTableAction" );
    editModifyTableAction->setEnabled( FALSE );
    editModifyTableAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "modify_table.png" ) ) );
    editCreateIndexAction = new QAction( this, "editCreateIndexAction" );
    editCreateIndexAction->setEnabled( FALSE );
    editCreateIndexAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "create_index.png" ) ) );
    editDeleteIndexAction = new QAction( this, "editDeleteIndexAction" );
    editDeleteIndexAction->setEnabled( FALSE );
    editDeleteIndexAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "delete_index.png" ) ) );
    fileImportSQLAction = new QAction( this, "fileImportSQLAction" );
    fileExportSQLAction = new QAction( this, "fileExportSQLAction" );


    // toolbars
    Toolbar = new QToolBar( QString(""), this, DockTop ); 

    fileNewAction->addTo( Toolbar );
    fileOpenAction->addTo( Toolbar );
    fileSaveAction->addTo( Toolbar );
    fileRevertAction->addTo( Toolbar );
    Toolbar->addSeparator();
    editCreateTableAction->addTo( Toolbar );
    editDeleteTableAction->addTo( Toolbar );
    editModifyTableAction->addTo( Toolbar );
    editCreateIndexAction->addTo( Toolbar );
    editDeleteIndexAction->addTo( Toolbar );
    Toolbar->addSeparator();
    sqlLogAction->addTo( Toolbar );
    Toolbar->addSeparator();
    helpWhatsThisAction->addTo( Toolbar );


    // menubar
    menubar = new QMenuBar( this, "menubar" );


    fileMenu = new QPopupMenu( this );
    fileNewAction->addTo( fileMenu );
    fileOpenAction->addTo( fileMenu );
    fileCloseAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    fileSaveAction->addTo( fileMenu );
    fileRevertAction->addTo( fileMenu );
    fileCompactAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    QPopupMenu *PopupMenuEditor_9 = new QPopupMenu( this );
    fileMenu->setAccel( tr( "" ), 
        fileMenu->insertItem( fileImportAction->iconSet(),tr( "Import" ), PopupMenuEditor_9 ) );
    fileImportSQLAction->addTo( PopupMenuEditor_9 );
    fileImportCSVAction->addTo( PopupMenuEditor_9 );
    QPopupMenu *PopupMenuEditor_12 = new QPopupMenu( this );
    fileMenu->setAccel( tr( "" ), 
        fileMenu->insertItem( fileExportAction->iconSet(),tr( "Export" ), PopupMenuEditor_12 ) );
    fileExportSQLAction->addTo( PopupMenuEditor_12 );
    fileExportCSVAction->addTo( PopupMenuEditor_12 );
    fileMenu->insertSeparator();
    fileExitAction->addTo( fileMenu );
    menubar->insertItem( QString(""), fileMenu, 1 );

    EditMenu = new QPopupMenu( this );
    editCreateTableAction->addTo( EditMenu );
    editDeleteTableAction->addTo( EditMenu );
    editModifyTableAction->addTo( EditMenu );
    EditMenu->insertSeparator();
    editCreateIndexAction->addTo( EditMenu );
    editDeleteIndexAction->addTo( EditMenu );
    menubar->insertItem( QString(""), EditMenu, 2 );

    ViewMenu = new QPopupMenu( this );
    sqlLogAction->addTo( ViewMenu );
    menubar->insertItem( QString(""), ViewMenu, 3 );

    PopupMenu = new QPopupMenu( this );
    helpWhatsThisAction->addTo( PopupMenu );
    helpAboutAction->addTo( PopupMenu );
    menubar->insertItem( QString(""), PopupMenu, 4 );

    languageChange();
    resize( QSize(702, 552).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( fileExitAction, SIGNAL( activated() ), this, SLOT( fileExit() ) );
    connect( fileOpenAction, SIGNAL( activated() ), this, SLOT( fileOpen() ) );
    connect( fileNewAction, SIGNAL( activated() ), this, SLOT( fileNew() ) );
    connect( fileCloseAction, SIGNAL( activated() ), this, SLOT( fileClose() ) );
    connect( comboBrowseTable, SIGNAL( activated(const QString&) ), this, SLOT( populateTable(const QString&) ) );
    connect( buttonNewRecord, SIGNAL( clicked() ), this, SLOT( addRecord() ) );
    connect( buttonDeleteRecord, SIGNAL( clicked() ), this, SLOT( deleteRecord() ) );
    connect( buttonPrevious, SIGNAL( clicked() ), this, SLOT( navigatePrevious() ) );
    connect( buttonNext, SIGNAL( clicked() ), this, SLOT( navigateNext() ) );
    connect( editGoto, SIGNAL( returnPressed() ), this, SLOT( navigateGoto() ) );
    connect( buttonGoto, SIGNAL( clicked() ), this, SLOT( navigateGoto() ) );
    connect( buttonFind, SIGNAL( toggled(bool) ), this, SLOT( browseFind(bool) ) );
    connect( fileCompactAction, SIGNAL( activated() ), this, SLOT( compact() ) );
    connect( editCopyAction, SIGNAL( activated() ), this, SLOT( copy() ) );
    connect( editPasteAction, SIGNAL( activated() ), this, SLOT( paste() ) );
    connect( helpWhatsThisAction, SIGNAL( activated() ), this, SLOT( helpWhatsThis() ) );
    connect( helpAboutAction, SIGNAL( activated() ), this, SLOT( helpAbout() ) );
    connect( dataTable, SIGNAL( doubleClicked(int,int,int,const QPoint&) ), this, SLOT( doubleClickTable(int,int,int,const QPoint&) ) );
    connect( mainTab, SIGNAL( selected(const QString&) ), this, SLOT( mainTabSelected(const QString&) ) );
    connect( sqlLogAction, SIGNAL( toggled(bool) ), this, SLOT( toggleLogWindow(bool) ) );
    connect( executeQueryButton, SIGNAL( clicked() ), this, SLOT( executeQuery() ) );
    connect( fileImportCSVAction, SIGNAL( activated() ), this, SLOT( importTableFromCSV() ) );
    connect( fileExportCSVAction, SIGNAL( activated() ), this, SLOT( exportTableToCSV() ) );
    connect( fileRevertAction, SIGNAL( activated() ), this, SLOT( fileRevert() ) );
    connect( fileSaveAction, SIGNAL( activated() ), this, SLOT( fileSave() ) );
    connect( editDeleteIndexAction, SIGNAL( activated() ), this, SLOT( deleteIndex() ) );
    connect( editCreateIndexAction, SIGNAL( activated() ), this, SLOT( createIndex() ) );
    connect( editCreateTableAction, SIGNAL( activated() ), this, SLOT( createTable() ) );
    connect( editDeleteTableAction, SIGNAL( activated() ), this, SLOT( deleteTable() ) );
    connect( editModifyTableAction, SIGNAL( activated() ), this, SLOT( editTable() ) );
    connect( fileExportSQLAction, SIGNAL( activated() ), this, SLOT( exportDatabaseToSQL() ) );
    connect( fileImportSQLAction, SIGNAL( activated() ), this, SLOT( importDatabaseFromSQL() ) );
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
mainForm::~mainForm()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void mainForm::languageChange()
{
    setCaption( tr( "Browser" ) );
    QToolTip::add( mainTab, QString::null );
    dblistView->header()->setLabel( 0, tr( "Name" ) );
    dblistView->header()->setLabel( 1, tr( "Object" ) );
    dblistView->header()->setLabel( 2, tr( "Type" ) );
    dblistView->header()->setLabel( 3, tr( "Schema" ) );
    QWhatsThis::add( dblistView, tr( "This area shows the structure of your database, including all tables and indexes." ) );
    mainTab->changeTab( structure, tr( "Database Structure" ) );
    textLabel1->setText( tr( "Table:" ) );
    comboBrowseTable->clear();
    comboBrowseTable->insertItem( tr( "select" ) );
    comboBrowseTable->insertItem( tr( "users" ) );
    QToolTip::add( comboBrowseTable, tr( "Select a table to browse data" ) );
    QWhatsThis::add( comboBrowseTable, tr( "Use this list to select a table to be displayed in the database view" ) );
    buttonFind->setText( QString::null );
    QToolTip::add( buttonFind, tr( "Open or close the floating find window" ) );
    QWhatsThis::add( buttonFind, tr( "This button toggles the appearance of the Find window, used to search records in the database view" ) );
    buttonNewRecord->setText( tr( "New Record" ) );
    QToolTip::add( buttonNewRecord, tr( "Insert a new record in the current table" ) );
    QWhatsThis::add( buttonNewRecord, tr( "This button creates a new, empty record in the database" ) );
    buttonDeleteRecord->setText( tr( "Delete Record" ) );
    QToolTip::add( buttonDeleteRecord, tr( "Delete the current record" ) );
    QWhatsThis::add( buttonDeleteRecord, tr( "This button deletes the record currently selected in the database" ) );
    QToolTip::add( dataTable, QString::null );
    QWhatsThis::add( dataTable, tr( "This is the database view. You can double-click any record to edit its contents in the cell editor window." ) );
    buttonPrevious->setText( tr( "<" ) );
    QToolTip::add( buttonPrevious, tr( "Go to previous record set page" ) );
    QWhatsThis::add( buttonPrevious, tr( "This button is used to navigate to the previous set of 1000 records in the database view" ) );
    labelRecordset->setText( tr( "1000 - 2000 of 100000" ) );
    buttonNext->setText( tr( ">" ) );
    QToolTip::add( buttonNext, tr( "Go to next record set page" ) );
    QWhatsThis::add( buttonNext, tr( "This button is used to navigate to the next 1000 records set in the database view" ) );
    buttonGoto->setText( tr( "Go to:" ) );
    QToolTip::add( buttonGoto, tr( "Click here to browse the specified record " ) );
    QWhatsThis::add( buttonGoto, tr( "This button is used to navigate to the record number specied in the Go to: area." ) );
    editGoto->setText( tr( "0" ) );
    QToolTip::add( editGoto, tr( "Enter record number to browse" ) );
    QWhatsThis::add( editGoto, tr( "Type a record number in this area and click the Go to: button to display the record in the database view" ) );
    mainTab->changeTab( browser, tr( "Browse Data" ) );
    textLabel1_2->setText( tr( "SQL string:" ) );
    QToolTip::add( sqlTextEdit, tr( "Type SQL string here" ) );
    QWhatsThis::add( sqlTextEdit, tr( "Type the SQL query in this area, and press the button below to execute it. Results and error messages will be shown in the fields at the bottom of this form." ) );
    executeQueryButton->setText( tr( "Execute query" ) );
    QToolTip::add( executeQueryButton, tr( "Execute SQL query" ) );
    QWhatsThis::add( executeQueryButton, tr( "This button is used to execute the SQL query entered in the text area above." ) );
    textLabel2->setText( tr( "Error message from database engine:" ) );
    QToolTip::add( queryErrorLineEdit, tr( "Error messages" ) );
    QWhatsThis::add( queryErrorLineEdit, tr( "Messages returned from the database engine when the SQL query is executed are displayed in this area." ) );
    textLabel3->setText( tr( "Data returned:" ) );
    QToolTip::add( queryResultListView, tr( "Query generated data" ) );
    QWhatsThis::add( queryResultListView, tr( "This table displays data returned from the database engine as a result of the SQL query. You can not modify data directly on this view, only consult it." ) );
    mainTab->changeTab( query, tr( "Execute SQL" ) );
    fileNewAction->setText( tr( "New Database" ) );
    fileNewAction->setMenuText( tr( "&New Database" ) );
    fileNewAction->setToolTip( tr( "Create a new database file" ) );
    fileNewAction->setWhatsThis( tr( "This option is used to create a new database file." ) );
    fileNewAction->setAccel( tr( "Ctrl+N" ) );
    fileOpenAction->setText( tr( "Open Database" ) );
    fileOpenAction->setMenuText( tr( "&Open Database" ) );
    fileOpenAction->setToolTip( tr( "Open an existing database file" ) );
    fileOpenAction->setWhatsThis( tr( "This option is used to open an existing database file." ) );
    fileOpenAction->setAccel( tr( "Ctrl+O" ) );
    fileExitAction->setText( tr( "Exit" ) );
    fileExitAction->setMenuText( tr( "E&xit" ) );
    fileExitAction->setAccel( QString::null );
    editCopyAction->setText( tr( "Copy" ) );
    editCopyAction->setMenuText( tr( "&Copy" ) );
    editCopyAction->setWhatsThis( tr( "Copies the selected text to the clipboard" ) );
    editCopyAction->setAccel( tr( "Ctrl+C" ) );
    editPasteAction->setText( tr( "Paste" ) );
    editPasteAction->setMenuText( tr( "&Paste" ) );
    editPasteAction->setWhatsThis( tr( "Pastes text from the clipboard into the current text insertion point" ) );
    editPasteAction->setAccel( tr( "Ctrl+V" ) );
    editFindAction->setText( tr( "Find" ) );
    editFindAction->setMenuText( tr( "&Find..." ) );
    editFindAction->setAccel( tr( "F" ) );
    helpContentsAction->setText( tr( "Contents" ) );
    helpContentsAction->setMenuText( tr( "&Contents..." ) );
    helpContentsAction->setAccel( QString::null );
    helpIndexAction->setText( tr( "Index" ) );
    helpIndexAction->setMenuText( tr( "&Index..." ) );
    helpIndexAction->setAccel( QString::null );
    helpAboutAction->setText( tr( "About" ) );
    helpAboutAction->setMenuText( tr( "&About" ) );
    helpAboutAction->setAccel( QString::null );
    fileCloseAction->setText( tr( "Close Database" ) );
    fileCloseAction->setMenuText( tr( "Close Database" ) );
    newRecordAction->setText( tr( "New Record" ) );
    newRecordAction->setMenuText( tr( "New Record" ) );
    fileCompactAction->setText( tr( "Compact Database" ) );
    fileCompactAction->setMenuText( tr( "Compact Database" ) );
    fileCompactAction->setStatusTip( tr( "Compact the database file, removing space wasted by deleted records." ) );
    fileCompactAction->setWhatsThis( tr( "Compact the database file, removing space wasted by deleted records." ) );
    helpWhatsThisAction->setText( tr( "What's This?" ) );
    helpWhatsThisAction->setMenuText( tr( "What's This?" ) );
    helpWhatsThisAction->setAccel( tr( "Shift+F1" ) );
    sqlLogAction->setText( tr( "SQL Log" ) );
    sqlLogAction->setMenuText( tr( "SQL Log" ) );
    sqlLogAction->setWhatsThis( tr( "Display or hide the SQL log window, which stores all SQL commands issued by the user or the application." ) );
    fileImportCSVAction->setText( tr( "Table from CSV file" ) );
    fileImportCSVAction->setMenuText( tr( "Table from CSV file" ) );
    fileImportCSVAction->setToolTip( tr( "Open a wizard that lets you import data from a comma separated text file into a database table." ) );
    fileImportCSVAction->setWhatsThis( tr( "Open a wizard that lets you import data from a comma separated text file into a database table. CSV files can be created on most database and spreadsheet applications." ) );
    fileExportCSVAction->setText( tr( "Table as CSV file" ) );
    fileExportCSVAction->setMenuText( tr( "Table as CSV file" ) );
    fileExportCSVAction->setToolTip( tr( "Export a database table as a comma separated text file." ) );
    fileExportCSVAction->setWhatsThis( tr( "Export a database table as a comma separated text file, ready to be imported into other database or spreadsheet applications." ) );
    fileSaveAction->setText( tr( "Save Database" ) );
    fileSaveAction->setMenuText( tr( "Save Database" ) );
    fileSaveAction->setToolTip( tr( "Save changes to the database file" ) );
    fileSaveAction->setWhatsThis( tr( "This option is used to save changes to the database file." ) );
    fileRevertAction->setText( tr( "Revert Database" ) );
    fileRevertAction->setMenuText( tr( "Revert Database" ) );
    fileRevertAction->setToolTip( tr( "Revert database to last saved state" ) );
    fileRevertAction->setWhatsThis( tr( "This option is used to revert the current database file to its last saved state. All changes made since the last save operation are lost." ) );
    fileImportAction->setText( tr( "Import" ) );
    fileImportAction->setMenuText( tr( "Import" ) );
    fileExportAction->setText( tr( "Export" ) );
    fileExportAction->setMenuText( tr( "Export" ) );
    editCreateTableAction->setText( tr( "Create Table" ) );
    editCreateTableAction->setMenuText( tr( "Create Table" ) );
    editCreateTableAction->setWhatsThis( tr( "Open the Create Table wizard, where it is possible to define the name and fields for a new table in the database" ) );
    editDeleteTableAction->setText( tr( "Delete Table" ) );
    editDeleteTableAction->setMenuText( tr( "Delete Table" ) );
    editDeleteTableAction->setWhatsThis( tr( "Open the Delete Table wizard, where you can select a database table to be dropped." ) );
    editModifyTableAction->setText( tr( "Modify Table" ) );
    editModifyTableAction->setMenuText( tr( "Modify Table" ) );
    editModifyTableAction->setWhatsThis( tr( "Open the Modify Table wizard, where it is possible to rename an existing table. It is also possible to add or delete fields form a table, as well as modify field names and types." ) );
    editCreateIndexAction->setText( tr( "Create Index" ) );
    editCreateIndexAction->setMenuText( tr( "Create Index" ) );
    editCreateIndexAction->setWhatsThis( tr( "Open the Create Index wizard, where it is possible to define a new index on an existing database table." ) );
    editDeleteIndexAction->setText( tr( "Delete Index" ) );
    editDeleteIndexAction->setMenuText( tr( "Delete Index" ) );
    editDeleteIndexAction->setWhatsThis( tr( "Open the Delete Index wizard, where you can select a database index to be dropped." ) );
    fileImportSQLAction->setText( tr( "Database from SQL file" ) );
    fileImportSQLAction->setMenuText( tr( "Database from SQL file" ) );
    fileImportSQLAction->setToolTip( tr( "Import data from an .sql dump text file into a new or existing database." ) );
    fileImportSQLAction->setWhatsThis( tr( "This option lets you import data from an .sql dump text file into a new or existing database. SQL dump files can be created on most database engines, including MySQL and PostgreSQL." ) );
    fileExportSQLAction->setText( tr( "Database to SQL file" ) );
    fileExportSQLAction->setMenuText( tr( "Database to SQL file" ) );
    fileExportSQLAction->setToolTip( tr( "Export a database to a .sql dump text file." ) );
    fileExportSQLAction->setWhatsThis( tr( "This option lets you export a database to a .sql dump text file. SQL dump files contain all data necessary to recreate the database on most database engines, including MySQL and PostgreSQL." ) );
    Toolbar->setLabel( tr( "Toolbar" ) );
    menubar->findItem( 1 )->setText( tr( "&File" ) );
    menubar->findItem( 2 )->setText( tr( "Edit" ) );
    menubar->findItem( 3 )->setText( tr( "View" ) );
    menubar->findItem( 4 )->setText( tr( "Help" ) );
}

