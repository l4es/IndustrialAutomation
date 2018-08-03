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
#include <QtCore>


#include "importexport.h"
#include "pagectrl.h"
#include "prototype.h"
#include "batchdialog.h"
#include "fbcallconverter.h"


ImportExport::ImportExport(PageCtrl *pageCtrl, Prototype *prototype) 
  : m_PageCtrl(pageCtrl), m_Prototype(prototype) //m_PageCtrl as member VAR of PrefDialog
{ //! creating function for ImportExport
  //! \param PageCtrl class, Prototype class

  progressBar = new QProgressBar(0); //create progressbar
  retranslateUi();					 //create texts
}


ImportExport::~ImportExport()
{ //! virtual destructor

  delete progressBar; //delete progressbar
}


QString ImportExport::createSucosoftHeader(QString Str)
{ //! revise Moeller Sucosoft header from string input
	
  QStringList list1;
  QString tmpStr, dayName, monthName, timeStamp;
  QDate date;
  QTime time;

  //get day string
  switch ( date.currentDate().dayOfWeek() )
  {
  	case 1 : { dayName = "Mon"; break; } //monday
    case 2 : { dayName = "Tue"; break; } //Tuesday
    case 3 : { dayName = "Wed"; break; } //Wednesday
    case 4 : { dayName = "Thu"; break; } //Thursday
    case 5 : { dayName = "Fri"; break; } //Friday
    case 6 : { dayName = "Sat"; break; } //Saturday
    case 7 : { dayName = "Sun"; break; } //Sunday
  }
  //get month string
  switch ( date.currentDate().month() )
  {
  	case 1 : { monthName = "Jan"; break; } //January
    case 2 : { monthName = "Feb"; break; } //February
    case 3 : { monthName = "Mar"; break; } //March
    case 4 : { monthName = "Apr"; break; } //April
    case 5 : { monthName = "May"; break; } //May
    case 6 : { monthName = "Jun"; break; } //June
    case 7 : { monthName = "Jul"; break; } //July
    case 8 : { monthName = "Aug"; break; } //August
    case 9 : { monthName = "Sep"; break; } //September
    case 10: { monthName = "Oct"; break; } //October
    case 11: { monthName = "Nov"; break; } //November
    case 12: { monthName = "Dec"; break; } //December
  }  
  //set timestamp
  timeStamp = "TIMESTAMP = " 
  + dayName + " " + monthName + " " + date.currentDate().toString("dd") 
  + " " + time.currentTime().toString("hh:mm:ss") 
  + " " + date.currentDate().toString("yyyy");     

  
  list1.clear();
  
  if (!Str.isEmpty())
  { //if string is filled replace only cahngeable items
    list1 = Str.split("\n"); 
    for (int i=0; i<list1.size(); i++)
    {
      tmpStr = list1[i];
      if (tmpStr.contains("KM_TEXT"))	
        list1[i] = m_PageCtrl->Sucosoft_KM_TEXT;
      if (tmpStr.contains("S40_VERSION"))	
        list1[i] = m_PageCtrl->Sucosoft_S40_VERSION;
      if (tmpStr.contains("TIMESTAMP"))	
        list1[i] = timeStamp;
      if (tmpStr.contains("tKisInstrEdi"))
      	if (m_PageCtrl->currentPrgLng() == "IL")
      	  list1[i] = "tKisInstrEdi = kAwl"; //SucoSoft AWL marker
      	else 
      	  list1[i] = "tKisInstrEdi = ST"; //SucoSoft ST marker
      if (tmpStr.contains("tKisSplitMode"))
        if (m_PageCtrl->currentSplitMode() == "v")
          list1[i] = "tKisSplitMode = kVer";
        else 
          list1[i] = "tKisSplitMode = kHor";
      if (tmpStr.contains("m_ulSplitSize"))
        list1[i] = ("m_ulSplitSize = " + QString::number(m_PageCtrl->currentSplitSize())); 
    }
  }
  else
  { //if string was empty set complete new Moeller SucoSoft file header
    list1.append("$BEGIN_KM_HEADER");
    list1.append("<LINES=00004-SIZE=00000078-NEXTPOS=00000000>");
    list1.append(m_PageCtrl->Sucosoft_KM_TEXT);
    list1.append(m_PageCtrl->Sucosoft_S40_VERSION);
    list1.append("CRC = 34102");
    list1.append(timeStamp);
    list1.append("<LINES=00010-SIZE=0000012B-NEXTPOS=00000000>");
    list1.append("tKisDeclEdi = kTxt");
    if (m_PageCtrl->currentPrgLng() == "IL")
      list1.append("tKisInstrEdi = kAwl"); 	//SucoSoft AWL marker
    else 
      list1.append("tKisInstrEdi = ST"); 	//SucoSoft ST marker
    list1.append("tKisWinArea = kImpl");
    list1.append("m_WndRect = 0#0#650#283#");
    if (m_PageCtrl->currentSplitMode() == "v")
      list1.append("tKisSplitMode = kVer");
    else 
      list1.append("tKisSplitMode = kHor");
    list1.append("tKisKindOfDeclBlock = kDeclVar");
    list1.append("m_ulSplitSize = " + QString::number(m_PageCtrl->currentSplitSize())); 
    list1.append("m_InstrPos = 0/0#0/0#0/0#");
    list1.append("m_DeclPos = 0/0#0/0#");
    list1.append("m_ulGridColWidth = ");
    list1.append("<LINES=00007-SIZE=00000059-NEXTPOS=00000000>");
    list1.append("PROJECT = ");
    list1.append("OEMNAME = ");
    list1.append("OEMDATE = ");
    list1.append("COMPANY = ");
    list1.append("VERSION = ");
    list1.append("SPS_TYPE = ");
    list1.append("POETXTSIZE = 0");
    list1.append("$END_KM_HEADER");
  }
  
  //return list 
  return(list1.join("\n"));
}


void ImportExport::setProgressBar(int files, int minFiles, QString msg)
{ //! set progressbar with number of files, min. files to be activated, message string

  if (files < minFiles)
    return; //return if progressbar is not needed (not enough files)
  
  #ifdef Q_WS_MAC 
    progressBar->setStyle(new QCleanlooksStyle());
  #endif
  
  progressBar->setGeometry((QRect(QDesktopWidget().screenGeometry()).width()-200)/2, (QRect(QDesktopWidget().screenGeometry()).height()-28)/2, 200, 28);
  progressBar->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
  progressBar->setAlignment(Qt::AlignHCenter);
  progressBar->reset();
  progressBar->setRange(0, files);
  progressBar->setFormat(msg + "  %v / %m");
  progressBar->show();
}


void ImportExport::retranslateUi()
{ //! retranslate Ui

//messages
  msg_progress_fopen = tr("open file");
  msg_progress_batch = tr("batch progress");
  msg_error_pou_line = tr("in line");
}




//------------------------------------------------------------------------------------
// B A T C H   F U N C T I O N + + + + + + + + + + + + + + + + + + + + + + + + + + + +
//------------------------------------------------------------------------------------
void ImportExport::batchConverter()
{ //! batch converting / file resaving function
  /*! The batch dialog returns the full string of the combobox: */
  /*! 'only copy' */
  /*! 'plain POU (.txt)' */
  /*! 'Sucosoft S40 (.poe)' */
  /*! 'CoDeSys (.EXP)' */
  /*! 'STEP 7 (.awl / .scl - keep current language)' */
  /*! 'MULTIPROG (.IL / .ST - keep current language)' */
  /*! 'MELSOFT (.asc)' */
  /*! If Siemens of KW-Software were detected the called saveAllPages(QString batchPath, int batchExt) function will only get batchExt=4 or batchExt=6 to keep the current programming language by self detection. */
  	
  int  batchExt=0, batchConvOption=0;
  QString batchPath, batchExtStr;

  if (m_PageCtrl->countPages() < 1)
    return; //return if no tab exists
    
  BatchDialog *batchDialog = new BatchDialog(0);
  
  batchDialog->presetPath(lastDir);
  if (!batchDialog->exec())
  { delete batchDialog; return; } //delete dialog and return if exec()=0
   
  //set wait cursor
  QApplication::setOverrideCursor(Qt::WaitCursor);
  emit active();
  
  //get path and extension
  batchPath = batchDialog->selectedPath();
  batchExt = batchDialog->selectedCompatibility();
  batchConvOption = batchDialog->selectedConversion();
  
  //set path if not exists
  if (!QDir(batchPath).exists());
    QDir(batchPath).mkpath(batchPath);

  saveAllPages(batchPath, batchExt, batchConvOption); //call save all pages routine
  
  //clean up
  delete batchDialog;					//delete batchDialog 
  
  QApplication::restoreOverrideCursor();//give std. cursor back
  emit ready();							//emit ready signal
}


