// TestCaseLoopbk.cpp: implementation of the CTestCaseLoopbk class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestSvr.h"
#include "TestCaseLoopbk.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTestCaseLoopbk::CTestCaseLoopbk() : CTestCase("serial port loopback", "com3 to com4 connection test 9600,8,n,1 - 'Hello'")
{

}

CTestCaseLoopbk::~CTestCaseLoopbk()
{

}


bool CTestCaseLoopbk::Run()
{
   // open the ports, send something across
CPortSettingBase  set;
//CDataBuffer       msg;
CProtFrame        messageShort;
CProtFrame        messageLong;
CProtFrame        response;

   SetTestResult(EUnknown);
   messageShort.AppendText("Hello");

   set.portName = "COM3";
   set.dcb.BaudRate = CBR_9600;
   set.dcb.ByteSize = DATABITS_8;
   set.dcb.Parity = (BYTE)PARITY_NONE;
   set.dcb.StopBits = STOPBITS_10;

   CHECKRESULT(portA.Open(set), "Open Communication port A");
   set.portName = "COM4";
   CHECKRESULT(portB.Open(set), "Open Communication port B");
   
   portA.Send(messageShort);

   portB.Recv(response, messageShort.GetSize());

   CHECKRESULT(messageShort == response, "Compare data bytes");

   portA.Close();
   portB.Close();

   if (TestResult() == EUnknown)
      SetTestResult(EPass);

   // deliver a result
   return(TestResult() == EPass);
}
