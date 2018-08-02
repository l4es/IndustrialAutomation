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

#ifdef ENV_INT
 #include "../flowExternalIOLib.cpp"
#else //ENV_INT

#include <wx/wx.h>

#ifdef ENV_DEBUGGER
#include "DebugWin.h"
#endif

#ifdef _MSC_VER
 #pragma warning (disable: 4996)
#endif

#include "common/oapcMutexLocker.h"
#include "iff.h"
#include "globals.h"
#include "flowExternalIOLib.h"
#include "oapc_libio.h"
#ifdef ENV_EDITOR
#include "DlgConfigflowExternalIOLib.h"
#endif


flowExternalIOLib::flowExternalIOLib(ExternalIOLib *lib,wxInt32 dataID):flowObject(NULL)
{
   returnOK=0;
   this->lib=lib;
   this->data.type=FLOW_TYPE_EXTERNAL_LIB;
   this->data.id=dataID;
   m_cycleMicros=125000;
#ifndef ENV_PLAYER
   flowBitmap=NULL;
#endif
   if ((!lib->oapc_get_capabilities) ||
       ((!lib->oapc_get_input_flags) && (!lib->oapc_get_output_flags)))
   {
#ifndef ENV_HPLAYER
      wxMessageBox(lib->libname+_T(":\n")+_("Could not load required symbols, plugin seems to be corrupt!"),_("Warning"),wxICON_WARNING|wxOK);
#else
      printf("Could not load required symbols, plugin seems to be corrupt!\n");
#endif
      return;
   }
   m_pluginData=NULL;
   if (lib->oapc_create_instance2)
#ifdef ENV_PLAYER
   m_pluginData=lib->oapc_create_instance2(OAPC_INSTANCE_OPERATION);
#else
   m_pluginData=lib->oapc_create_instance2(OAPC_INSTANCE_MINIMUM_INIT);
#endif
   else m_pluginData=lib->oapc_create_instance();
   if (!m_pluginData)
   {
#ifndef ENV_PLUGGER
      g_OAPCMessageBox(OAPC_ERROR_NO_MEMORY,lib,NULL,(wxWindow*)g_mainWin,wxEmptyString);
#else
 #ifdef ENV_LINUX
      #warning implement error message handling via IServer
 #endif
#endif
      return;
   }
#ifdef ENV_EDITOR
   xmlDecoder=new XMLDecoder(this,(wxWindow*)g_mainWin,lib,m_pluginData);
   getLibConfigData();
#else
   m_accessMutex=oapc_thread_mutex_create();
   if (lib->getLibCapabilities() & OAPC_ACCEPTS_IO_CALLBACK)
   {
      if (lib->oapc_set_io_callback)
       lib->oapc_set_io_callback(m_pluginData,FlowWalkerPool_oapc_io_callback,data.id); // oapc_io_callback is located in flowWalkerPool.cpp
   }   
#endif
   if (lib->getLibCapabilities() & OAPC_HAS_INPUTS) this->data.stdIN=lib->getLibInputFlags();
   else this->data.stdIN=0;
   if (lib->getLibCapabilities() & OAPC_HAS_OUTPUTS) this->data.stdOUT=lib->getLibOutputFlags();
   else this->data.stdOUT=0;
   if (lib->getLibCapabilities() & OAPC_ACCEPTS_IO_CALLBACK) m_cycleMicros=0;

   returnOK=1;
}



flowExternalIOLib::~flowExternalIOLib()
{
#ifndef ENV_PLAYER
   if (flowBitmap!=NULL) delete flowBitmap;
#else
   oapc_thread_mutex_release(m_accessMutex);
   if (lib->oapc_exit) lib->oapc_exit(m_pluginData);
#endif
   if (lib->oapc_delete_instance) lib->oapc_delete_instance(m_pluginData);
#ifdef ENV_EDITOR
   delete xmlDecoder;
#endif
}



wxString flowExternalIOLib::getDefaultName()
{
    return lib->name;
}



#ifndef ENV_PLAYER
void flowExternalIOLib::getLibConfigData()
{
   char             *data;

   {
      char         *p,*oldLoc=NULL;

      p=setlocale(LC_NUMERIC,NULL);
      if (p) 
      {
         oldLoc=strdup(p);
         setlocale(LC_NUMERIC,"C");
      }
      data=lib->oapc_get_config_data(m_pluginData);
      if (p) 
      {
         setlocale(LC_NUMERIC,oldLoc);
         free(oldLoc);
      }
   }
   
   xmlDecoder->setData(data);
   flowBitmap=xmlDecoder->getFlowBitmap();
}
#endif