void ImportExport::saveAllPages(QString batchPath, int batchExt, int batchConvOption)
{ //! save all pages
  //! \param batchPath contains save path, if empty call PageCtrl->filePathOf(i)
  //! \param batchExt contains the selected file extension, if set to 99 use PageCtrl->fileExtOf(i)
  //! \param batchConvOption holds the function block call model conversion option
  /*! supported extension values: */
  /*! - 0 = .txt */
  /*! - 1 = .poe */
  /*! - 2 = .EXP */
  /*! - 3 = .awl */
  /*! - 4 = .scl */
  /*! - 5 = .IL */
  /*! - 6 = .ST */
  /*! - 7 = .asc */
  /*! - 99 = PageCtrl->fileExtOf(i) */
  /*! supported conversion values: */
  /*! option 0: do nothing */
  /*! option 1: all into selected compatibility call model */
  /*! option 2: outputs into simple IEC call model */
  /*! option 3: all into simple IEC call model */
      
  bool error=false;
  int Ext=0, i=0;
  QString path, fileName, ExtStr;
 
  if (m_PageCtrl->countPages() < 1)
    return; //return if no tab exists
    
  QStringList *tmpList = new QStringList();
  
  setProgressBar(m_PageCtrl->countPages(), 1, msg_progress_batch); //set progressbar
  	
  for (i=0; i < m_PageCtrl->countPages(); i++)//for1
  { 
    //inc. progressbar
    progressBar->setValue(i); 
    progressBar->update();
    QApplication::processEvents((QEventLoop::AllEvents));
  	//clear tmpList
	tmpList->clear();
	//get current filename
	fileName = m_PageCtrl->fileNameOf(i);
	
	if ((!m_PageCtrl->currentFilePath().isEmpty()) && (m_PageCtrl->currentTabText() != "untitled"))//if1
	{ //only save file if path is not empty/'untitled'
	  
	  //check if only copying or converting requested (value 99)
	  if (batchExt == 99)
	  {
	  	Ext = m_PageCtrl->fileExtOf(i);
	  }
	  else
	  	Ext = batchExt;

      //force save routine to store TYPEs as CoDeSys .EXP only!
      if (m_PageCtrl->currentFileType().contains("TYPE"))
  	    Ext = 2;
      //force save routine to handle GLOBAL_VARIABEL_LIST and CONFIGURATION
      if ((m_PageCtrl->currentFileType().contains("GLOBAL_VARIABLE_LIST")) || (m_PageCtrl->currentFileType().contains("CONFIGURATION")))
        if (Ext == 3) 
          Ext = 3; //force to store "CONFIGURATION" as MELSOFT .asc if selected
        else 
          Ext = 2; //force sto store GLOBAL_VARIABEL_LISTs as CoDeSys .EXP if not MELSOFT is selected

	  //get EXT string
	  switch ( Ext )
      {
  	    case 0 : { ExtStr = ".txt"; break; }
	  	case 1 : { ExtStr = ".poe"; break; }
	  	case 2 : { ExtStr = ".EXP"; break; }
	  	case 3 : { ExtStr = ".awl"; Ext = 3;
	  	 		   if (m_PageCtrl->prgLngOf(i) == "ST") //used if batch extension was choosen
	  	 		   {ExtStr = ".scl"; Ext = 4;}
	  	 		   break;                  }
	  	case 4 : { ExtStr = ".scl"; break; }			//used if 'only copy' was choosen
	  	case 5 : { ExtStr = ".IL" ; Ext = 5;
	  	 		   if (m_PageCtrl->prgLngOf(i) == "ST")  //used if batch extension was choosen
	  	 		   {ExtStr = ".ST"; Ext = 6;}
	  	 		   break;                  }
	  	case 6 : { ExtStr = ".ST";  break; }			//used if 'only copy' was choosen
	  	case 7 : { ExtStr = ".asc"; break; }
	  	default: { ExtStr = ".txt"; break; }
	  }
	  
	  //call saveprepare fcn. for page 'i'
      SavePrepare(i, fileName, tmpList, Ext, batchConvOption, 0); 
    
      //check batch path if empty (is empty if called by prefrences timer)
      if (batchPath.isEmpty())
        path = m_PageCtrl->filePathOf(i); //use page information and original path
      else
      	path = batchPath;				 //use batchPath
      
      //create file
      QFile filePOU(path + fileName + ExtStr);
      //QFileInfo fileInfo(filePOU);
      //QDateTime created = fileInfo.lastModified();

      if (!filePOU.open(QFile::WriteOnly | QIODevice::Text))//if3
        error = true;
      if (error == false)//if4
      { //create text stream
        QTextStream tsPOU(&filePOU);
        //set text codec
        tsPOU.setCodec(QTextCodec::codecForName("UTF-8"));
        //save tmpList from saveprepare fcn. to file
        tsPOU << tmpList->join("\n");   
        //get last modified date & time from file
        m_PageCtrl->setCurrentFileDate((QFileInfo(filePOU).lastModified()).toString());
      }//if4
    }//if1
  }//for1
  
  progressBar->hide(); //hide progressbar
  
  delete tmpList; 		//delete tmpList	
}




