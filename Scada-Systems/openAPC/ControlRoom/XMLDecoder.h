#ifndef XMLDECODER_H
#define XMLDECODER_H

#include <vector>

#include <wx/xml/xml.h>
#include <wx/event.h>
#include <wx/spinctrl.h>
#include <wx/bookctrl.h>

#include "oapcNoSpaceCtrl.h"
#include "DlgConfigflowBase.h"

class ExternalIOLib;

typedef enum
{
   TYPE_UNDEFINED  = 0,
   TYPE_INT        = 1,
   TYPE_FLOAT      = 2,
   TYPE_STRING     = 3,
   TYPE_OPTION     = 4,
   TYPE_COLORBUTTON= 5,
   TYPE_CHECKBOX   = 6,
   TYPE_unused= 7,
   TYPE_FILELOAD   = 8,
   TYPE_FILESAVE   = 9,
   TYPE_DIRSELECT  =10,
   TYPE_FONTBUTTON =11
} uiType;


class uiElementDefinition:public wxObject
{
public:
    uiElementDefinition();
    ~uiElementDefinition();

    wxInt32                           id,id2,idRW;
    wxString                          name,name2,text;
    uiType                            type,type2;
    wxInt32                           intValue,intValue2;
    wxFloat32                         min,min2,max,max2,floatValue,floatValue2;
    wxString                          stringValue,stringValue2,unit;
    wxWindow                         *uiElement,*uiElement2;
    wxStaticText                     *uiText,*uiUnit;
    wxByte                            disabled,panelNum,accuracy;
    wxFont                           *font;
    std::vector<uiElementDefinition*> masterEnableList; // list of checkboxes that all have to be checked to enable this element
    std::vector<uiElementDefinition*> slaveEnableList;  // list of elements that are about to be enabled/disabled depending on own checkbox state
    std::vector<uiElementDefinition*> masterDisableList; // list of checkboxes that all have to be checked to disable this element
    std::vector<uiElementDefinition*> slaveDisableList;  // list of elements that are about to be enabled/disabled depending on own checkbox state
};



class XMLDecoder:public DlgConfigflowBase
{
public:
   XMLDecoder(flowObject *object,wxWindow *parent,ExternalIOLib *extIOLib,void *instData);
   ~XMLDecoder();

   void              parseXMLDescr(wxXmlNode *node);
   void              setData(char *data);
   void              setInstanceData(void *instData);
   wxBitmap         *getFlowBitmap();
   void              inputChanged(wxCommandEvent &event);
   wxByte            getNextPair(wxString *name,wxString *value,wxFont *font);
   void              buttonClick(wxCommandEvent &event);
   wxScrolledWindow *getConfigPanel(bool *morePanelsAvailable,wxString *name,wxWindow *parent,wxInt32 num);
   void              constructUI(wxDialog *parent,wxString *objectName,bool *returnOK,wxUint64 stdOUT,wxUint32 *cycleMicros,bool hideISConfig);
   bool              getValuesFromUI();
   void              reInit();

   void            OnButtonClick(wxCommandEvent &event);
   void            OnCheckBoxCheck(wxCommandEvent &event);

   wxXmlNode  *m_dialogueXMLNode;

private:
#if wxCHECK_VERSION(3,0,0)
#else
   void                 toDouble(wxString &tmpString,double *d);
#endif
   void                 sendPanelParams();
   void                 OnInputChanged(wxCommandEvent &event);
   void                 cleanElementList();
   wxString             getUIDescription(uiElementDefinition *def);
   wxScrolledWindow    *constructUIBasicPanel(wxXmlNode *node,wxUint64 stdOUT,wxUint32 cycleMicros,wxString *panelName,wxByte panelNum);
   wxScrolledWindow    *constructUIStdPanel(wxXmlNode *node,wxString *panelName,wxWindow *parent,wxByte panelNum);
   wxScrolledWindow    *constructUIDualPanel(wxXmlNode *node,wxString *panelName,wxWindow *parent,wxByte panelNum);
   wxScrolledWindow    *constructUIParamPanel(wxXmlNode *node,wxWindow *parent,wxByte panelNum);
   uiElementDefinition *constructUIElements(wxXmlNode *node,wxScrolledWindow &parentPanel,wxSizer &sizer,wxInt32 rowNum,wxByte panelNum);
   wxScrolledWindow    *getConfigPanel(bool *morePanelsAvailable,wxString *name,wxWindow *parent,wxInt32 num,wxUint64 stdOUT,wxUint32 *cycleMicros);
   void                 updateWinSize();
   char                *base64Decode(const wxChar *input,size_t len,wxInt32 *outLen);
   bool                 setElemDefType(uiElementDefinition *elemDef,wxString &tmpString,wxPanel &parentPanel,wxComboBox **comboBox,int setNum);
   bool                 setElemDefDefault(uiElementDefinition *elemDef,wxString &tmpString);
   void                 setScrollParams(wxScrolledWindow *rootPanel,wxSizer *fSizer);
   void                 checkEnableDependencies(uiElementDefinition *elemDef);
   void                 checkDisableDependencies(uiElementDefinition *elemDef);
   void                 FloatCtrlCommandEvent(wxCommandEvent &event);
   void                 FloatCtrlFocusEvent(wxFocusEvent &event);
#ifndef ENV_BEAM
   wxScrolledWindow    *constructUIHelpPanel(wxXmlNode *node,wxWindow *parent);

   oapcNoSpaceCtrl *m_nameField;
#endif
   wxBitmap        *m_flowBitmap;
   wxSpinCtrl      *cycleTimeField;
   wxDialog        *m_parent;
   wxList          *uiElemList;
   wxString        *m_objectName;
   wxUint32        *m_cycleMicros;
   bool            *m_returnOK;
   wxNotebook      *m_book;
   wxBoxSizer      *m_bSizer;
   wxWindow        *m_dialogueParent;
   ExternalIOLib   *m_extIOLib;
   void            *m_instData;
   void            *m_eventMutex;

//   DECLARE_EVENT_TABLE()
};

#endif
