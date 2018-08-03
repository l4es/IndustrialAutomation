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


#include "fbcallconverter.h"
#include "pagectrl.h"


FBCallConverter::FBCallConverter(PageCtrl *pageCtrl) 
  : m_PageCtrl(pageCtrl) //m_PageCtrl as member VAR of converter
{ //! creating function for FBCallConverter -function block call<CAL> model converter-
  //! \param PageCtrl class

}


FBCallConverter::~FBCallConverter()
{ //! virtual destructor
}


//=============================================================================
// - - - public SLOTs - - -
void FBCallConverter::convert(QString *Str, int pageIndex, int from, int to, int option)
{ //! FB call convert routine
  //! \param *Str contains the string wich will be worked
  //! \param pageIndex is the index of the selected page
  //! \param from contains the current extension of the file/page
  //! \param to contains the requested extension of the file/page
  //! \param option contains the information about special conversion options
  //! option 0: do nothing
  //! option 1: all into selected compatibility call model
  //! option 2: outputs into simple IEC call model
  //! option 3: all into simple IEC call model

  if ((option < 1) || (from == to) || (from == 5) || (from == 6))
    return; //return if option < 1(means do nothing) or from=to or any MULTIPROG source

  int i=0, start=0, end=0;
  QStringList *tmpList = new QStringList();

  tmpList->append(Str->split("\n")); 		//get instruction section into string list

  for (i=0; i<tmpList->size(); i++)			//loop to work tmpList
  {
    //get position of next function block call model
    start = getNextCALPos(tmpList, pageIndex, end);	
    //handle FB call conversion at start pos.
    end = convertFBCAllAtTo(tmpList, pageIndex, start, to, option); 
    //check if length of list reached
    if (end >= tmpList->size()-1)
      break;
  }

  //insert revised instructions into page - - - - - -
  Str->clear();							//clear tmpStr
  Str->append(tmpList->join("\n"));		//return worked list into string

  delete tmpList;						//delete tmpList
}