#ifndef ENV_PLAYER
void flowExternalIOLib::doDataFlowDialog(bool hideISConfig)
{
   wxMBConvUTF8      conv;
   wxString          title;

   getLibConfigData();

   title=name+_T(" ")+_("Definition")+name.Format(_T(" (%d)"),data.id);
   DlgConfigflowExternalIOLib dlg(&name,xmlDecoder,NULL,data.stdOUT,&m_cycleMicros,title,hideISConfig);

   dlg.ShowModal();
   dlg.Destroy();
}



wxInt32 flowExternalIOLib::saveEXIO(wxFile *FHandle,bool isCompiled)
{
   struct externalFlowData flowData;
   wxInt32                 length;
   unsigned long           extLibDataSize;
   void                   *extLibData=NULL;
   wxMBConvUTF16BE         conv;

   if (!FHandle) return 0;

   FHandle->Write(CHUNK_FLOW_EXIO,4);
   if (lib->oapc_get_save_data) extLibData=lib->oapc_get_save_data(m_pluginData,&extLibDataSize);
   else extLibDataSize=0;
   if (isCompiled) length=sizeof(struct externalFlowData)+(flowList.GetCount()*FLOW_CONNECTION_HEAD_SIZE)+extLibDataSize;
   else length=sizeof(struct externalFlowData)+(flowList.GetCount()*sizeof(struct flowConnectionData))+extLibDataSize;
   length=htonl(length);
   FHandle->Write(&length,4);

   wxASSERT(data.id>0);
   flowData.flowData.id       =htonl(data.id);
   flowData.flowData.version  =htonl(1);
   flowData.flowData.usedFlows=htonl(flowList.GetCount());
   flowData.flowData.maxEdges =htonl(MAX_CONNECTION_EDGES);
   flowData.flowX             =htonl(getFlowPos().x);
   flowData.flowY             =htonl(getFlowPos().y);
   flowData.userPriviFlags    =oapc_util_htonll(data.userPriviFlags);
   flowData.dataFlowFlags     =htonl(data.dataFlowFlags);
   flowData.flowFlags         =htonl(m_flowFlags);
   flowData.type              =htonl(data.type);
   flowData.cycleMicros       =htonl(m_cycleMicros);
   conv.WC2MB(flowData.store_name,name,sizeof(flowData.store_name));

   conv.WC2MB(flowData.store_libname,lib->libname,sizeof(flowData.store_libname));
   flowData.extLibDataSize    =htonl(extLibDataSize);
   length=FHandle->Write(&flowData,sizeof(struct externalFlowData));

   wxList::compatibility_iterator node;
   FlowConnection                *connection;

   node=flowList.GetFirst();
   while (node)
   {
      connection=(FlowConnection*)node->GetData();
      length+=connection->saveFlow(FHandle,isCompiled);
      node=node->GetNext();
   }
   if ((extLibDataSize>0) && (extLibData)) length+=FHandle->Write(extLibData,extLibDataSize);
   return length;
}
#endif




wxInt32 flowExternalIOLib::loadEXIO(wxFile *FHandle,struct externalFlowData *flowData,wxUint32 IDOffset,bool isCompiled)
{
   wxInt32 loaded=0;
   void   *extLibData;

   data.userPriviFlags=flowData->userPriviFlags;
   data.dataFlowFlags =flowData->dataFlowFlags;
   m_flowFlags        =flowData->flowFlags;
   data.id=flowData->flowData.id;
   m_cycleMicros      =flowData->cycleMicros;
//   data.stdIN        =ntohl(flowData->stdIN);  do not overwrite the data here, the original value is retrieved from the plugin
//   data.stdOUT       =ntohl(flowData->stdOUT); do not overwrite the data here, the original value is retrieved from the plugin

#ifndef ENV_PLUGGER
   loaded=flowObject::loadFlow(FHandle,&flowData->flowData,IDOffset,false,isCompiled);
#endif
   extLibData=malloc(flowData->extLibDataSize);
   if (extLibData)
   {
      loaded=loaded+FHandle->Read(extLibData,flowData->extLibDataSize);
      if (lib->oapc_set_loaded_data)
       lib->oapc_set_loaded_data(m_pluginData,flowData->extLibDataSize,(char*)extLibData);
      free(extLibData);
   }
   else
   {
      if (FHandle->Seek(flowData->extLibDataSize,wxFromCurrent)>0) loaded+=flowData->extLibDataSize;
   }
#ifdef ENV_PLAYER
   wxInt32 rcode;

   if (lib->oapc_init)
   {
      rcode=lib->oapc_init(m_pluginData);
      if (rcode!=OAPC_OK)
      {

         g_OAPCMessageBox(rcode,lib,m_pluginData,g_mainWin,wxEmptyString);
//bumi
           returnOK=0;
      }
   }
#endif
   return loaded;
}



#ifdef ENV_PLAYER
/**
Sets an digital input and checks if the same thread has set that input before
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input (0 or 1)
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0 if the same thread already has set a value; the thread has to finish
        in this case to avoid endless loops; 1 if the input value was set correctly
*/
wxByte flowExternalIOLib::setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   wxInt32  rcode;

