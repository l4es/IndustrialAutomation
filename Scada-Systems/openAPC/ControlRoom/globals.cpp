/******************************************************************************

This file is part of ControlRoom process control/HMI software.

ControlRoom is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

ControlRoom is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
details.

You should have received a copy of the GNU General Public License along with
ControlRoom. If not, see http://www.gnu.org/licenses/

*******************************************************************************

For different licensing and/or usage of the sources apart from GPL or any other
open source license, please contact us at https://openapc.com/contact.php

*******************************************************************************/

#include <wx/aboutdlg.h>

#ifndef __WXMSW__
#include <arpa/inet.h>
#endif

#include "oapc_libio.h"
#include "liboapc.h"
#include "ExternalIOLib.h"
#include "PlugInPanel.h"
#include "UserList.h"
#include "../OpenAPC/common/oapcBinHead.h"
#ifndef ENV_HPLAYER
 #include "../OpenAPC/common/oapcResourceBundle.h"
#endif

#ifdef ENV_BEAM
 #include "../CNConstruct/MainWin.h"
 #include "../CNConstruct/beamconstruct.h"
 #include "../CNConstruct/libbeamconstruct.h"
#endif

#ifndef ENV_CORRCORRECT
#ifndef ENV_BEAM
#ifndef ENV_CNCO

#include "globals.h"
#include "flowLog.h"
#include "common.h"

#ifndef ENV_HPLAYER
 #include "oapcResourceBundle.h"
 #include "hmiUserMgmntPanel.h"
 #ifndef ENV_EDITOR
  #include "WatchWin.h"
  #include "DBWatchWin.h"
 #endif
#endif

#ifdef ENV_WINDOWS
 #ifdef ENV_WINDOWSCE
  #include "Winsock2.h"
 #endif
#endif

#ifdef ENV_BEAM
 #include "beamconstruct.h"
#endif

const wxChar *PROJECT_FILETYPES = _T("Project files|*.apcp|All files|*");
const wxChar *GROUP_FILETYPES = _T("Flow group files|*.apcg|All files|*");
const wxChar *COMPILED_FILETYPES = _T("Compiled project files|*.apcx|All files|*");
const wxChar *IMAGE_FILETYPES = _T("Image files|*.bmp;*.xbm;*.xpm;*.gif;*.png;*.jpg;*.jpeg|All files|*");



wxString               g_projectTouchFontFaceName=_T("");

bool                   d_isCompiling=false;

#ifdef ENV_PLUGGER
flowExternalIOLib     *g_object=NULL;
#endif
ObjectList             g_objectList
#ifndef ENV_HPLAYER
		((HMICanvas*)NULL/*,1*/)
#endif
		;
#ifndef ENV_HPLAYER
 UserList              *g_userList=NULL;
#endif
struct userprivi_data  g_userPriviData;
#ifdef ENV_EDITOR
 std::vector<wxString> g_undoSteps,g_redoSteps;
 wxString              g_undoPath;
 FlowCanvas           *g_flowCanvas=NULL;
 PlugInPanel          *g_plugInPanel=NULL;
 ObjectList            g_selectedList,g_cuttedList;
 bool                  g_isSaved,g_isEmpty;
 FlowObjects           g_flowObjects;
 MainWin              *g_mainWin=NULL;
 wxMenu               *g_mainMenuFile=NULL;
 hmiSpecialPanel      *g_hmiUserMgmntPanel=NULL;
 wxStaticText         *g_userMgmntButton=NULL;
 wxInt32               g_nextHMILeftClickOperation=0;
#else
 wxFrame              *g_mainWin;
 void                 *g_ispaceHandle=NULL;
 #ifdef _DEBUG
  int                  dbg_line;
  char                 dbg_file[200+1]={"empty"};
 #endif
#endif
bool                   g_fileLoadWarned=false;

ExternalIOLibs         *g_externalIOLibs,*g_externalHMILibs;

class HMICanvas;
HMICanvas              *g_hmiCanvas;
flowLog                *g_flowLog=NULL;

