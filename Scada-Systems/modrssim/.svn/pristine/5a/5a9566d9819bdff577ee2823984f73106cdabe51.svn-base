// HtmlOutput.cpp
//
// Mod_RSSim (c) Embedded Intelligence Ltd. 1993,2009
// AUTHOR: Conrad Braam.  http://www.plcsimulator.org
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Affero General Public License as
//    published by the Free Software Foundation, either version 3 of the
//    License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Affero General Public License for more details.
//
//    You should have received a copy of the GNU Affero General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "stdafx.h"
#include "htmloutput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC( CHTMLOutput, CObject )


// ----------------------------------- CHTMLOutput ---------------------------------
CHTMLOutput::CHTMLOutput()
{
   m_hInputFileEvent = NULL;
   m_shut = FALSE;
   m_pwThread = NULL;
   m_hShutDownEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
   m_hThreadDoneEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

}

// ----------------------------------- ~CHTMLOutput ---------------------------------
CHTMLOutput::~CHTMLOutput() 
{
   StopWatching();
   
   if (m_hInputFileEvent)
   {
      CloseHandle(m_hInputFileEvent);  // turn off notification
      WaitForSingleObject(m_hThreadDoneEvent,15000);
   }
   
   //Sleep(100);
   if (m_hShutDownEvent)
   {
      CloseHandle(m_hShutDownEvent);
      CloseHandle(m_hThreadDoneEvent);
   }
}

// ----------------------------------- Watch ---------------------------------
void CHTMLOutput::StopWatching()
{
   if (this->m_pwThread)
      OutputDebugString("Stop watcher thread now!\n");
   m_shut = TRUE;
   SetEvent(m_hShutDownEvent); // trigger early
   // m_pwThread will self-delete, the trick is for destruction to wait long enough for it
}

// ----------------------------------- Watch ---------------------------------
void CHTMLOutput::Watch(DWORD dwInterval, LPCTSTR inFile, LPCTSTR outFile)
{
   if (0!= strcmp(m_inputFileName, inFile))
   {
      StopWatching();
   }
   m_inputFileName = inFile;
   m_outputFileName = outFile;
   m_dwMsInterval = dwInterval*1000;
   if (m_shut)
   {
      m_shut = FALSE;
      OutputDebugString("Creating HTML worker thread now.\n");
      m_pwThread = AfxBeginThread(&threadfunc, this);// AFX_THREADPROC 
   }
}

// ----------------------------------- threadfunc ---------------------------------
UINT CHTMLOutput::threadfunc(void* pThis)
{
CHTMLOutput *pMyClass;
   
   pMyClass = (CHTMLOutput *)pThis;
   ASSERT (pMyClass->IsKindOf(RUNTIME_CLASS(CHTMLOutput)));
   UINT ret = pMyClass->NotifierThread();
   OutputDebugString("HTML output Thread exiting now...\n");
   SetEvent(pMyClass->m_hThreadDoneEvent);
   return (ret);
}