#ifndef ENV_PLUGGER
   if (g_breakThreads) return 0;
   if (!threadIDOK(flowThreadID,false)) return 0;
#endif
   if (!lib->oapc_set_digi_value)
   {
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT,_T(""));
#endif
      return 0;
   }
   if (value!=0) value=1;
   wxASSERT((connection->targetInputNum>=0) && (connection->targetInputNum<MAX_NUM_IOS));
   rcode=lib->oapc_set_digi_value(m_pluginData,connection->targetInputNum,value);
   if (rcode!=OAPC_OK)
   {
#ifdef ENV_DEBUGGER
      // TODO: call of GUI-functions out of a thread will crash under Linux
      if ((rcode & OAPC_ERROR_MASK)==OAPC_ERROR_NO_SUCH_IO)
       g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      else
       g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_FAILED,_T(""));
      g_OAPCMessageBox(rcode,lib,m_pluginData,g_mainWin,wxEmptyString);
#endif
      return 0;
   }
   return 1;
}



wxByte flowExternalIOLib::getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
   wxByte   ret;
   oapcMutexLocker locker(m_accessMutex,__FILE__,__LINE__);

#ifndef ENV_PLUGGER
   if (g_breakThreads) return 0;
#endif
   if (!lib->oapc_get_digi_value)
   {
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT,_T(""));
#endif
      *rcode=OAPC_ERROR_NOT_SUPPORTEDi;
      return 0;
   }

   wxASSERT((connection->sourceOutputNum>=0) && (connection->sourceOutputNum<MAX_NUM_IOS));
   *rcode=lib->oapc_get_digi_value(m_pluginData,connection->sourceOutputNum,&ret);
   return ret;
}



/**
Sets an numerical input and checks if the same thread has set that input before
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0 if the same thread already has set a value; the thread has to finish
        in this case to avoid endless loops; 1 if the input value was set correctly
*/
wxByte flowExternalIOLib::setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   wxInt32  rcode;

#ifndef ENV_PLUGGER
   if (g_breakThreads) return 0;
   if (!threadIDOK(flowThreadID,false)) return 0;
#endif
   if (!lib->oapc_set_num_value)
   {
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT,_T(""));
#endif
      return 0;
   }

   wxASSERT((connection->targetInputNum>=0) && (connection->targetInputNum<MAX_NUM_IOS));
   rcode=lib->oapc_set_num_value(m_pluginData,connection->targetInputNum,value);
   if (rcode!=OAPC_OK)
   {
#ifdef ENV_DEBUGGER
      // TODO: call of GUI-functions out of a thread will crash under Linux
      if ((rcode & OAPC_ERROR_MASK)==OAPC_ERROR_NO_SUCH_IO)
       g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      else if ((rcode & OAPC_ERROR_MASK)==OAPC_ERROR_NOT_SUPPORTEDi)
       g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT,_T(""));
      else
       g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_FAILED,_T(""));
      g_OAPCMessageBox(rcode,lib,m_pluginData,g_mainWin,wxEmptyString);
#endif
      return 0;
   }
   return 1;
}



wxFloat64 flowExternalIOLib::getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
   wxFloat64 ret;
   oapcMutexLocker locker(m_accessMutex,__FILE__,__LINE__);

#ifndef ENV_PLUGGER
   if (g_breakThreads) return 0.0;
#endif
   if (!lib->oapc_get_num_value)
   {
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT,_T(""));
#endif
      *rcode=OAPC_ERROR_NOT_SUPPORTEDi;
      return 0.0;
   }

   wxASSERT((connection->sourceOutputNum>=0) && (connection->sourceOutputNum<MAX_NUM_IOS));
   *rcode=lib->oapc_get_num_value(m_pluginData,connection->sourceOutputNum,&ret);
   return ret;
}



/**
Sets an character input and checks if the same thread has set that input before
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0 if the same thread already has set a value; the thread has to finish
        in this case to avoid endless loops; 1 if the input value was set correctly
*/
wxByte flowExternalIOLib::setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   wxInt32       rcode;
   wxMBConvUTF8  conv;
   char         *c;

#ifndef ENV_PLUGGER
   if (g_breakThreads) return 0;
   if (!threadIDOK(flowThreadID,false)) return 0;
