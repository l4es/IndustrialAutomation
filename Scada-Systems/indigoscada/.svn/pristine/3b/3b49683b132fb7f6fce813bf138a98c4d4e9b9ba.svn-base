/****************************************************************************
** Form interface generated from reading ui file 'form1.ui'
**
** Created: Sat May 5 20:10:28 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef MAINFORM_H
#define MAINFORM_H

#include <qvariant.h>
#include <qmainwindow.h>
#include "qvalidator.h"
#include "findform.h"
#include "qclipboard.h"
#include "sqlitedb.h"
#include "editform.h"
#include "sqllogform.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QAction;
class QActionGroup;
class QToolBar;
class QPopupMenu;
class QTabWidget;
class QWidget;
class QListView;
class QListViewItem;
class QLabel;
class QComboBox;
class QPushButton;
class QTable;
class QLineEdit;
class QTextEdit;

class mainForm : public QMainWindow
{
    Q_OBJECT

public:
    mainForm( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~mainForm();

    QTabWidget* mainTab;
    QWidget* structure;
    QListView* dblistView;
    QWidget* browser;
    QLabel* textLabel1;
    QComboBox* comboBrowseTable;
    QPushButton* buttonFind;
    QPushButton* buttonNewRecord;
    QPushButton* buttonDeleteRecord;
    QTable* dataTable;
    QPushButton* buttonPrevious;
    QLabel* labelRecordset;
    QPushButton* buttonNext;
    QPushButton* buttonGoto;
    QLineEdit* editGoto;
    QWidget* query;
    QLabel* textLabel1_2;
    QTextEdit* sqlTextEdit;
    QPushButton* executeQueryButton;
    QLabel* textLabel2;
    QLineEdit* queryErrorLineEdit;
    QLabel* textLabel3;
    QListView* queryResultListView;
    QMenuBar *menubar;
    QPopupMenu *fileMenu;
    QPopupMenu *EditMenu;
    QPopupMenu *ViewMenu;
    QPopupMenu *PopupMenu;
    QToolBar *Toolbar;
    QAction* fileNewAction;
    QAction* fileOpenAction;
    QAction* fileExitAction;
    QAction* editCopyAction;
    QAction* editPasteAction;
    QAction* editFindAction;
    QAction* helpContentsAction;
    QAction* helpIndexAction;
    QAction* helpAboutAction;
    QAction* fileCloseAction;
    QAction* newRecordAction;
    QAction* fileCompactAction;
    QAction* helpWhatsThisAction;
    QAction* sqlLogAction;
    QAction* fileImportCSVAction;
    QAction* fileExportCSVAction;
    QAction* fileSaveAction;
    QAction* fileRevertAction;
    QAction* fileImportAction;
    QAction* fileExportAction;
    QAction* editCreateTableAction;
    QAction* editDeleteTableAction;
    QAction* editModifyTableAction;
    QAction* editCreateIndexAction;
    QAction* editDeleteIndexAction;
    QAction* fileImportSQLAction;
    QAction* fileExportSQLAction;

    sqlLogForm * logWin;
    editForm * editWin;
    QClipboard * clipboard;
    findForm * findWin;
    int recAtTop;
    int recsPerView;
    QIntValidator * gotoValidator;

public slots:
    virtual void fileOpen( const QString & fileName );
    virtual void fileOpen();
    virtual void fileNew();
    virtual void populateStructure();
    virtual void populateTable( const QString & tablename );
    virtual void resetBrowser();
    virtual void fileClose();
    virtual void fileExit();
    virtual void closeEvent( QCloseEvent * );
    virtual void addRecord();
    virtual void deleteRecord();
    virtual void updateTableView( int lineToSelect );
    virtual void selectTableLine( int lineToSelect );
    virtual void navigatePrevious();
    virtual void navigateNext();
    virtual void navigateGoto();
    virtual void setRecordsetLabel();
    virtual void browseFind( bool open );
    virtual void browseFindAway();
    virtual void lookfor( const QString & wfield, const QString & woperator, const QString & wsearchterm );
    virtual void showrecord( int dec );
    virtual void createTable();
    virtual void createIndex();
    virtual void compact();
    virtual void deleteTable();
    virtual void editTable();
    virtual void deleteIndex();
    virtual void copy();
    virtual void paste();
    virtual void helpWhatsThis();
    virtual void helpAbout();
    virtual void updateRecordText( int row, int col, QString newtext );
    virtual void logWinAway();
    virtual void editWinAway();
    virtual void editText( int row, int col );
    virtual void doubleClickTable( int row, int col, int button, const QPoint & mousepoint );
    virtual void executeQuery();
    virtual void mainTabSelected( const QString & tabname );
    virtual void toggleLogWindow( bool enable );
    virtual void importTableFromCSV();
    virtual void exportTableToCSV();
    virtual void dbState( bool dirty );
    virtual void fileSave();
    virtual void fileRevert();
    virtual void exportDatabaseToSQL();
    virtual void importDatabaseFromSQL();

protected:
    DBBrowserDB db;

    QVBoxLayout* mainFormLayout;
    QVBoxLayout* structureLayout;
    QVBoxLayout* browserLayout;
    QHBoxLayout* layout2;
    QHBoxLayout* layout9;
    QVBoxLayout* queryLayout;
    QHBoxLayout* layout5;

protected slots:
    virtual void languageChange();

private:
    void init();
    void destroy();

};

#endif // MAINFORM_H
