/****************************************************************************
** Form implementation generated from reading ui file 'mainwindow.ui'
**
** Created: lun nov 28 17:53:38 2005
**      by: The User Interface Compiler ($Id: mainwindow.cpp,v 1.2 2006/01/17 15:51:32 cirdan Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "./mainwindow.h"

#include <qvariant.h>
#include <qslider.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>

using namespace display;
/*
 *  Constructs a MainWindow as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
MainWindow::MainWindow( QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
{
    (void)statusBar();
    if ( !name )
	setName( "MainWindow" );
    setCentralWidget( new QWidget( this, "qt_central_widget" ) );
    MainWindowLayout = new QGridLayout( centralWidget(), 1, 1, 11, 6, "MainWindowLayout"); 

    sld_granularity = new QSlider( centralWidget(), "sld_granularity" );
    sld_granularity->setOrientation( QSlider::Vertical );

    MainWindowLayout->addWidget( sld_granularity, 0, 0 );

    glView = new Viewer( centralWidget(), "glView" );
    octreeview = new OctreeView();
    glView->addObject(octreeview);
    MainWindowLayout->addWidget( glView, 0, 1 );

    // actions
    fileOpenAction = new QAction( this, "fileOpenAction" );
    fileOpenAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "fileopen" ) ) );
    fileExitAction = new QAction( this, "fileExitAction" );
    helpAboutAction = new QAction( this, "helpAboutAction" );
    editUndoAction = new QAction( this, "editUndoAction" );
    editUndoAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "undo" ) ) );
    editUndoAction->setEnabled(false);
    fileSaveAction = new QAction( this, "fileSaveAction" );
    fileSaveAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "filesave" ) ) );
    fileSaveAction->setEnabled(false);
    editDeleteAction = new QAction( this, "editDeleteAction" );
    editDeleteAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "editcut" ) ) );
    generateAlpha_ShapeAction = new QAction( this, "generateAlpha_ShapeAction" );
    generateCrustAction = new QAction( this, "generateCrustAction" );
    fileSave_asAction = new QAction( this, "fileSave_asAction" );
    fileSave_asAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "filesave" ) ) );
    fileSave_asAction->setEnabled(false);

    // toolbars
    toolBar = new QToolBar( QString(""), this, DockTop ); 

    fileOpenAction->addTo( toolBar );
    fileSaveAction->addTo( toolBar );
    toolBar->addSeparator();
    editUndoAction->addTo( toolBar );
    editDeleteAction->addTo( toolBar );


    // menubar
    MenuBar = new QMenuBar( this, "MenuBar" );


    fileMenu = new QPopupMenu( this );
    fileOpenAction->addTo( fileMenu );
    fileSaveAction->addTo( fileMenu );
    fileSave_asAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    fileMenu->insertSeparator();
    fileExitAction->addTo( fileMenu );
    MenuBar->insertItem( QString(""), fileMenu, 1 );

    Edit = new QPopupMenu( this );
    editUndoAction->addTo( Edit );
    editDeleteAction->addTo( Edit );
    MenuBar->insertItem( QString(""), Edit, 2 );

    Generate = new QPopupMenu( this );
    generateAlpha_ShapeAction->addTo( Generate );
    generateCrustAction->addTo( Generate );
    MenuBar->insertItem( QString(""), Generate, 3 );

    helpMenu = new QPopupMenu( this );
    helpMenu->insertSeparator();
    helpAboutAction->addTo( helpMenu );
    MenuBar->insertItem( QString(""), helpMenu, 4 );

    languageChange();
    resize( QSize(600, 480).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( fileOpenAction, SIGNAL( activated() ), this, SLOT( fileOpen() ) );
    connect( fileExitAction, SIGNAL( activated() ), this, SLOT( close() ) );
    connect( helpAboutAction, SIGNAL( activated() ), this, SLOT( helpAbout() ) );
    connect( sld_granularity, SIGNAL( valueChanged(int) ), this, SLOT( sld_granularity_valueChanged(int) ) );
    connect( sld_granularity, SIGNAL( valueChanged(int) ), glView, SLOT(updateGL ()));
    connect( fileSave_asAction, SIGNAL( activated() ), this, SLOT( fileSave_as() ) );
    connect( fileSaveAction, SIGNAL( activated() ), this, SLOT( fileSave() ) );
    connect( editUndoAction, SIGNAL( activated() ), this, SLOT( editUndo() ) );
    connect( editUndoAction, SIGNAL( activated() ), glView, SLOT(updateGL ()));
    connect( editDeleteAction, SIGNAL( activated() ), this, SLOT( editDelete() ) );
    connect( editDeleteAction, SIGNAL( activated() ), glView, SLOT(updateGL ()));
    connect( generateCrustAction, SIGNAL( activated() ), this, SLOT( generateCrust() ) );
    connect( generateAlpha_ShapeAction, SIGNAL( activated() ), this, SLOT( generateAlphaShape() ) );
    
}

/*
 *  Destroys the object and frees any allocated resources
 */
