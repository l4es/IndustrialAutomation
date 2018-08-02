#ifndef hmiLCDNumber_H
#define hmiLCDNumber_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/spinctrl.h>

#define MAX_NUM_ANGULARMETER_SECTORS 20

struct hmiLCDNumberData
{
   wxByte      digits,decimalPlaceDigits;
   wxUint16    pad2;
   wxUint32    reserved1,reserved2,reserved3;
};


class hmiLCDNumber : public hmiHGauge
{
public:
	hmiLCDNumber(BasePanel *parent,flowObject *obj);
	virtual ~hmiLCDNumber();

#ifdef ENV_EDITOR
   virtual wxString  getDefaultName();
   virtual wxPanel  *getConfigPanel(wxString *name,wxWindow *parent,wxInt32 num);
   virtual void      setConfigData();
   virtual void      setData(flowObject *object);
#endif
   virtual wxWindow *createUIElement();
   virtual void      doApplyData(const wxByte all);

protected:
#ifdef ENV_EDITOR
   virtual wxInt32   save(wxFile *FHandle);
#endif
   virtual wxInt32   load(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset);

private:
#ifdef ENV_EDITOR
   void               OnButtonClick(wxCommandEvent &event);
   wxSpinCtrl        *m_numDigits,*m_numDecimalPlaceDigits;
#endif
   struct hmiLCDNumberData lcdData;
};

#endif
