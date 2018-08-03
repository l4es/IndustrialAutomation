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


#include <QtGui>

#include "fbdialog.h"
#include "pagectrl.h"


FBDialog::FBDialog(QWidget *parent, PageCtrl *pageCtrl)
    : QDialog(parent), m_PageCtrl(pageCtrl)
{ //! creating function

  setWindowFlags(windowFlags()^Qt::WindowContextHelpButtonHint);
  
  setupUi(this);
  retranslateUi();
   
  connect(FBD_listWidget_FBs, 			SIGNAL(itemSelectionChanged()), this, SLOT(setOkButtonState()));
  connect(FBD_checkBox_insertdatatypes, SIGNAL(clicked(bool)),  		this, SLOT(setFBD_checkBox_insertdatatypesState(bool)));
  connect(FBD_checkBox_insertIOComments,SIGNAL(clicked(bool)),  		this, SLOT(setFBD_checkBox_insertIOCommentsState(bool)));
  connect(FBD_listWidget_FBs, 			SIGNAL(currentRowChanged(int)), this, SLOT(showCurrentFBofFBs(int)));
  
  FBD_label_currentFBofFBs->setText("0/0");
}


FBDialog::~FBDialog()
{ //! virtual destructor
}


void FBDialog::fillBlockList()
{ //! fill list with function block names

  FBD_listWidget_FBs->clear(); //clear list
  
  for (int i=0; i < m_PageCtrl->countPages(); i++)//for1
  { //get all tab titles
    QString tmpStr = m_PageCtrl->fileNameOf(i);
    //use of accept FBs only
    if ((m_PageCtrl->fileTypeOf(i) == "FUNCTION_BLOCK")) //disabled untitled blockage && (tmpStr != "untitled"))
  	  FBD_listWidget_FBs->addItem(tmpStr);
  }
 
  showCurrentFBofFBs(0); //show number of function blocks
}


QString FBDialog::getCallModelItem(QString line, QString searchStr1, QString searchStr2, QString addStr)
{ //! get call model item from string by detecting one of the search strings to remove all not needed chars
  /*! The addStr will be added to the return value.*/

  line.remove(" "); 		//remove all blanks
  line.remove("\t"); 		//remove all tabulators
  
  if (line.contains(searchStr1))
    return(line.remove(line.indexOf(searchStr1), line.size()) + addStr); //convert line to call model item
    
  if (line.contains(searchStr2))
    return(line.remove(line.indexOf(searchStr2), line.size()) + addStr); //convert line to call model item

  return(line + addStr); //if nothing could be found return with line and addStr
}

QString FBDialog::setItemComment(QString line)
{ //! get data type and I/O comments as commented string

  QString dataTypeStr="", ioCommentStr="", sepStr="";
  
  if (FBD_checkBox_insertIOComments->isChecked())//if1
    ioCommentStr = getIOComment(line); //get I/O comment

  if (FBD_checkBox_insertdatatypes->isChecked())//if2
    dataTypeStr = getDataType(line); //get data type

  if ((ioCommentStr.length() > 0) && (dataTypeStr.length() > 0))
    sepStr = "; "; //get separator
  
  if ((ioCommentStr.length() > 0) || (dataTypeStr.length() > 0))
    return("(*" + dataTypeStr + sepStr + ioCommentStr + "*)");
  else
    return(""); //return with NULL if no Str is filled
}


QString FBDialog::getDataType(QString line)
{ //! get data type from line

  line.remove(" "); 		//remove all blanks
  line.remove("\t"); 		//remove all tabulators
  line.replace("ARRAY[", "ARRAY [", Qt::CaseInsensitive);//rearange any maybe broken 'ARRAY[..]OF' part 1
  line.replace("]OF", "] OF ", Qt::CaseInsensitive);//rearange any maybe broken 'ARRAY[..]OF' part 2

  if ((line.contains("{")) && (line.contains("}")))
    line.remove(0, line.lastIndexOf(":")+1); //structures handling
  else
    line.remove(0, line.indexOf(":")+1); 	//normal line handling
            
  return(line.remove(line.indexOf(";"), line.length())); //return data type
}