#ifdef ENV_PLAYER
 void                   *g_imagePaintMutex;
 FlowWalkerPool          g_flowPool;
 FlowDispatcherThread   *g_flowDispatcher;
 FlowDataController      g_dataCtrl;
 wxByte                  g_breakThreads=0;
 wxString                g_projectFile,g_projectDir,g_projectPath;
 OSNumpad               *g_numpad=NULL;
 struct oapc_ispace_auth g_auth={OAPC_ISERVER_AUTH_ID_MAIN};
 std::list<wxInt32>      g_pluggerIDList;
 bool                    g_allowInputPolling=true;
 #ifndef ENV_DEBUGGER
  bool                   g_invisibleMode=false;
 #endif
#endif

#ifdef ENV_DEBUGGER
wxByte                 g_threadsDisabled=0;
DebugWin              *g_debugWin;
wxInt32                g_debugStepNum=0,g_debugStepMode=0;
WatchWin              *g_watchWin=NULL;
DBWatchWin            *g_DBwatchWin=NULL;
void                  *g_watchWinLock=oapc_thread_mutex_create();
#endif


#ifndef ENV_EDITOR
 #ifdef _DEBUG
  void setDebugInfo(char *file,int line)
  {
      dbg_line=line;
      strncpy(dbg_file,file,200);
  }
 #endif
#endif


struct hmiProjectData *g_initProjectSettings(bool deleteUserList)
{
   struct hmiProjectData *projectData;

#ifndef ENV_HPLAYER
   wxWindow win;
   wxFont   font;
#else

   deleteUserList=deleteUserList;
#endif
   projectData=(struct hmiProjectData*)malloc(sizeof (struct hmiProjectData));
   if (!projectData) return NULL;
#ifndef ENV_HPLAYER
   if (deleteUserList)
   {
      if (g_userList) delete g_userList;
      g_userList=NULL;
   }
#endif
// !!!   if (!g_objectList.m_projectData) return;
#ifndef ENV_PLAYER
   projectData->gridW=10;
   projectData->gridH=10;
   projectData->flowW=622;
   projectData->flowH=622;
   projectData->editorX=50;
   projectData->editorY=50;
   projectData->editorW=656;
   projectData->editorH=600;
#endif
   projectData->flags=0;
   projectData->totalW=800;
   projectData->totalH=600;
   projectData->bgCol=0xFFFFFF;
   projectData->flowTimeout=1000;
   projectData->timerResolution=25;
   //new since 1.1
   projectData->touchFactor=1250;
#ifndef ENV_HPLAYER
   font=win.GetFont();
   projectData->font.pointSize=font.GetPointSize();
   projectData->font.style=font.GetStyle();
   projectData->font.weight=font.GetWeight();
   g_projectTouchFontFaceName=font.GetFaceName();
#endif
   //new since 1.3
   strcpy(projectData->m_remSingleIP,"127.0.0.1");
   //new since 1.4
   strcpy(projectData->m_remDoubleIP,"192.168.1.1");
   //end of new since

#ifdef ENV_EDITOR
   if (deleteUserList)
   {
      g_userPriviData.enabled=false;
      g_userPriviData.privisEnabled=hmiUserMgmntPanel::priviFlagEnabled[1]| // shifted by 1, 0 is the supervision privilege which is non-editable
                                    hmiUserMgmntPanel::priviFlagEnabled[2]|
                                    hmiUserMgmntPanel::priviFlagEnabled[3]|
                                    hmiUserMgmntPanel::priviFlagEnabled[4]|
                                    hmiUserMgmntPanel::priviFlagEnabled[5]|
                                    hmiUserMgmntPanel::priviFlagEnabled[6]|
                                    hmiUserMgmntPanel::priviFlagEnabled[7]|
                                    hmiUserMgmntPanel::priviFlagEnabled[8];
      g_userPriviData.priviName[0]=_("Manage users");
      g_userPriviData.priviName[1]=_("Exit application");
      g_userPriviData.priviName[2]=_("Manual Control");
      g_userPriviData.priviName[3]=_("Modify Parameters");
      g_userPriviData.priviName[4]=_("Edit Recipe");
      g_userPriviData.priviName[5]=_("Select Recipe");
      g_userPriviData.priviName[6]=_("Start Process");
      g_userPriviData.priviName[7]=_("Stop Process");
      g_userPriviData.priviName[8]=_("unused");
      g_userPriviData.priviName[9]=_("unused");
      g_userPriviData.priviName[10]=_("unused");
      g_userPriviData.priviName[11]=_("unused");
      g_userPriviData.priviName[12]=_("unused");
      g_userPriviData.priviName[13]=_("unused");
      g_userPriviData.priviName[14]=_("unused");
   }
   g_plugInPanel->setEnabled((projectData->flags & PROJECT_FLAG_ISPACE_MASK)!=0);
#endif
   return projectData;
}



