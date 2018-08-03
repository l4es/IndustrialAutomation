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


#include "prototype.h"
#include "pagectrl.h"


Prototype::Prototype(PageCtrl *pageCtrl)
  : m_PageCtrl(pageCtrl) //m_PageCtrl as member VAR of PrefDialog
{ //! creating function
  //! \param PageCtrl class

}


Prototype::~Prototype()
{ //! virtual destructor
}


void Prototype::currentPrototype(QStringList *PTTList)
{ //! get current protoype into input variable PTTList 
/*example:
FUNCTION_BLOCK TEST1
VAR_INPUT
 i : INT := 1;
END_VAR
VAR_IN_OUT
 io : BYTE;
END_VAR
VAR_OUTPUT
 Q : BOOL;
END_VAR
END_FUNCTION_BLOCK
*/

  QStringList tmpList;
  QString tmpStr, tmpStr2, VAR_INPUT_Str, VAR_OUTPUT_Str, VAR_INOUT_Str;
  int i=0;
  int VAR_INPUT_start=0, VAR_INPUT_end=0, VAR_OUTPUT_start=0, VAR_OUTPUT_end=0, VAR_INOUT_start=0, VAR_INOUT_end=0;
  
  if (m_PageCtrl->countPages() < 1)
    return; //return if no page is active
  
  //get declaration section into stringlist
  tmpStr = m_PageCtrl->currentDeclaration();
  tmpList.clear();
  tmpList = tmpStr.split("\n");  
  for (i=0; i<tmpList.size(); i++) 
  { //revise all string list items
    tmpList[i].remove(" "); 		//remove all blanks
    tmpList[i].remove("\t"); 		//remove all tabulators
    tmpList[i].replace(":", " : ");	//replace any ':' with ' : ' to get a defined number of blanks between VAR and it's datatype
    tmpList[i].replace(": =", ":= ");//rearange any maybe resulting ': ='   
    tmpList[i].replace("ARRAY[", "ARRAY [", Qt::CaseInsensitive);//rearange any maybe broken 'ARRAY[..]OF' part 1
    tmpList[i].replace("]OF", "] OF ", Qt::CaseInsensitive);//rearange any maybe broken 'ARRAY[..]OF' part 2
  }

  //get positions of VAR_ .. IN/OUT/INOUT sections
  VAR_INPUT_start 	= tmpList.indexOf("VAR_INPUT");
  VAR_INPUT_end 	= tmpList.indexOf("END_VAR", VAR_INPUT_start);
  VAR_INOUT_start 	= tmpList.indexOf("VAR_IN_OUT");
  VAR_INOUT_end 	= tmpList.indexOf("END_VAR", VAR_INOUT_start);
  VAR_OUTPUT_start 	= tmpList.indexOf("VAR_OUTPUT");
  VAR_OUTPUT_end 	= tmpList.indexOf("END_VAR", VAR_OUTPUT_start);
  
  VAR_INPUT_Str.clear();
  VAR_OUTPUT_Str.clear();
  VAR_INOUT_Str.clear();
  
  //fill string for section VAR_INPUT
  if ((VAR_INPUT_end > VAR_INPUT_start) && (VAR_INPUT_start > -1) && (VAR_INPUT_end > -1))
  {
    VAR_INPUT_Str.append(tmpList[VAR_INPUT_start] + "\n");
    for (i=VAR_INPUT_start+1; i<VAR_INPUT_end; i++)
      if (tmpList[i].contains(";")) //get declaration line with cutting off all after the semikolon
        VAR_INPUT_Str.append(" " + tmpList[i].remove(tmpList[i].indexOf(";")+1, tmpList[i].size()) + "\n");
    VAR_INPUT_Str.append(tmpList[VAR_INPUT_end] + "\n");
  }

  //fill string for section VAR_INOUT
  if ((VAR_INOUT_end > VAR_INOUT_start) && (VAR_INOUT_start > -1) && (VAR_INOUT_end > -1))
  {
    VAR_INOUT_Str.append(tmpList[VAR_INOUT_start] + "\n");
    for (i=VAR_INOUT_start; i<VAR_INOUT_end+1; i++)
      if (tmpList[i].contains(";"))
        VAR_INOUT_Str.append(" " + tmpList[i].remove(tmpList[i].indexOf(";")+1, tmpList[i].size()) + "\n");
    VAR_INOUT_Str.append(tmpList[VAR_INOUT_end] + "\n");
  }
   
  //fill string for section VAR_OUTPUT
  if ((VAR_OUTPUT_end > VAR_OUTPUT_start) && (VAR_OUTPUT_start > -1) && (VAR_OUTPUT_end > -1))
  {
    VAR_OUTPUT_Str.append(tmpList[VAR_OUTPUT_start] + "\n");
    for (i=VAR_OUTPUT_start; i<VAR_OUTPUT_end+1; i++)
      if (tmpList[i].contains(";"))
        VAR_OUTPUT_Str.append(" " + tmpList[i].remove(tmpList[i].indexOf(";")+1, tmpList[i].size()) + "\n");
    VAR_OUTPUT_Str.append(tmpList[VAR_OUTPUT_end] + "\n");
  }
    
  //copy strings into PTT stringlist
  tmpStr = m_PageCtrl->currentFileName(); //get current filename without extension 
  tmpStr2 = m_PageCtrl->currentFileType() + " " + tmpStr + m_PageCtrl->currentFncType() + "\n";
  if (!VAR_INPUT_Str.isEmpty())
    tmpStr2.append(VAR_INPUT_Str);
  if (!VAR_INOUT_Str.isEmpty())
    tmpStr2.append(VAR_INOUT_Str); 	
  if (!VAR_OUTPUT_Str.isEmpty())
    tmpStr2.append(VAR_OUTPUT_Str);  
  tmpStr2.append(m_PageCtrl->currentFileEnd());

  PTTList->append(tmpStr2);
}
