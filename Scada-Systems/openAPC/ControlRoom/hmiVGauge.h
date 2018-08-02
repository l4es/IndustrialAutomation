#ifndef HMIVGAUGE_H
#define HMIVGAUGE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class hmiVGauge : public hmiHGauge
{
public:
	hmiVGauge(BasePanel *parent,flowObject *obj);
	virtual ~hmiVGauge();

#ifdef ENV_EDITOR
    virtual wxString  getDefaultName();
#endif
    virtual wxWindow *createUIElement();

protected:
#ifdef ENV_EDITOR
    virtual wxInt32   save(wxFile *FHandle);
#endif

};

#endif