//=============================================================================
// - - - diverse private functions - - - 
int FBCallConverter::convertFBCAllAtTo(QStringList *tmpList, int pageIndex, int index, int to, int option)
{ //! convert default call at index in tmpList into Moeller style
  //! \param *tmpList temporary list which will be filled with the file content
  //! \param pageIndex is the index of the selected page
  //! \param index contains position of the next function block call model
  //! \param to holds the appointed file extension
  //! \param option contains the information about special conversion options
  /*! supported conversion values: */
  /*! option 0: do nothing */
  /*! option 1: all into selected compatibility call model */
  /*! option 2: outputs into simple IEC call model */
  /*! option 3: all into simple IEC call model */
  
  bool foundMid=false, foundEnd=false, defaultMid=false, SucosoftMid=false, anyMid=false;
  int i=0, mid=0, end=0, oStyle=0;
  QString convStr="", inputStr="", outputStr="",tagStr="", commentStr="", callEnd="", instanceStr="";
  QString tmpStr="", calStr="", prgLng = m_PageCtrl->prgLngOf(pageIndex);

  for (i=index; i<tmpList->size(); i++)
  { //get positions of mid and end
    tmpStr = tmpList->at(i); //get single line
    tmpStr.simplified();
    //get first default call output delimiter
    if ((tmpStr.contains("=>") || tmpStr.contains("(*outputs*)") || tmpStr.contains("|")) && (foundMid == false))
    { mid = i; foundMid = true; 
      if (tmpStr.contains("=>")) 		defaultMid = true;
      if (tmpStr.contains("|")) 		SucosoftMid = true;
      if (tmpStr.contains("(*outputs*)")) 	anyMid = true;
    }
    //get call end
    if (((tmpStr.contains(")") && (prgLng == "IL")) || (tmpStr.contains(");") && (prgLng == "ST"))) && (foundEnd == false) && (foundMid == true))
      if ((tmpStr.contains("*)") == false) || (tmpStr.indexOf(")") < tmpStr.indexOf("*)"))) 
      { end = i; foundEnd= true; break; }
  }
  if ((foundMid == false) || (foundEnd == false))
    return(i); //return if mid or end cannot be found
    
  tmpStr.clear();
  

  //handle FB CAL line - - - - - - - - - - - - - - - - 
  //get 'CAL(L,C,NC)'
  calStr = tmpList->at(index);
  calStr = calStr.simplified();
  calStr = calStr.remove(calStr.indexOf(" "), calStr.length());
  calStr = calStr.remove(" ");
  calStr = calStr.toUpper();
  if (calStr.contains("LL"))
    calStr = calStr.replace("LL", "L"); //if necc. change 'CALL' to 'CAL' to get 'CAL' as standard
  
  //get 'INSTANCE' 
  instanceStr = tmpList->at(index);
  instanceStr = instanceStr.simplified();
  if (calStr.length() > 0) //remove 'CAL' until first occurence of ' ' if any call strin exists only
    instanceStr = instanceStr.remove(0, instanceStr.indexOf(" "));
  instanceStr = instanceStr.remove(instanceStr.indexOf("("), instanceStr.length());
  instanceStr = instanceStr.remove(" ");
  
  //re-connect instance line
  if ((option < 3) && (to != 5) && (to != 6))
  { 
    if ((to == 3) || (to == 4)) //if any Siemens POU change 'CAL' to 'CALL'
      calStr = calStr.replace("CAL", "CALL");

    convStr.append(calStr); //append CAL
      
    convStr.append("\t"); convStr.append(instanceStr); convStr.append("( ");  //finish instance line
  }
  //- - - - - - - - - - - - - - - - - - - - - - - - -


  //handle FB intputs - - - - - - - - - - - - - - - - 
  //get maybe existing input in start line
  tmpStr = tmpList->at(index);
  tmpStr.remove(0, tmpStr.indexOf("(")+1);
  if ((option == 3) || (to == 5) ||(to == 6))
    convStr.append(getIECInputLine(instanceStr, tmpStr, 0, prgLng));
  else
    convStr.append(getDefaultInputLine(tmpStr, 0)); 
   
  //get all following input lines
  for (i = index+1; i < mid; i++)
  { //get FB inputs from line
    tmpStr = tmpList->at(i);
    if ((option == 3) || (to == 5) ||(to == 6))
      convStr.append(getIECInputLine(instanceStr, tmpStr, 0, prgLng));
    else
      convStr.append(getDefaultInputLine(tmpStr, 0)); 
  }
  
  //set end ')' if IEC outputs requested or end of call model reached
  if ((option == 2) && (to != 5) && (to != 6))
    convStr.replace(convStr.lastIndexOf(","), 1, " )");
  //- - - - - - - - - - - - - - - - - - - - - - - - -


  //handle FB outputs - - - - - - - - - - - - - - - - 
  //revise mid position if any mid or Sucosoft mid sign was found
  if ((anyMid == true) || (SucosoftMid == true))
    mid = mid+1;

  //conversion prehandling for from Sucosoft into any
  if ((m_PageCtrl->fileExtOf(pageIndex) == 1) && (prgLng == "IL"))
    oStyle = 1;

  //conversion prehandling for into Sucosoft IL
  if ((option < 2) && (to == 1) && (prgLng == "IL"))
  { 
    convStr.append("\t|\n");
    convStr.remove(convStr.lastIndexOf(","), 1); //remove last ','
  }
   
  //conversion prehandling for MELSOFT and STEP 7
  if ((option < 2) && ((to == 3) || (to == 4) || (to == 7)))
    convStr.append("(*outputs*)\n");

  //add function block call if complete IEC mode is requested
  if (((option > 2) && (prgLng == "IL")) || (to == 5)) 
    convStr.append("CAL\t" + instanceStr + "\n");
  if (((option > 2) && (prgLng == "ST")) || (to == 6)) 
    convStr.append(instanceStr + "();\n");
  	
  //get all following output lines
  for (i = mid; i < end+1; i++)
  { //get FB outputs from line
    tmpStr = tmpList->at(i);
      
    //conversion into requested IEC outputs
    if ((option > 1) || (to == 5) || (to == 6)) 
      convStr.append(getIECOutputLine(instanceStr, tmpStr, oStyle, prgLng));
      
    //conversion into plain POU or CoDeSys or Sucosoft ST
    if (((option < 2) && ((to == 0) || (to == 2))) || ((to == 1) && (prgLng == "ST")))
      convStr.append(getDefaultOutputLine(tmpStr, oStyle)); 
      
    //conversion into Sucosoft IL
    if ((option < 2) && (to == 1) && (prgLng == "IL"))
     convStr.append(getMoellerILOutputLine(tmpStr, oStyle)); 

    //conversion into STEP7 or MELSOFT
    if ((option < 2) && ((to == 3) || (to == 4) || (to == 7)))
      convStr.append(getDefaultInputLine(tmpStr, oStyle)); 
  }

  convStr.remove(convStr.lastIndexOf("\n"), 1); //remove last '\n'
  //- - - - - - - - - - - - - - - - - - - - - - - - -

  if ((prgLng == "ST") && (option < 3))
    convStr.replace(convStr.lastIndexOf(" )"), 2, " );");
  
  
  //remove old lines of FB call - - - - - - - - - - -
  for (i = 0; i < (end-index+1); i++)
    tmpList->removeAt(index);
  
  
  //insert new lines of FB call - - - - - - - - - - - 
  tmpList->insert(index, convStr);

  return(index+1);						//return index +1

}