QString FBDialog::getIOComment(QString line)
{ //! get line comment

  if (!line.contains("(*"))
    return(""); //return if no comment start tag can be found

  if (line.contains("*)") && (line.indexOf("(*") < line.indexOf("*)")))
    line.remove(line.indexOf("*)"), line.length()); //remove all strings after the comment
  
  line.remove(0, line.indexOf("(*")+2); //remove all strings before the comment

  return(line); //return line comment
}


QString FBDialog::getInVARSectionLine(QString FBName, int Ext, QString line)
{ //! get line of any input VAR section
	
  QString itemStr="", itemComment="";

  if (line.contains(";"))//if1
  { //if it is a declaration line run process

    //get data type and I/O comments as commented string
    itemComment = setItemComment(line);
      
    //convert into call mode line
    switch ( Ext )//switch1
    { 
      //default operation (can be used by Ext: 0, 1, 2, 3, 4, 7)
      default :{ itemStr.append("\n\t");
                 itemStr.append(getCallModelItem(line, "{", ":", " := ,"));
                 itemStr.append("\t" + itemComment);
                 break;
               }
      //least IEC call model, used for MULTIPROG IL/ST (.IL/.ST) and if flag flg_useIECCall is true
      case 5 : 
      case 6 : { if (m_PageCtrl->currentPrgLng() == "IL")
                 { itemStr.append("\nLD\t" + itemComment + "\n" + "ST\t" + FBName + ".");  
                   itemStr.append(getCallModelItem(line, "{", ":", ""));
                 } 
                 else
                 { itemStr.append("\n" + FBName + ".");
                   itemStr.append(getCallModelItem(line, "{", ":", " := ;"));
                   itemStr.append("\t" + itemComment);
                 }
               }
    }//switch1
  }//if1

  return(itemStr);
}


QString FBDialog::getOutVARSectionLine(QString FBName, int Ext, QString line)
{ //! get line of any output VAR section
	
  QString itemStr="", itemComment="";

  if (line.contains(";"))//if1
  { //if it is a declaration line run process

    //get data type and I/O comments as commented string and prepare line by removing all blanks and tabulators
    itemComment = setItemComment(line);

    //convert into call mode line
    switch ( Ext )//switch1
    { 
      //default operation (can be used by Ext: 0, 2)
      default :{ itemStr.append("\n\t");
                 itemStr.append(getCallModelItem(line, "{", ":", " => ,"));
                 itemStr.append("\t" + itemComment);
                 break;
               }
      //Sucosoft POE (.poe)
      case 1 : { itemStr.append("\n\t");
                 if (m_PageCtrl->currentPrgLng() == "IL")//IL
                   itemStr.append(":= " + getCallModelItem(line, "{", ":", " ,"));
                 else
                   itemStr.append(getCallModelItem(line, "{", ":", " => ,"));
                 itemStr.append("\t" + itemComment);
                 break;
               }
      //STEP 7 AWL/SCL (.AWL/.SCL) and MELSOFT (.asc)
      case 3 : 
      case 4 : 
      case 7 : { itemStr.append("\n\t");
                 itemStr.append(getCallModelItem(line, "{", ":", " := ,"));
                 itemStr.append("\t" + itemComment);
                 break;
               }
      //least IEC call model, used for MULTIPROG IL/ST (.IL/.ST) and if flag flg_useIECCall is true
      case 5 : 
      case 6 : { if (m_PageCtrl->currentPrgLng() == "IL")//IL, LD FBName.xyz \n ST (*data type*)
                 { itemStr.append("\nLD\t" + FBName + ".");  
                   itemStr.append(getCallModelItem(line, "{", ":", ""));
                   itemStr.append("\nST\t" + itemComment);  
                 } 
                 else //ST, (*data type*) := FBName.xyz;
                 { itemStr.append("\n\t:= " + FBName + ".");
                   itemStr.append(getCallModelItem(line, "{", ":", " ;"));
                   itemStr.append("\t" + itemComment);
                 }
                 break;
               }
    }//switch1
  }//if1

  return(itemStr);
}


