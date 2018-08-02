#ifndef HMIVSLIDER_H
#define HMIVSLIDER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class hmiVSlider : public hmiHSlider
{
public:
	hmiVSlider(BasePanel *parent,flowObject *obj);
	virtual ~hmiVSlider();

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