#ifndef ENV_HPLAYER
void g_setFont(wxWindow *ui,wxFont font)
{
   wxWindowList      winList;
   wxWindowListNode *node;

   ui->SetFont(font);
   winList=ui->GetChildren();
   node=winList.GetFirst();
   while (node)
   {
      g_setFont((wxWindow*)node->GetData(),font);
      node=node->GetNext();
   }
}
#endif



#if defined ENV_EDITOR || ENV_DEBUGGER
void g_About(void)
{
    wxAboutDialogInfo dlg;

#ifdef ENV_EDITOR
    dlg.SetName(_T("ControlRoom Editor"));
#else
    dlg.SetName(_T("ControlRoom Debugger"));
#endif
    dlg.SetVersion(GLOBAL_VERSION);
    dlg.SetCopyright(_T("(c) 2008-2018 by OpenAPC Project Group"));
    dlg.SetDescription(_T("Open Advanced Process Control / Process Controlling Application"));
    dlg.SetWebSite(_T("http://www.openapc.com"));
	 dlg.SetIcon(wxIcon(_T(DATA_PATH"icons/splash.png"),wxBITMAP_TYPE_PNG));
    wxAboutBox(dlg);
}



void g_Credits(void)
{
   wxMessageBox(_("Credits go to")+_T(":\n\
* Axel von Arnim/Frewitt Printing SA for Coherent Avia and Sill Focus plug-in\n\
* Markku Tavasti for wxSMTP\n\
* Marco Cavallini/koansoftware.com for the kwxIndustrialControls\n\
* David Schalig and Davide Rondini for wxMathPlot2D\n\
* Murat Oezdemir/emarti for his analogue clock\n\
* Eran Ifrah for the wxFlatNotebook\n\
* OXY / VWP for their Resource Bundle class\n\
* The ESpeak team for their TTS converter\n\
* The wxWidgets development team for their excellent toolkit"),
                _T("Credits"),wxICON_INFORMATION|wxOK);
}


void g_fileVersionError(void)
{
   if (!g_fileLoadWarned)
    wxMessageBox(_("This project file was created using a newer version of the Application\nand may not work as expected.\nPlease update your software to proceed with this project file!"),_("Error"),wxICON_ERROR|wxOK);
   wxASSERT(0);
   g_fileLoadWarned=true;
}



#else


void g_fileVersionError(void)
{
   if (!g_fileLoadWarned)
#ifndef ENV_HPLAYER
    wxMessageBox(_("This project file was created using the wrong version of the Application\nand may not work as expected.\nPlease load and save the project file using version")+_T(" ")+GLOBAL_VERSION,_("Error"),wxICON_ERROR|wxOK);
#else
    printf("Error: This project file was created using the wrong version of the Application and may not work as expected.\nPlease load and save the project file using version!\n");
#endif
   wxASSERT(0);
   g_fileLoadWarned=true;
}


#endif



void g_checkString(wxString &val)
{
   val.Replace(_T(" "),_T("_"));
   val.Replace(_T("\t"),_T("_"));
   if (val.Find(_T("system"))==0) val.Replace(_T("system"),_T("usersys"));
}



