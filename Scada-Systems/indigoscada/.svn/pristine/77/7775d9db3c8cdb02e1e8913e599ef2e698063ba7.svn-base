/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */
#include <common.h>
#include "helpwindow.h"
#include <qt.h>
#include "forward.xpm"
#include "back.xpm"
#include "home.xpm"
#include <ctype.h>
HelpWindow::HelpWindow( const QString& home_, const QString& _path,
QWidget* parent, const char *name )
: QMainWindow( parent, name, WDestructiveClose ),
pathCombo( 0 ), selectedURL(),
path( QFileInfo( home_ ).dirPath( TRUE ), "*.html *.htm" )
{
	readHistory();
	readBookmarks();
	fileList = path.entryList();
	browser = new QTextBrowser( this );
	browser->mimeSourceFactory()->setFilePath( _path );
	browser->setFrameStyle( QFrame::Panel | QFrame::Sunken );
	connect( browser, SIGNAL( textChanged() ),
	this, SLOT( textChanged() ) );
	setCentralWidget( browser );
	if ( !home_.isEmpty() )
	browser->setSource( home_ );
	connect( browser, SIGNAL( highlighted( const QString&) ),
	statusBar(), SLOT( message( const QString&)) );
	resize( 640,700 );
	QPopupMenu* file = new QPopupMenu( this );
	file->insertItem( tr("&New Window"), this, SLOT( newWindow() ), ALT | Key_N );
	file->insertItem( tr("&Open File"), this, SLOT( openFile() ), ALT | Key_O );
	file->insertItem( tr("&Print"), this, SLOT( print() ), ALT | Key_P );
	file->insertSeparator();
	file->insertItem( tr("&Close"), this, SLOT( close() ), ALT | Key_Q );
	QPopupMenu* go = new QPopupMenu( this );
	backwardId = go->insertItem( QPixmap("back.xpm"),
	tr("&Backward"), browser, SLOT( backward() ),
	ALT | Key_Left );
	forwardId = go->insertItem( QPixmap("forward.xpm"),
	tr("&Forward"), browser, SLOT( forward() ),
	ALT | Key_Right );
	go->insertItem( QPixmap("home.xpm"), tr("&Home"), browser, SLOT( home() ) );
	QPopupMenu* help = new QPopupMenu( this );
	help->insertItem( tr("&About ..."), this, SLOT( about() ) );
	help->insertItem( tr("About &Qt ..."), this, SLOT( aboutQt() ) );
	hist = new QPopupMenu( this );
	QStringList::Iterator it = history.begin();
	for ( ; it != history.end(); ++it )
	mHistory[ hist->insertItem( *it ) ] = *it;
	connect( hist, SIGNAL( activated( int ) ),
	this, SLOT( histChosen( int ) ) );
	bookm = new QPopupMenu( this );
	bookm->insertItem( tr( "Add Bookmark" ), this, SLOT( addBookmark() ) );
	bookm->insertSeparator();
	QStringList::Iterator it2 = bookmarks.begin();
	for ( ; it2 != bookmarks.end(); ++it2 )
	mBookmarks[ bookm->insertItem( *it2 ) ] = *it2;
	connect( bookm, SIGNAL( activated( int ) ),
	this, SLOT( bookmChosen( int ) ) );
	menuBar()->insertItem( tr("&File"), file );
	menuBar()->insertItem( tr("&Go"), go );
	menuBar()->insertItem( tr( "History" ), hist );
	menuBar()->insertItem( tr( "Bookmarks" ), bookm );
	menuBar()->insertSeparator();
	menuBar()->insertItem( tr("&Help"), help );
	menuBar()->setItemEnabled( forwardId, FALSE);
	menuBar()->setItemEnabled( backwardId, FALSE);
	connect( browser, SIGNAL( backwardAvailable( bool ) ),
	this, SLOT( setBackwardAvailable( bool ) ) );
	connect( browser, SIGNAL( forwardAvailable( bool ) ),
	this, SLOT( setForwardAvailable( bool ) ) );
	QToolBar* toolbar = new QToolBar( this );
	addToolBar( toolbar, "Toolbar");
	QToolButton* button;
	button = new QToolButton( QPixmap((const char **)back), tr("Backward"), "", browser, SLOT(backward()), toolbar );
	connect( browser, SIGNAL( backwardAvailable(bool) ), button, SLOT( setEnabled(bool) ) );
	button->setEnabled( FALSE );
	button = new QToolButton( QPixmap((const char **)forward), tr("Forward"), "", browser, SLOT(forward()), toolbar );
	connect( browser, SIGNAL( forwardAvailable(bool) ), button, SLOT( setEnabled(bool) ) );
	button->setEnabled( FALSE );
	button = new QToolButton( QPixmap((const char **)home), tr("Home"), "", browser, SLOT(home()), toolbar );
	toolbar->addSeparator();
	pathCombo = new QComboBox( TRUE, toolbar );
	connect( pathCombo, SIGNAL( activated( const QString & ) ),
	this, SLOT( pathSelected( const QString & ) ) );
	toolbar->setStretchableWidget( pathCombo );
	setRightJustification( TRUE );
	setDockEnabled( Left, FALSE );
	setDockEnabled( Right, FALSE );
	pathCombo->insertItem( home_ );
	browser->setFocus();
}
void HelpWindow::setBackwardAvailable( bool b)
{
	menuBar()->setItemEnabled( backwardId, b);
}
void HelpWindow::setForwardAvailable( bool b)
{
	menuBar()->setItemEnabled( forwardId, b);
}
void HelpWindow::textChanged()
{
	if ( browser->documentTitle().isNull() )
	setCaption( browser->context() );
	else
	setCaption( browser->documentTitle() ) ;
	selectedURL = caption();
	if ( !selectedURL.isEmpty() && pathCombo ) {
		path = QDir( QFileInfo( selectedURL ).dirPath( TRUE ), "*.html *.htm" );
		fileList = path.entryList();
		bool exists = FALSE;
		int i;
		for ( i = 0; i < pathCombo->count(); ++i ) {
			if ( pathCombo->text( i ) == selectedURL ) {
				exists = TRUE;
				break;
			}
		}
		if ( !exists ) {
			pathCombo->insertItem( selectedURL, 0 );
			pathCombo->setCurrentItem( 0 );
			mHistory[ hist->insertItem( selectedURL ) ] = selectedURL;
		} else
		pathCombo->setCurrentItem( i );
		selectedURL = QString::null;
	}
}
HelpWindow::~HelpWindow()
{
	history.clear();
	QMap<int, QString>::Iterator it = mHistory.begin();
	for ( ; it != mHistory.end(); ++it )
	history.append( *it );
	QFile f( QDir::currentDirPath() + "/.history" );
	f.open( IO_WriteOnly );
	QDataStream s( &f );
	s << history;
	f.close();
	bookmarks.clear();
	QMap<int, QString>::Iterator it2 = mBookmarks.begin();
	for ( ; it2 != mBookmarks.end(); ++it2 )
	bookmarks.append( *it2 );
	QFile f2( QDir::currentDirPath() + "/.bookmarks" );
	f2.open( IO_WriteOnly );
	QDataStream s2( &f2 );
	s2 << bookmarks;
	f2.close();
}
void HelpWindow::about()
{
	QMessageBox::about( this, "HelpViewer Example",
	"<p>This example implements a simple HTML help viewer "
	"using Qt's rich text capabilities</p>"
	"<p>It's just about 100 lines of C++ code, so don't expect too much :-)</p>"
	);
}
void HelpWindow::aboutQt()
{
	QMessageBox::aboutQt( this, "QBrowser" );
}
void HelpWindow::openFile()
{
	QString fn = QFileDialog::getOpenFileName( QString::null, QString::null, this );
	if ( !fn.isEmpty() )
	browser->setSource( fn );
}
void HelpWindow::newWindow()
{
	( new HelpWindow(browser->source(), "qbrowser") )->show();
}
void HelpWindow::print()
{
	QPrinter printer;
	printer.setFullPage(TRUE);
	if ( printer.setup() ) {
		QPainter p( &printer );
		QPaintDeviceMetrics metrics(p.device());
		int dpix = metrics.logicalDpiX();
		int dpiy = metrics.logicalDpiY();
		const int margin = 72; // pt
		QRect body(margin*dpix/72, margin*dpiy/72,
		metrics.width()-margin*dpix/72*2,
		metrics.height()-margin*dpiy/72*2 );
		QFont font("times", 10);
		QSimpleRichText richText( browser->text(), font, browser->context(), browser->styleSheet(),
		browser->mimeSourceFactory(), body.height() );
		richText.setWidth( &p, body.width() );
		QRect view( body );
		int page = 1;
		do {
			richText.draw( &p, body.left(), body.top(), view, colorGroup() );
			view.moveBy( 0, body.height() );
			p.translate( 0 , -body.height() );
			p.setFont( font );
			p.drawText( view.right() - p.fontMetrics().width( QString::number(page) ),
			view.bottom() + p.fontMetrics().ascent() + 5, QString::number(page) );
			if ( view.top()  >= richText.height() )
			break;
			printer.newPage();
			page++;
		} while (TRUE);
	}
}
void HelpWindow::pathSelected( const QString &_path )
{
	browser->setSource( _path );
	path = QDir( QFileInfo( _path ).dirPath( TRUE ), "*.html *.htm" );
	fileList = path.entryList();
	QMap<int, QString>::Iterator it = mHistory.begin();
	bool exists = FALSE;
	for ( ; it != mHistory.end(); ++it ) {
		if ( *it == _path ) {
			exists = TRUE;
			break;
		}
	}
	if ( !exists )
	mHistory[ hist->insertItem( _path ) ] = _path;
}
void HelpWindow::readHistory()
{
	if ( QFile::exists( QDir::currentDirPath() + "/.history" ) ) {
		QFile f( QDir::currentDirPath() + "/.history" );
		f.open( IO_ReadOnly );
		QDataStream s( &f );
		s >> history;
		f.close();
		while ( history.count() > 20 )
		history.remove( history.begin() );
	}
}
void HelpWindow::readBookmarks()
{
	if ( QFile::exists( QDir::currentDirPath() + "/.bookmarks" ) ) {
		QFile f( QDir::currentDirPath() + "/.bookmarks" );
		f.open( IO_ReadOnly );
		QDataStream s( &f );
		s >> bookmarks;
		f.close();
	}
}
void HelpWindow::histChosen( int i )
{
	if ( mHistory.contains( i ) )
	browser->setSource( mHistory[ i ] );
}
void HelpWindow::bookmChosen( int i )
{
	if ( mBookmarks.contains( i ) )
	browser->setSource( mBookmarks[ i ] );
}
void HelpWindow::addBookmark()
{
	mBookmarks[ bookm->insertItem( caption() ) ] = caption();
}