//------------------------------------------------------------------------------------
// O P E N   F U N C T I O N S + + + + + + + + + + + + + + + + + + + + + + + + + + + +
//------------------------------------------------------------------------------------
bool ImportExport::OpenPrepare(QStringList files)
{ //! prepare file opening of string list content and call open reinforce function
	
  QFile		  filePOU;
  bool        okOpenReinforce=false, error=false;
  int         i=0, j=0, pouEndIndex=0, listLength=0;
  QString     filePath="", fileName="", ExtStr="";
  QStringList *tmpList = new QStringList();

  //clear errorFiles list
  errorFiles.clear();
  //set  inhibit create highlighters flag to prevent slow opening, depending on highlighters creation
  m_PageCtrl->flg_inhibit_createHighlighters = true; 
  //set wait cursor
  QApplication::setOverrideCursor(Qt::WaitCursor);
  emit active();
  
  setProgressBar(files.count(), 2, msg_progress_fopen); //set progressbar
  
  //correct path style, used with Windows > XP
  files.replaceInStrings("\\", "/");

  //if files are selected, get each in a new page
  for (i=0; i< files.count(); i++)//for1
  { 
    //inc. progressbar
    progressBar->setValue(i); 
    progressBar->update();
    QApplication::processEvents((QEventLoop::AllEvents));
  	//clear tmplist and reset error
    tmpList->clear();
    error = false; 

    //get stripped file name
    fileName = strippedFileName(files[i]); //strippedName contains already .simplified() 
    //get file extension
    ExtStr = fileName;
    ExtStr = ExtStr.remove(0, ExtStr.lastIndexOf("."));
     
    //check if extension correct
    if (!m_PageCtrl->definedPOUExtList.contains(ExtStr, Qt::CaseInsensitive))
      error = true;
    //set filename with full path information
    filePOU.setFileName(files[i].simplified());
    if (!filePOU.exists())//if1
  	  error = true; //set error flag
    if (!filePOU.open(QIODevice::ReadOnly | QIODevice::Text))//if2
  	  error = true; //set error flag
    
    if (error == false)//if3
    { 
      //get file into stream
  	  QTextStream ts(&filePOU);
      //get file content into StringList
      while (!filePOU.atEnd())
        tmpList->append(filePOU.readLine()); 
        
      //get maybe included files by file type end index
      listLength = tmpList->count();
      for (j=0; j < listLength; j++)//for2
      {
        m_PageCtrl->newPage(); 								//create new page
        m_PageCtrl->setCurrentFileDate((QFileInfo(filePOU).lastModified()).toString()); //get last modified date & time from file
        pouEndIndex = nextPOUEndIndexAt(tmpList, j, ExtStr);//get next pou end index
        
        if (pouEndIndex > -1) //if4
        {
          okOpenReinforce = OpenReinforce(tmpList, j, pouEndIndex, ExtStr);

          if (okOpenReinforce == true)//if5
          { //get current file path
            filePath = files[i]; 
            m_PageCtrl->setCurrentFilePath(filePath.remove(filePath.lastIndexOf("/")+1, filePath.length())); //set file path
            m_PageCtrl->setPageModifiedOf(m_PageCtrl->currentTabIndex(), false); //set page modified to false
          }
          else 
	      { //pou error handling
	        m_PageCtrl->deletePage(); //delete page
	        errorFiles.append(files[i].simplified() + "<br>" + msg_error_pou_line + ": " + QString::number(j+1)); //get file path and error line
	      }//if5
	    }
  	    
  	    //prepare variables for next loop run
  	    listLength = tmpList->count();
  	    j = pouEndIndex+1;
  	    
  	    if (pouEndIndex < 0)//if6
  	    { //end loop if fileEndIndex was returned as fals value
  	      m_PageCtrl->deletePage(); 
  	      break;
  	    }//if6
 	  }//for2
	}
	else
	{ //fill errorFiles list
	  errorFiles.append(files[i].simplified());
	}//if3
  }//for1
  
  progressBar->hide(); //hide progressbar
  
  //reset inhibit create highlighters flag
  m_PageCtrl->flg_inhibit_createHighlighters = false; 
  if (m_PageCtrl->countPages() > 0)//if8
  { //set List and Tab component visible, needed if no page is already active
    m_PageCtrl->createHighlighters(0);		//call int-SLOT create highlighter, for last opened file
    m_PageCtrl->setFileListVisible(m_PageCtrl->flg_fileListVisible);	//set file list visible 
    m_PageCtrl->setTabCtrlVisible(true);	//set tab ctrl. visible
  }//if8

  //check status of use preset dir flag
  if (usePresetDir == false)//if9
  { //save last dir
    lastDir = files[i-1];					//get last Dir, (=i-1)
    lastDir = lastDir.remove(lastDir.lastIndexOf("/")+1, lastDir.length()); 
  }//if9

  emit modified();						 //emit modified information
  
  //clean up
  delete tmpList;						//delete tmpList
  
  QApplication::restoreOverrideCursor();//give std. cursor back
  emit ready();							//emit ready signal
  
  //return handling
  if (errorFiles.count() < 1)//if10
    return(true);
  else
    return(false);
}



bool ImportExport::checkFileContentAndExtension(QStringList *tmpList, int index, int endPos, QString *ExtStr)
{ //! check unexpected content in file according to file extension

  bool Sucosoft_found=false;
  int i=0, cntMULTIPROG=0;
  
  //check file with extension .poe
  if (ExtStr->contains(".poe", Qt::CaseInsensitive))
  {
    for (i=index; i < endPos; i++)
    { //check all lines between index and endPos
      if (tmpList->at(i).contains("S40_VERSION = SucoSoft"))
      { Sucosoft_found = true; break; } //set found flag true if value was found and break loop
    }
    if (!Sucosoft_found) //check status of Sucosoft flag 
    { ExtStr->clear(); ExtStr->append(".txt"); } //change file extension if not Sucosoft file
  }
  
  
  //check file with extension .ST
  if (ExtStr->contains(".ST", Qt::CaseInsensitive))
  {
    for (i=index; i < endPos; i++)
    { //check all lines between index and endPos
      if (tmpList->at(i).contains("@KEY@"))
        cntMULTIPROG++; //increase counter if value found
    }
    if (cntMULTIPROG < 2) //check if counter < 2
    { ExtStr->clear(); ExtStr->append(".txt"); } //change file extension if not MULTIPROG file
  }
 
  //even return true; maybe return value will be used later
  return(true);
}


QString ImportExport::getCoDeSysVISU(QStringList *tmpList, int index, int end)
{ //! return CoDeSys VISU section from a list, by using a start index and an end position
	
  bool visu_found=false, endvisu_found=false;
  int i=0, nextBlock_pos=0, visu_pos=0, endvisu_pos=0;
  QString tmpStr="";

  //get next POU block position
  nextBlock_pos = nextPOUStartIndexAt(tmpList, index, end);
 
  for (i = index; i < nextBlock_pos; i++)//for1
  { //get data block begin position
    if ((tmpList->at(i).contains("VISUALISATION")) && (visu_found == false))//if1
    {  visu_found = true; visu_pos = i; }
    if ((tmpList->at(i).contains("END_VISUALISATION")) && (endvisu_found == false))//if2
    {  endvisu_found = true; endvisu_pos = i; break; }
  }//for1

  if ((!visu_found) || (!endvisu_found))//if3
    return(""); //return if no VISU can be found
  
  for (i = visu_pos; i < endvisu_pos+1; i++)//for2
  { //collect each single line of the VISU
    tmpStr.append(tmpList->at(i));
  }//for2

  //return visu data
  return(tmpStr);
}


QString ImportExport::getMELSOFTConfig(QStringList *tmpList, int index, int end)
{ //! return MELSOFT configuration data from a list, by using a start index and an end position

  int i=0, nextBlock_pos=0, END_RESOURCE_pos=0;
  QString tmpStr="";

  //get next POU block position
  nextBlock_pos = nextPOUStartIndexAt(tmpList, index, end);
 
  for (i = index; i < nextBlock_pos; i++)//for1
  { //configuration data collector loop
    if (tmpList->at(i).contains("END_RESOURCE"))//if1
      break; 						//break loop if END_RESOURCE is detected
    else
      tmpStr.append(tmpList->at(i));//otherwise get all config lines
  }//for1

  return(tmpStr);
}


QString ImportExport::getSTEP7DBData(QStringList *tmpList, int index, int end)
{ //! return Siemens DB data from a list, by using a start index and an end position
	
  bool db_found=false;
  int i=0, nextBlock_pos=0, DB_pos=0;
  QString tmpStr="";

  //get next POU block position
  nextBlock_pos = nextPOUStartIndexAt(tmpList, index, end);

  for (i = index; i < nextBlock_pos; i++)//for1
  { //get data block begin position
    if (tmpList->at(i).contains("DATA_BLOCK"))//if1
    {  db_found = true; DB_pos = i; break;  }
  }//for1

  if (!db_found)//if2
    return(""); //return if no "DATA_BLOCK" wasfound
  
  for (i = DB_pos; i < nextBlock_pos; i++)//for2
  { //collect each single line of the DB data
    tmpStr.append(tmpList->at(i));
  }//for2

  //return DB data
  return(tmpStr);
}


