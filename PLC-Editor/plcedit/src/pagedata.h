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


#ifndef PAGEDATA_H
#define PAGEDATA_H


#include <QSplitter>


class PageData : public QSplitter 
{

  Q_OBJECT
  
public:
	PageData(QWidget *parent=0);
	virtual ~PageData();
	
	//page data variables
    QString pd_FilePath;				//!< contains path to file without file name 
    QString pd_FileExt; 				//!< contains POU extension 
    QString pd_FileName;				//!< contains POU name
    QString pd_FileDate;                //!< contains POU file storage date
    QString pd_FileType;				//!< contains POU type mark
    QString pd_FncType;					//!< contains type of function
    QString pd_FileEnd; 				//!< contains POU end mark
    QString pd_PROGLNG;  				//!< contains programming language of file
    QString pd_IDERef; 					//!< contains IDE reference
    QString pd_SucosoftHeader;			//!< contains Moeller Sucosoft header
    QString pd_MULTIPROGDescription;	//!< contains KW-Software description section
    QString pd_MULTIPROGProperties; 	//!< contains KW-Software properties section
    QString pd_MULTIPROGWorkSheetName;	//!< contains KW-Software work sheet name
    QString pd_STEP7DBData; 			//!< contains Siemens DB Data
    QString pd_MELSOFTConfig;			//! contains Mitsubishi MELSOFT config data
    QString pd_highlighters;			//!< contains highlighter allowance


private:

};
#endif