bool FBCallConverter::getCallLineItems(QString str, int ioStyle, QString *fbPart, QString *tag, QString *comment, QString *lineEnd)
{ //! get function block part and connected tag by solving str into fbPart and tag
  //! \param str contains line
  //! \param ioStyle used to handle standard inputs/outputs (0) and Moeller outputs (1)
  //! \param *fbPart will filled with function block in/out substring
  //! \param *tag will filled with connected tag substring
  //! \param *comment will filled with comment substring
  //! \param *lineEnd will filled with the line end sign

  QString ioDelimiter = "=>";

  //clear fbPart, tag, comment
  fbPart->clear();
  tag->clear();
  comment->clear();
  lineEnd->clear();

  str.simplified(); 						//remove any whitespace from the start and the end
  str.remove("\t"); 						//remove any tab
  
  if (!str.isEmpty())
  { //go through string handling if string is not empty
    if (str.contains("(*"))
    { //separate an existing comment
      comment->append(getComment(str));					//call get comment routine
      str.remove(str.indexOf("(*"), str.length()); 	//remove comment from string
    }

    //remove all blanks and ',' in line
    str.remove(" ");
    str.remove(",");
    str.remove(";");
    
    if (str.contains(")"))
    { str.remove(")");	lineEnd->append(" )"); }
    else 				lineEnd->append(" ,");

    //check io Delimiter
    if (str.contains(":="))
      ioDelimiter = ":=";
    else
      ioDelimiter = "=>";
      

    if (ioStyle == 0)
    { //standard input / output Style 'io := tag' / 'io => tag'
      //get connected tag if line is in std. style
      tag->append(str);
      tag->remove(0, (tag->indexOf(ioDelimiter)+ioDelimiter.length()));
      //get FB part
      fbPart->append(str);
      fbPart->remove(fbPart->indexOf(ioDelimiter), fbPart->length()); 
    }
    else
    { //Moeller output style 'tag := io'
      //get connected tag if line is in Moeller style
      tag->append(str);
      tag->remove(tag->indexOf(ioDelimiter), tag->length());
      //get FB part
      fbPart->append(str);
      fbPart->remove(0, (fbPart->indexOf(ioDelimiter)+ioDelimiter.length()));
    }
  }
  
  //simplifiy tag & fbPart
  tag->simplified();
  fbPart->simplified();
  
  //return fbPart is empty flag
  return(!fbPart->isEmpty());
}


QString FBCallConverter::getComment(QString str)
{ //! get comment from str

  if (str.contains("(*"))					//if comment signs exists extract comment and prepend a tab
    return("\t" + str.remove(0, str.indexOf("(*")));
  else
    return("");								//return empty string
}


QString FBCallConverter::getDefaultInputLine(QString str, int ioStyle)
{ //! get default input line: '	input	:= tag,' 

  bool ok=false;
  QString comment="", fbPart="",tag="", callEnd="", lineEnd="";  
  
  ok = getCallLineItems(str, ioStyle, &fbPart, &tag, &comment, &lineEnd);
  str.clear();
  
  if (ok)
  {
    //connect all FB input parts
    str.append("\t");
    str.append(fbPart);
    str.append("\t:= ");
    str.append(tag);
    str.append(lineEnd);
  }
  str.append(comment);						//add comment string
  
  if (!str.isEmpty())
    str.append("\n");						//add line break
  
  return(str);								//return worked string
}


QString FBCallConverter::getDefaultOutputLine(QString str, int ioStyle)
{ //! get default output line: '	output	=> tag,'  
  
  bool ok=false;
  QString comment="", fbPart="",tag="", callEnd="", lineEnd="";  
  
  ok = getCallLineItems(str, ioStyle, &fbPart, &tag, &comment, &lineEnd);
  str.clear();
  
  if (ok)
  {
    //connect all FB output parts
    str.append("\t");
    str.append(fbPart);
    str.append("\t=> ");
    str.append(tag);
    str.append(lineEnd);
  }
  str.append(comment);						//add comment string

  if (!str.isEmpty())
    str.append("\n");						//add line break
  
  return(str);								//return worked string
}


