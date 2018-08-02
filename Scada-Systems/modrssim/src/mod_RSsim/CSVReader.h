#ifndef __CSVREADER_H_INCLUDED
#define __CSVREADER_H_INCLUDED

// include the instrument#
#define MAX_CSVFILE_COLUMNS   17
#define MAX_DEBUG_STR_LEN     256


class CRegisterUpdaterIF
{
public:
   virtual void DebugMessage(LPCTSTR message)=0;
   virtual BOOL SetRegister(LONG index, WORD value)=0;
   virtual BOOL ModbusClone() = 0;
};


//////////////////////////////////////////////////////////////////////////////
// CCSVTextImporter
class CCSVTextImporter : public CObject
{
public:
   CCSVTextImporter();
   virtual ~CCSVTextImporter();


private:
   //friend class
   //////////////////////////////////////////////////////////////////////////////
   // CCSVTextLine
   class CCSVTextLine : public CString
   {
   //DECLARE_DYNAMIC(CCSVTextLine)
   friend class CCSVLineArray;

   public:
      CCSVTextLine(LPCSTR string);
      CCSVTextLine(CCSVTextLine& other);
      CCSVTextLine();

      CCSVTextLine & operator = (CCSVTextLine &otherLine);
      CCSVTextLine & operator = (CString &otherString);

      // functions to get the values out
      double GetElement(LONG index);

      void  Parse();

   private:
      //double m_double;

      // misc variables
      BOOL     m_init;
      LONG     m_instrumentNum;
      double   m_values[MAX_CSVFILE_COLUMNS];

   };

   //////////////////////////////////////////////////////////////////////////////
   // CCSVLineArray
   class CCSVLineArray : public CObArray
   {
   friend class CCSVTextLine;
   public:
      ~CCSVLineArray();
   
      LONG Add(CCSVTextLine *pLine);
      CCSVTextLine *operator [](LONG index);
      CCSVTextLine *GetAt(LONG index);
   };


   //////////////////////////////////////////////////////////////////////////////
   // CCSVTextFile
   class CCSVTextFile : public CFile
   {
   public:
      CCSVTextFile(LPCTSTR fileName, UINT flags);
      ~CCSVTextFile();

      LPCTSTR Data();
      
   private:
      DWORD  m_length;
      BYTE * m_data;
   };

// main class
   friend class CCSVLineArray;
public:

   LONG ImportFile(LPCTSTR csvName, BOOL manual=FALSE, CRegisterUpdaterIF * =0);
   BOOL HandleTimer(LPCTSTR importFolder, CRegisterUpdaterIF *pParentInterface);

   double GetElement(LONG line, LONG index);
   LONG LineCount() {return((myArray?myArray->GetSize():0));}; // # instruments

   void Open(LPCTSTR fileName);
   BOOL  LoadedOK();

   LONG UpdateRegisters();


private:
   CString m_lastProcessed;
   SYSTEMTIME  m_lastInterval;

   CCSVLineArray *myArray;
   CRegisterUpdaterIF *m_parentInterface;
};

#endif // __CSVREADER_H_INCLUDED
