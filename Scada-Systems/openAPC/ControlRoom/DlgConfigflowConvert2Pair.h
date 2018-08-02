#ifndef DlgConfigflowConvert2Pair_H
#define DlgConfigflowConvert2Pair_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "oapcNoSpaceCtrl.h"
#include "flowConverter.h"
#include "DlgConfigflowName.h"


class flowConverterMixed2Char;

class DlgConfigflowConvert2Pair : public DlgConfigflowName  
{
public:
	DlgConfigflowConvert2Pair(flowConverter2Pair *object,bool toPair,wxWindow* parent, const wxString& title,bool hideISConfig);
	virtual ~DlgConfigflowConvert2Pair();

   DECLARE_CLASS(DlgConfigflowConvert2Pair)

private:
   void                     OnButtonClick(wxCommandEvent &event);
   void                     OnNumberFocusChanged(wxFocusEvent &event);


   flowConverter2Pair      *m_object;
   wxTextCtrl              *m_cmdNameField[MAX_NUM_IOS];
   DECLARE_EVENT_TABLE()

};

#endif
