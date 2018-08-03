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


#include <QtCore>


#include "pagedata.h"


PageData::PageData(QWidget *parent) 
  : QSplitter(parent)
{ //! creating function
  //! \param QSplitter as parent

  //presetting for public page data variables
  //FilePath
  pd_FilePath = "";
  //FileExt
  pd_FileExt = "0";
  //FileName
  pd_FileName = "untitled";
  //FileDate
  pd_FileDate = "";
  //FileType
  pd_FileType = "";
  //FunctionType
  pd_FncType = "";
  //FileEnd
  pd_FileEnd = "";
  //Programming Lang.
  pd_PROGLNG = "";
  //IDE reference
  pd_IDERef = "0"; 					//IDE reference is defined as number
  //Sucosoft Header Code, KMHeader
  pd_SucosoftHeader ="";
  //MULTIPROG description section
  pd_MULTIPROGDescription = "";
  //MULTIPROG properties section
  pd_MULTIPROGProperties = "TYPE: POU\nLOCALE: 0\nIEC_LANGUAGE: IL\nPLC_TYPE: independent\nPROC_TYPE: independent";
  //MULTIPROG work sheet name
  pd_MULTIPROGWorkSheetName = "";
  //STEP 7 DB data
  pd_STEP7DBData = "";
  //MELSOFT config data
  pd_MELSOFTConfig = "";
  //highlighter flag
  pd_highlighters = "OFF";

  
  //hide this class to prevent an additional spitter section on the page
  this->hide();
}


PageData::~PageData()
{ //! virtual destructor
}