void FBDialog::setFBD_checkBox_insertdatatypesState(bool state)
{ //! set FBD_checkBox_insertdatatypes state dependend from the state of FBD_checkBox_insertcallmodel

  if (FBD_checkBox_insertcallmodel->isChecked() == false)  
    FBD_checkBox_insertdatatypes->setChecked(false);
}


void FBDialog::setFBD_checkBox_insertIOCommentsState(bool state)
{ //! set FBD_checkBox_insertIOComments state dependend from the state of FBD_checkBox_insertcallmodel

  if (FBD_checkBox_insertcallmodel->isChecked() == false)  
    FBD_checkBox_insertIOComments->setChecked(false);
}


void FBDialog::setOkButtonState()
{ //! set ok Button by detecting the selected list item

  if (FBD_listWidget_FBs->currentRow() > -1)  
    FBD_okButton->setEnabled(true);
  else
    FBD_okButton->setDisabled(true);
}


QString FBDialog::selectedBlockModel()
{ //! get call model of selected function block from list
  /*! The programming language and the file extension of the selected list item will be used to create the correct call model. */
/*example, CoDeSys conform:
CAL TEST1(
	i := ,	(*INT := 1*)
	io :=	(*BYTE*)
	Q => )	(*BOOL*)
*/

  QStringList tmpList, tmpList2;
  QString outStr="", FBName, tmpStr="", prgLngStr, VAR_INPUT_Str, VAR_OUTPUT_Str, VAR_INOUT_Str;
  QString lStr, sStr1, sStr2, rStr1, rStr2;
  QString callStr1="", callStr2="", closeStr1="", closeStr2="";
  int i=0, Ext=0, callModel=0, commentIndex=0;
  int VAR_INPUT_start=0, VAR_INPUT_end=0, VAR_OUTPUT_start=0, VAR_OUTPUT_end=0, VAR_INOUT_start=0, VAR_INOUT_end=0;
  bool VAR_INPUT_empty=false, VAR_INOUT_empty=false, VAR_OUTPUT_empty=false;
  
  //check if any item was selected, otherwise return with empty string
  if (FBD_listWidget_FBs->selectedItems().size() < 1)//if1
    return("");

  //get programming language of selected list item
  prgLngStr = m_PageCtrl->currentPrgLng();
  //get extension of selected list item
  Ext = m_PageCtrl->currentFileExt();
  callModel = Ext;
  //overwrite extension, if IEC call model is selected or extension is any MULTIPROG source
  if ((flg_useSimpleIECCallModel == true) || (Ext == 5) || (Ext == 6))
    callModel = 5;

  //get declaration section of selected list item - - - - - - - - - -
  FBName = FBD_listWidget_FBs->currentItem()->text(); //current list item
  for (i=0; i < m_PageCtrl->countPages(); i++)//for1
  { //get all tab titles
    if(m_PageCtrl->tabTextOf(i).contains(FBName))//if2
    { tmpStr = m_PageCtrl->declarationOf(i); break; }
  }//for1
  
  tmpList = tmpStr.split("\n"); //insert tmpString sep. by '\n'
  tmpList2 = tmpList; 			//assign tmpList to tmpList2
   
  for (int i=0; i < tmpList2.size(); i++)
  { //revise all string list items for trouble-fre VAR_ search
    tmpList2[i].remove(" "); 		//remove all blanks
    tmpList2[i].remove("\t"); 		//remove all tabulators
  }
  
  //get positions of VAR_ .. IN/OUT/INOUT sections - - - - - - - - - -
  VAR_INPUT_start 	= tmpList2.indexOf("VAR_INPUT");
  VAR_INPUT_end 	= tmpList2.indexOf("END_VAR", VAR_INPUT_start);
  VAR_INOUT_start 	= tmpList2.indexOf("VAR_IN_OUT");
  VAR_INOUT_end 	= tmpList2.indexOf("END_VAR", VAR_INOUT_start);
  VAR_OUTPUT_start 	= tmpList2.indexOf("VAR_OUTPUT");
  VAR_OUTPUT_end 	= tmpList2.indexOf("END_VAR", VAR_OUTPUT_start);
  
  
  //filling VAR strings - - - - - - - - - -
  VAR_INPUT_Str.clear();
  VAR_OUTPUT_Str.clear();
  VAR_INOUT_Str.clear();
  
  //fill string for section VAR_INPUT  
  if ((VAR_INPUT_end > VAR_INPUT_start) && (VAR_INPUT_start > -1) && (VAR_INPUT_end > -1))//if3
    for (i=VAR_INPUT_start+1; i<VAR_INPUT_end; i++)//for1
    {
      VAR_INPUT_Str.append(getInVARSectionLine(FBName, callModel, tmpList[i]));
    }//for3
  //if3
      
  //fill string for section VAR_INOUT
  if ((VAR_INOUT_end > VAR_INOUT_start) && (VAR_INOUT_start > -1) && (VAR_INOUT_end > -1))//if4
    for (i=VAR_INOUT_start+1; i<VAR_INOUT_end; i++)//for4
    {
      VAR_INOUT_Str.append(getInVARSectionLine(FBName, callModel, tmpList[i]));
    }//for4
  //if4
      
  //fill string for section VAR_OUTPUT
  if ((VAR_OUTPUT_end > VAR_OUTPUT_start) && (VAR_OUTPUT_start > -1) && (VAR_OUTPUT_end > -1))//if5
  {
    for (i=VAR_OUTPUT_start+1; i<VAR_OUTPUT_end; i++)//for5
    {
      VAR_OUTPUT_Str.append(getOutVARSectionLine(FBName, callModel, tmpList[i]));
    }//for5
  }//if5
  

  //set call strings - - - - - - - - - - 
  switch ( callModel )//switch1
    { 
      case 0 :
      case 1 :
      case 2 :
      case 7 : {callStr1 = "CAL\t" + FBName + " ("; closeStr1 = ")";
                callStr2 = ""; closeStr2 = "";
                break;
               } //.txt; .poe; .exp
      case 3 :
      case 4 : {callStr1 = "CALL " + FBName + ", DB ("; closeStr1 = ")";
                callStr2 = ""; closeStr2 = "";
                break;
               } //.awl; .scl
      default :{callStr1 = ""; closeStr1 = "";
                callStr2 = "\nCAL\t" + FBName; closeStr2 = "";
                break;
               } //default, used if IEC call model is requested or source is .IL or .ST
    }//switch1


  //copy strings to output string - - - - - - - - - -
  outStr.clear();

  if (FBD_checkBox_insertcallmodel->isChecked())
  { //insert first call string, used by plain POU, Moeller, CodeSys, Step7
    outStr.append(callStr1);

    //include existing sections of call model if enabled in dialog
    if (!VAR_INPUT_Str.isEmpty())						//VAR_INPUT
      outStr.append(VAR_INPUT_Str);
    else
      VAR_INPUT_empty = true;
      
    if (!VAR_INOUT_Str.isEmpty())						//VAR_IN_OUT
     outStr.append(VAR_INOUT_Str); 
    else
      VAR_INOUT_empty = true;
    
    //if Sucosoft file and prg. lang is IL erase last ',' and append a '|'
    if ((callModel == 1) && (prgLngStr.contains("IL")) && (!VAR_OUTPUT_Str.isEmpty())) 
    {
      commentIndex = outStr.length();					//get output length as comment position
      if (outStr.lastIndexOf("(*") > -1)				//if one '(*' exist
        commentIndex = outStr.lastIndexOf("(*");		//revise comment index position
      if (!VAR_INPUT_Str.isEmpty())						//check if VAR_INPUT_Str is filled
        outStr.replace(outStr.lastIndexOf(",", commentIndex), 1, "");	//erase last ','
      outStr.append("\n\t|");							//insert one '|'
    }
    
    //insert second call string, used by IEC call and MULTIPROG
    outStr.append(callStr2);
    outStr.append(closeStr2);
    if ((prgLngStr.contains("ST")) && ((callModel == 5) || (callModel == 6)))
      outStr.append(" ;");	

    if (!VAR_OUTPUT_Str.isEmpty()) 						//VAR_OUTPUT
      outStr.append(VAR_OUTPUT_Str);
    else
      VAR_OUTPUT_empty = true;

    //closing sign ')' handling if all VAR blocks are empty
    if ((VAR_INPUT_empty) && (VAR_INOUT_empty) && (VAR_OUTPUT_empty)) 
      outStr.append(closeStr1); 								//set closing if all VAR are empty 
    else
      if ((callModel != 5) && (callModel != 6))
      {
        commentIndex = outStr.length()-1; 						//length-1 to get last ',' if nothing is commented '(*..*)'
        if (outStr.lastIndexOf("(*") > -1)
          commentIndex = outStr.lastIndexOf("(*");
        outStr.replace(outStr.lastIndexOf(",", commentIndex), 1, closeStr1);	//otherwise replace last ',' with a closing if not IEC call
      }
      
    //handle some replacements 
    if ((callModel == 1) && (!VAR_INPUT_Str.isEmpty()))			//if Sucosoft file replace first '\n\t'
      outStr.replace(outStr.indexOf("\n\t"), 2, " ");
    
    if (((callModel == 0) || (callModel == 2)) && prgLngStr.contains("ST"))   //if .txt oder .EXP call replace first CAL in ST language
      outStr.replace(outStr.indexOf("CAL\t"), 4, "");

    if ((callModel == 5) || (callModel == 6))					//if IEC call replace first '\n'
      outStr.replace(outStr.indexOf("\n"), 1, "");


    if ((prgLngStr.contains("ST") || (Ext == 3) || (Ext == 4)) && (callModel != 5) && (callModel != 6))
      outStr.replace(outStr.lastIndexOf(" )"), 2,  " );");		//append ';' for ST language and STEP 7 sources
  }
  else 
  { //handling if call model insertion is disabled - - - - - - - - - -
    outStr.append("CAL\t" + FBName);
    if (prgLngStr.contains("ST") || (Ext == 3) || (Ext == 4))	//append ';' for ST language and STEP 7 sources
      outStr.append(" ;");	
  }
  
  //clear selection
  FBD_listWidget_FBs->clearSelection();
  
  
  //return outStr - - - - - - - - - -
  return(outStr);
}