// ----------------------------------- NotifierThread ---------------------------------
UINT CHTMLOutput::NotifierThread()
{
DWORD error;
CHAR fullPathName[MAX_PATH];
CHAR *pFileName;
DWORD length, startTicks, currentTicks, interValTicks;

   // Do the first 'read' of the file now
   startTicks = GetTickCount(); // we use this to time how long it really did take first around
   if (!ExistFile(m_inputFileName))
   {
      pGlobalDialog->SetEnableHTMLGUI(FALSE);
      return(0);
   }
   RefreshOutput();

   // set up watch on the input file's folder
   length = GetFullPathName (m_inputFileName, MAX_PATH, fullPathName, &pFileName);
   pFileName--;
   *pFileName = '\0';

   ASSERT(length);
   SetLastError(0);
   m_hInputFileEvent = FindFirstChangeNotification (fullPathName, FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE);
   error = GetLastError();
   ASSERT(!error);
   while FOREVER
   {
   HANDLE arr[2];
      arr[0] = m_hInputFileEvent;
      arr[1] = m_hShutDownEvent;
      if (m_shut)
         return(0);

      // detect whether the output file is in the same folder as the input file
      CString s1(fullPathName);
      GetFullPathName (m_outputFileName, MAX_PATH, fullPathName, &pFileName);
      CString s2(fullPathName);
      s1.MakeLower();
      s2.MakeLower();
      if (0==strncmp(s1,s2, (LONG)pFileName-(LONG)fullPathName))
      { // swallow the duplicate notification because the output file is in the input folder too
         DWORD waitStatus = WaitForMultipleObjects(2, &arr[0], FALSE, this->m_dwMsInterval);
         FindNextChangeNotification (m_hInputFileEvent);
      }

      // roughly calculate next interval to compensate for the time take to process the files
      currentTicks = GetTickCount();
      interValTicks = this->m_dwMsInterval - (currentTicks - startTicks);
      if (interValTicks > this->m_dwMsInterval) 
         interValTicks = this->m_dwMsInterval;
      // wait for a change in the file, or our timer to expire whichever occurs first
      DWORD waitStatus = WaitForMultipleObjects(2, &arr[0], FALSE, interValTicks);
      error = GetLastError();
      
      if (m_shut)    // we can be in shutdown if the folder-Event got fired from our shut-down code
         return(0);  // ...allow thread to end

      startTicks = GetTickCount(); // timer will incorporate processing time
      RefreshOutput(waitStatus == WAIT_OBJECT_0);
      
      // get ready to wait again
      FindNextChangeNotification (m_hInputFileEvent);
   }
}


// ----------------------------------- FetchRegisterValue ---------------------------------
void CHTMLOutput::FetchRegisterValue(LONG index, LONG subIndex, CString& value, LONG format)
{
DWORD dwValue;
WORD aFormat = (WORD)format;

   if(format<0)
      aFormat= CMemoryEditorList::VIEWFORMAT_WORD;
   dwValue = pGlobalDialog->GetPLCMemoryValue(index, subIndex, aFormat);
   pGlobalDialog->m_listCtrlData.Format(dwValue, CMemoryEditorList::ListViewFormats_(aFormat), value);
}


// ----------------------------------- ParseInnards ---------------------------------
void CHTMLOutput::ParseInnards(LPCTSTR formatter, CString &replacement)
{
LONG index(-1), subIndex(-1);
int format=-1, fields;

   fields = sscanf(formatter, "%d,%d,%d", &index, &subIndex, &format);
   if (fields >=2)
      FetchRegisterValue(index, subIndex, replacement, format);
   else
      replacement.Format("?%s?", formatter);
}

