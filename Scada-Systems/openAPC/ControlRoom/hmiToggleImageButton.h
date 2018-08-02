#ifndef HMITOGGLEIMAGEBUTTON_H
#define HMITOGGLEIMAGEBUTTON_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class hmiToggleImageButton : public hmiImageButton
{
public:
	hmiToggleImageButton(BasePanel *parent,flowObject *obj);
	virtual ~hmiToggleImageButton();

#ifndef ENV_PLAYER
    virtual wxString  getDefaultName();
    virtual void      setData(flowObject *object);
#endif
    virtual wxWindow *createUIElement();


protected:
#ifdef ENV_EDITOR
    virtual wxInt32     save(wxFile *FHandle);
#endif
    virtual void        doApplyData(const wxByte all);

private:

};

#endif

