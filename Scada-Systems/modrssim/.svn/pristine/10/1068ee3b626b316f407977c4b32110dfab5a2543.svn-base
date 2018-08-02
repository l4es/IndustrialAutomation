// DataBuffer.h: interface for the CDataBuffer class.
//
//////////////////////////////////////////////////////////////////////

// wrapper to make displaying the data RX and TX as well as handling it easier


#if !defined(AFX_DATABUFFER_H__79A22E51_1599_4A44_A7AB_1755730E635F__INCLUDED_)
#define AFX_DATABUFFER_H__79A22E51_1599_4A44_A7AB_1755730E635F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define EFAILED   (int)(INVALID_HANDLE_VALUE)
#define ESUCCESS  0

#define MAX_FRAME_LENGTH   512





// base class for interface to String-printers
class IPrinter
{
public:
   // called when displayof a telegram starts
   virtual int OnReset(int lenToPrint) {return(EFAILED); /*unsupported*/}
   // called at the end of a telegram/frame
   virtual int OnComplete() {return(ESUCCESS);}
   // called for each byte
   virtual int PrintByte(char* buffer,   // dest
                         const int len,
                         const BYTE data,
                         const int printFormat // options
                          )=0;      // length of buffer
};



class CDataBuffer : public CByteArray  
{
public:
	CDataBuffer();
   CDataBuffer(const CDataBuffer& other) {*this= other;}
	virtual ~CDataBuffer();

   // overloads to make adding data easy
   CDataBuffer& operator + (BYTE byte);
   CDataBuffer& operator + (CDataBuffer& other);

   //friend CDataBuffer operator += (BYTE byte, CDataBuffer& self);
   CDataBuffer& operator += (const CDataBuffer& other);
   CDataBuffer& operator += (BYTE other);
   
   CDataBuffer& operator = (const CDataBuffer& other) { if (this == &other) \
      return *this; this->Copy(other);return(*this);}

   void Print(CString & dest, IPrinter &printer);

   int AppendText(LPCTSTR text);
};


// a protocol frame has direction, and a check-sum flag
class CProtFrame : public CDataBuffer
{
public:
   CProtFrame(const CDataBuffer& other) :CDataBuffer (other){ };
   CProtFrame();

   bool Compare(const CDataBuffer& other);
   bool operator == (const CDataBuffer& other) {return Compare(other);};

   enum EFrameDirection {EDirectionIN, EDirectionOUT};

   bool CRCValid() {return(m_crcOK);};
   void ResetCRC() {m_crcOK = FALSE;};

   void CheckCRC();
   EFrameDirection    GetDirection() {return(m_dir);};
   void SetDirection(EFrameDirection aDir) {m_dir=aDir;};

   // data
protected:
   EFrameDirection   m_dir;
   bool              m_crcOK;

};


// implements IPrinter
class CSimplePrinter:public IPrinter
{
public:
   CSimplePrinter();
   int OnReset(int lenToPrint) {m_reset = true; return(EFAILED); }

   // called for each byte
   int PrintByte(char* buffer,   // dest         
                  const int len,
                  const BYTE data,                     
                  const int printFormat // options      
                  );      // length of buffer (incl. null)
private:
   int m_reset;

};

#endif // !defined(AFX_DATABUFFER_H__79A22E51_1599_4A44_A7AB_1755730E635F__INCLUDED_)