wxBitmapType g_getBitmapFlag(wxString path)
{
   if (!(path.Lower().Mid(path.Length()-4,4).CompareTo(_T(".bmp")))) return wxBITMAP_TYPE_BMP;
   else if (!(path.Lower().Mid(path.Length()-4,4).CompareTo(_T(".gif")))) return wxBITMAP_TYPE_GIF;
   else if (!(path.Lower().Mid(path.Length()-4,4).CompareTo(_T(".xbm")))) return wxBITMAP_TYPE_XBM;
   else if (!(path.Lower().Mid(path.Length()-5,5).CompareTo(_T(".jpeg")))) return wxBITMAP_TYPE_JPEG;
   else if (!(path.Lower().Mid(path.Length()-4,4).CompareTo(_T(".jpg")))) return wxBITMAP_TYPE_JPEG;
   else if (!(path.Lower().Mid(path.Length()-4,4).CompareTo(_T(".tif")))) return wxBITMAP_TYPE_TIF;
   else if (!(path.Lower().Mid(path.Length()-5,5).CompareTo(_T(".tiff")))) return wxBITMAP_TYPE_TIF;
   else if (!(path.Lower().Mid(path.Length()-4,4).CompareTo(_T(".png")))) return wxBITMAP_TYPE_PNG;
   else if (!(path.Lower().Mid(path.Length()-4,4).CompareTo(_T(".gif")))) return wxBITMAP_TYPE_GIF;
   else if (!(path.Lower().Mid(path.Length()-4,4).CompareTo(_T(".pcx")))) return wxBITMAP_TYPE_PCX;
   else if (!(path.Lower().Mid(path.Length()-4,4).CompareTo(_T(".pnm")))) return wxBITMAP_TYPE_PNM;
   return wxBITMAP_TYPE_XPM;
}

#endif //ENV_BEAM
#endif //ENV_CNCO

