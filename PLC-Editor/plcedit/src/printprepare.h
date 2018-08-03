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


#ifndef PRINTPREPARE_H
#define PRINTPREPARE_H

#include <QStringList>
#include <QObject>


class PageCtrl; 		//get access to PageCtrl class
class Prototype; 		//get access to Prototype class
class ImportExport; 	//get access to ImportExport class

class PrintPrepare : public QObject
{
  Q_OBJECT
  
public:
	PrintPrepare(PageCtrl *pageCtrl, Prototype *prototype, ImportExport *importExport);
	virtual ~PrintPrepare();


public:
	//public VAR
	bool 	printDeclarations; 	//!< flag print declaration section
    bool 	printInstructions; 	//!< flag print instruction section
    bool 	printPrototype; 	//!< flag print prototype section
    bool 	printPOUI;  		//!< flag print POU info section
    
    //public functions
	QString prepareCurrentSucosoftHeader(QString SucosoftHeader);
	void prepareCurrentPage(QStringList *printList, QString SucosoftHeader);
	

private:	
    PageCtrl *m_PageCtrl; 			//!< instance of PageCtrl class
    Prototype *m_Prototype; 		//!< instance of Prototype class
    ImportExport *m_ImportExport; 	//!< instance of ImportExport class
};
#endif