void FBDialog::setSortingEnabled(bool enable)
{ //! set sorting enable in list

  FBD_listWidget_FBs->setSortingEnabled(enable);
}


void FBDialog::showCurrentFBofFBs(int i)
{ //! show current function block number to all function blocks
  //! \param i is only needed for the signal slot handling

  FBD_label_currentFBofFBs->setText(QString::number(FBD_listWidget_FBs->currentRow()+1) + "/" + QString::number(FBD_listWidget_FBs->count()));
}


void FBDialog::retranslateUi()
{ //! retranslate Ui

  this->setWindowTitle(tr("Insert Function Block")); 
  
  //ok Button
  FBD_okButton->setText(tr("Ok"));
  FBD_okButton->setStatusTip(tr("Accept selection and close window"));
  FBD_okButton->setToolTip(FBD_okButton->statusTip());
  
  //cancel Button
  FBD_cancelButton->setText(tr("Cancel"));
  FBD_cancelButton->setStatusTip(tr("Cancel selection and close window"));
  FBD_cancelButton->setToolTip(FBD_cancelButton->statusTip());
  
  //checkBox FBD_checkBox_insertcallmodel
  FBD_checkBox_insertcallmodel->setText(tr("Insert with call model"));
  FBD_checkBox_insertcallmodel->setStatusTip(tr("Insert function block with call model"));
  FBD_checkBox_insertcallmodel->setToolTip(FBD_checkBox_insertcallmodel->statusTip());

  //checkBox FBD_checkBox_insertdatatypes
  FBD_checkBox_insertdatatypes->setText(tr("Insert with data types as comment"));
  FBD_checkBox_insertdatatypes->setStatusTip(tr("Insert function block with data types as comment next to the call model items"));
  FBD_checkBox_insertdatatypes->setToolTip(FBD_checkBox_insertdatatypes->statusTip());

  //checkBox FBD_checkBox_insertIOComments
  FBD_checkBox_insertIOComments->setText(tr("Insert with I/O comments"));
  FBD_checkBox_insertIOComments->setStatusTip(tr("Insert function block with the comments for the call model items"));
  FBD_checkBox_insertIOComments->setToolTip(FBD_checkBox_insertIOComments->statusTip());
}