wxString g_OAPCMessage(const wxUint32 rcode,const ExternalIOLib *lib,const void *pluginData,const wxString filename,wxString *pluginName)
{
   typedef char *(*lib_oapc_get_name)(void);
   typedef unsigned long (*lib_oapc_get_error_message)(void *instanceData,unsigned long length,char* value);

   lib_oapc_get_name          oapc_get_name;
   lib_oapc_get_error_message oapc_get_error_message;

   if (rcode==OAPC_OK) return wxEmptyString;
   if (pluginName)
   {
      *pluginName=wxEmptyString;
      if (lib)
      {
         oapc_get_name=(lib_oapc_get_name)oapc_dlib_get_symbol(lib->m_dynLib,"oapc_get_name");
         if (oapc_get_name)
         {
            char                     *cname;
            wchar_t                   buf[200];
            wxMBConvUTF8              conv;
            cname=oapc_get_name();
            if (strlen(cname)>0)
            {
               conv.MB2WC(buf,cname,sizeof(buf));
#ifndef ENV_HPLAYER
               *pluginName=g_res->getString(buf);
#else
               *pluginName=wxString(buf);
#endif
            }
         }
         if (pluginName->Length()<=0) *pluginName=lib->libname;
      }
   }
   if ((lib) && ((rcode & OAPC_ERROR_MASK)==OAPC_ERROR_CUSTOM))
   {
      char     messagetext[200+4];

      oapc_get_error_message=(lib_oapc_get_error_message)oapc_dlib_get_symbol(lib->m_dynLib,"oapc_get_error_message");
      if ((oapc_get_error_message) && (oapc_get_error_message(const_cast<void*>(pluginData),200,messagetext)==OAPC_OK))
      {         
#ifndef ENV_HPLAYER
         wxString ts;
             
         oapc_unicode_charToStringUTF8(messagetext,200,&ts);
         return ts;
#else
         printf("Error while accessing loaded Plug-in!");
         return wxEmptyString;
#endif
      }
      else
      {
#ifndef ENV_HPLAYER
         return _("An undefined error occurred");
#else
         printf("Undefined error while accessing loaded Plug-in!");
         return wxEmptyString;
#endif
      }
   }
   else switch (rcode)
   {
      case OAPC_ERROR_DEVICE:
         return _("Could not access required device!");
      case OAPC_ERROR_CONNECTION:
         return _("Could not access required connection!");
      case OAPC_ERROR_NOT_SUPPORTEDi:
         return _("Requested feature is not supported by plug-in!");
      case OAPC_ERROR_NO_SUCH_IO:
         return _("Requested IO is not supported by plug-in!");
      case OAPC_ERROR_RESOURCE:
         return _("Could not allocate required resources!");
      case OAPC_ERROR_NO_MEMORY:
         return _("Could not allocate enough memory!");
      case OAPC_ERROR_AUTHENTICATION:
         return _("Authentication failed, username/password did not match!");
      case OAPC_ERROR_CONVERSION_ERROR:
         return _("Conversion error, the given data have an illegal format and can't be converted!");
      case OAPC_ERROR_STILL_IN_PROGRESS:
         return _("Operation is still in progress, can't accept new data!");
      case OAPC_ERROR_RECV_DATA:
         return _("Could not receive data, transfer was interrupted or timeout occured!");
      case OAPC_ERROR_SEND_DATA:
         return _("Could not transmit data, transfer was interrupted or timeout occured!");
      case OAPC_ERROR_PROTOCOL:
         return _("Protocol error, invalid data could not be handled");
      case OAPC_ERROR_INVALID_INPUT:
         return _("Invalid input data or invalid input data format could not be handled by Plug-In");
      case OAPC_ERROR_CREATE_FILE_FAILED:
         if (filename.Length()<1) return _("Could not create file, saving of data failed!");
         return filename+_T(":\n")+_("Could not create file, saving of data failed!");
      case OAPC_ERROR_OPEN_FILE_FAILED:
         return filename+_T(":\n")+_("Could not open file, reading of data failed!");
      case OAPC_ERROR_WRITE_FILE_FAILED:
         return filename+_T(":\n")+ _("Could not write data into file, complete saving of data failed!");
      case OAPC_ERROR_READ_FILE_FAILED:
         return filename+_T(":\n")+_("Could not read from file completely, loading of data failed!");
      case OAPC_ERROR_NO_DATA_AVAILABLE:
         return filename+_T(":\n")+_("No more data available!");
      case OAPC_ERROR_OUT_OF_RANGE:
         return filename+_T(":\n")+_("Data are out of valid range!");
      case OAPC_ERROR_LICENSE:
#ifndef ENV_BEAMLOCK
#ifdef ENV_BEAM
         if ((MainWin::m_initValues->internalFlags & 0x01)==0x01)
          return _("Function could not be executed due to missing license! Do you want to order now?");
         else
#endif // ENV_BEAM
#endif // ENV_BEAMLOCK
          return _("Function could not be executed due to missing PRO license! Do you want to upgrade now?");
      case OAPC_ERROR_LIBRARY_MISSING:
#ifdef ENV_WINDOWS
         return _("A DLL required to operate the hardware is missing. It is available from the vendor and should be placed in WINDOWS\\system32 or WINDOWS\\SysWOW64!");
#else
         return _("A shared library required to operate the hardware is missing. It is available from the vendor and possibly should be placed in /usr/lib or /usr/lib64!");
#endif
      case OAPC_ERROR:
         return _("An undefined error occurred, operation could not be finished!");

      // CNC/Beam specific error texts
      case CNCO_ERROR_UNSUPP_FILEFORMAT_VARIANT:
         return filename+_T(":\n")+_("Unsupported variant of file format, loading canceled!");
      case OAPC_ERROR_UNKNOWN_FILEFORMAT:
         return filename+_T(":\n")+_("Could not recognise file format, loading failed!");
      case CNCO_ERROR_UNKNOWN_INPUT_DATA:
         return filename+_T(":\n")+_("Input data are unknown and can't be processed!");
      case CNCO_ERROR_IN_GEOMETRY:
         return _("Invalid or corrupt data, operation can't be executed with this geometry!");
      default:
         wxASSERT(0);
         return wxString::Format(_("An unknown error occured (%d)"),rcode);
   }
}


