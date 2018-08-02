// TestSvr.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TestSvr.h"
#include "DataBuffer.h"

#include "testcaseloopbk.h"
#include "testcasembreadholding.h"
#include "TestCaseMBForceMultipleCoils.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



void CTestCase::CheckResult(bool result, LPCTSTR errorCause, int lineNum, LPCTSTR srcFileName)
{
   if (!result)
   {
      printf("CHECK FAILED : %s %d - %s\n", srcFileName, lineNum, errorCause);
      SetTestResult(EFail);
   }
}


CTestCase::CTestCase(LPCTSTR name, LPCTSTR description) : m_name(name), m_description(description)
{

}


LPCTSTR CTestCase::TestResultName()
{
char *pRet;
   switch (TestResult())
   {
   case EFail:
      pRet = "FAIL";
      break;
   case EPass:
      pRet = "PASS";
      break;
   case ENotRun:
      pRet = "NOT RUN";
      break;
   default:
      ASSERT(0);  // error ! invalid result, drop through for now!
   case EUnknown:
      pRet = "UNKNOWN";
      break;
   }
   return(pRet);
}


bool CTestServer::Execute(CTestCase *pTest)
{
   printf("\nSTART TEST:\n");
   printf("  name: %s\n", pTest->m_name);
   printf("  desc: %s\n", pTest->m_description);

   pTest->Run();

   printf("RESULT: %s : %s\n%s\n", pTest->TestResultName(), pTest->m_name, HORIZ_DIVIDER);

   return(pTest->TestResult() == CTestCase::EPass);
}


bool CTestCase::Run()
{
bool last = FALSE;

   while(!last)
   {
      BuildFrame();
      SendFrame();
      RecieveFrame();
      TestFrame(last);
   }

   return(TestResult()==EPass);
}

//////////////////////////////////////////////////////////////////////////////
void DoTests()
{
   // add test cases here
   CTestCaseLoopbk   loopBack;
   CTestCaseMBReadHolding  readHolding;
   CTestCaseMBWriteHolding writeHolding;
   CTestCaseMBMaskWriteHolding maskHolding;
   CTestCaseMBForceMultipleCoils forceCoils;
   CTestCaseMBForceMultipleCoils30 forceCoils30;

   printf ("Execute Tests\n");
   printf ("=============\n");

   // run
   CTestServer testServer;

   //testServer.Execute(&loopBack);
   
   //testServer.Execute(&readHolding);
   //testServer.Execute(&writeHolding);
   //testServer.Execute(&maskHolding);
   testServer.Execute(&forceCoils);
   testServer.Execute(&forceCoils30);
}


/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.
		CString strHello;
		strHello.LoadString(IDS_HELLO);
		cout << (LPCTSTR)strHello << endl;
	}

   CDataBuffer buff1,buff2;
   CString text;
   CSimplePrinter hexPrinter;

   buff2 += 0x32;
   buff2.Print(text, hexPrinter);

   buff1 = buff2;
   buff1 += 0xFF;
   buff1.Print(text, hexPrinter);
 
   DoTests();

   printf("\r\nPress any key to end...");
   getch();
	return nRetCode;
}


