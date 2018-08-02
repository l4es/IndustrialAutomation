#ifndef HMILINE_H
#define HMILINE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class hmiLine : public hmiObject
{
public:
	hmiLine(BasePanel *parent,flowObject *obj);
	virtual ~hmiLine();

#ifndef ENV_PLAYER
    virtual wxString  getDefaultName();
    virtual wxPoint   getMinSize();
#endif
    virtual wxWindow *createUIElement();

protected:
#ifdef ENV_EDITOR
    virtual wxInt32     save(wxFile *FHandle);
#endif
    virtual void        doApplyData(const wxByte all);

};

#endif