void g_OAPCMessageBox(const wxUint32 rcode,const ExternalIOLib *lib,const void *pluginData,wxWindow *parent,const wxString filename)
{
   wxString message,pluginName;
   
   if ((rcode==OAPC_OK) || (rcode==OAPC_OK_ENTITY_FINISHEDi)) return;
#ifdef ENV_BEAM
#ifndef ENV_BEAMLOCK
   if ((MainWin::m_initValues->uiFlags & BEAM_SHOWUI_ERRORMESSAGES)==0) return;
 #endif // ENV_BEAMLOCK
#endif //ENV_BEAM
   message=g_OAPCMessage(rcode,lib,pluginData,filename,&pluginName);
#ifdef ENV_HPLAYER
   parent=parent;
#endif // ENV_HPLAYER

#ifndef ENV_HPLAYER
   if (rcode==OAPC_ERROR_LICENSE)
   {
      if (wxMessageBox(message,_("Error"),wxICON_ERROR|wxYES_NO|wxSTAY_ON_TOP|wxCENTRE,parent)==wxYES)
      {

#ifdef ENV_BEAM
#ifndef ENV_BEAMLOCK
         if ((MainWin::m_initValues->internalFlags & 0x01)==0x01)
          wxLaunchDefaultBrowser(_T("https://replisls3d.com/#variants"));
         else
#endif // ENV_BEAMLOCK
          wxLaunchDefaultBrowser(_T("https://halaser.eu/pro_license.php"));
#else // ENV_BEAM
         wxLaunchDefaultBrowser(_T("https://openapc.com/pro_license.php"));
#endif
      }
   }
   else if (pluginName.length()>0) wxMessageBox(pluginName+_T(":\n")+g_res->getString(message),_("Error"),wxICON_ERROR|wxOK|wxSTAY_ON_TOP,parent);
   else wxMessageBox(message,_("Error"),wxICON_ERROR|wxOK|wxSTAY_ON_TOP,parent);
#else
    printf("Error %d while accessing loaded Plug-in!",rcode);
#endif
}



#ifdef ENV_PLAYER
wxImage *g_getImageFromGrey8(oapcBinHeadSp &value)
{
//   kujhztrzujp
   unsigned char *src,*dest;
   int            cnt;
   wxImage       *img;

   img=new wxImage(value->m_bin->param1,value->m_bin->param2);
   if (!img) return NULL;
   cnt=value->m_bin->param1*value->m_bin->param2;
   src=(unsigned char*)&value->m_bin->data;
   dest=img->GetData();
   do
   {
      *dest=*src; dest++;
      *dest=*src; dest++;
      *dest=*src; dest++;
      src++;
      cnt--;
   }
   while (cnt>0);
   return img;
}
#endif

