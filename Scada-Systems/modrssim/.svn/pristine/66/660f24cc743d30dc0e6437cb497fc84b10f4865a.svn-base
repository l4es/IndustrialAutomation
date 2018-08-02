// TestCaseMBForceMultipleCoils.cpp: implementation of the CTestCaseMBForceMultipleCoils class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestSvr.h"
#include "TestCaseMBReadHolding.h"
#include "TestCaseMBForceMultipleCoils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTestCaseMBForceMultipleCoils::CTestCaseMBForceMultipleCoils() : CTestCaseMBBase("MBForceMultipleCoils", "Modbus RTU force multiple coils")
{

}


CTestCaseMBForceMultipleCoils::~CTestCaseMBForceMultipleCoils()
{

}


bool CTestCaseMBForceMultipleCoils::Run()
{

   return(Step());
} 


bool CTestCaseMBForceMultipleCoils::Step()
{
// open the ports, send something across
CProtFrame        message;
CProtFrame        response;
DWORD          expectedSize(6);
WORD     registerStart = 0;
WORD     numPoints = 5;
BYTE     data[255] = {0xFF,0,0,0}; // ...

   SetTestResult(EUnknown);
   message.Add(01); // station #
   message.Add(MBFUNCTION_WRITE_MULTIPLE_COILS);
   message.Add(HIBYTE(registerStart));
   message.Add(LOBYTE(registerStart));
   message.Add(HIBYTE(numPoints));
   message.Add(LOBYTE(numPoints));

   message.Add(LOBYTE(numPoints/8) + (numPoints%8?1:0));
   //datas
   message.Add(data[0]);
   OpenChannel();

   portA.Send(message);

   portA.Recv(response, expectedSize);

   CHECKRESULT(response.GetSize()== expectedSize, "Compare response length");

   if (TestResult() != EFail)
   {
      CHECKRESULT(response[0] == message[0], "compare STN");
      CHECKRESULT(response[1] == message[1], "compare FN");
      CHECKRESULT(response[2] == message[2], "compare addHI");
      CHECKRESULT(response[3] == message[3], "compare addLO");
      CHECKRESULT(response[4] == message[4], "compare cntHI");
      CHECKRESULT(response[4] == message[4], "compare cntLO");

      //value = response[4] + (response[3]<<8);
   }
   CHECKRESULT(CRCValid(response), "Check CRC");

   portA.Close();

   if (TestResult() == EUnknown)
      SetTestResult(EPass);

   // deliver a result
   return(TestResult() == EPass);
}


CTestCaseMBForceMultipleCoils30::CTestCaseMBForceMultipleCoils30() : CTestCaseMBBase("MBForceMultipleCoils", "Modbus RTU force multiple coils")
{

}


CTestCaseMBForceMultipleCoils30::~CTestCaseMBForceMultipleCoils30()
{

}


bool CTestCaseMBForceMultipleCoils30::Run()
{

   return(Step());
} 


bool CTestCaseMBForceMultipleCoils30::Step()
{
// open the ports, send something across
CProtFrame        message;
CProtFrame        response;
DWORD          expectedSize(6);
WORD     registerStart = 0;
WORD     numPoints = 30;
BYTE     data[255] = {0xFF,0xFF,0xFF,0xFF}; // ...

   SetTestResult(EUnknown);
   message.Add(01); // station #
   message.Add(MBFUNCTION_WRITE_MULTIPLE_COILS);
   message.Add(HIBYTE(registerStart));
   message.Add(LOBYTE(registerStart));
   message.Add(HIBYTE(numPoints));
   message.Add(LOBYTE(numPoints));

   message.Add(LOBYTE(numPoints/8) + (numPoints%8?1:0));
   //datas
   message.Add(data[0]);
   message.Add(data[1]);
   message.Add(data[2]);
   message.Add(data[3]);
   OpenChannel();

   portA.Send(message);

   portA.Recv(response, expectedSize);

   CHECKRESULT(response.GetSize()== expectedSize, "Compare response length");

   if (TestResult() != EFail)
   {
      CHECKRESULT(response[0] == message[0], "compare STN");
      CHECKRESULT(response[1] == message[1], "compare FN");
      CHECKRESULT(response[2] == message[2], "compare addHI");
      CHECKRESULT(response[3] == message[3], "compare addLO");
      CHECKRESULT(response[4] == message[4], "compare cntHI");
      CHECKRESULT(response[4] == message[4], "compare cntLO");

      //value = response[4] + (response[3]<<8);
   }
   CHECKRESULT(CRCValid(response), "Check CRC");

   portA.Close();

   if (TestResult() == EUnknown)
      SetTestResult(EPass);

   // deliver a result
   return(TestResult() == EPass);
}