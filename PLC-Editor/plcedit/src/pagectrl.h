/****************************************************************************
**
** This file is part of PLCEdit, an open-source cross-platform editor 
** for PLC source files (e.g. from Moeller, CoDeSys and Siemens).
** Copyright (C) 2005-2010  M. Rehfeldt
**
** This software uses classes of Trolltech Qt toolkit and is freeware. 
** This file may be used under the terms of the GNU General Public License 
** version 2.0 or (at your option) any later version as published by the 
** Free Software Foundation and appearing in the file LICENSE.GPL included 
** in the packaging of this file. 
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** Contact e-mail: M. Rehfeldt <info@plcedit.org>
** Program URL   : http://www.plcedit.org
**
****************************************************************************/


#ifndef PAGECTRL_H
#define PAGECTRL_H

#include <QSplitter>
#include <QFrame>
#include <QTextEdit>
#include <QLabel>


class QListWidget;
class QToolButton;
class TabWidget;
class Highlighter_Dec;
class Highlighter_Ins;
class Editor;


class PageCtrl : public QSplitter
{
    Q_OBJECT

public:
    PageCtrl(QWidget *parent = 0);
    virtual ~PageCtrl();
    
    //public VAR
    QString	defaultPOUExt;					//!< default extension for new POUs as into string converted number/index
    QStringList definedPOUExtList;			//!< list of supported POU extensions
    bool	flg_fileListVisible; 			//!< flag file list visible, used by preferences dialog
    bool 	flg_inhibit_createHighlighters; //!< flag inhibit create highlighters, used by ImportExport class
    bool	flg_activate_highlighters;		//!< flag activate highlighters, used by preferences dialog
    bool	flg_enableHighlightLines;		//!< flag enable highlighting lines
    QColor  hiLineColor;					//!< highlighting color for current line
    QFont	font;							//!< current application-global font
    QString Sucosoft_KM_TEXT;				//!< Moeller Sucosoft header copyright string, used by ImportExport class 
    int		maxRecentFiles;					//!< number of recent files to be hold
    QString prgLngStr;   					//!< application-global programming language
    int		printArea;						//!< print area indicator postion for declarations and instructions
    QStringList recentFiles;				//!< recent file paths list
    QString Sucosoft_S40_VERSION;			//!< Moeller Sucosoft header compatibility string, used by ImportExport class 
    QString splitMode;						//!< application-global split mode for inputwindows
    int     splitSize;						//!< application-global split size for inputwindows
    int 	tabPosition;					//!< application-global tab position for pages
    int 	tabStopWidth;					//!< application-global tabstop width for inputwindows
    bool	unsavedNotice;					//!< flag notification for unsaved files