/* XPM */
const char * icon_xpm[] = {
"32 32 256 2",
"  	c #060304",
". 	c #848210",
"+ 	c #504604",
"@ 	c #FC8BC4",
"# 	c #D1C90C",
"$ 	c #FC2054",
"% 	c #A30504",
"& 	c #FC4664",
"* 	c #D0CE44",
"= 	c #CEAC04",
"- 	c #943204",
"; 	c #9A7007",
"> 	c #FC132C",
", 	c #282A08",
"' 	c #EFEF4C",
") 	c #F4E604",
"! 	c #FB0205",
"~ 	c #FC465C",
"{ 	c #FC4F97",
"] 	c #BCBE42",
"^ 	c #F4861C",
"/ 	c #AC560C",
"( 	c #A19B2E",
"_ 	c #FC3E84",
": 	c #6C0204",
"< 	c #AFAF23",
"[ 	c #9A7E11",
"} 	c #5C550C",
"| 	c #FCA6D4",
"1 	c #C8C634",
"2 	c #FC3262",
"3 	c #7C3A0C",
"4 	c #FCFE49",
"5 	c #FC2644",
"6 	c #FC73AB",
"7 	c #ECD66C",
"8 	c #F9F934",
"9 	c #F63307",
"0 	c #8C1E04",
"a 	c #3C3808",
"b 	c #776824",
"c 	c #4C5224",
"d 	c #FC9FC7",
"e 	c #FCC2EC",
"f 	c #FC5B82",
"g 	c #A8950B",
"h 	c #F84E10",
"i 	c #DEDF49",
"j 	c #DCDE34",
"k 	c #FCFE15",
"l 	c #FC60A4",
"m 	c #171704",
"n 	c #BCBE24",
"o 	c #F61010",
"p 	c #CFB804",
"q 	c #E4D604",
"r 	c #BC8604",
"s 	c #8C5A04",
"t 	c #FC5284",
"u 	c #342E24",
"v 	c #C4B65C",
"w 	c #B71607",
"x 	c #3C3E24",
"y 	c #DC321C",
"z 	c #000000",
"A 	c #000000",
"B 	c #000000",
"C 	c #000000",
"D 	c #000000",
"E 	c #000000",
"F 	c #000000",
"G 	c #000000",
"H 	c #000000",
"I 	c #000000",
"J 	c #000000",
"K 	c #000000",
"L 	c #000000",
"M 	c #000000",
"N 	c #000000",
"O 	c #000000",
"P 	c #000000",
"Q 	c #000000",
"R 	c #000000",
"S 	c #000000",
"T 	c #000000",
"U 	c #000000",
"V 	c #000000",
"W 	c #000000",
"X 	c #000000",
"Y 	c #000000",
"Z 	c #000000",
"` 	c #000000",
" .	c #000000",
"..	c #000000",
"+.	c #000000",
"@.	c #000000",
"#.	c #000000",
"$.	c #000000",
"%.	c #000000",
"&.	c #000000",
"*.	c #000000",
"=.	c #000000",
"-.	c #000000",
";.	c #000000",
">.	c #000000",
",.	c #000000",
"'.	c #000000",
").	c #000000",
"!.	c #000000",
"~.	c #000000",
"{.	c #000000",
"].	c #000000",
"^.	c #000000",
"/.	c #000000",
"(.	c #000000",
"_.	c #000000",
":.	c #000000",
"<.	c #000000",
"[.	c #000000",
"}.	c #000000",
"|.	c #000000",
"1.	c #000000",
"2.	c #000000",
"3.	c #000000",
"4.	c #000000",
"5.	c #000000",
"6.	c #000000",
"7.	c #000000",
"8.	c #000000",
"9.	c #000000",
"0.	c #000000",
"a.	c #000000",
"b.	c #000000",
"c.	c #000000",
"d.	c #000000",
"e.	c #000000",
"f.	c #000000",
"g.	c #000000",
"h.	c #000000",
"i.	c #000000",
"j.	c #000000",
"k.	c #000000",
"l.	c #000000",
"m.	c #000000",
"n.	c #000000",
"o.	c #000000",
"p.	c #000000",
"q.	c #000000",
"r.	c #000000",
"s.	c #000000",
"t.	c #000000",
"u.	c #000000",
"v.	c #000000",
"w.	c #000000",
"x.	c #000000",
"y.	c #000000",
"z.	c #000000",
"A.	c #000000",
"B.	c #000000",
"C.	c #000000",
"D.	c #000000",
"E.	c #000000",
"F.	c #000000",
"G.	c #000000",
"H.	c #000000",
"I.	c #000000",
"J.	c #000000",
"K.	c #000000",
"L.	c #000000",
"M.	c #000000",
"N.	c #000000",
"O.	c #000000",
"P.	c #000000",
"Q.	c #000000",
"R.	c #000000",
"S.	c #000000",
"T.	c #000000",
"U.	c #000000",
"V.	c #000000",
"W.	c #000000",
"X.	c #000000",
"Y.	c #000000",
"Z.	c #000000",
"`.	c #000000",
" +	c #000000",
".+	c #000000",
"++	c #000000",
"@+	c #000000",
"#+	c #000000",
"$+	c #000000",
"%+	c #000000",
"&+	c #000000",
"*+	c #000000",
"=+	c #000000",
"-+	c #000000",
";+	c #000000",
">+	c #000000",
",+	c #000000",
"'+	c #000000",
")+	c #000000",
"!+	c #000000",
"~+	c #000000",
"{+	c #000000",
"]+	c #000000",
"^+	c #000000",
"/+	c #000000",
"(+	c #000000",
"_+	c #000000",
":+	c #000000",
"<+	c #000000",
"[+	c #000000",
"}+	c #000000",
"|+	c #000000",
"1+	c #000000",
"2+	c #000000",
"3+	c #000000",
"4+	c #000000",
"5+	c #000000",
"6+	c #000000",
"7+	c #000000",
"8+	c #000000",
"9+	c #000000",
"0+	c #000000",
"a+	c #000000",
"b+	c #000000",
"c+	c #000000",
"d+	c #000000",
"e+	c #000000",
"f+	c #000000",
"g+	c #000000",
"h+	c #000000",
"i+	c #000000",
"j+	c #000000",
"k+	c #000000",
"l+	c #000000",
"m+	c #000000",
"n+	c #000000",
"o+	c #000000",
"p+	c #000000",
"q+	c #000000",
"r+	c #000000",
"s+	c #000000",
"t+	c #000000",
"u+	c #000000",
"v+	c #000000",
"w+	c #000000",
"x+	c #000000",
"y+	c #000000",
"z+	c #000000",
"A+	c #000000",
"B+	c #000000",
"C+	c #000000",
"D+	c #000000",
"E+	c #000000",
"F+	c #000000",
"G+	c #000000",
"x 4 i m   , 4 4 +     ( 4 n     , 4 ' m   m j 4 }     1 4 ]     ",
"4 4 a     * 4 }     g 4 j m   m ' ' m   m 1 4 (     } 4 v     m ",
"4 a     < 4 <     , ' i m   m j ' ,     b 4 (     + 4 4 m     * ",
"<     b 4 *     m ' 4 / o ~ f t t & 2 w 8 8 m   m ' 4 ,     1 8 ",
"    , 4 4 m   m ' ^ 5 { 6 6 6 @ d | 6 l 2 > : m i 4 b     b 4 * ",
"  m 4 4 +     [ o $ 2 f l d e @ @ d | 6 2 5 5 9 4 (     a 4 4 m ",
"  7 4 x     w > > $ _ { l @ @ | d | @ f _ $ > > 9     m 4 4 ,   ",
"n 4 n     % > > 5 5 t { l { { l 6 l l f t $ > o > o   * 4 }     ",
"4 *       ! o > > > 5 & 2 2 2 2 $ $ _ 2 $ 5 > o o ! h 4 <     a ",
"4       ! ! o o > 5 $ 5 > o o o ! > > > 5 > > o o ! ! /     a 4 ",
",     [ ! ! ! o > o ! o ! ! ! ! ! ! ! ! o > > ! ! ! ! !     ' 8 ",
"    ] 9 ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! o o o ! ! ! ! : * 4 < ",
"  a 7 ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! 9 4 ]   ",
", 4 ^ ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! '     ",
"' 4 9 ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! :     ",
"4 c ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! :   n ",
"n   ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! : b 4 ",
"    ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! 3 4 4 ",
"    y ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! 8 4 , ",
"  1 ^ ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! 4 u   ",
". 4 ( ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! h (     ",
"4 1   ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ;     , ",
"'       ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! !       8 ",
",     p o ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! !       * 8 ",
"    [ ) ; ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! :     # 8 1 ",
"  , 8 #     ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! %     a k ]   ",
", 4 k m     s ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! %     a k 8 m   ",
"i 8 a     } p } % ! ! ! ! ! ! ! ! ! ! ! ! ! ! 0       # k ,     ",
"4 .     , = g       w ! ! ! ! ! ! ! ! ! % - [       p q ,     . ",
"n     , ) =       . q a   : % % % : : m g g m     ; p }     a 4 ",
"    m k k m     ; = +     m = # m     s r m     a # =     a 8 8 ",
"    ] k +     + # [     m p = m     } = ,     , q g     m 4 4 a "};

#endif // ENV_CORRCORRECT


#ifndef ENV_HPLAYER
void g_createDialogButtons(wxDialog *parent,wxSizer *bSizer,const int flags)
{
   wxSizer *sizerBtns=parent->CreateButtonSizer(flags);
   wxSizerItemList list=sizerBtns->GetChildren();

   wxSizerItemList::iterator iter;
   for (iter=list.begin(); iter!=list.end(); ++iter)
   {
      wxSizerItem *current = *iter;
      if (current->IsWindow())
      {
         wxButton* button=dynamic_cast<wxButton*>(current->GetWindow());
         if (button)
         {
            button->SetLabel(g_res->getString(button->GetLabel()));
         }
      }
   };

   if (sizerBtns)
   {
      bSizer->Add(sizerBtns,wxSizerFlags().Expand().Border());
   }
}
#endif //ENV_HPLAYER