QString FBCallConverter::getIECInputLine(QString instance, QString str, int ioStyle, QString prgLng)
{ //! get IEC intput line according to the programming language
  //! IL: 'LD	tag   ST	INSTANCE.input	(*...*)\n'
  //! ST: 'INSTANCE.input	:=	tag;	(*...*)\n'

  bool ok=false;
  QString comment="", fbPart="",tag="", callEnd="", lineEnd="";  
  
  ok = getCallLineItems(str, ioStyle, &fbPart, &tag, &comment, &lineEnd);
  str.clear();

  if (ok && (!tag.isEmpty()) && (prgLng == "IL"))
  {
    //connect all FB input parts
    str.append("LD\t" + tag + "\n");
    str.append("ST\t");
    str.append(instance + "." + fbPart + comment); 
  }


  if (ok && (prgLng == "ST"))
  {
    //connect all FB input parts
    str.append(instance + "." + fbPart);
    str.append("\t:=\t");
    str.append(tag + ";");
    str.append(comment);
  }
  
  if (!str.isEmpty())
    str.append("\n");						//add line break
  
  return(str);								//return worked string
}


QString FBCallConverter::getIECOutputLine(QString instance, QString str, int ioStyle, QString prgLng)
{ //! get IEC output line according to the programming language
  //! IL: 'LD	INSTANCE.output	(*...*)\n   ST	tag\n'
  //! ST: 'tag	:=	INSTANCE.output;	(*...*)\n'

  bool ok=false;
  QString comment="", fbPart="",tag="", callEnd="", lineEnd="";  
  
  ok = getCallLineItems(str, ioStyle, &fbPart, &tag, &comment, &lineEnd);
  str.clear();
  
  if (ok && (prgLng == "IL"))
  { //IL call model
    //connect all FB output parts
    str.clear();
    str.append("LD\t");
    str.append(instance +"." + fbPart + comment);
    if (!tag.isEmpty())
    { str.append("\nST\t"); str.append(tag); }
  }

  if (ok && (!tag.isEmpty()) && (prgLng == "ST"))
  { //ST call model
    //connect all FB output parts
    str.append(tag);
    str.append("\t:=\t");
    str.append(instance +"." + fbPart + ";");
    str.append(comment);
  }

  if (!str.isEmpty())
    str.append("\n");						//add line break
  
  return(str);								//return worked string
}


QString FBCallConverter::getMoellerILOutputLine(QString str, int ioStyle)
{ //! get Moeller output line: '	tag	:= > output,'  
	 
  bool ok=false;
  QString comment="", fbPart="",tag="", callEnd="", lineEnd="";  
  
  ok = getCallLineItems(str, ioStyle, &fbPart, &tag, &comment, &lineEnd);
  str.clear();
  
  if (ok)
  {
    //connect all FB output parts
    str.append("\t");
    str.append(tag);
    str.append("\t:= ");
    str.append(fbPart);
    str.append(lineEnd);
  }
  str.append(comment);						//add comment string

  if (!str.isEmpty())
    str.append("\n");						//add line break
  
  return(str);								//return worked string
}


int FBCallConverter::getNextCALPos(QStringList *tmpList, int pageIndex, int index)
{ //! get position of next FB call 'CAL'/'CALL'/'abcdefg ('

  int i=0;
  QString tmpStr="", prgLng = m_PageCtrl->prgLngOf(pageIndex);;
  QRegExp rxCAL("\\b(:=|CAL|CALC|CALCN|CALL|CALLC|CALLCN)\\b");
  QRegExp rxLogic("\\b(:=|U|AND|ANDN|O|OR|ORN|XOR|XORN)\\b");

  for (i=index; i<tmpList->size(); i++)
  { //get position of next FB call 'CAL'/'CALL/instance(' 
    tmpStr = tmpList->at(i); 	//get single line
    tmpStr.simplified();	//remove any whitespace from the start and the end

    if (tmpStr.startsWith("CAL", Qt::CaseInsensitive))
      if (tmpStr.contains(rxCAL)) //check if reg. expression is truly a CAL
        break; //break loop if 'CAL'/'CALL' was found 	<-- IL language

    if (tmpStr.contains("(") && (tmpStr.indexOf("(") != tmpStr.indexOf("(*")) && ((tmpStr.indexOf(rxLogic) < 0) || (tmpStr.indexOf(rxLogic) > tmpStr.indexOf("("))) && (prgLng == "ST"))
      break; //break loop if 'FBName(' was found 				<-- ST language
  }
  
  return(i); //return i as indicator of next occurance or list end
}