// ----------------------------------- RefreshOutput ---------------------------------
// this is a huge function which does 3 things
// 1. reads the file
// 2. does the text replacement
// 3. write output file
void CHTMLOutput::RefreshOutput(bool inputchanged )
{
CHAR errMsg[MAX_ERRMESSAGELEN];
CString outputTextBuffer; 

   // We only read the file on the first occasion, and there-after only when it changes
   if (inputchanged)
   { // reload input file
   int sharingRetries = 5; // we try to open the file 5 times if it is locked
   int errCode;
      //OutputDebugString("Read input HTML file.\n");
      try{

         do {
            m_inputFile.Open(this->m_inputFileName, CFile::modeRead, NULL);
            errCode = GetLastError();
            // catter for situation where the file is locked due to 
            if (ERROR_SHARING_VIOLATION == errCode) //still being written to when we get notification
            {
               Sleep(500); // wait long enough for author to finnish
               sharingRetries--;
            }
         } while ((ERROR_SHARING_VIOLATION == errCode) && (sharingRetries>0));
         DWORD len = (DWORD)m_inputFile.GetLength();
         m_inputTextBuffer = "";
         CHAR *buff = m_inputTextBuffer.GetBufferSetLength(len+1);
         m_inputFile.Read(buff, len);  //TODO: if the read fails, we may leak the string!
         // terminate the string before we release the buff
         buff[len] = '\0';
         m_inputTextBuffer.ReleaseBuffer(len);
      }
      catch (CFileException *E)
      {
         E->GetErrorMessage(errMsg, MAX_ERRMESSAGELEN, NULL);
         E->Delete();
         // TODO: copy this to the debug log
         LogError("I/O Error in input file :");
         LogError(errMsg);
      }
      m_inputFile.Close();
   }
   if (m_shut)
      return;
   // replace text contents
   {
   int pos;
   int posStart=0;
   bool found=TRUE;
   int prefixLen = strlen(REPLACEMENT_TAGPREFIX);
   int sufixLen = strlen(REPLACEMENT_TAGSUFIX);
   int closeTagLen;

      outputTextBuffer= m_inputTextBuffer;   // brute-force approach for now
      // add a refresh tag <META http-equiv="refresh" content="3,http://www.acme.com/intro.html">
      pos = outputTextBuffer.Find("<HEAD>");
      if (pos>0)
      {
      CString refreshTag;
      
         refreshTag.Format("\r\n<META http-equiv=\"refresh\" content=\"%d,%s\">", (int)(ceil(m_dwMsInterval/1000.0)), this->m_outputFileName);
         outputTextBuffer.Insert(pos+6, refreshTag);
      }
      // find replaceable string untill no more left

      while (found)
      {
      int posEnd, posEndLC, posNextTag;
         // does case insensitive searches still
         pos = outputTextBuffer.Find(REPLACEMENT_TAGPREFIX, posStart);
         if (pos <1)
            pos = outputTextBuffer.Find(REPLACEMENT_TAGPREFIXLC, posStart);
         found = pos>0;
         if (found) 
         {
            posStart = pos;
            closeTagLen = sufixLen;
            // search for our own 'closing' tag, this is possibly simpler than searching for '<' + '>' and comparing 
            // it's contents to our closing tag
            posEnd = outputTextBuffer.Find(REPLACEMENT_TAGSUFIX, pos);
            posEndLC = outputTextBuffer.Find(REPLACEMENT_TAGSUFIXLC, pos);
            posNextTag = outputTextBuffer.Find('<', pos+1);
            if ((posEndLC > 0) && (posEndLC < posEnd))
               posEnd = posEndLC;
            if ((posNextTag>0) && ((posNextTag < posEnd)||(-1==posEnd))) //catter for when a closing tag was not provided
            {
               closeTagLen = 0;
               posEnd = posNextTag;
            }
            found = posEnd>0; // we have a complete tag (incase the input file was truncated)
         }
         if (found) 
         {
            // fetch the innards of the tag, ...and insert the new innards
            CString innards, replacement;
            innards = outputTextBuffer.Mid(pos + prefixLen, posEnd - pos - prefixLen);
            // parse
            ParseInnards(innards, replacement);
            // new innards replaces the whole tag
            outputTextBuffer.Delete(pos, posEnd - pos + closeTagLen/*sufixLen*/);
            outputTextBuffer.Insert(pos, replacement);
         }

      }
   }
   // check if we are asked to shut the thread down while we were busy
   if (m_shut)
      return;
   // write output file to disk
   //OutputDebugString("Write output HTML file.\n");

   try{
      int sharingRetries = 10; // we try to open the file 5 times if it is locked
      DWORD errCode;
      do {
         SetLastError(0);
         m_outputFile.Open(this->m_outputFileName, CFile::modeWrite | CFile::modeCreate, NULL);
         errCode = GetLastError();
         // catter for situation where the file is locked due to 
         if (ERROR_SHARING_VIOLATION == errCode) //still being written to when we get notification
         {
            Sleep(100); // wait long enough for author to finnish
            sharingRetries--;
         }
      } while ((ERROR_SHARING_VIOLATION == errCode) && (sharingRetries>0));

      m_outputFile.Write(outputTextBuffer, outputTextBuffer.GetLength());
   }
   catch (CFileException *E)
   {
      E->GetErrorMessage(errMsg, MAX_ERRMESSAGELEN, NULL);
      E->Delete();
      // TODO: copy this to the debug log
      LogError("I/O Error in input file :");
      LogError(errMsg);
   }
   m_outputFile.Close();
}

// ---------------------------- LogError ----------------------------
void CHTMLOutput::LogError(LPCTSTR errorString)
{
   pGlobalDialog->AddCommsDebugString(errorString);
}