int ImportExport::nextPOUEndIndexAt(QStringList *tmpList, int index, QString ExtStr)
{ //! return next POU end index of tmpList up from index
	
  bool		end_found=false;
  int       i=0;
  QString   tmpStr="";
  
  //get pos information and lines content
  if (tmpList->count() < 1)//if1
    return(-1);
   
  for (i = index; i < tmpList->count(); i++)//for1
  { //run through tmpList
    tmpStr = tmpList->at(i);//get single line
    tmpStr.simplified();
    //get FileEnd position
    if (tmpStr.contains("END_PROGRAM") || tmpStr.contains("END_ORGANIZATION_BLOCK") ||
        tmpStr.contains("END_FUNCTION_BLOCK") || tmpStr.contains("END_FUNCTION") ||
        tmpStr.contains("@OBJECT_END") || tmpStr.contains("END_CONFIGURATION") ||
        (tmpStr.contains("END_TYPE") && (ExtStr.contains("EXP", Qt::CaseInsensitive))) )//if2
    { end_found = true; break; }//if2
  }//for1

  //return
  if (end_found == true)//if6
    return(i);
  else
    return(-1);
}


int ImportExport::nextPOUStartIndexAt(QStringList *tmpList, int index, int end)
{ //! return next POU start index of tmpList up from index

  bool nextBlock_found=false;
  int i=0, nextBlock_pos=0;
  QString tmpStr="";
  
  for (i = index; i < end; i++)//for1
  { //get next POU block position
    tmpStr = tmpList->at(i);//get single line
    if (tmpStr.contains("PROGRAM") || tmpStr.contains("ORGANIZATION_BLOCK") || 
    	tmpStr.contains("FUNCTION_BLOCK") || tmpStr.contains("FUNCTION") || 
    	tmpStr.contains("GLOBAL_VARIABLE_LIST")  || tmpStr.contains("TYPE") || 
    	tmpStr.contains("CONFIGURATION"))//if3
    { nextBlock_found = true; nextBlock_pos = i; break; }//if1
  }//for1

  if (!nextBlock_found)//if2
    nextBlock_pos = end;

  return(nextBlock_pos);
}


