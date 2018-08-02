#ifndef hmiAngularRegulator_H
#define hmiAngularRegulator_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_NUM_ANGULARREGULATOR_TAGS 20

struct hmiAngularRegulatorData
{
   wxInt32     numTags,reserved1;
   wxInt32     startAngle,endAngle; // 1/10 degrees
   wxUint32    reserved2,reserved3;
   wxUint32    extCircleColour[2],intCircleColour[2],knobBorderColour[2],knobColour[2],limitsColour[2],tagsColour[2];
};



class hmiAngularRegulator : public hmiHSlider
{
public:
	hmiAngularRegulator(BasePanel *parent,flowObject *obj);
	virtual ~hmiAngularRegulator();

#ifdef ENV_EDITOR
    virtual wxString  getDefaultName();
    virtual wxPanel  *getConfigPanel(wxString *name,wxWindow *parent,wxInt32 num);
    virtual void      setConfigData();
    virtual wxPoint   getMinSize();
    virtual void      setData(flowObject *object);
#else
    virtual wxFloat64 getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
    virtual void      setUIElementState(wxInt32 state);
#endif
    virtual wxWindow *createUIElement();
    virtual void      doApplyData(const wxByte all);

protected:
#ifdef ENV_EDITOR
    virtual wxInt32   save(wxFile *FHandle);
#endif
    virtual wxInt32   load(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset);

private:
   void              setColours();
#ifdef ENV_EDITOR
   void              OnButtonClick(wxCommandEvent &event);

   wxButton         *bgLow[6],*bgHigh[6];
   wxSpinCtrl       *m_numTicks;
   wxTextCtrl       *m_sAngle,*m_eAngle;
   wxStaticText     *bgText[6];
#else
   wxUint32          disExtCircleColour[2],disIntCircleColour[2],disKnobBorderColour[2],disKnobColour[2],
                     disLimitsColour[2],disTagsColour[2],disBgColour[2],disFgColour[2];
#endif

   struct hmiAngularRegulatorData meterData;
};

#endif
