#include <wx/wx.h>
#include <wx/dir.h>
#include <wx/filename.h>

#include "globals.h"
#include "ExternalMacros.h"
#include "ExternalMacro.h"
#include "iff.h"
#include "flowGroup.h"
#include "common/oapcResourceBundle.h"


ExternalMacros::ExternalMacros()
{
   wxDir dir(_T(MACRO_PATH));

   if (dir.IsOpened())
   {
      wxString filename;

      bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES);
      while ( cont )
      {
         if (!(filename.Lower().Mid(filename.Length()-5,5).CompareTo(_T(".apcg")))) importMacro(filename);
         cont = dir.GetNext(&filename);
      }
   }
}



ExternalMacros::~ExternalMacros()
{
   ExternalMacro *extIOMacro;

   extIOMacro=getMacro(true);
   while (extIOMacro)
   {
      delete extIOMacro;
      extIOMacro=getMacro(false);
   }
   m_macroList.clear();
}



void ExternalMacros::getMacroTitle(wxString file,wxString *title,wxUint32 *category)
{
   wxFile      *FHandle;
   char         rawData[5];
   wxInt32      chunkSize,subChunkSize,subSubChunkSize;
   size_t       readSize;

   FHandle=new wxFile(_T(MACRO_PATH)+file,wxFile::read);
   if (FHandle->IsOpened())
   {
      FHandle->Read(rawData,4);
      if (strncmp(rawData,CHUNK_FORM,4))
      {
         FHandle->Close();
         delete FHandle;
         *title=file;
         return;
      }
      FHandle->Seek(4,wxFromCurrent); // TODO: evaluate length information

      FHandle->Read(rawData,4);
      if (strncmp(rawData,CHUNK_APCG,4))
      {
         FHandle->Close();
         delete FHandle;
         *title=file;
         return;
      }
      while (1)
      {
         readSize=FHandle->Read(rawData,4);
         readSize+=FHandle->Read(&chunkSize,4);
         if (readSize!=8) break;
         chunkSize=ntohl(chunkSize);

         if (chunkSize>0)
         {
            if (!strncmp(rawData,CHUNK_FLOW,4))
            {
               while (chunkSize>0)
               {
                  readSize=FHandle->Read(rawData,4);
                  readSize+=FHandle->Read(&subChunkSize,4);
                  if (readSize!=8) break;
                  subChunkSize=ntohl(subChunkSize);
                  if (!strncmp(rawData,CHUNK_FLOW_FGRP,4))
                  {

                     while (subChunkSize>0)
                     {
                        readSize=FHandle->Read(rawData,4);
                        readSize+=FHandle->Read(&subSubChunkSize,4);
                        if (readSize!=8) break;
                        subChunkSize=ntohl(subSubChunkSize);
                        if (!strncmp(rawData,CHUNK_FLOW_FGRP_GRPD,4))
                        {
                           wxMBConvUTF16BE conv;
                           groupFlowData   flowData;
                           wchar_t         buf[MAX_TEXT_LENGTH];
                           wxString        name;

                           subSubChunkSize-=FHandle->Read(&flowData,sizeof(struct groupFlowData));
                           conv.MB2WC(buf,flowData.store_name,sizeof(buf));
                           FHandle->Close();
                           delete FHandle;
                           name=buf;
                           name.Replace(_T("_"),_T(" "));
                           *title=g_res->getString(name);
                           *category=ntohl(flowData.categoryFlag);
                           return;
                        }
                        FHandle->Seek(chunkSize,wxFromCurrent);
                     }

                  }
                  FHandle->Seek(chunkSize,wxFromCurrent);
               }
            }
            FHandle->Seek(chunkSize,wxFromCurrent);
         }
      }
      FHandle->Close();
      delete FHandle;
   }
}



void ExternalMacros::importMacro(wxString filename)
{       
   ExternalMacro *macro=new ExternalMacro(filename);
   getMacroTitle(macro->m_filename,&macro->m_name,&macro->m_category);

   if (m_macroList.empty()) m_macroList.push_back(macro);
   else
   {
      list<ExternalMacro*>::iterator it;

      it=m_macroList.begin();
      while (it!=m_macroList.end())
      {
         if ((*it)->m_name>macro->m_name)
         {
            m_macroList.insert(it,macro);
            return;
         }
         it++;
      }
      m_macroList.push_back(macro);
   }
}



ExternalMacro *ExternalMacros::getMacro(bool start)
{
   static list<ExternalMacro*>::iterator it;

   if (start) it=m_macroList.begin();
   if (m_macroList.empty()) return NULL;
   return (it!=m_macroList.end()) ? *it++ : NULL;
}