bool ImportExport::OpenReinforce(QStringList *tmpList, int index, int endPos, QString ExtStr)
{ //! reinforce import of file content in tmpList up from index until endPos, respecting the ExtStr value
  //! \param *tmpList temporary list which will be filled with the file content
  //! \param index is the index of the next POU start poisition
  //! \param endPos is the position of the POU end mark
  //! \param ExtStr contains the file extension as STRING
  /*! supported extension values: */
  /*! - '.txt' */
  /*! - '.poe' */
  /*! - '.EXP' */
  /*! - '.awl' */
  /*! - '.scl' */
  /*! - '.IL' */
  /*! - '.ST' */
  /*! - '.asc' */
  
  bool      toIntDone=true, foundFT=false, flg_STsource=false, flg_ext13=false, contentOk=false;
  int       i=0, k=0, Ext=0, startPos=0, EndOfFile=0;
  int       FileType_pos=100000, FileEnd_pos=0, END_VAR_pos=0;
  int       STassignementfound=0;
  QString   tmpStr="", fileName="", headerStr="", worksheetStr="", visuStr="";
  QString   FilePath="", FileType="", FncType="", FNgetter="", FileName="", FileEnd="";

  //check unexpected content in file according to file extension
  contentOk = checkFileContentAndExtension(tmpList, index, endPos, &ExtStr);
  if (!contentOk) return(false);
    

  //get Ext as int for later case construct
  if (ExtStr.contains(".txt", Qt::CaseInsensitive))
    Ext = 0; //get Extension as INT for case construct
  if (ExtStr.contains(".poe", Qt::CaseInsensitive))
    Ext = 1;
  if (ExtStr.contains(".exp", Qt::CaseInsensitive))
    Ext = 2;
  if (ExtStr.contains(".awl", Qt::CaseInsensitive))
    Ext = 3;
  if (ExtStr.contains(".scl", Qt::CaseInsensitive))
    Ext = 4;
  if (ExtStr.contains(".IL", Qt::CaseInsensitive))
    Ext = 5;
  if (ExtStr.contains(".ST", Qt::CaseInsensitive))
    Ext = 6;
  if (ExtStr.contains(".asc", Qt::CaseInsensitive))
    Ext = 7;


  //get pos information and lines content
  if (tmpList->count() < 1)//if1
    return(false);
    
  //get corrected start position, source is tmplist
  switch ( Ext )
  {
  	case 1  : { //Sucosoft POU  (.poe)
  	            startPos = tmpList->indexOf("$END_KM_HEADER\n", index);  break; 
              } //case 1
  	case 5  : 
  	case 6  : { //MULTIPROG IL/ST  (.IL/.ST)
                startPos = tmpList->indexOf("(*@KEY@:END_DESCRIPTION*)\n", index);  break; 
  	          } //case 5, 6
  	default : { //empty
                startPos = index; break;
              } //default
  }//switch



  for (i = startPos; i < endPos; i++)//for1
  { //run through tmpList
    tmpStr = tmpList->at(i);//get single line

    //file type / function type / file name ---------------------------------------
    if ((tmpStr.contains("PROGRAM") || tmpStr.contains("ORGANIZATION_BLOCK") ||
         tmpStr.contains("FUNCTION_BLOCK") || tmpStr.contains("FUNCTION") ||
         tmpStr.contains("GLOBAL_VARIABLE_LIST") || (tmpStr.contains("TYPE") && (Ext == 2)) ||
         tmpStr.contains("CONFIGURATION")) && (foundFT == false))//if2
    {
      //FileType start pos.
      FileType_pos = i;															//get file type position
      FileType = tmpStr;
      if (FileType.startsWith(" ")) //check if any blank can be found at the start
        FileType = FileType.remove(FileType.indexOf(" "), FileType.indexOf(QRegExp("[A-z]"))); //remove all blanks until a first char
      FileType = (FileType.remove(FileType.indexOf(" "), FileType.length())); 	//get File Type

      //FileType end pos. 
      FileEnd_pos = endPos;			//set end position
      FileEnd = tmpList->at(endPos);//get File End
      
      //file name handling
      fileName = tmpStr.simplified();											//get file name
      fileName = (fileName.remove(0, fileName.indexOf(" ")+1)); 				//cut file type
            
      if (tmpStr.contains("GLOBAL_VARIABLE_LIST"))//if3
      { //VAR List handling
        fileName = fileName.remove(0, fileName.indexOf("'")+1);
        fileName = fileName.remove(fileName.indexOf("'"), fileName.length());
        FileType_pos = FileType_pos+2;
        FileType = "GLOBAL_VARIABLE_LIST";
        FileEnd = ("OBJECT_END");
      }//if3
      
      if (Ext == 7)
        FileType_pos = FileType_pos+2;
      
      if (fileName.contains(":"))
      { //get Function, marked by ':'
        FncType = fileName;
        FncType = FncType.remove(0, FncType.indexOf(":"));							//get function mark
        fileName = fileName.remove(fileName.indexOf(" "), fileName.length());	//cut function mark
      }	
      
      fileName = fileName + ExtStr; //concat filenam + extension 
      foundFT = true;       		//found flag

    }//if2
      
    //END_VAR pos. ---------------------------------------
    if (tmpStr.contains("END_VAR") || (tmpStr.contains("END_STRUCT") && (Ext != 1)) || tmpStr.contains("(* @END_DECLARATION := '0' *)"))//if5
      END_VAR_pos = i;     //count END_VAR, END_STRUCT (but not in Sucosoft files) and @END_DECLARATION items
  }//for1

  //return with false if no END or file type could be found
  if((END_VAR_pos == 0) || (FileType.isEmpty()))//if2 
    return(false);



  //T E 1   S E C T I O N  ---------------------------------------
  tmpStr.clear(); //clear string
  //fill declaration section = = = = = = = = = = = =
  for (i=FileType_pos+1; i < END_VAR_pos; i++)//for1
    tmpStr.append(tmpList->at(i));
  if (!tmpList->at(END_VAR_pos).contains("(* @END_DECLARATION := '0' *)")) //check if no @END_DECL... exists
    tmpStr.append(tmpList->at(END_VAR_pos).simplified());//get last line simplified to prevent additional '\n'
  m_PageCtrl->fillCurrentDeclaration(tmpStr);

  tmpStr.clear(); //clear string


  //T E 2   S E C T I O N  ---------------------------------------
  EndOfFile = endPos; //set EOF position

  //EXTENSION Handling, source is tmplist
  switch ( Ext )
  {
  	case 0  : { //plain POE  (.txt)
  	            break;                                    //leave case contruct
              } //case 0

  	case 1  : { //Sucosoft POE  (.poe)
  	            QFontMetrics fm(m_PageCtrl->font);
  				m_PageCtrl->setCurrentTabStops(8*fm.width(" "));      //set TabStop width of Dec & Ins
  				
  				headerStr.clear();                          //clear MHC
        	    for (i = index; i < FileType_pos-1; i++ )   //get KM_Header in MHC
        	      headerStr.append(tmpList->value(i));      //get Strings
        	    headerStr.append("$END_KM_HEADER");         //set KMH end string (needed becc. \n would copied with org. end string)	
        	    m_PageCtrl->setCurrentSucosoftHeader(headerStr);
        	    if (headerStr.contains("kAwl", Qt::CaseInsensitive))
        	      m_PageCtrl->setCurrentPrgLng("IL"); 		//set programming language
        	    else
        	      m_PageCtrl->setCurrentPrgLng("ST"); 		//set programming language
            	//handle Splitter Size
            	int splitSize = (tmpList->value(13).remove(0, 15)).toInt(&toIntDone, 10); //get splitSize from File
            	if (splitSize < 100)  //if size to small, set size by default value
            	  m_PageCtrl->splitSize = 200;
            	m_PageCtrl->setCurrentSplitSize(splitSize); 
            	if (tmpList->value(11).contains("kVer"))
            	  m_PageCtrl->setCurrentSplitMode("v");  	//set Splitter orientation
            	else 
            	  m_PageCtrl->setCurrentSplitMode("h");  	//set Splitter orientation
              	break;                                    	//leave case contruct
              } //case 1

  	case 2  : { //CoDeSys EXP  (.EXP)
  				QFontMetrics fm(m_PageCtrl->font);
  				m_PageCtrl->setCurrentTabStops(4*fm.width(" "));      //set TabStop width of Dec & Ins
				visuStr = getCoDeSysVISU(tmpList, endPos+1, tmpList->count());
    		    break;
              } //case 2

  	case 3  : { //STEP 7 AWL  (.AWL)
    	        m_PageCtrl->setCurrentPrgLng("IL");
                m_PageCtrl->setCurrentSTEP7DBData(getSTEP7DBData(tmpList, endPos+1, tmpList->count())); //get DB data
                break;
              } //case 3

  	case 4  : { //STEP 7 SCL  (.SCL)
                m_PageCtrl->setCurrentPrgLng("ST");
                m_PageCtrl->setCurrentSTEP7DBData(getSTEP7DBData(tmpList, endPos+1, tmpList->count())); //get DB data
                break;
              } //case 4

  	case 5  : { //MULTIPROG IL  (.IL)
                m_PageCtrl->setCurrentPrgLng("IL");
                
                headerStr.clear();                           //clear header string
        	    for (i = tmpList->indexOf("(*@KEY@:DESCRIPTION*)\n", index)+1; i < tmpList->indexOf("(*@KEY@:END_DESCRIPTION*)\n", index); i++ )     	//get KM_Header in MHC
        	      headerStr.append(tmpList->value(i));    	//get Strings
        	    m_PageCtrl->setCurrentMULTIPROGDescription(headerStr.trimmed());
        	    
                k = tmpList->indexOf("(*@KEY@: WORKSHEET\n", END_VAR_pos)+1;
                worksheetStr = tmpList->at(k);
                worksheetStr.remove(0, worksheetStr.indexOf(" ")+1);
                m_PageCtrl->setCurrentMULTIPROGWorkSheetName(worksheetStr.simplified());
	  		    END_VAR_pos = k+2;
	  		    EndOfFile = EndOfFile-1;
                break;
              } //case 5
              
    case 6  : { //MULTIPROG ST  (.ST)
                m_PageCtrl->setCurrentPrgLng("ST");
                
                headerStr.clear();                           //clear header string
        	    for (i = tmpList->indexOf("(*@KEY@:DESCRIPTION*)\n", index)+1; i < tmpList->indexOf("(*@KEY@:END_DESCRIPTION*)\n", index); i++ )     	//get KM_Header in MHC
        	      headerStr.append(tmpList->value(i));    	//get Strings
        	    m_PageCtrl->setCurrentMULTIPROGDescription(headerStr.trimmed());
        	    
                k = tmpList->indexOf("(*@KEY@: WORKSHEET\n", END_VAR_pos)+1;
                worksheetStr = tmpList->at(k);
                worksheetStr.remove(0, worksheetStr.indexOf(" ")+1);
                m_PageCtrl->setCurrentMULTIPROGWorkSheetName(worksheetStr.simplified());
	  		    END_VAR_pos = k+2;
	  		    EndOfFile = EndOfFile-1;
                break;
              } //case 6

  	case 7  : { //MELSOFT  (.asc)
	  		    tmpStr.append(tmpList->at(END_VAR_pos+1));
	  		    tmpStr.remove("'");
    	        m_PageCtrl->setCurrentPrgLng(tmpStr.simplified());
	  		    tmpStr.clear(); //clear string
	  		    
	  		    m_PageCtrl->setCurrentMELSOFTConfig(getMELSOFTConfig(tmpList, END_VAR_pos, tmpList->count()));
	  		    
	  		    END_VAR_pos = END_VAR_pos+2;
	  		    //check if language is IL
	  		    if (m_PageCtrl->currentPrgLng() == "IL")
	  		      for (i=END_VAR_pos; i < EndOfFile-1; i++)//for1
  				  { //get line at pos. i until i<EOF-1
    				tmpStr = tmpList->at(i);
    				if (tmpStr.contains("END_WORKSPACE"))
	  		        { END_VAR_pos = END_VAR_pos+4; break; }
  		    	  }
  		    	tmpStr.clear(); //clear string  
	  		    EndOfFile = EndOfFile-1;
                break;
              }

  	default : { //empty
                break;
              } //default
  }//switch


  //fill intruction section = = = = = = = = = = = =
  for (i=END_VAR_pos+1; i < EndOfFile-1; i++)//for2
  { //get line at pos. i until i<EOF-1
    tmpStr.append(tmpList->at(i));
    if (usePrgLngDetection == true)
    { //try to get PRGLNG independent from any POU extension
      if (tmpList->at(i).contains(":=") && tmpList->at(i).contains(";"))
        if ((tmpList->at(i).indexOf("(*") < 0) || ((tmpList->at(i).indexOf("(*") > -1) && (tmpList->at(i).indexOf(":=") < tmpList->at(i).indexOf("(*"))))
          STassignementfound++;
    }
  }//for2
  tmpStr.append(tmpList->at(EndOfFile-1).simplified());//get last line (at i=EOF-1) simplified to prevent additional '\n'
  
  //set programming language for current page if noPrgLngDetection is disabled
  if (usePrgLngDetection == true)
  { //m_PageCtrl->newPage() sets the predefined programming language as standard, so if 
  	//usePrgLngDetection is false, there is no need to run m_PageCtrl->setCurrentPrgLng()
  	//get some flags
    flg_STsource = (STassignementfound > 0);
    flg_ext13 = ((Ext == 0) || (Ext == 2));
    //revise the programming language information
    if ((flg_STsource == true) && (flg_ext13 == true ))
      m_PageCtrl->setCurrentPrgLng("ST"); 					//set if detected and extension is .txt or .exp
    if ((flg_STsource == false) && (flg_ext13 == true ))
      m_PageCtrl->setCurrentPrgLng("IL"); 					//set if detected and extension is .txt or .exp
  }

  //add visu string if any CoDeSys source
  if (Ext == 2)
    tmpStr.append("\n" + visuStr);

  //copy Str to TextEdit 2/Instruction Section & clear Str
  m_PageCtrl->fillCurrentInstruction(tmpStr);   
  tmpStr.clear();  
    
  //get file informations into page components
  m_PageCtrl->setCurrentFileEnd(FileEnd);	          	//End of File
  m_PageCtrl->setCurrentFileExt(QString::number(Ext));	//Extension of File
  m_PageCtrl->setCurrentFileListItemText(fileName);   	//add list item
  m_PageCtrl->setCurrentFileType(FileType);	          	//Type of File
  m_PageCtrl->setCurrentFncType(FncType);				//Type of Function
  m_PageCtrl->setCurrentFileName(fileName); 			//File Name
    
  //CLOSING OPERATIONS
  //delete handling for VAR Lists
  if (FileType.contains("GLOBAL_VARIABLE_LIST") || FileType.contains("TYPE") || FileType.contains("CONFIGURATION"))//
	m_PageCtrl->delVARTypeItems();   //delete TE2, becc. it will not used
    
  //set Splitter visible
  m_PageCtrl->setNewSplitterVisible();
  //return with 'ok" indication
  return(true);
}




