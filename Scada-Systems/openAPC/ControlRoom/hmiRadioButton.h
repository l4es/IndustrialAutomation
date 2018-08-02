#ifndef hmiRadioButton_H
#define hmiRadioButton_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "hmiToggleButton.h"

class hmiRadioButton : public hmiToggleButton
{
public:
	hmiRadioButton(BasePanel *parent,flowObject *obj,wxUint32 type);
	virtual ~hmiRadioButton();

   virtual wxWindow *createUIElement();
#ifdef ENV_EDITOR
   virtual wxInt32   save(wxFile *FHandle);
   virtual wxString  getDefaultName();
#endif

};

#endif
