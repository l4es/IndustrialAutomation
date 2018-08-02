#ifndef FLOWCANVAS_H
#define FLOWCANVAS_H

#include <list>

#include "hmiObject.h"
#include "flowGroup.h"
#include "ExternalMacros.h"

#define POPUP_PUT_CONTROL           0x1001
#define POPUP_DEL_CONNECTION        0x1002
#define POPUP_DEL_INPUT             0x1003
#define POPUP_DEL_ELEM              0x1004
#define POPUP_EDIT_ELEM             0x1005
#define POPUP_EXTEND_TO_LEFT        0x1006
#define POPUP_EXTEND_TO_RIGHT       0x1007
#define POPUP_EXTEND_TO_TOP         0x1008
#define POPUP_EXTEND_TO_BOTTOM      0x1009
#define POPUP_DEL_UNUSED_CONN       0x100A
#define POPUP_GROUP_FLOWOBJECTS     0x100C
#define POPUP_GROUP_ENTER           0x100D
#define POPUP_GROUP_LEAVE           0x100E
#define POPUP_ADD_INCONNECTOR_DIGI  0x100F
#define POPUP_ADD_OUTCONNECTOR_DIGI 0x1010
#define POPUP_ADD_INCONNECTOR_NUM   0x1011
#define POPUP_ADD_OUTCONNECTOR_NUM  0x1012
#define POPUP_ADD_INCONNECTOR_CHAR  0x1013
#define POPUP_ADD_OUTCONNECTOR_CHAR 0x1014
#define POPUP_GROUP_SAVE            0x1015
#define POPUP_GROUP_LOAD            0x1016
#define POPUP_ADD_CONV_DIGI2NUM     0x1017
#define POPUP_ADD_CONV_DIGI2CHAR    0x1018
#define POPUP_ADD_CONV_NUM2DIGI     0x1019
#define POPUP_ADD_CONV_NUM2CHAR     0x1020
#define POPUP_ADD_CONV_CHAR2DIGI    0x1021
#define POPUP_ADD_CONV_CHAR2NUM     0x1022
#define POPUP_ADD_CONV_MIXED2CHAR   0x1023
#define POPUP_ADD_CONV_NUM2BITS     0x1024
#define POPUP_ADD_LOGI_DIGINOT      0x1025
#define POPUP_ADD_LOGI_NUMNOT       0x1026
#define POPUP_ADD_LOGI_DIGIOR       0x1027
#define POPUP_ADD_LOGI_NUMOR        0x1028
#define POPUP_ADD_LOGI_DIGIXOR      0x1029
#define POPUP_ADD_LOGI_NUMXOR       0x1030
#define POPUP_ADD_LOGI_DIGIAND      0x1031
#define POPUP_ADD_LOGI_NUMAND       0x1033
#define POPUP_ADD_LOGI_NUMADD       0x1034
#define POPUP_ADD_LOGI_NUMSUB       0x1035
#define POPUP_ADD_LOGI_NUMMUL       0x1036
#define POPUP_ADD_LOGI_NUMDIV       0x1037
#define POPUP_ADD_FLOW_START        0x1038
#define POPUP_ADD_FLOW_STOP         0x1039
#define POPUP_ADD_FLOW_DELAY        0x1040
#define POPUP_ADD_FLOW_TIMER        0x1041
#define POPUP_ADD_MISC_LOG          0x1042
#define POPUP_ADD_MISC_LOG_REC      0x1043
#define POPUP_ADD_INCONNECTOR_BIN   0x1044
#define POPUP_ADD_OUTCONNECTOR_BIN  0x1045
#define POPUP_ADD_CONV_DIGI2PAIR    0x1046
#define POPUP_ADD_CONV_NUM2PAIR     0x1047
#define POPUP_ADD_CONV_CHAR2PAIR    0x1048
#define POPUP_ADD_CONV_BIN2PAIR     0x1049
#define POPUP_ADD_FLOW_NUMTGATE     0x1050
#define POPUP_ADD_FLOW_CHARTGATE    0x1051
#define POPUP_ADD_FLOW_BINTGATE     0x1052
#define POPUP_ADD_FLOW_DIGITGATE    0x1053
#define POPUP_ADD_CONV_PAIR2CHAR    0x1054
#define POPUP_ADD_CONV_PAIR2DIGI    0x1055
#define POPUP_ADD_CONV_PAIR2NUM     0x1056
#define POPUP_ADD_CONV_PAIR2BIN     0x1057
#define POPUP_ADD_LOGI_DIGINOP      0x1058
#define POPUP_ADD_LOGI_NUMNOP       0x1059
#define POPUP_ADD_LOGI_CHARNOP      0x1060
#define POPUP_ADD_LOGI_BINNOP       0x1061
#define POPUP_ADD_REPEAT            0x1062
#define POPUP_ADD_LOGI_DIGISHIFT    0x1063
#define POPUP_ADD_LOGI_NUMSHIFT     0x1064
#define POPUP_ADD_LOGI_CHARSHIFT    0x1065
#define POPUP_ADD_LOGI_BINSHIFT     0x1066
#define POPUP_EXTEND_INSERT_ROW     0x1067
#define POPUP_EXTEND_INSERT_COL     0x1068
#define POPUP_ADD_FLOW_NUMGATE      0x1069
#define POPUP_ADD_FLOW_CHARGATE     0x1070
#define POPUP_ADD_FLOW_BINGATE      0x1071
#define POPUP_ADD_FLOW_DIGIGATE     0x1072
#define POPUP_ADD_LOGI_DIGIRSFF     0x1073
#define POPUP_ADD_MATH_DIGICMP      0x1074
#define POPUP_ADD_MATH_NUMCMP       0x1075
#define POPUP_ADD_MATH_CHARCMP      0x1076
#define POPUP_ADD_MATH_DIGICTR      0x1077
#define POPUP_ADD_MATH_NUMCTR       0x1078
#define POPUP_ADD_MATH_CHARCTR      0x1079
#define POPUP_ADD_MATH_BINCTR       0x1080
#define POPUP_ADD_LOGI_DIGITFF      0x1081
#define POPUP_ADD_MISC_LOGIN        0x1082
#define POPUP_ADD_MISC_ISCO         0x1083