//------------------------------------------------------------------------------------
// S A V E  F U N C T I O N S  + + + + + + + + + + + + + + + + + + + + + + + + + + + +
//------------------------------------------------------------------------------------
int ImportExport::savePage(QString Str)
{ //! save page content into path which is setted in Str and call saveprepare function
	
  bool        error = 0;
  int         Ext=0;
  QString     fileName = strippedFileName(Str);;

  if (Str.isEmpty())
  	return(1); //return if string is empty

  m_PageCtrl->setPageCtrlSignalsBlocked(true);  //block signals of class PageCtrl

  QStringList *tmpList = new QStringList();

  //set wait cursor
  QApplication::setOverrideCursor(Qt::WaitCursor);
  emit active();

  //get file extension
  if (Str.contains(".txt", Qt::CaseInsensitive))
    Ext = 0; //get Extension as INT for later case construct
  if (Str.contains(".poe", Qt::CaseInsensitive))
    Ext = 1;
  if (Str.contains(".exp", Qt::CaseInsensitive))
    Ext = 2;
  if (Str.contains(".awl", Qt::CaseInsensitive))
    Ext = 3;
  if (Str.contains(".scl", Qt::CaseInsensitive))
    Ext = 4;  
  if (Str.contains(".IL", Qt::CaseInsensitive))
    Ext = 5;
  if (Str.contains(".ST", Qt::CaseInsensitive))
    Ext = 6;
  if (Str.contains(".asc", Qt::CaseInsensitive))
    Ext = 7;


  //add List item with name
  m_PageCtrl->setCurrentFileListItemText(fileName);
  //set File Name
  m_PageCtrl->setCurrentFileName(fileName);


  //execute saveprepare fcn.
  SavePrepare(m_PageCtrl->currentTabIndex(), fileName, tmpList, Ext, conversionOption, 1);


  // create POU file
  QFile filePOU(Str);
  if (filePOU.open(QFile::WriteOnly | QIODevice::Text))
  { //create text stream
    QTextStream tsPOU(&filePOU);
    //set text codec
    tsPOU.setCodec(QTextCodec::codecForName("UTF-8"));
    //save tmpList from saveprepare fcn. to file
    tsPOU << tmpList->join("\n");
    //get last modified date & time from file
    m_PageCtrl->setCurrentFileDate((QFileInfo(filePOU).lastModified()).toString());
 
    //diverse page settings
    m_PageCtrl->setCurrentFileExt(QString::number(Ext));//Extension of FileQString::number(Ext));//Extension of File
    QString filePath = Str; //get full file name
    m_PageCtrl->setCurrentFilePath(filePath.remove(filePath.lastIndexOf("/")+1, filePath.length())); //set file path
  }
  else error = 1;

  
  if ((savePTT == true) && (m_PageCtrl->currentFileType() != "GLOBAL_VARIABLE_LIST") && (m_PageCtrl->currentFileType() != "TYPE") && (m_PageCtrl->currentFileType() != "CONFIGURATION"))
  { //change filename
  	Str = Str.remove(Str.lastIndexOf("."), Str.length()) + ".ptt";
  	//get PTT content
  	tmpList->clear();
    m_Prototype->currentPrototype(tmpList);		//get curent prototype
    // create file
    QFile filePTT(Str);
    if (filePTT.open(QFile::WriteOnly | QIODevice::Text))
    { //create text stream
      QTextStream tsPTT(&filePTT);
      //set text codec
      tsPTT.setCodec(QTextCodec::codecForName("UTF-8"));
      //save tmpList from saveprepare fcn. to file
      tsPTT << tmpList->join("\n");
    }
    else error = 2;
  }
  

  //save last dir
  if (usePresetDir == false)
    lastDir = Str.remove(Str.lastIndexOf("/")+1, Str.length()); 

  
  //clean up
  delete tmpList;					//delete tmpList
  
  m_PageCtrl->setPageCtrlSignalsBlocked(false);         //unblock signals of class PageCtrl, pos. changed in r113

  emit modified();  					//emit modiefied information

  m_PageCtrl->setPageModifiedOf(m_PageCtrl->currentTabIndex(), false); //set page modified to false
  /*commented out because it was causing errors...
  //work around, otherwise file extension will not shown after storage
  //add List item with name
  m_PageCtrl->setCurrentFileListItemText(fileName);
  //set File Name
  m_PageCtrl->setCurrentFileName(fileName); 
  */
  QApplication::restoreOverrideCursor();//give std. cursor back
  emit ready();							//emit ready signal
  
  return(error);						//return
}