    //public funtions
    QString	activeInputWindow();
    int     countPages();  
    void    correctRecentFilesListLength(); 
    QString currentDeclaration();
    bool	currentDeclarationIsVisible();
    QString currentFncType();
    QString currentFileDate();
    QString currentFileEnd();
    int     currentFileExt();
    QString currentFileExtStr();
    QString	currentFileName();
    QString currentFilePath();
    QString currentFileType();
    QString currentFileTypeShort();
    QString currentInstruction();
    QString currentMELSOFTConfig();
    QString currentMULTIPROGProperties();
    QString currentMULTIPROGDescription();
	QString currentMULTIPROGWorkSheetName();
	QString currentSTEP7DBData();
    QString currentSucosoftHeader();
    QString currentPrgLng();
    QString currentSelection();
    QString	currentSplitMode();
    int		currentSplitSize();
    int		currentTabIndex();
    int		currentTabStopWidth();
    QString currentTabText();
    QString cursorPosDec();
    QString cursorPosIns();
    QString declarationOf(int index);
    QString strFromDefinedPOUExtListAt(int index);
    void    delVARTypeItems();
    void	fillCurrentDeclaration(QString Str);
    void	fillCurrentInstruction(QString Str);
    void 	fillDeclarationOf(int index, QString Str);
    void 	fillInstructionOf(int index, QString Str);
    void	fillRecentFilesList(int index);
    QString fileEndOf(int index);
    int	    fileExtOf(int index);
    QString fileExtStrOf(int index);
    QString fileNameOf(int index);
    QString fncTypeOf(int index);
    QString filePathOf(int index);
    QString fileTypeOf(int index);
    bool    find(QString window, QString fStr, QString rStr, QTextDocument::FindFlags options);
    void    insertText(QString window, QString Str);
    void    insertTextOf(QString window, QString Str, int index);
    QString instructionOf(int index);
    void	moveToStartDeclarations();
    void	moveToStartInstructions();
    QString MELSOFTConfigOf(int index);
    QString MULTIPROGDescriptionOf(int index);
    QString MULTIPROGPropertiesOf(int index);
    QString MULTIPROGWorkSheetNameOf(int index);
    bool	pageModifiedOf(int index);
    bool	pagesModified();
    QString prgLngOf(int index);
    void	setCurrentDeclarationVisible(bool visible);
    void	setCurrentFncType(QString Str);
    void    setCurrentFileDate(QString str);
    void	setCurrentFileEnd(QString Str);
    void	setCurrentFileExt(QString Str);    
    void	setCurrentFileListItemText(QString Str);
    void 	setCurrentFileName(QString Str);
    void	setCurrentFilePath(QString Str);
    void	setCurrentFileType(QString Str);
    void	setCurrentFont(QFont font);
    void	setCurrentMELSOFTConfig(QString Str);
    void 	setCurrentMULTIPROGDescription(QString Str);
    void	setCurrentMULTIPROGProperties(QString Str);
    void 	setCurrentMULTIPROGWorkSheetName(QString Str);
    void    setCurrentPrgLng(QString Str);
    void	setCurrentSplitMode(QString Str);
    void 	setCurrentSplitSize(int i);
    void	setCurrentSTEP7DBData(QString Str);
    void 	setCurrentSucosoftHeader(QString Str);
    void    setCurrentTypeData(QString filetype, QString functiontype, QString fileendmark);
    void 	setCurrentTabStops(int width);
    void 	setCurrentTabText(QString Str);
    void	setFileListVisible(bool visible);
    void	setMELSOFTConfigOf(int index, QString Str);
    void	setMULTIPROGPropertiesOf(int index, QString Str);
    void	setPageCtrlSignalsBlocked(bool blocked);
    void	setPrgLngOf(int index, QString Str);
    void	setPageModifiedOf(int index, bool modified);
    void	setSplitModeOf(int index, QString Str);
    void	setTabCtrlVisible(bool visible);
    void 	setTabPosition(int pos);
    QString STEP7DBDataOf(int index);
    QString SucosoftHeaderOf(int index);
    QString tabTextOf(int index);
    
    
        
public slots:
//public slots
    void 	changeSplitMode();
    void	changeInputWindow();
    void 	configureCurrentEditor(int i);
    void 	copy();
    void 	createHighlighters(int i);
    void 	cut();
    void	deleteCurrentHighlighters(int i);
    void 	deletePage();
    void	deletePageIndex(int index);
    void 	newPage();
    void 	setPageModified(bool modified);
    void 	paste();
    void	redo();
    void 	selectAll();
    void 	setNewSplitterVisible();
    void	undo();
    void	retranslateUi();

signals:
    void compareWith();				//!< signal file comparision with
    void compareWithNext();			//!< signal file comparision with next
    void compareWithPrev();			//!< signal file comparision with previous
    void currentChanged(int i);		//!< signal page selection changed to i
    void cursorModified();			//!< signal cursor position modified at current inputwindow
    void modified();				//!< signal page was modified, used by MainWindow class to update status bar
    void pageDeleted(int i);		//!< signal page i was deleted, used by create highlighters slot
    void saveRequest();				//!< signal page save requested
    
private slots:
    void compareWithNextCheck();
    void compareWithPrevCheck();
    void cursorPositionChanged();
    #if QT_VERSION >= 0x040500
      void moveFileListItem(int from, int to);
    #endif
    void setFileListRow(int index);

    
private:
  //private variables
  bool openInProcess;

  //MESSAGES
    QString msg_text_closepage;		//!< message text for close page dialog 
    QString okButtonText;			//!< messagebox button text ok
    QString cancelButtonText;		//!< messagebox button text cancel
    QString saveButtonText;			//!< messagebox button text save

  //private class instances
  QListWidget     *FileList;		//!< list of opened files
  Highlighter_Dec *highlighter_dec; //!< syntax highlighter for declaration section
  Highlighter_Ins *highlighter_ins; //!< syntax highlighter for instruction section
  TabWidget       *TabCtrl;  		//!< QTabwidget instance, holds pages
  QToolButton 	  *delPageButton;	//!< delete pages button, shown in tab row
  
  //private functions
  QString strippedFileName(const QString &fullFileName);
};

#endif