#define BUTTON_FOLDBAR              0x2001

class wxFoldPanelBar;
class wxFoldPanel;

class FlowCanvas : public wxPanel
{
public:
	FlowCanvas(wxScrolledWindow *parent);
	virtual ~FlowCanvas();

   int             OnKeyDown(wxKeyEvent& event);
   void            addDeviceSubmenu(wxMenu *addMenu);

   void            refreshProject(wxByte setToTop);
   void            showElement(flowObject *object);
   void            resetPointers();
   wxFoldPanelBar *createFoldBar(wxWindow *parent);
   void            addFoldBarSubmenu(wxFoldPanelBar *bar,bool setLocalEvent);

private:
   void    ShowContextMenu();
   void    OnFoldbarClicked(wxMouseEvent& event);
   void    OnEditElem(wxCommandEvent& event);
   void    OnMouseEvent(wxMouseEvent& event);
   void    OnPaint(wxPaintEvent& event);
   void    OnPutControl(wxCommandEvent& event);
   void    OnDelConnection(wxCommandEvent& event);
   void    OnDelInput(wxCommandEvent& event);
   void    OnDelElem(wxCommandEvent& event);
   void    OnPopupMenu(wxCommandEvent& event);
   void    OnLeaveGroup(wxCommandEvent &event);
   void    OnSaveGroup(wxCommandEvent &event);
   void    OnLoadGroup(wxCommandEvent &event);
   void    OnEnterGroup(wxCommandEvent &event);

   void              loadGroup(wxString filepath,bool display);
   void              drawInLines(flowObject *currentObject,ObjectList *drawObjectList,wxAutoBufferedPaintDC *dc);
   wxInt32           addFoldBarItem(wxFoldPanelBar *bar,wxFoldPanel *foldItem,wxInt32 id,wxString name,bool setLocalEvent);

   wxInt32              m_lastElementID,m_nextLeftClickOperation;
   ExternalMacros      *m_externalMacros;

   wxScrolledWindow    *parent;
   wxPoint              lastMousePos,startMousePos,rectStart,rectEnd;
   wxBitmap            *bm1in1out,*bm2in2out,*bm1in1out1cin,*bm2in2out1cin,*bmunknown,*bm1in1out1nin2nout,*bm2in1out1nin2nout,*bm1in1out1nin1nout,*bm2in1out1cin2cout;
   wxBitmap            *bmimgbutton,*bmimg;
   wxBitmap            *bmConChar,*bmConDigi,*bmConNum,*bmConBin;
   wxBitmap            *bmSelEnDigiSelEnDisNum,*bmSelEnDigi;
   wxBitmap            *bmDigi2Num,*bmDigi2Char,*bmNum2Digi,*bmNum2Char,*bmChar2Digi,*bmChar2Num,*bmMixed2Char,*bmNum2Bits;
   wxBitmap            *bmDigi2Pair,*bmNum2Pair,*bmChar2Pair,*bmBin2Pair,*bmPair2Char,*bmPair2Digi,*bmPair2Num,*bmPair2Bin;
   wxBitmap            *bmDigiNOT,*bmNumNOT,*bmDigiOR,*bmNumOR,*bmDigiXOR,*bmNumXOR,*bmDigiAND,*bmNumAND,*bmDigiRSFF,*bmDigiTFF;
   wxBitmap            *bmDigiNOP,*bmNumNOP,*bmCharNOP,*bmBinNOP,*bmDigiShift,*bmNumShift,*bmCharShift,*bmBinShift;
   wxBitmap            *bmNumCmp,*bmDigiCmp,*bmCharCmp,*bmDigiCtr,*bmNumCtr,*bmCharCtr,*bmBinCtr;
   wxBitmap            *bmNumAdd,*bmNumSub,*bmNumMul,*bmNumDiv;
   wxBitmap            *bmFlowStart,*bmFlowStop,*bmFlowDelay,*bmFlowTimer,*bmFlowNumTGate,*bmFlowCharTGate,*bmFlowBinTGate,*bmFlowDigiTGate;
   wxBitmap            *bmFlowNumGate,*bmFlowCharGate,*bmFlowBinGate,*bmFlowDigiGate;
   wxBitmap            *bmMiscLog,*bmMiscLogRec,*bmMiscLogIn,*bmMiscISCO;
   flowObject          *currentObject,*inputObject;
   flowGroup           *m_currentGroupObject;
   FlowConnection      *currentFlowCon;
   wxInt32              lockMode,currentFlowLineIdx;
   wxUint64             currentOutConnector,currentInConnector;
   wxByte               drawDir,isMouseDrag,isMouseDown,m_checkIncoming;
   ObjectList          *m_drawObjectList;
   wxString             m_groupDir,m_groupFile;
   flowObject          *highlightObject; // highlight this after search operation
   wxFoldPanelBar      *m_flowBar;
   std::list<wxStaticText*> m_foldBarList;

   DECLARE_EVENT_TABLE()
};

#endif
