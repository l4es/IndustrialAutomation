// TestCaseMBReadHolding.cpp: implementation of the CTestCaseMBReadHolding class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestSvr.h"
#include "TestCaseMBReadHolding.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTestCaseMBBase::CTestCaseMBBase(LPCTSTR name, LPCTSTR description) : CTestCase(name,description)
{

}

bool CTestCaseMBBase::CRCValid( CProtFrame &frame)
{
   return(frame.CRCValid());
}






CTestCaseMBReadHolding::CTestCaseMBReadHolding() : CTestCaseMBBase("MBReadHolding", "Modbus RTU read register")
{

}


CTestCaseMBReadHolding::~CTestCaseMBReadHolding()
{

}


bool CTestCaseMBReadHolding::Run()
{
WORD value;
   return(StepReadSingleHolding(0, value));
} 


bool CTestCaseMBBase::OpenChannel()
{
   set.portName     = "COM4";
   set.dcb.BaudRate = CBR_9600;
   set.dcb.ByteSize = DATABITS_8;
   set.dcb.Parity   = (BYTE)PARITY_NONE;
   set.dcb.StopBits = STOPBITS_10;

   CHECKRESULT(portA.Open(set), "Open Communication port A");

   portA.Flush();
   return (EFail != TestResult());
}


bool CTestCaseMBBase::StepReadSingleHolding(WORD registerStart, WORD &value)
{
   // open the ports, send something across
CProtFrame        message;
CProtFrame        response;
DWORD          expectedSize(5);
WORD           numPoints(1);

   SetTestResult(EUnknown);
   message.Add(01); // station #
   message.Add(MBFUNCTION_READHOLDING);
   message.Add(HIBYTE(registerStart));
   message.Add(LOBYTE(registerStart));
   message.Add(HIBYTE(numPoints));
   message.Add(LOBYTE(numPoints));
   OpenChannel();

   portA.Send(message);

   portA.Recv(response, expectedSize);

   CHECKRESULT(response.GetSize()== expectedSize, "Compare response length");

   if (TestResult() != EFail)
   {
      CHECKRESULT(response[0] == message[0], "compare 1");
      CHECKRESULT(response[1] == message[1], "compare 2");
      CHECKRESULT(response[2] == 2, "compare 3");

      value = response[4] + (response[3]<<8);
   }
   CHECKRESULT(CRCValid(response), "Check CRC");

   portA.Close();

   if (TestResult() == EUnknown)
      SetTestResult(EPass);

   // deliver a result
   return(TestResult() == EPass);
}



CTestCaseMBWriteHolding::CTestCaseMBWriteHolding() : CTestCaseMBBase("MBWriteHolding", "Modbus RTU Write register")
{

}


CTestCaseMBWriteHolding::~CTestCaseMBWriteHolding()
{

}


bool CTestCaseMBWriteHolding::Run()
{
WORD value(0xAA55);

   return(StepWriteSingleHolding(0, value));
}


bool CTestCaseMBBase::StepWriteSingleHolding(WORD registerStart, WORD value)
{
   // open the ports, send something across
CProtFrame        message;
CProtFrame        response;
DWORD          expectedSize(6);
WORD           numPoints(1);

   SetTestResult(EUnknown);
   message.Add(01); // station #
   message.Add(MBFUNCTION_PRESETSINGLEHOLDING);
   message.Add(HIBYTE(registerStart));
   message.Add(LOBYTE(registerStart));
   message.Add(HIBYTE(value));
   message.Add(LOBYTE(value));

   OpenChannel();
   
   portA.Send(message);

   portA.Recv(response, expectedSize);

   CHECKRESULT(response.GetSize()== expectedSize, "Compare response length");
   if (TestResult() != EFail)
   {

      CHECKRESULT(response[0] == message[0], "compare 1");
      CHECKRESULT(response[1] == message[1], "compare 2");
      DWORD newValue, responseAddr;

      responseAddr = response[3] + (response[3]<<8);
      CHECKRESULT(responseAddr==registerStart, "Response addresses miss-match");

      newValue = response[5] + (response[4]<<8);

      CHECKRESULT(newValue==value, "Compare value after set");
   }
   CHECKRESULT(CRCValid(response), "Check CRC");

   portA.Close();

   if (TestResult() == EUnknown)
      SetTestResult(EPass);

   // deliver a result
   return(TestResult() == EPass);
}


/* CTestCaseMBMaskWriteHolding
 *
 */
CTestCaseMBMaskWriteHolding::CTestCaseMBMaskWriteHolding() 
   : CTestCaseMBBase("MBWriteMaskHolding", "Modbus RTU Mask-Write register")
{

}


CTestCaseMBMaskWriteHolding::~CTestCaseMBMaskWriteHolding()
{

}


bool CTestCaseMBMaskWriteHolding::Run()
{

   return(StepMaskWriteHolding(0, 0xDD, 0XCC00));
}


bool CTestCaseMBMaskWriteHolding::StepMaskWriteHolding(WORD registerStart, WORD andMask, WORD orMask)
{
bool result;
WORD valueBefore;
WORD valueAfter;

   // read the current value back
   result = StepReadSingleHolding(registerStart, valueBefore);
   CHECKRESULT(result, "Error reading register (1)");
   if (!result)
      return(FALSE);

   // mask it
CPortSettingBase  set;
CProtFrame        message;
CProtFrame        response;
DWORD          expectedSize(8);
WORD           numPoints(1);

   SetTestResult(EUnknown);
   message.Add(01); // station #
   message.Add(MBFUNCTION_MASKWRITEHOLDING);
   message.Add(HIBYTE(registerStart));
   message.Add(LOBYTE(registerStart));
   message.Add(HIBYTE(andMask));
   message.Add(LOBYTE(andMask));
   message.Add(HIBYTE(orMask));
   message.Add(LOBYTE(orMask));

   OpenChannel();
   
   portA.Send(message);
   portA.Recv(response, expectedSize);
   
   CHECKRESULT(response.GetSize()== expectedSize, "Compare response length");
   if (TestResult() != EFail)
   {
      CHECKRESULT(response[0] == message[0], "compare 1");
      CHECKRESULT(response[1] == message[1], "compare 2");

      CHECKRESULT(CRCValid(response), "Check CRC");
   }
   portA.Close();


   if (TestResult() != EFail)
   {
      // Read it back and compare it
      result = StepReadSingleHolding(registerStart, valueAfter);
      CHECKRESULT(result, "Error reading register (2)");

      // work out the expected result

   }
   if (TestResult() == EUnknown)
      SetTestResult(EPass);

   // deliver a result
   return(TestResult() == EPass);
}