MainWindow::~MainWindow()
{
  delete octreeview;
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void MainWindow::languageChange()
{
    setCaption( tr( "Crust" ) );
    fileOpenAction->setText( tr( "Open" ) );
    fileOpenAction->setMenuText( tr( "&Open..." ) );
    fileOpenAction->setAccel( tr( "Ctrl+O" ) );
    fileExitAction->setText( tr( "Exit" ) );
    fileExitAction->setMenuText( tr( "E&xit" ) );
    fileExitAction->setAccel( QString::null );
    helpAboutAction->setText( tr( "About" ) );
    helpAboutAction->setMenuText( tr( "&About" ) );
    helpAboutAction->setAccel( QString::null );
    editUndoAction->setText( tr( "Undo" ) );
    editUndoAction->setMenuText( tr( "&Undo" ) );
    editUndoAction->setAccel( tr( "Ctrl+Z" ) );
    fileSaveAction->setText( tr( "Save" ) );
    fileSaveAction->setMenuText( tr( "&Save" ) );
    fileSaveAction->setAccel( tr( "Ctrl+S" ) );
    editDeleteAction->setText( tr( "Delete" ) );
    editDeleteAction->setMenuText( tr( "&Delete" ) );
    editDeleteAction->setAccel( tr( "Del" ) );
    generateAlpha_ShapeAction->setText( tr( "Alpha Shape..." ) );
    generateAlpha_ShapeAction->setMenuText( tr( "Alpha Shape..." ) );
    generateCrustAction->setText( tr( "Crust..." ) );
    generateCrustAction->setMenuText( tr( "Crust..." ) );
    fileSave_asAction->setText( tr( "Save as..." ) );
    fileSave_asAction->setMenuText( tr( "Save as..." ) );
    toolBar->setLabel( tr( "Tools" ) );
    if (MenuBar->findItem(1))
        MenuBar->findItem(1)->setText( tr( "&File" ) );
    if (MenuBar->findItem(2))
        MenuBar->findItem(2)->setText( tr( "&Edit" ) );
    if (MenuBar->findItem(3))
        MenuBar->findItem(3)->setText( tr( "&Generate" ) );
    if (MenuBar->findItem(4))
        MenuBar->findItem(4)->setText( tr( "&Help" ) );
}

void MainWindow::fileOpen()
{
  QString fn = QFileDialog::getOpenFileName( QString::null, "Wavefront Object (*.obj)",
					     this);
  if ( !fn.isEmpty() ) {
    statusBar()->message( fn, 2000 );
    sld_granularity->setMaxValue(octreeview->load(fn));
    sld_granularity->setTickmarks( QSlider::Left );
    glView->setSceneRadius (octreeview->getRadius());
    glView->setSceneCenter (octreeview->getCenter());
    glView->showEntireScene();
    filename = fn;
    fileSaveAction->setEnabled(true);
    fileSave_asAction->setEnabled(true);
    setCaption(QString("Crust : " )+QDir::convertSeparators(filename));
  }
  else {
    statusBar()->message( "Loading aborted", 2000 );
  }
}

void MainWindow::helpAbout()
{
  QMessageBox::about( this, "Qt Interface for Crust",
		      "Beta Release");
}


void MainWindow::sld_granularity_valueChanged( int value )
{
  octreeview->setDepth(value);
}


void MainWindow::fileSave()
{
  if ( filename.isEmpty() ) {
    fileSave_as();
    return;
  }
  octreeview->save(filename);
  setCaption(QString("Crust : " )+QDir::convertSeparators(filename));
  statusBar()->message( QString( "File %1 saved" ).arg( filename ), 2000 );
}


void MainWindow::fileSave_as()
{
  QString fn = QFileDialog::getSaveFileName( QString::null, QString::null,
					     this );
  if (QFile::exists(fn)) {
    int ret = QMessageBox::warning(this, tr("File exists"),
				   tr("File %1 already exists. \n"
				      "Do you want to overwrite it?")
				   .arg(QDir::convertSeparators(fn)),
				   QMessageBox::Yes | QMessageBox::Default,
				   QMessageBox::No | QMessageBox::Escape);
    if (ret == QMessageBox::No)
      fn = "";
  }
  if ( !fn.isEmpty() ) {
    filename = fn;
    fileSave();
  } else {
    statusBar()->message( "Saving aborted", 2000 );
  }
}


void MainWindow::editUndo()
{
  octreeview->restoreSelected();
  editUndoAction->setEnabled(false);
}


void MainWindow::editDelete()
{
  octreeview->deleteSelected();
  editUndoAction->setEnabled(true);
}


void MainWindow::generateAlphaShape()
{
  DlgGenerate* dlgAlphaShape = new DlgGenerate(this, "Alpha Shape", false, Qt::WDestructiveClose);
  AlphaView* alphaView = new AlphaView();
  
  alphaView->insert(octreeview->first(), octreeview->end());

  QSlider* sld_alpha = new QSlider( dlgAlphaShape, "sld_alpha" );
  sld_alpha->setOrientation( QSlider::Vertical );
  dlgAlphaShape->addWidget(sld_alpha);
  sld_alpha->setMinValue(1);
  sld_alpha->setMaxValue(alphaView->as.number_of_alphas());
  connect( sld_alpha, SIGNAL( valueChanged(int) ), alphaView, SLOT( valueChanged(int) ) );
  connect( sld_alpha, SIGNAL( valueChanged(int) ), dlgAlphaShape->glView, SLOT(updateGL ()));

  dlgAlphaShape->addObject(alphaView);
  dlgAlphaShape->glView->setSceneRadius (octreeview->getRadius());
  dlgAlphaShape->glView->setSceneCenter (octreeview->getCenter());
  dlgAlphaShape->glView->showEntireScene();
  dlgAlphaShape->show();
}

void MainWindow::generateCrust()
{
  DlgGenerate* dlgCrust = new DlgGenerate(this, "Crust", false, Qt::WDestructiveClose);
  QButtonGroup* btg_nbPoles;
  QRadioButton* rdb_onePole;
  QRadioButton* rdb_twoPoles;
  QButtonGroup* btg_poleType;
  QRadioButton* rdb_extrema;
  QRadioButton* rdb_average;
  QVBoxLayout* btg_nbPolesLayout;
  QVBoxLayout* btg_poleTypeLayout;

  btg_nbPoles = new QButtonGroup( dlgCrust, "btg_nbPoles" );
  btg_nbPoles->setColumnLayout(0, Qt::Vertical );
  btg_nbPoles->layout()->setSpacing( 6 );
  btg_nbPoles->layout()->setMargin( 11 );
  btg_nbPolesLayout = new QVBoxLayout( btg_nbPoles->layout() );
  btg_nbPolesLayout->setAlignment( Qt::AlignTop );

  rdb_onePole = new QRadioButton( btg_nbPoles, "rdb_onePole" );
  btg_nbPolesLayout->addWidget( rdb_onePole );

  rdb_twoPoles = new QRadioButton( btg_nbPoles, "rdb_twoPoles" );
  rdb_twoPoles->setChecked( TRUE );
  btg_nbPoles->insert( rdb_twoPoles, 0 );
  btg_nbPolesLayout->addWidget( rdb_twoPoles );
  dlgCrust->addWidget( btg_nbPoles ); // add widget

  btg_poleType = new QButtonGroup( dlgCrust, "btg_poleType" );
  btg_poleType->setColumnLayout(0, Qt::Vertical );
  btg_poleType->layout()->setSpacing( 6 );
  btg_poleType->layout()->setMargin( 11 );
  btg_poleTypeLayout = new QVBoxLayout( btg_poleType->layout() );
  btg_poleTypeLayout->setAlignment( Qt::AlignTop );

  rdb_extrema = new QRadioButton( btg_poleType, "rdb_extrema" );
  rdb_extrema->setChecked( TRUE );
  btg_poleType->insert( rdb_extrema, 1 );
  btg_poleTypeLayout->addWidget( rdb_extrema );

  rdb_average = new QRadioButton( btg_poleType, "rdb_average" );
  btg_poleTypeLayout->addWidget( rdb_average );
  dlgCrust->addWidget( btg_poleType ); // add widget

  btg_nbPoles->setTitle( tr( "Number of Poles" ) );
  rdb_onePole->setText( tr( "One pole" ) );
  rdb_twoPoles->setText( tr( "Two poles" ) );
  btg_poleType->setTitle( tr( "Type of poles" ) );
  rdb_extrema->setText( tr( "Extrema" ) );
  rdb_average->setText( tr( "Average" ) );



  QProgressDialog* progress = new QProgressDialog( "Adding points...", "Abort", 
						   octreeview->currentNumPoints(),
						   this, "progress", TRUE );
  CrustView* crustView = new CrustView();
  crustView->setProgressDialog(progress);

  connect( rdb_onePole, SIGNAL( toggled(bool) ), crustView, SLOT( onePole_toggled(bool) ) );
  connect( rdb_onePole, SIGNAL( toggled(bool) ), dlgCrust->glView, SLOT(updateGL ()));
  connect( rdb_extrema, SIGNAL( toggled(bool) ), crustView, SLOT( extrema_toggled(bool) ) );
  connect( rdb_extrema, SIGNAL( toggled(bool) ), dlgCrust->glView, SLOT(updateGL ()));
  
  progress->show();

  crustView->insert(octreeview->first(), octreeview->end());
  crustView->computePoles();
  crustView->facetsComparaison();

  dlgCrust->addObject(crustView);
  dlgCrust->glView->setSceneRadius (octreeview->getRadius());
  dlgCrust->glView->setSceneCenter (octreeview->getCenter());
  dlgCrust->glView->showEntireScene();
  dlgCrust->show();
  
}