#endif
   if (!lib->oapc_set_char_value)
   {
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT,_T(""));
#endif
      return 0;
   }

   c=(char*)malloc((value.Length()*2)+1);
   wxASSERT(c);
   if (!c)
   {
      return 0;
   }
   if (value.Length()>0) conv.WC2MB(c,value,value.Length()*2);
   else c[0]=0;

   wxASSERT((connection->targetInputNum>=0) && (connection->targetInputNum<MAX_NUM_IOS));
   rcode=lib->oapc_set_char_value(m_pluginData,connection->targetInputNum,c);
   free(c);
   if (rcode!=OAPC_OK)
   {
#ifdef ENV_DEBUGGER
      // TODO: call of GUI-functions out of a thread will crash under Linux
      if ((rcode & OAPC_ERROR_MASK)==OAPC_ERROR_NO_SUCH_IO)
       g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      else if ((rcode & OAPC_ERROR_MASK)==OAPC_ERROR_NOT_SUPPORTEDi)
       g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT,_T(""));
      else
       g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_FAILED,_T(""));
      g_OAPCMessageBox(rcode,lib,m_pluginData,g_mainWin,wxEmptyString);
#endif
      return 0;
   }
   return 1;
}



wxString flowExternalIOLib::getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput))
{
   wxMBConvUTF8  conv;
   wxInt32       length=300;
   char          c[300];
   wchar_t       wc[300];
   oapcMutexLocker locker(m_accessMutex,__FILE__,__LINE__);

#ifndef ENV_PLUGGER
   if (g_breakThreads) return wxEmptyString;
#endif
   if (!lib->oapc_get_char_value)
   {
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT,_T(""));
#endif
      *rcode=OAPC_ERROR_NOT_SUPPORTEDi;
      return _T("");
   }

   wxASSERT((connection->sourceOutputNum>=0) && (connection->sourceOutputNum<MAX_NUM_IOS));
   *rcode=lib->oapc_get_char_value(m_pluginData,connection->sourceOutputNum,length,c);
   if (*rcode!=OAPC_OK)
   {
      return _T("");
   }
   conv.MB2WC(wc,c,300);
   return wxString(wc);
}



wxByte flowExternalIOLib::setBinInput(FlowConnection *connection,oapcBinHeadSp &value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   wxInt32  rcode;

#ifndef ENV_PLUGGER
   if (g_breakThreads) return 0;
   if (!threadIDOK(flowThreadID,false)) return 0;
#endif
   if (!lib->oapc_set_bin_value)
   {
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT,_T(""));
#endif
      return 0;
   }

   wxASSERT((connection->targetInputNum>=0) && (connection->targetInputNum<MAX_NUM_IOS));
   rcode=lib->oapc_set_bin_value(m_pluginData,connection->targetInputNum,value->m_bin);
   if (rcode!=OAPC_OK)
   {
#ifdef ENV_DEBUGGER
      // TODO: call of GUI-functions out of a thread will crash under Linux
      if ((rcode & OAPC_ERROR_MASK)==OAPC_ERROR_NO_SUCH_IO)
       g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      else if ((rcode & OAPC_ERROR_MASK)==OAPC_ERROR_NOT_SUPPORTEDi)
       g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT,_T(""));
      else
       g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_FAILED,_T(""));
      g_OAPCMessageBox(rcode,lib,m_pluginData,g_mainWin,wxEmptyString);
#endif
      return 0;
   }
   return 1;
}



oapcBinHeadSp flowExternalIOLib::getBinOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput))
{
   struct oapc_bin_head *ret=NULL;
   oapcMutexLocker       locker(m_accessMutex,__FILE__,__LINE__);

#ifndef ENV_PLUGGER
   if (g_breakThreads) return oapcBinHeadSp();
#endif
   if (!lib->oapc_get_bin_value)
   {
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT,_T(""));
#endif
      *rcode=OAPC_ERROR_NOT_SUPPORTEDi;
      return oapcBinHeadSp();
   }

   wxASSERT((connection->sourceOutputNum>=0) && (connection->sourceOutputNum<MAX_NUM_IOS));
   *rcode=lib->oapc_get_bin_value(m_pluginData,connection->sourceOutputNum,&ret);

   if (*rcode!=OAPC_OK) return oapcBinHeadSp();
   if ((ret->type==OAPC_BIN_TYPE_TEXT) && (ret->subType==OAPC_BIN_SUBTYPE_TEXT_PLAIN) && (((int)ntohl(ret->param2))==-1))
   {
   // we received a debug/error text from the plug-in
#ifdef ENV_DEBUGGER
      wxString errorMessage;

      oapc_unicode_charToStringASCII(&ret->data,ntohl(ret->param1),&errorMessage);
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_RETURNED_ERROR,errorMessage);
#endif
      lib->oapc_release_bin_data(m_pluginData,connection->sourceOutputNum);
      *rcode=OAPC_ERROR;
      return oapcBinHeadSp();
   }
   return oapcBinHeadSp(new oapcBinHead(ret));
}



void flowExternalIOLib::releaseBinData(FlowConnection *connection)
{
   lib->oapc_release_bin_data(m_pluginData,connection->sourceOutputNum);
}


#endif

#endif //ENV_INT