// HtmlOutput.h

#if !defined(HTMLOUT__INCLUDED_)
#define HTMLOUT__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_ERRMESSAGELEN  180
#define FOREVER (TRUE)

///////////////////////////////////////////////////////////////////////////////////////////
// The CHTMLOutput class provides the HTML output feature
//
// Basically we do three things in this class on a thread
// 1. Read an input HTML file
// 2. Replace all occurences of the <SIMUL>registertype,registeroffset,formatting</SIMUL> tag with it's value
// 3. Write to output HTML file
//

//
// This is the TAG in the HTML that I use
#define REPLACEMENT_TAGPREFIX    "<SIMUL>"
#define REPLACEMENT_TAGSUFIX     "</SIMUL>"
#define REPLACEMENT_TAGPREFIXLC  "<simul>"   // lowercase variants
#define REPLACEMENT_TAGSUFIXLC   "</simul>"


class CHTMLOutput:public CObject
{
public:
   CHTMLOutput();

   virtual ~CHTMLOutput();

   void Watch(DWORD dwInterval, LPCTSTR inFile, LPCTSTR outFile); // create thread
   void StopWatching();          // kill thread

protected:
   static UINT threadfunc(void* pThis);
   UINT NotifierThread();

   void ParseInnards(LPCTSTR formatter, CString &replacement);
   void RefreshOutput(bool inputchanged = true);
   void FetchRegisterValue(LONG index, LONG subIndex, CString& value, LONG format=-1);
   void LogError(LPCTSTR errorString);

private:
   CFile m_inputFile;   // used only to read the file when it changes
   CFile m_outputFile;
   DWORD m_dwMsInterval;         // ms before we refresh the output file anyway
   HANDLE   m_hInputFileEvent;   // handle to incomming 'drop' folder change notification event
   HANDLE   m_hShutDownEvent;
   HANDLE   m_hThreadDoneEvent;

   CString m_outputFileName;
   CString m_inputFileName;

   CString m_inputTextBuffer;
   bool    m_shut;
   CWinThread *m_pwThread;

   DECLARE_DYNAMIC( CHTMLOutput )
};

#endif // HTMLOUT__INCLUDED_