void ImportExport::SavePrepare(int pageIndex, QString fn, QStringList *tmpList, int Ext, int convOption, bool sendConvBack)
{ //! prepare file storage for content at pageIndex into tmpList, with specified by filename and extension from fn and Ext
  //! \param pageIndex is the index of the selected page
  //! \param fn contains the file name
  //! \param *tmpList temporary list which will be filled with the file content
  //! \param Ext contains the file extension as INT
  //! \param convOption contains the conversion option as INT
  //! \param sendConvBack is used to revise the instruction section with all function block conversions
  /*! supported extension values: */
  /*! - 0 = .txt */
  /*! - 1 = .poe */
  /*! - 2 = .EXP */
  /*! - 3 = .awl */
  /*! - 4 = .scl */
  /*! - 5 = .IL */
  /*! - 6 = .ST */
  /*! - 7 = .asc */
  /*! supported conversion values: */
  /*! option 0: do nothing */
  /*! option 1: all into selected compatibility call model */
  /*! option 2: outputs into simple IEC call model */
  /*! option 3: all into simple IEC call model */


  
  int       i=0;
  QString   FileType, FncType, FileName, FileEnd, TimeString; //File name & TimeStamp getter
  QString   tmpStr, CoDeSysV2_tag1="", CoDeSysV2_tag2="", S7_tag1="", S7_tag2="", 
            KW_tag1="", KW_tag2="", KW_tag3="", 
            MELSOFT_tag1="", MELSOFT_tag2="", MELSOFT_tag3="", MELSOFT_tag4="", MELSOFT_tag5="";

  //prepare file information from Page
  FileType = m_PageCtrl->fileTypeOf(pageIndex);	//Type of File
  FncType  = m_PageCtrl->fncTypeOf(pageIndex);	//Type of Function
  FileEnd  = m_PageCtrl->fileEndOf(pageIndex);	//End of File
  
  //fileName String
  if (fn.contains("."))
    FileName = fn.remove(fn.lastIndexOf("."), fn.length()); //extraction for save in file
  else 
  	FileName = fn;
	
  //EXT selection
  switch ( Ext )
  {
  	case 0  : { //plain POU (.txt)
                if ((FileType == "ORGANIZATION_BLOCK") || (FileEnd == "END_ORGANIZATION_BLOCK"))
                { FileType = "PROGRAM"; FileEnd = "END_PROGRAM"; }
  	            
  	            break;                                    	//leave case contruct
              } //case 0
              
  	case 1  : { //Sucosoft POE (.poe)
                if ((FileType == "ORGANIZATION_BLOCK") || (FileEnd == "END_ORGANIZATION_BLOCK"))
                { FileType = "PROGRAM"; FileEnd = "END_PROGRAM"; }
                	
  	            tmpList->append(createSucosoftHeader(m_PageCtrl->SucosoftHeaderOf(pageIndex))); //copy Moeller header to stringlist
                
                break;                                    	//leave case contruct
              } //case 1

  	case 2  : { //CoDeSys EXP (.EXP)
  	            //normal handling for OBs
                if ((FileType == "ORGANIZATION_BLOCK") || (FileEnd == "END_ORGANIZATION_BLOCK"))
                { FileType = "PROGRAM"; FileEnd = "END_PROGRAM"; }
                if ((FileType == "CONFIGURATION") || (FileEnd == "END_CONFIGURATION"))
                { FileType = "GLOBAL_VARIABLE_LIST"; FileEnd = "OBJECT_END"; }

                //set first line as empty in stringlist
                tmpList->append(" ");

                //handling for VAR LIST files
                if (FileType == "GLOBAL_VARIABLE_LIST")
                  tmpList->append("(* @GLOBAL_VARIABLE_LIST := '" + FileName+ "' *)");
                else
                //handling for P, FB, F files
                tmpList->append(" ");

                //get path info
                tmpList->append("(* @PATH := '' *)");

                //get symfileflags
                if (FileType == "PROGRAM" || FileType == "GLOBAL_VARIABLE_LIST")
                  tmpList->append("(* @SYMFILEFLAGS := '2048' *)");
                else
                  if (FileType != "TYPE")
  	                tmpList->append("(* @SYMFILEFLAGS := '0' *)");
  	              
  	            CoDeSysV2_tag1 = "(* @END_DECLARATION := '0' *)";
  	            CoDeSysV2_tag2.append("(* @OBJECT_END := '" + FileName + "' *)\n");
                CoDeSysV2_tag2.append("(* @CONNECTIONS := " + FileName + "\n");
                CoDeSysV2_tag2.append("FILENAME : ''\n");
                CoDeSysV2_tag2.append("FILETIME : 0\n");
                CoDeSysV2_tag2.append("EXPORT : 0\n");
                CoDeSysV2_tag2.append("NUMOFCONNECTIONS : 0\n");
                CoDeSysV2_tag2.append("*)");
  	            
  	            break;                                    	//leave case contruct
              } //case 2

  	case 3  : { //STEP 7 AWL (.AWL)
                if ((FileType == "PROGRAM") || (FileEnd == "END_PROGRAM"))
                { FileType = "ORGANIZATION_BLOCK"; FileEnd = "END_ORGANIZATION_BLOCK"; }
                
                S7_tag1 = "TITLE =\n//Block comment\nAUTHOR : \nFAMILY : \nNAME : \nVERSION : 1.0";
                S7_tag2 = "BEGIN";
  	            
  	            break;                                    	//leave case contruct
              } //case 3

  	case 4  : { //STEP 7 SCL (.SCL)
                if ((FileType == "PROGRAM") || (FileEnd == "END_PROGRAM"))
                { FileType = "ORGANIZATION_BLOCK"; FileEnd = "END_ORGANIZATION_BLOCK"; }
                
                S7_tag1 = "TITLE =\n//Block comment\nAUTHOR : \nFAMILY : \nNAME : \nVERSION : 1.0";
                S7_tag2 = "BEGIN";
                
  	            break;                                    	//leave case contruct
              } //case 4

  	case 5 : { //MULTIPROG IL (.IL)
                if ((FileType == "ORGANIZATION_BLOCK") || (FileEnd == "END_ORGANIZATION_BLOCK"))
                { FileType = "PROGRAM"; FileEnd = "END_PROGRAM"; }
                  
                tmpList->append("(*@PROPERTIES_EX@");
				tmpStr = m_PageCtrl->MULTIPROGPropertiesOf(pageIndex);
				tmpStr.replace(QString("IEC_LANGUAGE: ST"), QString("IEC_LANGUAGE: IL"));
				m_PageCtrl->setMULTIPROGPropertiesOf(pageIndex, tmpStr);
                tmpList->append(m_PageCtrl->MULTIPROGPropertiesOf(pageIndex));
                tmpList->append("*)");
                tmpList->append("(*@KEY@:DESCRIPTION*)");
                tmpList->append(m_PageCtrl->MULTIPROGDescriptionOf(pageIndex));
                tmpList->append("(*@KEY@:END_DESCRIPTION*)");
                
                //KW_tag1 = "\n(*Group:Default*)\n\n";
                KW_tag2 = ("\n\n(*@KEY@: WORKSHEET\nNAME: " + FileName + "\nIEC_LANGUAGE: IL\n*)");
                KW_tag3 = "(*@KEY@: END_WORKSHEET *)";
                
  	            break;                                    	//leave case contruct
              } //case 5

  	case 6  : { //MULTIPROG ST (.ST)
                if ((FileType == "ORGANIZATION_BLOCK") || (FileEnd == "END_ORGANIZATION_BLOCK"))
                { FileType = "PROGRAM"; FileEnd = "END_PROGRAM"; }
                  
                tmpList->append("(*@PROPERTIES_EX@");
				tmpStr = m_PageCtrl->MULTIPROGPropertiesOf(pageIndex);
				tmpStr.replace(QString("IEC_LANGUAGE: IL"), QString("IEC_LANGUAGE: ST"));
				m_PageCtrl->setMULTIPROGPropertiesOf(pageIndex, tmpStr);
                tmpList->append(m_PageCtrl->MULTIPROGPropertiesOf(pageIndex));
                tmpList->append("*)");
                //tmpList->append("(*@PROPERTIES_EX@\nTYPE: POU\nLOCALE: 0\nIEC_LANGUAGE: ST\nPLC_TYPE: independent\nPROC_TYPE: independent\n*)");
                tmpList->append("(*@KEY@:DESCRIPTION*)");
                tmpList->append(m_PageCtrl->MULTIPROGDescriptionOf(pageIndex));
                tmpList->append("(*@KEY@:END_DESCRIPTION*)");
                
                //KW_tag1 = "\n(*Group:Default*)\n\n";
                KW_tag2 = ("\n\n(*@KEY@: WORKSHEET\nNAME: " + FileName + "\nIEC_LANGUAGE: ST\n*)");
                KW_tag3 = "(*@KEY@: END_WORKSHEET *)";
                
  	            break;                                    	//leave case contruct
              } //case 6

  	case 7  : { //MELSOFT asc (.asc)
                if ((FileType == "ORGANIZATION_BLOCK") || (FileEnd == "END_ORGANIZATION_BLOCK"))
                { FileType = "PROGRAM"; FileEnd = "END_PROGRAM"; }
                if ((FileType == "GLOBAL_VARIABLE_LIST") || (FileEnd == "OBJECT_END"))
                { FileType = "CONFIGURATION"; FileEnd = "END_CONFIGURATION"; }
                	
                if (FileType == "CONFIGURATION")
                { FileName = "scConfiguration"; FncType=""; }
                
                tmpList->append("(*SOFTCONTROL:\n  VERSION:7.03.00*)");
                if ((FileType != "PROGRAM") && (FileType != "CONFIGURATION"))
                  tmpList->append("(*$FLAG12*)\n(*$FLAG14*)");
  	            MELSOFT_tag1 = ("'" + (m_PageCtrl->prgLngOf(pageIndex)) + "'\nBODY");
  	            MELSOFT_tag2 = ("    WORKSPACE\n        NETWORK_LIST_TYPE := NWTYPEIL ;\n        ACTIVE_NETWORK := 1 ;\n    END_WORKSPACE");
  	            MELSOFT_tag3 = "END_BODY";
  	            MELSOFT_tag4 = "    RESOURCE scResource ON scResourceType";
  	            MELSOFT_tag5 = "    END_RESOURCE";
  	            break;                                    	//leave case contruct
              } //case 7
              
    default : { //empty
                if ((FileType == "ORGANIZATION_BLOCK") || (FileEnd == "END_ORGANIZATION_BLOCK"))
                { FileType = "PROGRAM"; FileEnd = "END_PROGRAM"; }
  	            
  	            break;
              } //default
  }//switch


  //different terms for manufacturer compliances
  //get Type + Name line in strinlist
  if ((FileType != "GLOBAL_VARIABLE_LIST"))
    tmpList->append(FileType + " " + FileName + " " + FncType);
    
  if ((Ext == 7) && (FileType != "CONFIGURATION"))
    tmpList->append("(**)\n(**)");
  
  if ((Ext == 7) && (FileType == "CONFIGURATION"))
    tmpList->append(MELSOFT_tag4);
  
  //get spec. lines for KW-Software MULTIPROG , not needed
  //if ((Ext == 5) || (Ext == 6))
  //  tmpList->append(KW_tag1);  


  //g e t   T E 1   i n   s t r i n g l i s t - - - - - - - - - - - - - - - - - - - - 
  tmpList->append(m_PageCtrl->declarationOf(pageIndex).split("\n", QString::KeepEmptyParts));
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 


  //insert tags for CoDeSys between TE1 & TE2  
  if ((Ext == 2) && (FileType != "GLOBAL_VARIABLE_LIST") && (FileType != "TYPE"))
    tmpList->append(CoDeSysV2_tag1);
    
  //insert tag1 for Siemens STEP 7 between TE1 & TE2 if no 'BEGIN' can be found
  if ((FileType != "GLOBAL_VARIABLE_LIST") && (FileType != "TYPE"))
    if (((Ext == 3) || (Ext == 4)) && (!m_PageCtrl->instructionOf(pageIndex).contains("BEGIN",Qt::CaseSensitive)))
      tmpList->append(S7_tag2);
  
  //insert tags for KW-Software MULTIPROG between TE1 & TE2
  if ((Ext == 5) || (Ext == 6))
    tmpList->append(KW_tag2);

  //insert tags for Mitsubishi MELSOFT between TE1 & TE2
  if ((Ext == 7) && (FileType != "CONFIGURATION"))
    tmpList->append(MELSOFT_tag1);
  if ((Ext == 7) && (m_PageCtrl->prgLngOf(pageIndex) == "IL") && (FileType != "CONFIGURATION"))
    tmpList->append(MELSOFT_tag2);
    
  //remove comments if file type is MELSOFT CONFIGURATION
  if ((Ext == 7) && (FileType == "CONFIGURATION"))
    for (i = tmpList->indexOf("CONFIGURATION scConfiguration\n")+2; i < tmpList->size(); i++)
    { //remove maybe existing comments from line of ressource informtion until 'VAR_GLOBAL' mark
       if (tmpList->at(tmpList->indexOf("CONFIGURATION scConfiguration\n")+2).contains("VAR_GLOBAL"))
         break;
       else
         tmpList->removeAt(tmpList->indexOf("CONFIGURATION scConfiguration\n")+2);
    }
    

  //g e t   T E 2   i n   s t r i n g   i f   f i l e   n o t   V A R L I S T - - - -
  tmpStr.clear();
  if ((FileType != "GLOBAL_VARIABLE_LIST") && (FileType != "TYPE") && (FileType != "CONFIGURATION"))
    tmpStr = m_PageCtrl->instructionOf(pageIndex);
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  
  
  //run converison of function blocks
  if (convOption > 0) //check if option > 0 ('0' means do nothing)
  {
    FBCallConverter *fbCallConverter = new FBCallConverter(m_PageCtrl);
    fbCallConverter->convert(&tmpStr, pageIndex, m_PageCtrl->fileExtOf(pageIndex), Ext, convOption);
    delete fbCallConverter;
    if (sendConvBack) //revise instruction section if send conversion back flag is set
      m_PageCtrl->fillCurrentInstruction(tmpStr);
      //also possible, but buggy if Ctr+Z is used
      //m_PageCtrl->selectAll();					//select all at instruction section
      //m_PageCtrl->cut();						//cut all at instruction section
      //m_PageCtrl->insertText("Ins", tmpStr);	//insert string into instruction section
  }

  
  //get FileEnd for 3S CoDeSys source
  if ((Ext == 2) && (FileType != "GLOBAL_VARIABLE_LIST") && (FileType != "TYPE"))
  { //handling to place file end correct if the file contains VISU information
    if (tmpStr.contains("VISUALISATION"))
    { //if VISU found get pos and set FileEnd mark + \n   
      i = tmpStr.indexOf("VISUALISATION");
      tmpStr.insert(i, (FileEnd + "\n(* @PATH := '' *)\n\n"));
    }
    else //if no VISU found add FileEnd mark
      tmpStr.append("\n" + FileEnd); 
  }
  //get FileEnd for 3S CoDeSys source which are TYPE
  if ((Ext == 2) && (FileType == "TYPE"))
    tmpStr.append(FileEnd);


  //a d d   T E 1   s t r i n g   t o   t m p L i s t - - - - - - - - - - - - - - - -
  tmpList->append(tmpStr);  //get TE2 into string list
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  

  //insert tags for KW-Software MULTIPROG behind instructions
  if ((Ext == 5) || (Ext == 6))
    tmpList->append(KW_tag3);

  //insert tags for Mitsubishi MELSOFT behind instructions
  if ((Ext == 7) && (FileType != "CONFIGURATION"))
    tmpList->append(MELSOFT_tag3);

  //insert config & end ressource tag at the end of file for Mitsubishi MELSOFT 
  if ((Ext == 7) && (FileType == "CONFIGURATION"))
  {
    tmpList->append(m_PageCtrl->MELSOFTConfigOf(pageIndex));
    tmpList->append(MELSOFT_tag5);
  }
    
  //get FileEnd in stringlist if not 3S CoDeSys source
  if (Ext != 2)
      tmpList->append(FileEnd);
  
  //get Siemens STEP 7 DB Data after file end mark in stringlist
  if ((Ext == 3) || (Ext == 4))
    tmpList->append(m_PageCtrl->STEP7DBDataOf(pageIndex));
    
  //special programming language settings
  if (((Ext == 3) || (Ext == 5)) && (sendConvBack)) //STEP 7 AWL and MULTIPROG IL
    m_PageCtrl->setPrgLngOf(pageIndex, "IL");		//programming language
  if (((Ext == 4) || (Ext == 6)) && (sendConvBack)) //STEP 7 SCL and MULTIPROG ST
    m_PageCtrl->setPrgLngOf(pageIndex, "ST");		//programming language
     
  //FileEnd lines for 3S CoDeSys source VAR LIST
  if ((Ext == 2) && (FileType == "GLOBAL_VARIABLE_LIST"))
    tmpList->append(CoDeSysV2_tag2);

  //FileEnd lines for 3S CoDeSys source TYPE
  if ((Ext == 2) && (FileType == "TYPE"))
    tmpList->append("\n\n" + CoDeSysV2_tag1);
}




//=============================================================================
// - - - diverse private functions - - - 
QString ImportExport::strippedFileName(const QString &fullFileName)
{ //! strip filename from fullFileName (path + file name)
  
  return(QFileInfo(fullFileName).fileName().simplified());
}
