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


#ifndef FBCALLCONVERTER_H
#define FBCALLCONVERTER_H

#include <QObject>
class PageCtrl; 			//get access to classes


class FBCallConverter : public QObject
{
  Q_OBJECT

public:
	FBCallConverter(PageCtrl *pageCtrl);
	virtual ~FBCallConverter();
	

public slots:
    void convert(QString *Str, int pageIndex, int from, int to, int option);


private:
    int		convertFBCAllAtTo(QStringList *tmpList, int pageIndex, int index, int to, int option);
    bool	getCallLineItems(QString str, int ioStyle, QString *fbPart, QString *tag, QString *comment, QString *lineEnd);
    QString getComment(QString str);
    QString getDefaultInputLine(QString str, int ioStyle);
    QString getDefaultOutputLine(QString str, int ioStyle);
    QString getIECInputLine(QString instance, QString str, int ioStyle, QString prgLng);
    QString getIECOutputLine(QString instance, QString str, int ioStyle, QString prgLng);
    QString getMoellerILOutputLine(QString str, int ioStyle);
    int 	getNextCALPos(QStringList *tmpList, int pageIndex, int index);
    
    PageCtrl 	 *m_PageCtrl; 	//!< instance of PageCtrl class
    
};
#endif
