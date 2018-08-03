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


#include "printprepare.h"
#include "pagectrl.h"
#include "prototype.h"
#include "importexport.h"


PrintPrepare::PrintPrepare(PageCtrl *pageCtrl, Prototype *prototype, ImportExport *importExport)
  : m_PageCtrl(pageCtrl), m_Prototype(prototype), m_ImportExport(importExport) //member VAR of PrintPrepare
{ //! creating function
  //! \param PageCtrl class, Protoype class, ImportExport class

}


PrintPrepare::~PrintPrepare()
{ //! virtual destructor
}


QString PrintPrepare::prepareCurrentSucosoftHeader(QString SucosoftHeader)
{ //! return prepformatted Moeller header

  QStringList list1 = SucosoftHeader.split("\n");
  QString tmpStr, outStr;
  int i=0, POETXT_start=0, POETXT_end=0;
  
  tmpStr.clear();
  outStr.clear();

  //get PRJ/NAME/DATE/COMPANY/VERS/SPS
  foreach (QString tmpStr, list1) 
  {
    if (tmpStr.contains("PROJECT ="))	
      outStr.append(tmpStr + "\n");
    if (tmpStr.contains("OEMNAME ="))	
      outStr.append(tmpStr + "\n");
    if (tmpStr.contains("OEMDATE ="))	
      outStr.append(tmpStr + "\n");
    if (tmpStr.contains("COMPANY ="))	
      outStr.append(tmpStr + "\n");
    if (tmpStr.contains("VERSION ="))	
      outStr.append(tmpStr + "\n");
    if (tmpStr.contains("SPS_TYPE ="))	
      outStr.append(tmpStr + "\n");
   }


  //get POETXT information
  for (i=24; i<list1.size(); i++)
  { //get index of POETXT start and end points
  	tmpStr = list1.value(i);
  	if (tmpStr.contains("POETXTSIZE ="))	
      POETXT_start = i+1;
    if (tmpStr.contains("$END_KM_HEADER"))	
      POETXT_end = i;
  }
  for (i = POETXT_start; i < POETXT_end; i++)
  { //get single lines and strip 'POETXT = ' from it 
  	tmpStr = list1.value(i).simplified();
  	outStr.append("\n" + tmpStr.remove(0, 9));
  }

  return(outStr);
}


void PrintPrepare::prepareCurrentPage(QStringList *printList, QString SucosoftHeader)
{ //! print content of current page

  //insert file type + file name
  printList->append("\n" + m_PageCtrl->currentFileType() + " " + m_PageCtrl->currentFileName() + "\n");
  //insert path + complete file name
  printList->append("\n" + m_PageCtrl->currentFilePath() + m_PageCtrl->currentFileName() + m_PageCtrl->currentFileExtStr() + "\n");

  if (printPOUI == true)
  { //get POU Info section
    printList->append("\n\n");
    //set delimiter
    printList->append("-----------------------------------------------------------------------------\n");
    printList->append(tr("POU Info section") + "\n");
    printList->append("-----------------------------------------------------------------------------\n\n");
    //get POU Info data
    if ((m_PageCtrl->currentFileExt() == 0) || (m_PageCtrl->currentFileExt() == 1))
      printList->append(prepareCurrentSucosoftHeader(SucosoftHeader));  
    if ((m_PageCtrl->currentFileExt() == 5) || (m_PageCtrl->currentFileExt() == 6))
      printList->append(m_PageCtrl->currentMULTIPROGDescription());
  }
  
  if (printPrototype == true)
  { //get prototype section
    printList->append("\n\n");
    //set delimiter
    printList->append("-----------------------------------------------------------------------------\n");
    printList->append(tr("Prototype section") + "\n");
    printList->append("-----------------------------------------------------------------------------\n\n");
    //get prototype data
    m_Prototype->currentPrototype(printList); 
  }
  
  if (printDeclarations == true)
  { //get declaration section
    printList->append("\n\n");
    //set delimiter
    printList->append("-----------------------------------------------------------------------------\n");
    printList->append(tr("Declaration section") + "\n");
    printList->append("-----------------------------------------------------------------------------\n\n");
    //get declaration data
    printList->append(m_PageCtrl->currentDeclaration()); 
  }

  if (!(m_PageCtrl->currentFileType().contains("GLOBAL_VARIABLE_LIST") || m_PageCtrl->currentFileType().contains("TYPE") || 
  	    m_PageCtrl->currentFileType().contains("CONFIGURATION")) && (printInstructions == true))
  { //get instruction section
    printList->append("\n\n");
    //set delimiter
    printList->append("-----------------------------------------------------------------------------\n");
    printList->append(tr("Instruction section") + "\n");
    printList->append("-----------------------------------------------------------------------------\n\n");
    //get instruction data
    printList->append(m_PageCtrl->currentInstruction());  
  }
}
