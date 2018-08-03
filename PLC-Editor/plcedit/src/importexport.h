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


#ifndef IMPORTEXPORT_H
#define IMPORTEXPORT_H


#include <QObject>
#include <QStringList>
#include <QProgressBar>


class PageCtrl; 			//get access to classes
class Prototype;
class BatchDialog;
class FBCallConverter;


class ImportExport : public QObject
{
  Q_OBJECT

public:
	ImportExport(PageCtrl *pageCtrl, Prototype *prototype);
	virtual ~ImportExport();

public:
	//public VAR
	int		conversionOption;	//!< option for fb call model conversion, set by preferences dialog
	QStringList errorFiles; 	//!< string list for files which could not be opened
	QString lastDir;			//!< last directory of save or open function
	bool 	usePresetDir;		//!< use preset dir flag, set by preferences dialog
	bool    usePrgLngDetection; //!< use programming language detection
	bool	savePTT;			//!< save prototype flag, set by preferences dialog

    
    //public functions
    QString createSucosoftHeader(QString Str);
    bool 	OpenPrepare(QStringList files); 									
    int 	savePage(QString Str);
    void 	SavePrepare(int pageIndex, QString fn, QStringList *tmpList, int Ext, int convOption, bool sendConvBack); 
    void 	saveAllPages(QString batchPath, int batchExt, int batchConvOption);

public slots:
    void batchConverter();
    void retranslateUi();


signals:
	void modified(); 		//!< signal page modified, emitted if save or open functions finished
	void active();	 		//!< signal savePage(QString Str) function active, emitted if save functions started
	void ready();	 		//!< signal savePage(QString Str) function ready, emitted if save function finished
	
	
private:
    QString msg_progress_fopen; //!< message at progressbar "file open"
    QString msg_progress_batch; //!< message at progressbar "batch converter"
    QString msg_error_pou_line; //!< message file open error in pou line

    QProgressBar *progressBar;	//!< instance of qprogressbar
    PageCtrl 	 *m_PageCtrl; 	//!< instance of PageCtrl class
    Prototype 	 *m_Prototype; 	//!< instance of Prototype class

    
    //private functions
    bool 	checkFileContentAndExtension(QStringList *tmpList, int index, int endPos, QString *ExtStr);
    QString getCoDeSysVISU(QStringList *tmpList, int index, int end);
	QString getMELSOFTConfig(QStringList *tmpList, int index, int end);
    QString getSTEP7DBData(QStringList *tmpList, int index, int end);
    int		nextPOUEndIndexAt(QStringList *tmpList, int index, QString ExtStr);
    int		nextPOUStartIndexAt(QStringList *tmpList, int index, int end);
	bool    OpenReinforce(QStringList *tmpList, int index, int endPos, QString ExtStr); 
    void 	setProgressBar(int files, int minFiles, QString msg);
    QString strippedFileName(const QString &fullFileName);

};
#endif
