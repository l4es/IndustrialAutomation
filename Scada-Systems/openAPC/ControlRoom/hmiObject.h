#ifdef ENV_INT
#include "../hmiObject.h"
#endif

#ifndef HMIOBJECT_H
#define HMIOBJECT_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/file.h>

#include "flowObject.h"
#include "flowConnection.h"
#include "BasePanel.h"
#include "LogDataCtrl.h"

class HMICanvas;


class hmiObject:public flowObject
{
public:
            hmiObject(BasePanel *parent,flowObject *obj);
   virtual ~hmiObject();

           void                           OnValueChanged(wxCommandEvent &event);
   virtual void                           setSize(wxRealPoint size);
   virtual wxRealPoint                    getSize();
   virtual void                           setPos(wxRealPoint pos);
   virtual void                           setParent(BasePanel *parent);
   virtual wxInt32                        getLoadError();
   virtual wxInt32                        load(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset);
#ifdef ENV_EDITOR
   virtual hmiObject                     *duplicate();
   virtual wxPoint                        getMinSize();
   virtual wxPoint                        getMaxSize();
   virtual wxPoint                        getDefSize();
   virtual wxString                       getDefaultName();
           wxInt32                        saveSOBJ(wxFile *FHandle,bool isCompiled);
   virtual wxPanel                       *getConfigPanel(wxString *name,wxWindow *parent,wxInt32 num);
   virtual void                           setConfigData();
#else
   virtual wxUint64                       getAssignedOutput(wxUint64 input);
   virtual void                           setSelected(bool selected);
   virtual void                           updateSelectionState();
   virtual void                           setUIElementState(wxInt32 state);
#endif
   virtual wxRealPoint                    getPos();
   virtual void                           drawObject(wxAutoBufferedPaintDC *dc);
   virtual wxWindow                      *createUIElement();
   virtual void                           doApplyData(const wxByte all);
   virtual void                           applyData(wxByte all);
   virtual void                           applyFont(wxWindow *ui);
   virtual bool                           applyLayoutData();

   wxWindow            *uiElement;
#ifdef ENV_EDITOR
   wxUint32             noUIFlags,noFlowFlags;
   wxUint32             m_userPriviEnable;
#endif
   BasePanel           *m_parent;
   bool                 returnOK;
#ifdef ENV_PLAYER
   wxInt32              m_mutexID;
#endif

#define HMI_TYPE_MASK              0x000000FF
#define HMI_TYPE_SIMPLEBUTTON      0x00000001
#define HMI_TYPE_TOGGLEBUTTON      0x00000002
#define HMI_TYPE_HSLIDER           0x00000003
#define HMI_TYPE_VSLIDER           0x00000004
#define HMI_TYPE_TEXTLABEL         0x00000005
#define HMI_TYPE_LINE              0x00000006
#define HMI_TYPE_IMAGEBUTTON       0x00000007
#define HMI_TYPE_IMAGE             0x00000008
#define HMI_TYPE_HGAUGE            0x00000009
#define HMI_TYPE_VGAUGE            0x0000000A
#define HMI_TYPE_NUMFIELD          0x0000000B
#define HMI_TYPE_FLOATFIELD        0x0000000C
#define HMI_TYPE_TEXTFIELD         0x0000000D
#define HMI_TYPE_TABBEDPANE        0x0000000E
#define HMI_TYPE_STACKEDPANE       0x0000000F
#define HMI_TYPE_ADDITIONALPANE    0x00000010
#define HMI_TYPE_TOGGLEIMAGEBUTTON 0x00000011
#define HMI_TYPE_ANGULARMETER      0x00000012
#define HMI_TYPE_LCDNUMBER         0x00000013
#define HMI_TYPE_ANGULARREGULATOR  0x00000014
#define HMI_TYPE_EXTERNAL_LIB      0x00000015
#define HMI_TYPE_SINGLEPANEL       0x00000016
#define HMI_TYPE_RADIOBUTTON       0x00000017
#define HMI_TYPE_CHECKBOX          0x00000018
#define HMI_TYPE_SP_USERMANAGEMENT 0x00000019
#define HMI_TYPE_PASSWORDFIELD     0x0000001A
#define HMI_TYPE_FREELINE          0x0000001B
#define HMI_TYPE_FRAME             0x0000001C
#define HMI_TYPE_ELLIPSE           0x0000001D

#define HMI_STATE_FLAG_DISABLED 0x0001
#define HMI_STATE_FLAG_RO       0x0004

#define HMI_FLOWFLAG_ON_DIGI_IN1_HI_OUT6 0x0001 /** trigger data output on OUT 6 in case of a signal on digital input 0*/
#define HMI_FLOWFLAG_ON_DIGI_IN1_LO_OUT6 0x0002 /** trigger data output on OUT 6 in case of a signal on digital input 0*/
#define HMI_FLOWFLAG_ON_DIGI_IN1_HI_OUT7 0x0004 /** trigger data output on OUT 7 in case of a signal on digital input 0*/
#define HMI_FLOWFLAG_ON_DIGI_IN1_LO_OUT7 0x0008 /** trigger data output on OUT 7 in case of a signal on digital input 0*/
#define HMI_FLOWFLAG_ON_CHANGING_OUT6    0x0010 /** trigger data output on OUT 6 in case of value is changing actually (includes HMI_FLOWFLAG_ON_CHANGED); in case of sliders or permanently changing data sources this may cause massive data transmission*/
#define HMI_FLOWFLAG_ON_CHANGED_OUT6     0x0020 /** trigger data output on OUT 6 in case of value was changed */
#define HMI_FLOWFLAG_ON_CHANGING_OUT7    0x0040 /** trigger data output on OUT 7 in case of value is changing actually (includes HMI_FLOWFLAG_ON_CHANGED); in case of sliders or permanently changing data sources this may cause massive data transmission*/
#define HMI_FLOWFLAG_ON_CHANGED_OUT7     0x0080 /** trigger data output on OUT 7 in case of value was changed */

#define HMI_NOFLOWFLAG_ON_DIGI_IN1 0x0001 /** do not give option to react on digital input 1 */
#define HMI_NOFLOWFLAG_ON_CHANGING 0x0002 /** do not give option to react on changing value */
#define HMI_NOFLOWFLAG_ON_CHANGED  0x0004 /** do not give option to react on changed value */
#define HMI_NOFLOWFLAG_OUT6        0x0008 /** do not give options for output 6*/

protected:
   LogDataCtrl *logDataCtrl;

   void         triggerGUIAction(const int id,const wxByte all);

private:
    wxInt32     lastFont_pointSize,lastFont_style,lastFont_weight;
    wxString    lastFont_faceName;
    wxInt32     m_loadError;

};

#endif
