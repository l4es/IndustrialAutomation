/////////////////////////////////////////////////////////////////////////////
//
// FILE : RS232Noise.h: headder file
//
// See _README.CPP
//
// Interface for the CRS232Noise class.
// The class takes a parameter to a 232 port, and a telegram, and attempts 
// to corrupt it, thereby simulating real-world noise. there are currently
// 6 types of noise:
//    inserts : Adds additional characters anywhere into the transmission
//    removes : Removes characters from anywhere
//    misframes : Simulate bad line noise, by changing the port parity and baud rate
//    corrupts : Replaces any character with another random character
//    ignore : does not respond at all
//    slowness/delays : simulates the act of thinking.
//
// The chances of an error being injected for any 1 transmission is based on 
// its length and the frequency parameter passed to the class. A frequency of 
// zero injects no errors.
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RS232NOISE_H__211E2031_B4D1_424C_BDC8_8E6ED6B307B2__INCLUDED_)
#define AFX_RS232NOISE_H__211E2031_B4D1_424C_BDC8_8E6ED6B307B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRS232Port;
#define MAXERRORFREQUENCY     100

class CRS232Noise : public CObject  
{
public:
	CRS232Noise();
	virtual ~CRS232Noise();
   void Inject(); // now
   
   // SET methods
   void SetErrorTypes(BOOL enable,
                      LONG freq,
                      LONG period = 1000,    
                      BOOL inserts = TRUE,
                      BOOL removes = TRUE, 
                      BOOL misFrames = FALSE,
                      BOOL corrupts = TRUE,
                      BOOL ignore = TRUE,
                      BOOL parityFaults = TRUE,
                      BOOL beep = TRUE
                      );
   // overload
   void SetErrorTypes(CRS232Noise *pOther);
   LONG InjectErrors(CRS232Port *pPort, 
                     const BYTE *transmitBuffer, 
                     int writeLength,
                     char *pDebugStr);

   void ReportError(LPCTSTR descr);
   
   // GET methods
   LONG MaxErrorFreQ() { return(MAXERRORFREQUENCY);};
   void GetErrorTypes(BOOL * enable,
                      int *freq,
                      int *period,    // delays by (0=disable)
                      BOOL *inserts, 
                      BOOL *removes, 
                      BOOL *misFrames,
                      BOOL *corrupts,
                      BOOL *ignore,
                      BOOL *parity,
                      BOOL *beep
                      );
   // get and clear the error counter
   LONG NumErrors() {LONG e = m_errorsInjected; \
                     m_errorsInjected=0; return(e); };
   LONG ErrorsEnabled() { return(m_enableFaults);};

   BOOL NukePort();
   BOOL NukeParity();
   BOOL UnNukePort();

protected:
   BOOL  m_nowInject;
   // data members that contain types of noise we could apply to the comms
   BOOL  m_enableFaults;        // Y/N flag

   BOOL  m_insertCharacters;
   BOOL  m_removeCharacters;
   BOOL  m_corruptFraming;      // change com port settings to corrupt the data.
   BOOL  m_modifyCharacters;    // overwrite some characters
   LONG  m_errorFrequency;      //
   LONG  m_errorsInjected;      // errors so far
   BOOL  m_parityFaults;

   LONG  m_delaysPeriod;      //
   BOOL  m_ignoreReq;         // ignore the req (don't send anything)
   BOOL  m_beep;              // beep afterwards

   BOOL  m_nukedPort;      // nuked Y/N flag
//   DWORD m_oldBaud;
   DWORD reserved;
   CRS232Port *m_pPort;
public:
   CDDKSrvSocket* m_pSocketObj;
};


class CEthernetNoise : public CRS232Noise
{
public:
   CEthernetNoise();

   LONG InjectErrors(CDDKSrvSocket *pSock,
                     SOCKET     openSock,
                     const BYTE *transmitBuffer, 
                     int writeLength,
                     char *pDebugStr);
   
};

#endif // !defined(AFX_RS232NOISE_H__211E2031_B4D1_424C_BDC8_8E6ED6B307B2__INCLUDED_)
