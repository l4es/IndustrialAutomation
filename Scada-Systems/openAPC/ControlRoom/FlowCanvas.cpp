/******************************************************************************

This file is part of ControlRoom process control/HMI software.

ControlRoom is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

ControlRoom is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
details.

You should have received a copy of the GNU General Public License along with
ControlRoom. If not, see http://www.gnu.org/licenses/

*******************************************************************************

For different licensing and/or usage of the sources apart from GPL or any other
open source license, please contact us at https://openapc.com/contact.php

*******************************************************************************/

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/filedlg.h>
#include <wx/accel.h>

#include "wx/foldbar/foldpanelbar.h"

#include "globals.h"
#include "oapc_libio.h"
#include "iff.h"
#include "MainWin.h"
#include "hmiObject.h"
#include "FlowCanvas.h"
#include "DlgControlSelector.h"
#include "ExternalIOLib.h"
#include "ExternalMacros.h"
#include "ExternalMacro.h"
#include "PlugInPanel.h"
#include "flowExternalIOLib.h"
#include "flowGroup.h"
#include "IOElementDefinition.h"
#include "flowConverterDigi2Num.h"
#include "flowConverterDigi2Char.h"
#include "flowConverterChar2Num.h"
#include "flowConverterChar2Digi.h"
#include "flowConverterNum2Digi.h"
#include "flowConverterNum2Char.h"
#include "flowConverterMixed2Char.h"
#include "flowConverterNum2Bits.h"
#include "flowConverterDigi2Pair.h"
#include "flowConverterNum2Pair.h"
#include "flowConverterChar2Pair.h"
#include "flowConverterPair2Digi.h"
#include "flowConverterPair2Char.h"
#include "flowConverterPair2Num.h"
#include "flowConverterPair2Bin.h"
#include "flowConverterBin2Pair.h"
#include "flowLogicDigiRSFF.h"
#include "flowLogicDigiTFF.h"
#include "flowLogicDigiNOT.h"
#include "flowLogicNumNOT.h"
#include "flowLogicDigiNOP.h"
#include "flowLogicNumNOP.h"
#include "flowLogicCharNOP.h"
#include "flowLogicBinNOP.h"
#include "flowLogicDigiShift.h"
#include "flowLogicNumShift.h"
#include "flowLogicCharShift.h"
#include "flowLogicBinShift.h"
#include "flowLogicDigiConcat.h"
#include "flowLogicNumConcat.h"
#include "flowMathDigiCmp.h"
#include "flowMathNumCmp.h"
#include "flowMathCharCmp.h"
#include "flowMathDigiCtr.h"
#include "flowMathNumCtr.h"
#include "flowMathCharCtr.h"
#include "flowMathBinCtr.h"
#include "flowStop.h"
#include "flowNumTrigGate.h"
#include "flowNumGate.h"
#include "flowCharTrigGate.h"
#include "flowCharGate.h"
#include "flowBinTrigGate.h"
#include "flowBinGate.h"
#include "flowDigiTrigGate.h"
#include "flowDigiGate.h"
#include "flowDelay.h"
#include "flowStart.h"
#include "flowTimer.h"
#include "flowLog.h"
#include "flowLogRecorder.h"
#include "flowLogInUser.h"
#include "flowISConnect.h"

#ifdef ENV_LINUX
#include <arpa/inet.h>
#endif

BEGIN_EVENT_TABLE(FlowCanvas, wxPanel)
  EVT_PAINT(FlowCanvas::OnPaint)
  EVT_MOUSE_EVENTS(FlowCanvas::OnMouseEvent)
  EVT_MENU(POPUP_PUT_CONTROL,          FlowCanvas::OnPutControl)
  EVT_MENU(POPUP_DEL_CONNECTION,       FlowCanvas::OnDelConnection)
  EVT_MENU(POPUP_DEL_UNUSED_CONN,      FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_DEL_INPUT,            FlowCanvas::OnDelInput)
  EVT_MENU(POPUP_DEL_ELEM,             FlowCanvas::OnDelElem)
  EVT_MENU(POPUP_EDIT_ELEM,            FlowCanvas::OnEditElem)
  EVT_MENU(POPUP_EXTEND_TO_LEFT,       FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_EXTEND_TO_RIGHT,      FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_EXTEND_TO_TOP,        FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_EXTEND_TO_BOTTOM,     FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_EXTEND_INSERT_ROW,    FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_EXTEND_INSERT_COL,    FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_INCONNECTOR_DIGI, FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_OUTCONNECTOR_DIGI,FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_INCONNECTOR_NUM,  FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_OUTCONNECTOR_NUM, FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_INCONNECTOR_CHAR, FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_INCONNECTOR_BIN,  FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_OUTCONNECTOR_CHAR,FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_OUTCONNECTOR_BIN, FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_GROUP_FLOWOBJECTS,    FlowCanvas::OnPopupMenu)

  EVT_MENU(POPUP_ADD_CONV_DIGI2NUM,    FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_CONV_DIGI2CHAR,   FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_CONV_NUM2DIGI,    FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_CONV_NUM2CHAR,    FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_CONV_CHAR2DIGI,   FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_CONV_CHAR2NUM,    FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_CONV_MIXED2CHAR,  FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_CONV_NUM2BITS,    FlowCanvas::OnPopupMenu)

  EVT_MENU(POPUP_ADD_CONV_DIGI2PAIR,   FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_CONV_NUM2PAIR,    FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_CONV_CHAR2PAIR,   FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_CONV_BIN2PAIR,    FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_CONV_PAIR2CHAR,   FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_CONV_PAIR2DIGI,   FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_CONV_PAIR2NUM,    FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_CONV_PAIR2BIN,    FlowCanvas::OnPopupMenu)

  EVT_MENU(POPUP_ADD_LOGI_DIGIRSFF,    FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_LOGI_DIGITFF,     FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_LOGI_DIGINOT,     FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_LOGI_NUMNOT,      FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_LOGI_DIGINOP,     FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_LOGI_NUMNOP,      FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_LOGI_CHARNOP,     FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_LOGI_BINNOP,      FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_LOGI_DIGIOR,      FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_LOGI_NUMOR,       FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_LOGI_DIGIXOR,     FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_LOGI_NUMXOR,      FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_LOGI_DIGIAND,     FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_LOGI_NUMAND,      FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_LOGI_NUMADD,      FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_LOGI_NUMSUB,      FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_LOGI_NUMMUL,      FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_LOGI_NUMDIV,      FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_LOGI_NUMSHIFT,    FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_LOGI_DIGISHIFT,   FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_LOGI_CHARSHIFT,   FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_LOGI_BINSHIFT,    FlowCanvas::OnPopupMenu)

  EVT_MENU(POPUP_ADD_MATH_DIGICMP,     FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_MATH_NUMCMP,      FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_MATH_CHARCMP,     FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_MATH_DIGICTR,     FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_MATH_NUMCTR,      FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_MATH_CHARCTR,     FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_MATH_BINCTR,      FlowCanvas::OnPopupMenu)

  EVT_MENU(POPUP_ADD_FLOW_START,       FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_FLOW_STOP,        FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_FLOW_DELAY,       FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_FLOW_TIMER,       FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_FLOW_NUMTGATE,    FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_FLOW_CHARTGATE,   FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_FLOW_BINTGATE,    FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_FLOW_DIGITGATE,   FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_FLOW_NUMGATE,     FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_FLOW_CHARGATE,    FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_FLOW_BINGATE,     FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_FLOW_DIGIGATE,    FlowCanvas::OnPopupMenu)

  EVT_MENU(POPUP_ADD_MISC_LOG,         FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_MISC_LOG_REC,     FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_MISC_LOGIN,       FlowCanvas::OnPopupMenu)
  EVT_MENU(POPUP_ADD_MISC_ISCO,        FlowCanvas::OnPopupMenu)

  EVT_MENU(POPUP_GROUP_ENTER,          FlowCanvas::OnEnterGroup)
  EVT_MENU(POPUP_GROUP_LEAVE,          FlowCanvas::OnLeaveGroup)
  EVT_MENU(POPUP_GROUP_SAVE,           FlowCanvas::OnSaveGroup)
  EVT_MENU(POPUP_GROUP_LOAD,           FlowCanvas::OnLoadGroup)
END_EVENT_TABLE()

#define DRAW_DIR_VERT_DOWN        1
#define DRAW_DIR_VERT             2
#define DRAW_DIR_HORIZONTAL       3
#define DRAW_DIR_HORIZONTAL_RIGHT 4

#define LOCK_NONE              0
#define LOCK_TO_CONNECTOR      1
#define LOCK_TO_FLOWLINE_HORIZ 2
#define LOCK_TO_FLOWLINE_VERT  3
#define LOCK_TO_EOLINE         4


FlowCanvas::FlowCanvas(wxScrolledWindow *parent)
           :wxPanel(parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL | wxNO_BORDER|wxWANTS_CHARS)
{
   this->parent=parent;
   m_lastElementID=0;
   m_externalMacros=new ExternalMacros();
//   SetBackgroundColour(*wxWHITE);
   bm1in1out=new wxBitmap(_T(DATA_PATH"img/1in1out.png"),wxBITMAP_TYPE_PNG);
   bm2in2out=new wxBitmap(_T(DATA_PATH"img/2in2out.png"),wxBITMAP_TYPE_PNG);
   bm1in1out1cin=new wxBitmap(_T(DATA_PATH"img/1in1out1cin.png"),wxBITMAP_TYPE_PNG);
   bm2in2out1cin=new wxBitmap(_T(DATA_PATH"img/2in2out1cin.png"),wxBITMAP_TYPE_PNG);
   bm2in1out1nin2nout=new wxBitmap(_T(DATA_PATH"img/2in1out1nin2nout.png"),wxBITMAP_TYPE_PNG);
   bm1in1out1nin2nout=new wxBitmap(_T(DATA_PATH"img/1in1out1nin2nout.png"),wxBITMAP_TYPE_PNG);
   bm1in1out1nin1nout=new wxBitmap(_T(DATA_PATH"img/1in1out1nin1nout.png"),wxBITMAP_TYPE_PNG);
   bm2in1out1cin2cout=new wxBitmap(_T(DATA_PATH"img/2in1out1cin2cout.png"),wxBITMAP_TYPE_PNG);
   bmunknown=new wxBitmap(_T(DATA_PATH"img/unknown.png"),wxBITMAP_TYPE_PNG);
   bmConChar=new wxBitmap(_T(DATA_PATH"img/connector_char.png"),wxBITMAP_TYPE_PNG);
   bmConDigi=new wxBitmap(_T(DATA_PATH"img/connector_digi.png"),wxBITMAP_TYPE_PNG);
   bmConNum=new wxBitmap(_T(DATA_PATH"img/connector_num.png"),wxBITMAP_TYPE_PNG);
   bmConBin=new wxBitmap(_T(DATA_PATH"img/connector_bin.png"),wxBITMAP_TYPE_PNG);
   bmSelEnDigiSelEnDisNum=new wxBitmap(_T(DATA_PATH"img/selendigi_selendisnum.png"),wxBITMAP_TYPE_PNG);
   bmSelEnDigi=new wxBitmap(_T(DATA_PATH"img/selendigi.png"),wxBITMAP_TYPE_PNG);

   bmimgbutton=new wxBitmap(_T(DATA_PATH"img/imgbutton.png"),wxBITMAP_TYPE_PNG);
   bmimg=new wxBitmap(_T(DATA_PATH"img/img.png"),wxBITMAP_TYPE_PNG);

   bmDigi2Num=new wxBitmap(_T(DATA_PATH"img/digi2num.png"),wxBITMAP_TYPE_PNG);
   bmDigi2Char=new wxBitmap(_T(DATA_PATH"img/digi2char.png"),wxBITMAP_TYPE_PNG);
   bmNum2Digi=new wxBitmap(_T(DATA_PATH"img/num2digi.png"),wxBITMAP_TYPE_PNG);
   bmNum2Bits=new wxBitmap(_T(DATA_PATH"img/num2bits.png"),wxBITMAP_TYPE_PNG);
   bmNum2Char=new wxBitmap(_T(DATA_PATH"img/num2char.png"),wxBITMAP_TYPE_PNG);
   bmChar2Digi=new wxBitmap(_T(DATA_PATH"img/char2digi.png"),wxBITMAP_TYPE_PNG);
   bmChar2Num=new wxBitmap(_T(DATA_PATH"img/char2num.png"),wxBITMAP_TYPE_PNG);
   bmMixed2Char=new wxBitmap(_T(DATA_PATH"img/mixed2char.png"),wxBITMAP_TYPE_PNG);

   bmDigi2Pair=new wxBitmap(_T(DATA_PATH"img/digi2pair.png"),wxBITMAP_TYPE_PNG);
   bmNum2Pair=new wxBitmap(_T(DATA_PATH"img/num2pair.png"),wxBITMAP_TYPE_PNG);
   bmChar2Pair=new wxBitmap(_T(DATA_PATH"img/char2pair.png"),wxBITMAP_TYPE_PNG);
   bmBin2Pair=new wxBitmap(_T(DATA_PATH"img/bin2pair.png"),wxBITMAP_TYPE_PNG);
   bmPair2Char=new wxBitmap(_T(DATA_PATH"img/pair2char.png"),wxBITMAP_TYPE_PNG);
   bmPair2Digi=new wxBitmap(_T(DATA_PATH"img/pair2digi.png"),wxBITMAP_TYPE_PNG);
   bmPair2Num=new wxBitmap(_T(DATA_PATH"img/pair2num.png"),wxBITMAP_TYPE_PNG);
   bmPair2Bin=new wxBitmap(_T(DATA_PATH"img/pair2bin.png"),wxBITMAP_TYPE_PNG);

   bmDigiRSFF=new wxBitmap(_T(DATA_PATH"img/digirsff.png"),wxBITMAP_TYPE_PNG);
   bmDigiTFF=new wxBitmap(_T(DATA_PATH"img/digitff.png"),wxBITMAP_TYPE_PNG);
   bmDigiNOT=new wxBitmap(_T(DATA_PATH"img/diginot.png"),wxBITMAP_TYPE_PNG);
   bmNumNOT=new wxBitmap(_T(DATA_PATH"img/numnot.png"),wxBITMAP_TYPE_PNG);
   bmDigiNOP=new wxBitmap(_T(DATA_PATH"img/diginop.png"),wxBITMAP_TYPE_PNG);
   bmNumNOP=new wxBitmap(_T(DATA_PATH"img/numnop.png"),wxBITMAP_TYPE_PNG);
   bmDigiCmp=new wxBitmap(_T(DATA_PATH"img/digicmp.png"),wxBITMAP_TYPE_PNG);
   bmNumCmp=new wxBitmap(_T(DATA_PATH"img/numcmp.png"),wxBITMAP_TYPE_PNG);
   bmCharCmp=new wxBitmap(_T(DATA_PATH"img/charcmp.png"),wxBITMAP_TYPE_PNG);
   bmDigiCtr=new wxBitmap(_T(DATA_PATH"img/digictr.png"),wxBITMAP_TYPE_PNG);
   bmNumCtr=new wxBitmap(_T(DATA_PATH"img/numctr.png"),wxBITMAP_TYPE_PNG);
   bmCharCtr=new wxBitmap(_T(DATA_PATH"img/charctr.png"),wxBITMAP_TYPE_PNG);
   bmBinCtr=new wxBitmap(_T(DATA_PATH"img/binctr.png"),wxBITMAP_TYPE_PNG);
   bmCharNOP=new wxBitmap(_T(DATA_PATH"img/charnop.png"),wxBITMAP_TYPE_PNG);
   bmBinNOP=new wxBitmap(_T(DATA_PATH"img/binnop.png"),wxBITMAP_TYPE_PNG);
   bmDigiOR=new wxBitmap(_T(DATA_PATH"img/digior.png"),wxBITMAP_TYPE_PNG);
   bmNumOR=new wxBitmap(_T(DATA_PATH"img/numor.png"),wxBITMAP_TYPE_PNG);
   bmDigiXOR=new wxBitmap(_T(DATA_PATH"img/digixor.png"),wxBITMAP_TYPE_PNG);
   bmNumXOR=new wxBitmap(_T(DATA_PATH"img/numxor.png"),wxBITMAP_TYPE_PNG);
   bmDigiAND=new wxBitmap(_T(DATA_PATH"img/digiand.png"),wxBITMAP_TYPE_PNG);
   bmNumAND=new wxBitmap(_T(DATA_PATH"img/numand.png"),wxBITMAP_TYPE_PNG);
   bmNumAdd=new wxBitmap(_T(DATA_PATH"img/numadd.png"),wxBITMAP_TYPE_PNG);
   bmNumSub=new wxBitmap(_T(DATA_PATH"img/numsub.png"),wxBITMAP_TYPE_PNG);
   bmNumMul=new wxBitmap(_T(DATA_PATH"img/nummul.png"),wxBITMAP_TYPE_PNG);
   bmNumDiv=new wxBitmap(_T(DATA_PATH"img/numdiv.png"),wxBITMAP_TYPE_PNG);
   bmDigiShift=new wxBitmap(_T(DATA_PATH"img/digishift.png"),wxBITMAP_TYPE_PNG);
   bmNumShift=new wxBitmap(_T(DATA_PATH"img/numshift.png"),wxBITMAP_TYPE_PNG);
   bmCharShift=new wxBitmap(_T(DATA_PATH"img/charshift.png"),wxBITMAP_TYPE_PNG);
   bmBinShift=new wxBitmap(_T(DATA_PATH"img/binshift.png"),wxBITMAP_TYPE_PNG);

   bmFlowStart=new wxBitmap(_T(DATA_PATH"img/flowstart.png"),wxBITMAP_TYPE_PNG);
   bmFlowStop=new wxBitmap(_T(DATA_PATH"img/flowstop.png"),wxBITMAP_TYPE_PNG);
   bmFlowDelay=new wxBitmap(_T(DATA_PATH"img/flowdelay.png"),wxBITMAP_TYPE_PNG);
   bmFlowTimer=new wxBitmap(_T(DATA_PATH"img/flowtimer.png"),wxBITMAP_TYPE_PNG);
   bmFlowNumTGate=new wxBitmap(_T(DATA_PATH"img/numgate.png"),wxBITMAP_TYPE_PNG);
   bmFlowCharTGate=new wxBitmap(_T(DATA_PATH"img/chargate.png"),wxBITMAP_TYPE_PNG);
   bmFlowBinTGate=new wxBitmap(_T(DATA_PATH"img/bingate.png"),wxBITMAP_TYPE_PNG);
   bmFlowDigiTGate=new wxBitmap(_T(DATA_PATH"img/digigate.png"),wxBITMAP_TYPE_PNG);
   bmFlowNumGate=new wxBitmap(_T(DATA_PATH"img/numsgate.png"),wxBITMAP_TYPE_PNG);
   bmFlowCharGate=new wxBitmap(_T(DATA_PATH"img/charsgate.png"),wxBITMAP_TYPE_PNG);
   bmFlowBinGate=new wxBitmap(_T(DATA_PATH"img/binsgate.png"),wxBITMAP_TYPE_PNG);
   bmFlowDigiGate=new wxBitmap(_T(DATA_PATH"img/digisgate.png"),wxBITMAP_TYPE_PNG);

   bmMiscLog=new wxBitmap(_T(DATA_PATH"img/log.png"),wxBITMAP_TYPE_PNG);
   bmMiscLogRec=new wxBitmap(_T(DATA_PATH"img/logrec.png"),wxBITMAP_TYPE_PNG);
   bmMiscLogIn=new wxBitmap(_T(DATA_PATH"img/login.png"),wxBITMAP_TYPE_PNG);
   bmMiscISCO=new wxBitmap(_T(DATA_PATH"img/ISCO.png"),wxBITMAP_TYPE_PNG);

   currentFlowCon=NULL;
   currentObject=NULL;
   inputObject=NULL;
   currentOutConnector=0;
   currentInConnector=0;
   currentFlowLineIdx=-1;
   lockMode=LOCK_NONE;
   isMouseDrag=0;
   isMouseDown=0;
   SetBackgroundStyle(wxBG_STYLE_CUSTOM);
   rectStart.x=-1;
   rectEnd.x=-1;
   m_drawObjectList=&g_objectList;
   m_currentGroupObject=NULL;
   highlightObject=NULL;
   m_nextLeftClickOperation=0;
}



FlowCanvas::~FlowCanvas()
{
   resetPointers();
   delete m_externalMacros;
   if (bm1in1out) delete bm1in1out;
   if (bm2in2out) delete bm2in2out;
   if (bm1in1out1cin) delete bm1in1out1cin;
   if (bm2in2out1cin) delete bm2in2out1cin;
   if (bm2in1out1nin2nout) delete bm2in1out1nin2nout;
   if (bm1in1out1nin2nout) delete bm1in1out1nin2nout;
   if (bm1in1out1nin1nout) delete bm1in1out1nin1nout;
   if (bm2in1out1cin2cout) delete bm2in1out1cin2cout;
   if (bmunknown) delete bmunknown;
   if (bmConChar) delete bmConChar;
   if (bmConDigi) delete bmConDigi;
   if (bmConNum) delete bmConNum;
   if (bmConBin) delete bmConBin;
   if (bmSelEnDigiSelEnDisNum) delete bmSelEnDigiSelEnDisNum;
   if (bmSelEnDigi) delete bmSelEnDigi;

   if (bmimgbutton) delete bmimgbutton;
   if (bmimg) delete bmimg;

   if (bmDigi2Num) delete bmDigi2Num;
   if (bmDigi2Char) delete bmDigi2Char;
   if (bmNum2Digi) delete bmNum2Digi;
   if (bmNum2Bits) delete bmNum2Bits;
   if (bmNum2Char) delete bmNum2Char;
   if (bmChar2Digi) delete bmChar2Digi;
   if (bmChar2Num) delete bmChar2Num;
   if (bmMixed2Char) delete bmMixed2Char;

   if (bmDigi2Pair) delete bmDigi2Pair;
   if (bmNum2Pair) delete bmNum2Pair;
   if (bmChar2Pair) delete bmChar2Pair;
   if (bmBin2Pair) delete bmBin2Pair;
   if (bmPair2Char) delete bmPair2Char;
   if (bmPair2Digi) delete bmPair2Digi;
   if (bmPair2Num) delete bmPair2Num;
   if (bmPair2Bin) delete bmPair2Bin;

   if (bmDigiRSFF) delete bmDigiRSFF;
   if (bmDigiTFF) delete bmDigiTFF;
   if (bmDigiNOT) delete bmDigiNOT;
   if (bmNumNOT) delete bmNumNOT;
   if (bmDigiNOP) delete bmDigiNOP;
   if (bmNumNOP) delete bmNumNOP;
   if (bmDigiCmp) delete bmDigiCmp;
   if (bmNumCmp) delete bmNumCmp;
   if (bmCharCmp) delete bmCharCmp;
   if (bmDigiCtr) delete bmDigiCtr;
   if (bmNumCtr) delete bmNumCtr;
   if (bmCharCtr) delete bmCharCtr;
   if (bmBinCtr) delete bmBinCtr;
   if (bmCharNOP) delete bmCharNOP;
   if (bmBinNOP) delete bmBinNOP;
   if (bmDigiOR) delete bmDigiOR;
   if (bmNumOR) delete bmNumOR;
   if (bmDigiXOR) delete bmDigiXOR;
   if (bmNumXOR) delete bmNumXOR;
   if (bmDigiAND) delete bmDigiAND;
   if (bmNumAND) delete bmNumAND;
   if (bmDigiShift) delete bmDigiShift;
   if (bmNumShift) delete bmNumShift;
   if (bmCharShift) delete bmCharShift;
   if (bmBinShift) delete bmBinShift;

   if (bmNumAdd) delete bmNumAdd;
   if (bmNumSub) delete bmNumSub;
   if (bmNumMul) delete bmNumMul;
   if (bmNumDiv) delete bmNumDiv;

   if (bmFlowStart) delete bmFlowStart;
   if (bmFlowStop) delete bmFlowStop;
   if (bmFlowDelay) delete bmFlowDelay;
   if (bmFlowTimer) delete bmFlowTimer;
   if (bmFlowNumTGate) delete bmFlowNumTGate;
   if (bmFlowCharTGate) delete bmFlowCharTGate;
   if (bmFlowBinTGate) delete bmFlowBinTGate;
   if (bmFlowDigiTGate) delete bmFlowDigiTGate;
   if (bmFlowNumGate) delete bmFlowNumGate;
   if (bmFlowCharGate) delete bmFlowCharGate;
   if (bmFlowBinGate) delete bmFlowBinGate;
   if (bmFlowDigiGate) delete bmFlowDigiGate;

   if (bmMiscLog) delete bmMiscLog;
   if (bmMiscLogRec) delete bmMiscLogRec;
   if (bmMiscLogIn) delete bmMiscLogIn;
   if (bmMiscISCO) delete bmMiscISCO;
}



void FlowCanvas::resetPointers()
{
   m_currentGroupObject=NULL;
   m_drawObjectList=NULL;
}



int FlowCanvas::OnKeyDown(wxKeyEvent &event) 
{
   wxCommandEvent cmdEvent;

   if ((event.m_keyCode==WXK_F2) && (m_lastElementID!=0))
   {
      cmdEvent.SetId(m_lastElementID);
      OnPopupMenu(cmdEvent);
      return 1;
   }
   else if (event.ShiftDown())
   {
      if ((event.m_keyCode==WXK_LEFT) && (m_lastElementID!=0))
      {
         cmdEvent.SetId(POPUP_EXTEND_TO_LEFT);
         OnPopupMenu(cmdEvent);
         return 1;
      }
      else if ((event.m_keyCode==WXK_RIGHT) && (m_lastElementID!=0))
      {
         cmdEvent.SetId(POPUP_EXTEND_TO_RIGHT);
         OnPopupMenu(cmdEvent);
         return 1;
      }
      else if ((event.m_keyCode==WXK_UP) && (m_lastElementID!=0))
      {
         cmdEvent.SetId(POPUP_EXTEND_TO_TOP);
         OnPopupMenu(cmdEvent);
         return 1;
      }
      else if ((event.m_keyCode==WXK_DOWN) && (m_lastElementID!=0))
      {
         cmdEvent.SetId(POPUP_EXTEND_TO_BOTTOM);
         OnPopupMenu(cmdEvent);
         return 1;
      }
   }
   return -1;
}


void FlowCanvas::showElement(flowObject *object)
{
   parent->Scroll((object->getFlowPos().x-160)/10,(object->getFlowPos().y-160)/10);
   highlightObject=object;
   Refresh();
}



void FlowCanvas::refreshProject(wxByte setToTop)
{
	wxInt32 x=0,y=0;

    if (setToTop)
    {
       m_drawObjectList=&g_objectList;
       m_currentGroupObject=NULL;
    }
    parent->GetViewStart(&x,&y);
    if (g_objectList.m_projectData)
    {
       SetSize(wxSize(g_objectList.m_projectData->flowW,g_objectList.m_projectData->flowH));
       parent->SetScrollbars(HMI_SCROLL_UNIT,HMI_SCROLL_UNIT,g_objectList.m_projectData->flowW/10,g_objectList.m_projectData->flowH/10,x,y);
    }
    Refresh();
}



void FlowCanvas::OnMouseEvent(wxMouseEvent& event)
{
   wxByte dontSearchConnections=0;
   static bool lastWasLeftDown=false;

   if (!g_mainWin->IsShown()) return;
   lastMousePos=event.GetPosition();

   if (event.Dragging())
   {
      wxInt32 x,y,scrollX=0,scrollY=0;

      if (!isMouseDown) return;
      lastWasLeftDown=false;
      currentOutConnector=0;
      currentInConnector=0;
      if ((lockMode==LOCK_NONE) && (currentObject))
      {
         wxPoint pos,delta;

         pos=currentObject->getFlowPos();
         pos.x+=lastMousePos.x-startMousePos.x;
         pos.y+=lastMousePos.y-startMousePos.y;

         if (pos.x<0) pos.x=0;
         else if (pos.x+currentObject->getFlowWidth()>g_objectList.m_projectData->flowW) pos.x=g_objectList.m_projectData->flowW-currentObject->getFlowWidth();
         if (pos.y<0) pos.y=0;
         else if (pos.y+currentObject->getFlowHeight()>g_objectList.m_projectData->flowH) pos.y=g_objectList.m_projectData->flowH-currentObject->getFlowHeight();

         if (m_currentGroupObject) delta=pos-currentObject->getFlowPos();
         if (currentObject->data.type==hmiObject::FLOW_TYPE_GROUP_IO_DEFINITION)
          ((IOElementDefinition*)currentObject)->setConnectorPos(m_drawObjectList,pos,0,m_checkIncoming);
         else
          currentObject->setFlowPos(m_drawObjectList,pos,0,1);
         if (m_currentGroupObject)
         {
            if (currentObject->data.type==hmiObject::FLOW_TYPE_GROUP_IO_DEFINITION)
             ((IOElementDefinition*)currentObject)->setIncomingConnectionPos(m_currentGroupObject->getINObjectList(),delta);
            else
             currentObject->setIncomingConnectionPos(m_currentGroupObject->getINObjectList(),delta);
         }
         Refresh();
         g_isSaved=false;
      }
      else if ((lockMode==LOCK_TO_CONNECTOR) || (lockMode==LOCK_TO_EOLINE))
      {
         isMouseDrag=1;
         switch (drawDir)
         {
            case DRAW_DIR_VERT_DOWN:
               if (currentFlowCon->getLastPos().y>lastMousePos.y) lastMousePos.y=currentFlowCon->getLastPos().y;
            case DRAW_DIR_VERT:
               if ((currentFlowCon->getLastPos().x+50<lastMousePos.x) || (currentFlowCon->getLastPos().x-50>lastMousePos.x))
               {
                   if (currentFlowCon->addPos(wxPoint(currentFlowCon->getLastPos().x,lastMousePos.y)))
                    drawDir=DRAW_DIR_HORIZONTAL;
               }
               else if ((currentFlowCon->getPrevPos().y+5>=lastMousePos.y) && (currentFlowCon->getPrevPos().y-5<=lastMousePos.y))
               {
                   if (currentFlowCon->removeLastPos())
                   {
                      if (currentFlowCon->getPosIdx()==0) drawDir=DRAW_DIR_HORIZONTAL_RIGHT;
                      else drawDir=DRAW_DIR_HORIZONTAL;
                   }
               }
               break;
            case DRAW_DIR_HORIZONTAL_RIGHT:
               if ((currentFlowCon) && (currentFlowCon->getLastPos().x>lastMousePos.x)) lastMousePos.x=currentFlowCon->getLastPos().x;
            case DRAW_DIR_HORIZONTAL:
               if (currentFlowCon)
               {
                  if ((currentFlowCon->getLastPos().y+50<lastMousePos.y) || (currentFlowCon->getLastPos().y-50>lastMousePos.y))
                  {
                     if (currentFlowCon->addPos(wxPoint(lastMousePos.x,currentFlowCon->getLastPos().y)))
                      drawDir=DRAW_DIR_VERT;
                  }
                  else if ((currentFlowCon->getPrevPos().x+5>=lastMousePos.x) && (currentFlowCon->getPrevPos().x-5<=lastMousePos.x))
                  {
                     if (currentFlowCon->removeLastPos())
                     {
                        if (currentFlowCon->getPosIdx()==0) drawDir=DRAW_DIR_VERT_DOWN;
                        else drawDir=DRAW_DIR_VERT;
                     }
                  }
               }
               break;
            default:
               break;
         }
         if (!m_drawObjectList) return;
         inputObject=m_drawObjectList->getDrawableFlowObject(lastMousePos,1);
         if ((!inputObject) && (m_currentGroupObject))
          inputObject=m_currentGroupObject->getOUTObjectList()->getDrawableFlowObject(lastMousePos,1);
         g_isSaved=false;
      }
      else if (lockMode==LOCK_TO_FLOWLINE_HORIZ)
      {
         wxPoint pos1,pos2;

         pos1=currentFlowCon->getPosAt(currentFlowLineIdx-1);
         pos2=currentFlowCon->getPosAt(currentFlowLineIdx);
         pos1.x+=lastMousePos.x-startMousePos.x;
         pos2.x+=lastMousePos.x-startMousePos.x;
         currentFlowCon->setPosAt(currentFlowLineIdx-1,pos1);
         currentFlowCon->setPosAt(currentFlowLineIdx,pos2);
         g_isSaved=false;
      }
      else if (lockMode==LOCK_TO_FLOWLINE_VERT)
      {
         wxPoint pos1,pos2;

         pos1=currentFlowCon->getPosAt(currentFlowLineIdx-1);
         pos2=currentFlowCon->getPosAt(currentFlowLineIdx);
         pos1.y+=lastMousePos.y-startMousePos.y;
         pos2.y+=lastMousePos.y-startMousePos.y;
         currentFlowCon->setPosAt(currentFlowLineIdx-1,pos1);
         currentFlowCon->setPosAt(currentFlowLineIdx,pos2);
         g_isSaved=false;
      }
      else if (rectStart.x>=0)
      {
         rectEnd=lastMousePos;
         Refresh();
      }
      startMousePos=lastMousePos;

      parent->GetViewStart(&x,&y);
      scrollX=x;
      scrollY=y;
      x*=HMI_SCROLL_UNIT;
      y*=HMI_SCROLL_UNIT;
      if (lastMousePos.x<x+HMI_SCROLL_UNIT) scrollX+=-1;
      else if (lastMousePos.x>x+parent->GetClientSize().x-HMI_SCROLL_UNIT) scrollX+=1;
      if (lastMousePos.y<y+HMI_SCROLL_UNIT) scrollY+=-1;
      else if (lastMousePos.y>y+parent->GetClientSize().y-HMI_SCROLL_UNIT) scrollY+=1;
      parent->Scroll(scrollX,scrollY);

      Refresh();
   }
   else if (event.Moving())
   {
      FlowConnection *prevFlowCon=currentFlowCon;
      wxUint64        prevOutConnector=currentOutConnector;
      wxUint64        prevInConnector=currentInConnector;

      currentObject=NULL;
      currentOutConnector=0;
      currentInConnector=0;
      dontSearchConnections=0;
      if (m_currentGroupObject)
      {
         currentObject=m_currentGroupObject->getINObjectList()->getDrawableFlowObject(lastMousePos,1);
         if (!currentObject)
         {
            currentObject=m_currentGroupObject->getOUTObjectList()->getDrawableFlowObject(lastMousePos,0);
            if (currentObject) dontSearchConnections=1;
         }
      }
      if ((!currentObject) && (m_drawObjectList))
       currentObject=m_drawObjectList->getDrawableFlowObject(lastMousePos,1);
      if (currentObject)
      {
         currentInConnector=g_flowObjects.getInConnectorType(currentObject,lastMousePos);
         currentOutConnector=g_flowObjects.getOutConnectorType(currentObject,lastMousePos);
         if (currentOutConnector)
         {
            currentFlowCon=NULL;
            wxSetCursor(wxCursor(wxCURSOR_CROSS));
            lockMode=LOCK_TO_CONNECTOR;
         }
         else
         {
            if (dontSearchConnections) currentFlowCon=NULL;
            else currentFlowCon=currentObject->getFlowConnection(lastMousePos,5);
            if (currentFlowCon)
            {
               if ((currentFlowCon->isAtEnd(lastMousePos,5)) && (currentFlowCon->data.targetID<=0))
               {
                  wxSetCursor(wxCursor(wxCURSOR_CROSS));
                  lockMode=LOCK_TO_EOLINE;

                  if (currentFlowCon->getLastPos().y==currentFlowCon->getPrevPos().y)
                  {
                     if (currentFlowCon->data.usedEdges<2)
                     {
                        if (currentOutConnector & 0xFFFFFFFF) drawDir=DRAW_DIR_VERT_DOWN;
                        else drawDir=DRAW_DIR_HORIZONTAL_RIGHT;
                     }
                     else drawDir=DRAW_DIR_VERT;
                  }
                  else drawDir=DRAW_DIR_HORIZONTAL;
               }
               else
               {
                  wxInt32 idx;

                  idx=currentFlowCon->getFlowLine(lastMousePos,5);
                  if (idx>1)
                  {
                     currentFlowLineIdx=idx;
                     if ((currentFlowCon->getPosAt(idx-1).x==currentFlowCon->getPosAt(idx).x) &&
                         (currentFlowCon->getPosAt(idx-1).y!=currentFlowCon->getPosAt(idx).y) &&
                         ((currentFlowCon->data.targetID<=0) || (!currentFlowCon->isLastLine(lastMousePos,5))))
                     {
                        wxSetCursor(wxCursor(wxCURSOR_SIZEWE));
                        lockMode=LOCK_TO_FLOWLINE_HORIZ;
                     }
                     else if ((currentFlowCon->getPosAt(idx-1).y==currentFlowCon->getPosAt(idx).y) &&
                              (currentFlowCon->getPosAt(idx-1).x!=currentFlowCon->getPosAt(idx).x))
                     {
                        wxSetCursor(wxCursor(wxCURSOR_SIZENS));
                        lockMode=LOCK_TO_FLOWLINE_VERT;
                     }
                     else
                     {
                        wxSetCursor(wxCursor(wxCURSOR_ARROW));
                        lockMode=LOCK_NONE;
                     }
                  }
               }
/*               else
               {
                  wxSetCursor(wxCursor(wxCURSOR_ARROW));
                  lockMode=LOCK_NONE;
               }*/
            }
            else
            {
               currentFlowCon=NULL;
               wxSetCursor(wxCursor(wxCURSOR_HAND));
               lockMode=LOCK_NONE;
            }
         }
      }
      else
      {
         currentFlowCon=NULL;
         wxSetCursor(wxCursor(wxCURSOR_ARROW));
         lockMode=LOCK_NONE;
      }
      if (prevFlowCon!=currentFlowCon) Refresh();
      else
      {
         if (prevOutConnector!=currentOutConnector) Refresh();
         if (prevInConnector!=currentInConnector) Refresh();
      }

   }
   else if (event.LeftDown())
   {
      startMousePos=lastMousePos;
      highlightObject=NULL;

      lastWasLeftDown=true;
      rectStart.x=-1;
      rectEnd.x=-1;
      if (lockMode==LOCK_TO_EOLINE)
      {
          g_mainWin->addUndoStep(_T("Draw Connection"));

/*         if (currentFlowLineIdx==1) drawDir=DRAW_DIR_VERT_DOWN;
         else if (currentFlowLineIdx%2==1) drawDir=DRAW_DIR_HORIZONTAL;
         else drawDir=DRAW_DIR_VERT;*/
      }
      else
      {
         currentObject=NULL;
         if (m_currentGroupObject)
         {
            currentObject=m_currentGroupObject->getINObjectList()->getDrawableFlowObject(lastMousePos,0);
            m_checkIncoming=1;
            if (!currentObject)
            {
               currentObject=m_currentGroupObject->getOUTObjectList()->getDrawableFlowObject(lastMousePos,0);
               m_checkIncoming=0;
            }
         }
         if ((!currentObject) && (m_drawObjectList))
         {
            currentObject=m_drawObjectList->getDrawableFlowObject(lastMousePos,0);
            m_checkIncoming=0;
         }
         if (currentObject)
         {
            if (lockMode==LOCK_TO_CONNECTOR)
            {
               g_mainWin->addUndoStep(_T("Draw Connection"));
               currentFlowCon=new FlowConnection(g_flowObjects.getOutConnectorPos(currentObject,lastMousePos)+currentObject->getFlowPos());
               if (currentOutConnector & 0xFFFFFFFF) drawDir=DRAW_DIR_VERT_DOWN;
               else drawDir=DRAW_DIR_HORIZONTAL_RIGHT;
               currentFlowCon->data.sourceOutput=currentOutConnector;
               currentObject->addFlowConnection(currentFlowCon);
            }
            else g_mainWin->addUndoStep(_T("Move"));
         }
         else rectStart=lastMousePos;
      }
      isMouseDown=1;
   }
   else if (event.LeftDClick())
   {
      wxCommandEvent e;

      highlightObject=NULL;
      if (currentObject)
      {
         if (currentObject->data.type==hmiObject::FLOW_TYPE_GROUP) OnEnterGroup(e);
         else OnEditElem(e);
      }
      else if (m_currentGroupObject) OnLeaveGroup(e);
   }
   else if (event.LeftUp())
   {
      if ((rectStart.x>=0) && (rectEnd.x>=0))
      {
         wxInt32 xchg;

         if (rectEnd.x<rectStart.x)
         {
            xchg=rectEnd.x;
            rectEnd.x=rectStart.x;
            rectStart.x=xchg;
         }
         if (rectEnd.y<rectStart.y)
         {
            xchg=rectEnd.y;
            rectEnd.y=rectStart.y;
            rectStart.y=xchg;
         }
      }

      if ((isMouseDrag) && (currentFlowCon))
      {
         switch (drawDir)
         {
            case DRAW_DIR_VERT_DOWN:
            case DRAW_DIR_VERT:
               currentFlowCon->addPos(wxPoint(currentFlowCon->getLastPos().x,lastMousePos.y));
               break;
            case DRAW_DIR_HORIZONTAL_RIGHT:
            case DRAW_DIR_HORIZONTAL:
               currentFlowCon->addPos(wxPoint(lastMousePos.x,currentFlowCon->getLastPos().y));
               break;
            default:
               break;
         }

         if ((currentFlowCon) && (inputObject))
         {
            wxPoint  pos,inPos;
            wxUint64 outType,inType;
            wxUint64 outDigi,inDigi,outNum,inNum,outChar,inChar,outBin,inBin;

            outType=currentFlowCon->data.sourceOutput;
            inType=g_flowObjects.getInConnectorType(inputObject,lastMousePos);

            outDigi=(outType & OAPC_DIGI_IO_MASK);
            outNum= (outType & OAPC_NUM_IO_MASK);
            outChar=(outType & OAPC_CHAR_IO_MASK);
            outBin= (outType & OAPC_BIN_IO_MASK);
            outDigi|=(currentFlowCon->data.sourceOutput & FLOW_TYPE_FLAG_DIGI_OVERFLOW_OUT);
            outNum|= (currentFlowCon->data.sourceOutput & FLOW_TYPE_FLAG_NUM_OVERFLOW_OUT);
            outChar|=(currentFlowCon->data.sourceOutput & FLOW_TYPE_FLAG_CHAR_OVERFLOW_OUT);
            outBin|= (currentFlowCon->data.sourceOutput & FLOW_TYPE_FLAG_BIN_OVERFLOW_OUT);

            inDigi =(inType & OAPC_DIGI_IO_MASK);
            inNum=  (inType & OAPC_NUM_IO_MASK);
            inChar= (inType & OAPC_CHAR_IO_MASK);
            inBin=  (inType & OAPC_BIN_IO_MASK);

            if (((outDigi) && (!inDigi)) || ((outNum) && (!inNum)) || 
               ((outChar) && (!inChar)) || ((outBin) && (!inBin)))
            {
               wxString       typeError;
               wxCommandEvent e;

               if (outDigi)      typeError=_("Output")+_T(": ")+_("digital");
               else if (outNum)  typeError=_("Output")+_T(": ")+_("numeric");
               else if (outChar) typeError=_("Output")+_T(": ")+_("characters");
               else if (outBin)  typeError=_("Output")+_T(": ")+_("binary");
               else 
               {
                  wxASSERT(0);
                  return;
               }
               typeError=typeError+_T(", ");
               if (inDigi)      typeError=typeError+_("Input")+_T(": ")+_("digital");
               else if (inNum)  typeError=typeError+_("Input")+_T(": ")+_("numeric");
               else if (inChar) typeError=typeError+_("Input")+_T(": ")+_("characters");
               else if (inBin)  typeError=typeError+_("Input")+_T(": ")+_("binary");
               else
               {
                  wxASSERT(0);
                  return;
               }

               wxMessageBox(_("IOs with different data types can't be connected with each other")+_T(":\n")+typeError,_("Error"),wxICON_ERROR);
               currentFlowCon->removeLastPos();
               if (currentFlowCon->getPosIdx()<=1)
                OnDelConnection(e);
            }
            else
            {
                inPos=g_flowObjects.getInConnectorPos(inputObject,lastMousePos);
                inPos=inPos+inputObject->getFlowPos();
                if (inPos.x>-1)
                {
                   if (drawDir==DRAW_DIR_HORIZONTAL)
                   {
                      pos=currentFlowCon->getLastPos();
                      pos.x=inPos.x;
                      currentFlowCon->setLastPos(pos);
                      currentFlowCon->addPos(inPos);
                   }
                   else
                   {
                      if (currentFlowCon->getPosIdx()>1)
                      {
                         pos=currentFlowCon->getLastPos();
                         pos.x=inPos.x;
                         pos.y=inPos.y;
                         currentFlowCon->setLastPos(pos);
                         pos=currentFlowCon->getPrevPos();
                         pos.x=inPos.x;
                         currentFlowCon->setPrevPos(pos);
                      }
                      else
                      {
                         if (drawDir!=DRAW_DIR_HORIZONTAL_RIGHT)
                         {
                            pos=currentFlowCon->getPosAt(0);
                            currentFlowCon->setPosAt(1,wxPoint(pos.x,(pos.y+inPos.y)/2));
                            currentFlowCon->addPos(wxPoint(inPos.x,(pos.y+inPos.y)/2));
                            currentFlowCon->addPos(inPos);
                         }
                         else
                         {
                            currentFlowCon->addPos(wxPoint(inPos.x,inPos.y));
                         }
                      }
                   }
                   if (inputObject->data.type==hmiObject::FLOW_TYPE_GROUP_IO_DEFINITION)
                   {
                      currentFlowCon->data.targetID=((IOElementDefinition*)inputObject)->data.id;
                      currentFlowCon->data.targetInput=inType;
                   }
                   else if (inputObject->data.type==hmiObject::FLOW_TYPE_GROUP)
                   {
                      currentFlowCon->data.targetID=inputObject->data.id; // the group is the global target

                      IOElementDefinition *connectingIODef;
                      connectingIODef=(IOElementDefinition*)((flowGroup*)inputObject)->getInConnectorObject(lastMousePos);
                      wxASSERT(connectingIODef);
                      if (connectingIODef) currentFlowCon->data.targetInput=connectingIODef->data.id; // and here we store the unique ID of the element that connects between inside and outside of the group
                   }
                   else
                   {
                      currentFlowCon->data.targetID=inputObject->data.id;
                      currentFlowCon->data.targetInput=inType;
                   }
                }
            }
         }
      }
      else if ((lockMode==LOCK_TO_FLOWLINE_VERT) || (lockMode==LOCK_TO_FLOWLINE_HORIZ))
      {
         if ((currentFlowCon) && (currentFlowCon->data.sourceOutput!=0)) // it is not a horizontal overflow output so we can do some optimizations here
          currentFlowCon->optimizeLineAt(currentFlowLineIdx);
      }
      else if ((lastWasLeftDown) && (m_nextLeftClickOperation) && (!currentObject))
      {
         wxCommandEvent cmdEvent;

         cmdEvent.SetId(m_nextLeftClickOperation);
         OnPopupMenu(cmdEvent);
      }
      lastWasLeftDown=false;

      currentFlowLineIdx=-1;
      inputObject=NULL;
      currentFlowCon=NULL;
      isMouseDrag=0;
      lockMode=LOCK_NONE;
      Refresh();
      isMouseDown=0;
   }
   else if (event.RightUp())
   {
      highlightObject=NULL;
      ShowContextMenu();
   }
}



void FlowCanvas::addDeviceSubmenu(wxMenu *addMenu)
{
   ExternalIOLib *extIOLib;
   wxMenu        *ioMenu=new wxMenu();
   wxMenu        *motMenu=new wxMenu();
   wxMenu        *laserMenu=new wxMenu();
   wxMenu        *dataMenu=new wxMenu();

   addMenu->AppendSubMenu(ioMenu,_("Input")+_T("/")+_("Output"));
   addMenu->AppendSubMenu(motMenu,_("Motion"));
   addMenu->AppendSubMenu(laserMenu,_("Laser"));
   addMenu->AppendSubMenu(dataMenu,_("Data"));
	
   extIOLib=g_externalIOLibs->getLib(true);
   while (extIOLib)
   {
      wxMenuItem *item=NULL;

      if ((extIOLib->getLibCapabilities() & OAPC_IS_DEPRECATED)==0)
      {
         if ((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)==OAPC_FLOWCAT_IO)
         {
            item=new wxMenuItem(ioMenu,wxID_ANY,extIOLib->name);
            ioMenu->Append(item);
         }
         else if ((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)==OAPC_FLOWCAT_MOTION)
         {
            item=new wxMenuItem(motMenu,wxID_ANY,extIOLib->name);
            motMenu->Append(item);
         }
         else if ((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)==OAPC_FLOWCAT_LASER)
         {
             item=new wxMenuItem(laserMenu,wxID_ANY,extIOLib->name);
             laserMenu->Append(item);
         }
         else if ((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)==OAPC_FLOWCAT_DATA)
         {
            item=new wxMenuItem(dataMenu,wxID_ANY,extIOLib->name);
            dataMenu->Append(item);
         }
         if (item)
         {
            extIOLib->uiID=item->GetId();
            Connect(extIOLib->uiID,wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(FlowCanvas::OnPopupMenu));
         }
      }
      extIOLib=g_externalIOLibs->getLib(false);
   }
}



void FlowCanvas::OnFoldbarClicked(wxMouseEvent& event)
{
   if (event.GetId()>=BUTTON_FOLDBAR)
   {
      std::list<wxStaticText*>::iterator it;
      wxStaticText                      *text;

      for ( it=m_foldBarList.begin() ; it != m_foldBarList.end(); it++ )
      {
         text=*it;
         if (event.GetId()==text->GetId())
         {
#ifdef __x86_64__
            if (m_nextLeftClickOperation==(wxInt64)text->GetClientData())
#else
            if (m_nextLeftClickOperation==(wxInt32)text->GetClientData())
#endif
            {
               text->SetForegroundColour(*wxBLACK);
               m_nextLeftClickOperation=0;
            }
            else
            {
               text->SetForegroundColour(BLUE_COLOUR);
#ifdef __x86_64__
               m_nextLeftClickOperation=(wxInt64)text->GetClientData();
#else
               m_nextLeftClickOperation=(wxInt32)text->GetClientData();
#endif
            }
         }
         else text->SetForegroundColour(*wxBLACK);
         text->Refresh();
      }
   }
}



wxInt32 FlowCanvas::addFoldBarItem(wxFoldPanelBar *bar,wxFoldPanel *foldItem,wxInt32 id,wxString name,bool setLocalEvent)
{
   wxStaticText   *text;
   static wxInt32  buttonCtr=0,useID;

   if (setLocalEvent) useID=BUTTON_FOLDBAR+buttonCtr;
   else useID=id;
   text=new wxStaticText(foldItem->GetParent(),useID,name,wxDefaultPosition,wxDefaultSize,wxALIGN_LEFT);
   if (setLocalEvent)
   {
      text->Connect(wxEVT_LEFT_DOWN,wxMouseEventHandler(FlowCanvas::OnFoldbarClicked),NULL,this);
      if (id==wxID_ANY) id=useID;
      text->SetClientData((void*)id);
      m_foldBarList.push_back(text);
   }
   else text->Connect(wxEVT_LEFT_DCLICK,wxMouseEventHandler(PlugInPanel::OnFoldbarClicked),NULL,g_plugInPanel);
   buttonCtr++;

   bar->AddFoldPanelWindow(*foldItem,text,wxFPB_ALIGN_WIDTH,0,0,0);
   return useID;
}



void FlowCanvas::addFoldBarSubmenu(wxFoldPanelBar *bar,bool setLocalEvent)
{
   ExternalIOLib *extIOLib;
   wxFoldPanel   foldItem(NULL);

   foldItem=bar->AddFoldPanel(_("Input")+_T("/")+_("Output"), true);
   extIOLib=g_externalIOLibs->getLib(true);
   while (extIOLib)
   {
      if (((extIOLib->getLibCapabilities() & OAPC_IS_DEPRECATED)==0) &&
          ((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)==OAPC_FLOWCAT_IO))
      {
         extIOLib->foldUiID=addFoldBarItem(bar,&foldItem,extIOLib->foldUiID,extIOLib->name,setLocalEvent);
      }
      extIOLib=g_externalIOLibs->getLib(false);
   }

   foldItem=bar->AddFoldPanel(_("Motion"), true);
   extIOLib=g_externalIOLibs->getLib(true);
   while (extIOLib)
   {
      if (((extIOLib->getLibCapabilities() & OAPC_IS_DEPRECATED)==0) &&
          ((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)==OAPC_FLOWCAT_MOTION))
      {
         extIOLib->foldUiID=addFoldBarItem(bar,&foldItem,extIOLib->foldUiID,extIOLib->name,setLocalEvent);
      }
      extIOLib=g_externalIOLibs->getLib(false);
   }

   foldItem=bar->AddFoldPanel(_("Laser"), true);
   extIOLib=g_externalIOLibs->getLib(true);
   while (extIOLib)
   {
       if (((extIOLib->getLibCapabilities() & OAPC_IS_DEPRECATED)==0) &&
           ((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)==OAPC_FLOWCAT_LASER))
       {
           extIOLib->foldUiID=addFoldBarItem(bar,&foldItem,extIOLib->foldUiID,extIOLib->name,setLocalEvent);
       }
       extIOLib=g_externalIOLibs->getLib(false);
   }

   foldItem=bar->AddFoldPanel(_("Data"), true);
   extIOLib=g_externalIOLibs->getLib(true);
   while (extIOLib)
   {
      if (((extIOLib->getLibCapabilities() & OAPC_IS_DEPRECATED)==0) &&
          ((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)==OAPC_FLOWCAT_DATA))
      {
         extIOLib->foldUiID=addFoldBarItem(bar,&foldItem,extIOLib->foldUiID,extIOLib->name,setLocalEvent);
      }
      extIOLib=g_externalIOLibs->getLib(false);
   }
}



wxFoldPanelBar *FlowCanvas::createFoldBar(wxWindow *parent)
{
   ExternalIOLib *extIOLib;
   wxFoldPanel   foldItem(NULL);

   m_flowBar = new wxFoldPanelBar(parent,wxID_ANY,wxDefaultPosition,wxSize(200,500),wxFPB_DEFAULT_STYLE |wxFPB_VERTICAL,0);

//   wxFoldPanel item = m_flowBar->AddFoldPanel(_T("Test me"), false);


/*   foldItem=m_flowBar->AddFoldPanel(_("Connectors"), true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_INCONNECTOR_DIGI,_("Digital Input Connector"));
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_OUTCONNECTOR_DIGI,_("Digital Output Connector"));
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_INCONNECTOR_NUM ,_("Numeric Input Connector"));
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_OUTCONNECTOR_NUM ,_("Numeric Output Connector"));
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_INCONNECTOR_CHAR,_("Character Input Connector"));
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_OUTCONNECTOR_CHAR,_("Character Output Connector"));
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_INCONNECTOR_BIN ,_("Binary Input Connector"));
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_OUTCONNECTOR_BIN ,_("Binary Output Connector"));
   m_flowBar->AddFoldPanelSeperator(foldItem,wxColour(200,200,210),0,0,0);*/

   foldItem=m_flowBar->AddFoldPanel(_("Data Conversion"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_CONV_DIGI2NUM,_("Digital to Number"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_CONV_DIGI2CHAR,_("Digital to Characters"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_CONV_DIGI2PAIR,_("Digital to CMD-Pair"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_CONV_PAIR2DIGI,_("CMD-Pair to Digital"),true);
   m_flowBar->AddFoldPanelSeperator(foldItem,wxColour(200,200,210),0,0,0);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_CONV_NUM2DIGI,_("Number to Digital"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_CONV_NUM2BITS,_("Number to Bits"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_CONV_NUM2CHAR,_("Number to Characters"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_CONV_NUM2PAIR,_("Number to CMD-Pair"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_CONV_PAIR2NUM,_("CMD-Pair to Number"),true);
   m_flowBar->AddFoldPanelSeperator(foldItem,wxColour(200,200,210),0,0,0);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_CONV_CHAR2DIGI,_("Characters to Digital"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_CONV_CHAR2NUM,_("Characters to Number"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_CONV_CHAR2PAIR,_("Characters to CMD-Pair"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_CONV_PAIR2CHAR,_("CMD-Pair to Characters"),true);
   m_flowBar->AddFoldPanelSeperator(foldItem,wxColour(200,200,210),0,0,0);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_CONV_BIN2PAIR,_("Binary to CMD-Pair"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_CONV_PAIR2BIN,_("CMD-Pair to Binary"),true);
   m_flowBar->AddFoldPanelSeperator(foldItem,wxColour(200,200,210),0,0,0);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_CONV_MIXED2CHAR,_("Mixed to Characters"),true);
   m_flowBar->AddFoldPanelSeperator(foldItem,wxColour(200,200,210),0,0,0);
   extIOLib=g_externalIOLibs->getLib(true);
   while (extIOLib)
   {
      if (((extIOLib->getLibCapabilities() & OAPC_IS_DEPRECATED)==0) &&
          ((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)==OAPC_FLOWCAT_CONVERSION))
      {
         extIOLib->foldUiID=addFoldBarItem(m_flowBar,&foldItem,wxID_ANY,extIOLib->name,true);
      }
      extIOLib=g_externalIOLibs->getLib(false);
   }

   foldItem=m_flowBar->AddFoldPanel(_("Logic Operations"), true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_LOGI_DIGINOT,_("Digital NOT"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_LOGI_DIGINOP,_("Digital NOP"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_LOGI_DIGIOR,_("Digital (N)OR"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_LOGI_DIGIXOR,_("Digital X(N)OR"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_LOGI_DIGIAND,_("Digital (N)AND"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_LOGI_DIGISHIFT,_("Digital Shift Register"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_LOGI_DIGIRSFF,_("SR Flip-Flop"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_LOGI_DIGITFF,_("Toggle Flip-Flop"),true);
   m_flowBar->AddFoldPanelSeperator(foldItem,wxColour(200,200,210),0,0,0);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_LOGI_NUMNOT,_("Numeric NOT"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_LOGI_NUMNOP,_("Numeric NOP"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_LOGI_NUMOR,_("Numeric (N)OR"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_LOGI_NUMXOR,_("Numeric X(N)OR"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_LOGI_NUMAND,_("Numeric (N)AND"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_LOGI_NUMSHIFT,_("Numeric Shift Register"),true);
   m_flowBar->AddFoldPanelSeperator(foldItem,wxColour(200,200,210),0,0,0);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_LOGI_CHARNOP,_("Character-NOP"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_LOGI_CHARSHIFT,_("Character Shift Register"),true);
   m_flowBar->AddFoldPanelSeperator(foldItem,wxColour(200,200,210),0,0,0);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_LOGI_BINNOP,_("Binary NOP"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_LOGI_BINSHIFT,_("Binary Shift Register"),true);
   m_flowBar->AddFoldPanelSeperator(foldItem,wxColour(200,200,210),0,0,0);
   extIOLib=g_externalIOLibs->getLib(true);
   while (extIOLib)
   {
      if (((extIOLib->getLibCapabilities() & OAPC_IS_DEPRECATED)==0) &&
          ((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)==OAPC_FLOWCAT_LOGIC))
      {
         extIOLib->foldUiID=addFoldBarItem(m_flowBar,&foldItem,wxID_ANY,extIOLib->name,true);
      }
      extIOLib=g_externalIOLibs->getLib(false);
   }

   foldItem=m_flowBar->AddFoldPanel(_("Mathematical"), true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_MATH_DIGICMP,_("Compare Digital"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_MATH_DIGICTR,_("Digital Counter"),true);
   m_flowBar->AddFoldPanelSeperator(foldItem,wxColour(200,200,210),0,0,0);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_LOGI_NUMADD,_("Numeric Addition"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_LOGI_NUMSUB,_("Numeric Subtraction"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_LOGI_NUMMUL,_("Numeric Multiplication"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_LOGI_NUMDIV,_("Numeric Division"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_MATH_NUMCMP,_("Compare Numbers"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_MATH_NUMCTR,_("Number Counter"),true);
   m_flowBar->AddFoldPanelSeperator(foldItem,wxColour(200,200,210),0,0,0);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_MATH_CHARCMP,_("Compare Characters"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_MATH_CHARCTR,_("Character Counter"),true);
   m_flowBar->AddFoldPanelSeperator(foldItem,wxColour(200,200,210),0,0,0);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_MATH_BINCTR,_("Binary Counter"),true);
   m_flowBar->AddFoldPanelSeperator(foldItem,wxColour(200,200,210),0,0,0);
   extIOLib=g_externalIOLibs->getLib(true);
   while (extIOLib)
   {
      if (((extIOLib->getLibCapabilities() & OAPC_IS_DEPRECATED)==0) &&
          ((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)==OAPC_FLOWCAT_CALC))
      {
         extIOLib->foldUiID=addFoldBarItem(m_flowBar,&foldItem,wxID_ANY,extIOLib->name,true);
      }
      extIOLib=g_externalIOLibs->getLib(false);
   }

   foldItem=m_flowBar->AddFoldPanel(_("Flow Control"), true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_FLOW_START,_("Initial Flow Start"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_FLOW_STOP,_("Exit Application"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_FLOW_DELAY,_("Delay"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_FLOW_TIMER,_("Timer"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_FLOW_DIGIGATE,_("Digital Gate"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_FLOW_DIGITGATE,_("Digital Triggered Gate"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_FLOW_NUMGATE,_("Numeric Gate"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_FLOW_NUMTGATE,_("Numeric Triggered Gate"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_FLOW_CHARGATE,_("Character Gate"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_FLOW_CHARTGATE,_("Character Triggered Gate"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_FLOW_BINGATE,_("Binary Gate"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_FLOW_BINTGATE,_("Binary Triggered Gate"),true);
   m_flowBar->AddFoldPanelSeperator(foldItem,wxColour(200,200,210),0,0,0);
   extIOLib=g_externalIOLibs->getLib(true);
   while (extIOLib)
   {
      if (((extIOLib->getLibCapabilities() & OAPC_IS_DEPRECATED)==0) &&
            ((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)==OAPC_FLOWCAT_FLOW))
      {
         extIOLib->foldUiID=addFoldBarItem(m_flowBar,&foldItem,wxID_ANY,extIOLib->name,true);
      }
      extIOLib=g_externalIOLibs->getLib(false);
   }

   addFoldBarSubmenu(m_flowBar,true);

   foldItem=m_flowBar->AddFoldPanel(_("Miscellaneous"), true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_MISC_LOG_REC,_("Log Data Recorder"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_MISC_LOG,_("Log Output"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_MISC_LOGIN,_("Log In User"),true);
   addFoldBarItem(m_flowBar,&foldItem,POPUP_ADD_MISC_ISCO,_("Interlock Server Connection"),true);
   extIOLib=g_externalIOLibs->getLib(true);
   while (extIOLib)
   {
      if (((extIOLib->getLibCapabilities() & OAPC_IS_DEPRECATED)==0) &&
          ((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)!=OAPC_FLOWCAT_CONVERSION) &&
          ((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)!=OAPC_FLOWCAT_LOGIC) &&
          ((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)!=OAPC_FLOWCAT_CALC) &&
          ((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)!=OAPC_FLOWCAT_FLOW) &&
          ((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)!=OAPC_FLOWCAT_IO) &&
          ((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)!=OAPC_FLOWCAT_MOTION) &&
          ((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)!=OAPC_FLOWCAT_LASER) &&
          ((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)!=OAPC_FLOWCAT_DATA))
      {
         extIOLib->foldUiID=addFoldBarItem(m_flowBar,&foldItem,wxID_ANY,extIOLib->name,true);
      }
      extIOLib=g_externalIOLibs->getLib(false);
   }

   return m_flowBar;
}



void FlowCanvas::ShowContextMenu()
{
   wxMenu                         menu;
   wxMenuItem                    *item,*retItem;
   wxList::compatibility_iterator node;
   ExternalIOLib                 *extIOLib;
   ExternalMacro                 *extMacro;
   bool                           convHasCustom=false,logiHasCustom=false,mathHasCustom=false,flowHasCustom=false,miscHasCustom=false;

   item=new wxMenuItem(&menu,POPUP_PUT_CONTROL, _("Put HMI Control"));
   menu.Append(item);

   wxMenu *addMenu=new wxMenu();
   wxMenu *conMenu=new wxMenu();
   wxMenu *convMenu=new wxMenu();
   wxMenu *logiMenu=new wxMenu();
   wxMenu *mathMenu=new wxMenu();
   wxMenu *flowMenu=new wxMenu();
   wxMenu *miscMenu=new wxMenu();

   item=new wxMenuItem(addMenu,m_lastElementID, _("Last element again")+_T("\tF2"));
   addMenu->Append(item);
   if (m_lastElementID==0) item->Enable(false);

   retItem=addMenu->AppendSubMenu(conMenu,_("Connectors"));
   item=new wxMenuItem(conMenu,POPUP_ADD_INCONNECTOR_DIGI, _("Digital Input Connector"));
   conMenu->Append(item);
   item=new wxMenuItem(conMenu,POPUP_ADD_OUTCONNECTOR_DIGI, _("Digital Output Connector"));
   conMenu->Append(item);

   item=new wxMenuItem(conMenu,POPUP_ADD_INCONNECTOR_NUM, _("Numeric Input Connector"));
   conMenu->Append(item);
   item=new wxMenuItem(conMenu,POPUP_ADD_OUTCONNECTOR_NUM, _("Numeric Output Connector"));
   conMenu->Append(item);

   item=new wxMenuItem(conMenu,POPUP_ADD_INCONNECTOR_CHAR, _("Character Input Connector"));
   conMenu->Append(item);
   item=new wxMenuItem(conMenu,POPUP_ADD_OUTCONNECTOR_CHAR, _("Character Output Connector"));
   conMenu->Append(item);

   item=new wxMenuItem(conMenu,POPUP_ADD_INCONNECTOR_BIN, _("Binary Input Connector"));
   conMenu->Append(item);
   item=new wxMenuItem(conMenu,POPUP_ADD_OUTCONNECTOR_BIN, _("Binary Output Connector"));
   conMenu->Append(item);
   if (m_drawObjectList==&g_objectList) retItem->Enable(false);

   addMenu->AppendSubMenu(convMenu,_("Data Conversion"));
   convMenu->Append(POPUP_ADD_CONV_DIGI2NUM,_("Digital to Number"));
   convMenu->Append(POPUP_ADD_CONV_DIGI2CHAR,_("Digital to Characters"));
   convMenu->Append(POPUP_ADD_CONV_DIGI2PAIR,_("Digital to CMD-Pair"));
   convMenu->Append(POPUP_ADD_CONV_PAIR2DIGI,_("CMD-Pair to Digital"));
   convMenu->AppendSeparator();
   convMenu->Append(POPUP_ADD_CONV_NUM2DIGI,_("Number to Digital"));
   convMenu->Append(POPUP_ADD_CONV_NUM2BITS,_("Number to Bits"));
   convMenu->Append(POPUP_ADD_CONV_NUM2CHAR,_("Number to Characters"));
   convMenu->Append(POPUP_ADD_CONV_NUM2PAIR,_("Number to CMD-Pair"));
   convMenu->Append(POPUP_ADD_CONV_PAIR2NUM,_("CMD-Pair to Number"));
   convMenu->AppendSeparator();
   convMenu->Append(POPUP_ADD_CONV_CHAR2DIGI,_("Characters to Digital"));
   convMenu->Append(POPUP_ADD_CONV_CHAR2NUM,_("Characters to Number"));
   convMenu->Append(POPUP_ADD_CONV_CHAR2PAIR,_("Characters to CMD-Pair"));
   convMenu->Append(POPUP_ADD_CONV_PAIR2CHAR,_("CMD-Pair to Characters"));
   convMenu->AppendSeparator();
   convMenu->Append(POPUP_ADD_CONV_BIN2PAIR,_("Binary to CMD-Pair"));
   convMenu->Append(POPUP_ADD_CONV_PAIR2BIN,_("CMD-Pair to Binary"));
   convMenu->AppendSeparator();
   convMenu->Append(POPUP_ADD_CONV_MIXED2CHAR,_("Mixed to Characters"));

   addMenu->AppendSubMenu(logiMenu,_("Logic Operations"));
   logiMenu->Append(POPUP_ADD_LOGI_DIGINOT,_("Digital NOT"));
   logiMenu->Append(POPUP_ADD_LOGI_DIGINOP,_("Digital NOP"));
   logiMenu->Append(POPUP_ADD_LOGI_DIGIOR,_("Digital (N)OR"));
   logiMenu->Append(POPUP_ADD_LOGI_DIGIXOR,_("Digital X(N)OR"));
   logiMenu->Append(POPUP_ADD_LOGI_DIGIAND,_("Digital (N)AND"));
   logiMenu->Append(POPUP_ADD_LOGI_DIGISHIFT,_("Digital Shift Register"));
   logiMenu->Append(POPUP_ADD_LOGI_DIGIRSFF,_("SR Flip-Flop"));
   logiMenu->Append(POPUP_ADD_LOGI_DIGITFF,_("Toggle Flip-Flop"));
   logiMenu->AppendSeparator();
   logiMenu->Append(POPUP_ADD_LOGI_NUMNOT,_("Numeric NOT"));
   logiMenu->Append(POPUP_ADD_LOGI_NUMNOP,_("Numeric NOP"));
   logiMenu->Append(POPUP_ADD_LOGI_NUMOR,_("Numeric (N)OR"));
   logiMenu->Append(POPUP_ADD_LOGI_NUMXOR,_("Numeric X(N)OR"));
   logiMenu->Append(POPUP_ADD_LOGI_NUMAND,_("Numeric (N)AND"));
   logiMenu->Append(POPUP_ADD_LOGI_NUMSHIFT,_("Numeric Shift Register"));
   logiMenu->AppendSeparator();
   logiMenu->Append(POPUP_ADD_LOGI_CHARNOP,_("Character-NOP"));
   logiMenu->Append(POPUP_ADD_LOGI_CHARSHIFT,_("Character Shift Register"));
   logiMenu->AppendSeparator();
   logiMenu->Append(POPUP_ADD_LOGI_BINNOP,_("Binary NOP"));
   logiMenu->Append(POPUP_ADD_LOGI_BINSHIFT,_("Binary Shift Register"));

   addMenu->AppendSubMenu(mathMenu,_("Mathematical"));
   mathMenu->Append(POPUP_ADD_MATH_DIGICMP,_("Compare Digital"));
   mathMenu->Append(POPUP_ADD_MATH_DIGICTR,_("Digital Counter"));
   mathMenu->AppendSeparator();
   mathMenu->Append(POPUP_ADD_LOGI_NUMADD,_("Numeric Addition"));
   mathMenu->Append(POPUP_ADD_LOGI_NUMSUB,_("Numeric Subtraction"));
   mathMenu->Append(POPUP_ADD_LOGI_NUMMUL,_("Numeric Multiplication"));
   mathMenu->Append(POPUP_ADD_LOGI_NUMDIV,_("Numeric Division"));
   mathMenu->Append(POPUP_ADD_MATH_NUMCMP,_("Compare Numbers"));
   mathMenu->Append(POPUP_ADD_MATH_NUMCTR,_("Number Counter"));
   mathMenu->AppendSeparator();
   mathMenu->Append(POPUP_ADD_MATH_CHARCMP,_("Compare Characters"));
   mathMenu->Append(POPUP_ADD_MATH_CHARCTR,_("Character Counter"));
   mathMenu->AppendSeparator();
   mathMenu->Append(POPUP_ADD_MATH_BINCTR,_("Binary Counter"));

   addMenu->AppendSubMenu(flowMenu,_("Flow Control"));
   flowMenu->Append(POPUP_ADD_FLOW_START,_("Initial Flow Start"));
   flowMenu->Append(POPUP_ADD_FLOW_STOP,_("Exit Application"));
   flowMenu->Append(POPUP_ADD_FLOW_DELAY,_("Delay"));
   flowMenu->Append(POPUP_ADD_FLOW_TIMER,_("Timer"));
   flowMenu->Append(POPUP_ADD_FLOW_DIGIGATE,_("Digital Gate"));
   flowMenu->Append(POPUP_ADD_FLOW_DIGITGATE,_("Digital Triggered Gate"));
   flowMenu->Append(POPUP_ADD_FLOW_NUMGATE,_("Numeric Gate"));
   flowMenu->Append(POPUP_ADD_FLOW_NUMTGATE,_("Numeric Triggered Gate"));
   flowMenu->Append(POPUP_ADD_FLOW_CHARGATE,_("Character Gate"));
   flowMenu->Append(POPUP_ADD_FLOW_CHARTGATE,_("Character Triggered Gate"));
   flowMenu->Append(POPUP_ADD_FLOW_BINGATE,_("Binary Gate"));
   flowMenu->Append(POPUP_ADD_FLOW_BINTGATE,_("Binary Triggered Gate"));
   
   addDeviceSubmenu(addMenu);

   miscMenu->Append(POPUP_ADD_MISC_LOG_REC,_("Log Data Recorder"));
   item=new wxMenuItem(miscMenu,POPUP_ADD_MISC_LOG,_("Log Output"));
   miscMenu->Append(item);
   if (g_flowLog) item->Enable(false);
   addMenu->AppendSubMenu(miscMenu,_("Miscellaneous"));

   item=new wxMenuItem(miscMenu,POPUP_ADD_MISC_LOGIN,_("Log In User"));
   miscMenu->Append(item);
   if (!g_userPriviData.enabled) item->Enable(false);

   item=new wxMenuItem(miscMenu,POPUP_ADD_MISC_ISCO,_("Interlock Server Connection"));
   miscMenu->Append(item);
   if ((g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)==0) item->Enable(false);

   extIOLib=g_externalIOLibs->getLib(true);
   while (extIOLib)
   {
      wxMenuItem *item=NULL;

      if ((extIOLib->getLibCapabilities() & OAPC_IS_DEPRECATED)==0)
      {
         if ((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)==OAPC_FLOWCAT_CONVERSION)
         {
            if (!convHasCustom) convMenu->AppendSeparator();
            convHasCustom=true;
            item=new wxMenuItem(convMenu,wxID_ANY,extIOLib->name);
            convMenu->Append(item);
         }
         else if ((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)==OAPC_FLOWCAT_LOGIC)
         {
            if (!logiHasCustom) logiMenu->AppendSeparator();
            logiHasCustom=true;
            item=new wxMenuItem(logiMenu,wxID_ANY,extIOLib->name);
            logiMenu->Append(item);
         }
         else if ((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)==OAPC_FLOWCAT_CALC)
         {
            if (!mathHasCustom) mathMenu->AppendSeparator();
            mathHasCustom=true;
            item=new wxMenuItem(mathMenu,wxID_ANY,extIOLib->name);
            mathMenu->Append(item);
         }
         else if ((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)==OAPC_FLOWCAT_FLOW)
         {
            if (!flowHasCustom) flowMenu->AppendSeparator();
            flowHasCustom=true;
            item=new wxMenuItem(flowMenu,wxID_ANY,extIOLib->name);
            flowMenu->Append(item);
         }
         else if (((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)!=OAPC_FLOWCAT_IO) &&
	   	         ((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)!=OAPC_FLOWCAT_MOTION) &&
                 ((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)!=OAPC_FLOWCAT_LASER) &&
                 ((extIOLib->getLibCapabilities() & OAPC_FLOWCAT_MASK)!=OAPC_FLOWCAT_DATA))
         {
            if (!miscHasCustom) miscMenu->AppendSeparator();
            miscHasCustom=true;
            item=new wxMenuItem(miscMenu,wxID_ANY,extIOLib->name);
            miscMenu->Append(item);
         }
 	      if (item)
         {
            extIOLib->uiID=item->GetId();
            Connect(extIOLib->uiID,wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(FlowCanvas::OnPopupMenu));
         }
      }
      extIOLib=g_externalIOLibs->getLib(false);
   }

   menu.AppendSubMenu(addMenu,_("Add Element"));

   wxMenu *macroMenu=new wxMenu();
   wxMenu *macConvMenu=new wxMenu();
   wxMenu *macLogiMenu=new wxMenu();
   wxMenu *macMathMenu=new wxMenu();
   wxMenu *macIoMenu=new wxMenu();
   wxMenu *macFlowMenu=new wxMenu();
   wxMenu *macMotMenu=new wxMenu();
   wxMenu *macLaserMenu=new wxMenu();
   wxMenu *macDataMenu=new wxMenu();
   wxMenu *macMiscMenu=new wxMenu();

   menu.AppendSubMenu(macroMenu,_("Add Macro"));

   macroMenu->AppendSubMenu(macConvMenu,_("Data Conversion"));
   macroMenu->AppendSubMenu(macLogiMenu,_("Logic Operations"));
   macroMenu->AppendSubMenu(macMathMenu,_("Mathematical"));
   macroMenu->AppendSubMenu(macFlowMenu,_("Flow Control"));
   macroMenu->AppendSubMenu(macIoMenu,_("Input")+_T("/")+_("Output"));
   macroMenu->AppendSubMenu(macMotMenu,_("Motion"));
   macroMenu->AppendSubMenu(macLaserMenu,_("Laser"));
   macroMenu->AppendSubMenu(macDataMenu,_("Data"));
   macroMenu->AppendSubMenu(macMiscMenu,_("Miscellaneous"));

   extMacro=m_externalMacros->getMacro(true);
   while (extMacro)
   {
      if (extMacro->m_category<<24==OAPC_FLOWCAT_CONVERSION)
      {
         item=new wxMenuItem(macConvMenu,wxID_ANY,extMacro->m_name);
         macConvMenu->Append(item);
      }      
      else if (extMacro->m_category<<24==OAPC_FLOWCAT_LOGIC)
      {
         item=new wxMenuItem(macLogiMenu,wxID_ANY,extMacro->m_name);
         macLogiMenu->Append(item);
      }      
      else if (extMacro->m_category<<24==OAPC_FLOWCAT_CALC)
      {
         item=new wxMenuItem(macMathMenu,wxID_ANY,extMacro->m_name);
         macMathMenu->Append(item);
      }      
      else if (extMacro->m_category<<24==OAPC_FLOWCAT_FLOW)
      {
         item=new wxMenuItem(macFlowMenu,wxID_ANY,extMacro->m_name);
         macFlowMenu->Append(item);
      }      
      else if (extMacro->m_category<<24==OAPC_FLOWCAT_IO)
      {
         item=new wxMenuItem(macIoMenu,wxID_ANY,extMacro->m_name);
         macIoMenu->Append(item);
      }      
      else if (extMacro->m_category<<24==OAPC_FLOWCAT_MOTION)
      {
         item=new wxMenuItem(macMotMenu,wxID_ANY,extMacro->m_name);
         macMotMenu->Append(item);
      }      
      else if (extMacro->m_category<<24==OAPC_FLOWCAT_LASER)
      {
          item=new wxMenuItem(macLaserMenu,wxID_ANY,extMacro->m_name);
          macLaserMenu->Append(item);
      }      
      else if (extMacro->m_category<<24==OAPC_FLOWCAT_DATA)
      {
         item=new wxMenuItem(macDataMenu,wxID_ANY,extMacro->m_name);
         macDataMenu->Append(item);
      }      
      else
      {
         item=new wxMenuItem(macMiscMenu,wxID_ANY,extMacro->m_name);
         macMiscMenu->Append(item);
      }      
      extMacro->m_uiID=item->GetId();
      Connect(extMacro->m_uiID,wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(FlowCanvas::OnPopupMenu));

      extMacro=m_externalMacros->getMacro(false);
   }

   item=new wxMenuItem(&menu,POPUP_EDIT_ELEM, _("Edit Element"));
   menu.Append(item);
   if ((!currentObject) || (currentFlowCon)) item->Enable(0);

   item=new wxMenuItem(&menu,POPUP_DEL_ELEM, _("Delete Element"));
   menu.Append(item);
   if ((!currentObject) || (currentFlowCon)) item->Enable(0);

   item=new wxMenuItem(&menu,POPUP_DEL_CONNECTION, _("Delete Connection"));
   menu.Append(item);
   if (!currentFlowCon) item->Enable(0);

   item=new wxMenuItem(&menu,POPUP_DEL_UNUSED_CONN, _("Delete unused Connections"));
   menu.Append(item);
   if (!currentObject) item->Enable(0);

   item=new wxMenuItem(&menu,POPUP_DEL_INPUT, _("Disconnect Input"));
   menu.Append(item);
   if ((!currentFlowCon) || (currentFlowCon->data.targetID<=0)) item->Enable(0);
   //   if (selectedList.GetCount()<=0) item->Enable(0);

   menu.AppendSeparator(); // group elements
   item=new wxMenuItem(&menu,POPUP_GROUP_FLOWOBJECTS, _("Group Elements"));
   menu.Append(item);
   if ((rectStart.x<0) || (rectEnd.x<0) || (rectStart.x==rectEnd.x) || (rectStart.y==rectEnd.y)) item->Enable(0);

   item=new wxMenuItem(&menu,POPUP_GROUP_ENTER, _("Enter Group"));
   menu.Append(item);
   if ((!currentObject) || (currentObject->data.type!=hmiObject::FLOW_TYPE_GROUP)) item->Enable(0);

   item=new wxMenuItem(&menu,POPUP_GROUP_LEAVE, _("Leave Group"));
   menu.Append(item);
   if (m_drawObjectList==&g_objectList) item->Enable(0);

   item=new wxMenuItem(&menu,POPUP_GROUP_SAVE, _("Save Group"));
   menu.Append(item);
   if ((!currentObject) || (currentObject->data.type!=hmiObject::FLOW_TYPE_GROUP)) item->Enable(0);

   item=new wxMenuItem(&menu,POPUP_GROUP_LOAD, _("Load Group"));
   menu.Append(item);
   if (currentObject) item->Enable(0);

   wxMenu *extendMenu=new wxMenu();
   extendMenu->Append(POPUP_EXTEND_TO_LEFT,_("To Left")+_T("\tShift-LEFT"));
   extendMenu->Append(POPUP_EXTEND_TO_RIGHT,_("To Right")+_T("\tShift-RIGHT"));
   extendMenu->Append(POPUP_EXTEND_TO_TOP,_("To Top")+_T("\tShift-UP"));
   extendMenu->Append(POPUP_EXTEND_TO_BOTTOM,_("To Bottom")+_T("\tShift-DOWN"));
   extendMenu->Append(POPUP_EXTEND_INSERT_ROW,_("Insert Row"));
   extendMenu->Append(POPUP_EXTEND_INSERT_COL,_("Insert Column"));

   menu.AppendSeparator();
   menu.AppendSubMenu(extendMenu,_("Extend Working Area"));

   PopupMenu(&menu);
}



void FlowCanvas::drawInLines(flowObject *currentObject,ObjectList *drawObjectList,wxAutoBufferedPaintDC *dc)
{
   flowObject     *object;
   wxNode         *node,*objNode;
   FlowConnection *connection;

   objNode=drawObjectList->getObject((wxNode*)NULL);
   while (objNode)
   {
      object=(flowObject*)objNode->GetData();

      node=object->getFlowConnection(NULL);
      while (node)
      {
         connection=(FlowConnection*)node->GetData();
         if ((connection->data.targetID==currentObject->data.id) && (connection->data.targetInput==currentInConnector))
         {
            connection->drawFlowConnection(dc,wxGREEN_PEN,0,0);
         }
         node=object->getFlowConnection(node);
      }
      objNode=drawObjectList->getObject(objNode);
   }
}



void FlowCanvas::OnPaint( wxPaintEvent &WXUNUSED(event))
{
    wxList::compatibility_iterator node;
    hmiObject                     *object;
    flowObject                    *flow;
    wxSize                         textSize;

/*    wxPaintDC dc( this );
    parent->DoPrepareDC( dc );*/
    wxAutoBufferedPaintDC dc(this);//,wxBUFFER_VIRTUAL_AREA);

    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();

    dc.SetFont(*wxSWISS_FONT);

    {
       wxFont font;
       
       font=dc.GetFont();
#ifdef ENV_WINDOWS
       font.SetPointSize(7);
#else
       font.SetPointSize(8);
#endif
       dc.SetFont(font);
    }

    if (highlightObject)
    {
       dc.SetBrush(*wxLIGHT_GREY_BRUSH);
       dc.SetPen(*wxLIGHT_GREY_PEN);
       dc.DrawRectangle(highlightObject->getFlowPos().x-13,highlightObject->getFlowPos().y-13,highlightObject->getFlowWidth()+26,highlightObject->getFlowHeight()+26);
    }

    // draw connections
    if (m_drawObjectList) node=m_drawObjectList->getDrawableFlowObject(NULL);
    else node=NULL;
    while (node)
    {
       object=(hmiObject*)node->GetData();
       if ((currentObject==object) && (currentOutConnector)) object->drawFlowConnections(&dc,currentOutConnector,0,0);
       else object->drawFlowConnections(&dc,currentFlowCon,0,0);
/*       if (m_currentGroupObject)
       {
          wxNode              *conNode;
          FlowConnection      *connection;
          IOElementDefinition *IODef;

          conNode=object->getFlowConnection(NULL);
          while (conNode)
          {
             connection=(FlowConnection*)conNode->GetData();
             IODef=(IOElementDefinition*)(m_currentGroupObject->getOUTObjectList()->getObject(connection->data.targetID));
             if (IODef)
             {
                textSize=dc.GetTextExtent(IODef->name);
                dc.DrawRotatedText(IODef->name,IODef->getFlowPos().x-textSize.y+4,IODef->getFlowPos().y-1,90);

                if (IODef->data.IOFlag & OAPC_CHAR_IO_MASK) dc.DrawBitmap(*bmConChar,IODef->data.conPosx,IODef->data.conPosy);
                else if (IODef->data.IOFlag & OAPC_NUM_IO_MASK) dc.DrawBitmap(*bmConNum,IODef->data.conPosx,IODef->data.conPosy);
                else if (IODef->data.IOFlag & OAPC_DIGI_IO_MASK) dc.DrawBitmap(*bmConDigi,IODef->data.conPosx,IODef->data.conPosy);
             }
             conNode=object->getFlowConnection(conNode);
          }
       }*/
       node=m_drawObjectList->getDrawableFlowObject(node);
    }

    if (m_currentGroupObject)
    {
       IOElementDefinition *IODef;

       node=m_currentGroupObject->getOUTObjectList()->getDrawableFlowObject(NULL);
       while (node)
       {
          IODef=(IOElementDefinition*)node->GetData();

          textSize=dc.GetTextExtent(IODef->name);
          dc.DrawRotatedText(IODef->name,IODef->getFlowPos().x-textSize.y+4,IODef->getFlowPos().y-1,90);

          if (IODef->data.IOFlag & OAPC_CHAR_IO_MASK) dc.DrawBitmap(*bmConChar,IODef->data.conPosx,IODef->data.conPosy);
          else if (IODef->data.IOFlag & OAPC_NUM_IO_MASK) dc.DrawBitmap(*bmConNum,IODef->data.conPosx,IODef->data.conPosy);
          else if (IODef->data.IOFlag & OAPC_DIGI_IO_MASK) dc.DrawBitmap(*bmConDigi,IODef->data.conPosx,IODef->data.conPosy);
          else if (IODef->data.IOFlag & OAPC_BIN_IO_MASK) dc.DrawBitmap(*bmConBin,IODef->data.conPosx,IODef->data.conPosy);
          node=m_currentGroupObject->getOUTObjectList()->getDrawableFlowObject(node);
       }

       node=m_currentGroupObject->getINObjectList()->getDrawableFlowObject(NULL);
       while (node)
       {
          IODef=(IOElementDefinition*)node->GetData();
          if (IODef->data.IOFlag & OAPC_CHAR_IO_MASK) IODef->drawFlowConnections(&dc,currentFlowCon,wxPoint(0,0),bmConChar);
          else if (IODef->data.IOFlag & OAPC_NUM_IO_MASK) IODef->drawFlowConnections(&dc,currentFlowCon,wxPoint(0,0),bmConNum);
          else if (IODef->data.IOFlag & OAPC_DIGI_IO_MASK) IODef->drawFlowConnections(&dc,currentFlowCon,wxPoint(0,0),bmConDigi);
          else if (IODef->data.IOFlag & OAPC_BIN_IO_MASK) IODef->drawFlowConnections(&dc,currentFlowCon,wxPoint(0,0),bmConBin);
          node=m_currentGroupObject->getINObjectList()->getDrawableFlowObject(node);
       }
    }

    // draw symbols
    if (m_drawObjectList) node=m_drawObjectList->getDrawableFlowObject(NULL);
    else node=NULL;
    while (node)
    {
       object=(hmiObject*)node->GetData();

       if (object->data.type & FLOW_TYPE_MASK)
       {
          flow=(flowObject*)object;
          if (flow->flowBitmap)
           dc.DrawBitmap(*flow->flowBitmap,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_CONV_CHAR2DIGI)  dc.DrawBitmap(*bmChar2Digi,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_CONV_CHAR2NUM)   dc.DrawBitmap(*bmChar2Num,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_CONV_NUM2DIGI)   dc.DrawBitmap(*bmNum2Digi,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_CONV_NUM2BITS)   dc.DrawBitmap(*bmNum2Bits,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_CONV_NUM2CHAR)   dc.DrawBitmap(*bmNum2Char,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_CONV_DIGI2CHAR)  dc.DrawBitmap(*bmDigi2Char,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_CONV_DIGI2NUM)   dc.DrawBitmap(*bmDigi2Num,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_CONV_MIXED2CHAR) dc.DrawBitmap(*bmMixed2Char,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_CONV_DIGI2PAIR)  dc.DrawBitmap(*bmDigi2Pair,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_CONV_NUM2PAIR)   dc.DrawBitmap(*bmNum2Pair,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_CONV_CHAR2PAIR)  dc.DrawBitmap(*bmChar2Pair,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_CONV_PAIR2CHAR)  dc.DrawBitmap(*bmPair2Char,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_CONV_PAIR2DIGI)  dc.DrawBitmap(*bmPair2Digi,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_CONV_PAIR2NUM)   dc.DrawBitmap(*bmPair2Num,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_CONV_PAIR2BIN)   dc.DrawBitmap(*bmPair2Bin,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_CONV_BIN2PAIR)   dc.DrawBitmap(*bmBin2Pair,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_LOGI_DIGINOT)    dc.DrawBitmap(*bmDigiNOT,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_LOGI_DIGIRSFF)   dc.DrawBitmap(*bmDigiRSFF,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_LOGI_DIGITFF)    dc.DrawBitmap(*bmDigiTFF,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_LOGI_DIGINOP)    dc.DrawBitmap(*bmDigiNOP,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_LOGI_NUMNOT)     dc.DrawBitmap(*bmNumNOT,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_LOGI_NUMNOP)     dc.DrawBitmap(*bmNumNOP,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_LOGI_CHARNOP)    dc.DrawBitmap(*bmCharNOP,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_LOGI_BINNOP)     dc.DrawBitmap(*bmBinNOP,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_LOGI_DIGISHIFT)  dc.DrawBitmap(*bmDigiShift,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_LOGI_NUMSHIFT)   dc.DrawBitmap(*bmNumShift,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_LOGI_CHARSHIFT)  dc.DrawBitmap(*bmCharShift,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_LOGI_BINSHIFT)   dc.DrawBitmap(*bmBinShift,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_LOGI_DIGIOR)     dc.DrawBitmap(*bmDigiOR,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_LOGI_NUMOR)      dc.DrawBitmap(*bmNumOR,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_LOGI_DIGIXOR)    dc.DrawBitmap(*bmDigiXOR,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_LOGI_NUMXOR)     dc.DrawBitmap(*bmNumXOR,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_LOGI_DIGIAND)    dc.DrawBitmap(*bmDigiAND,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_LOGI_NUMAND)     dc.DrawBitmap(*bmNumAND,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_LOGI_NUMADD)     dc.DrawBitmap(*bmNumAdd,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_LOGI_NUMSUB)     dc.DrawBitmap(*bmNumSub,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_LOGI_NUMMUL)     dc.DrawBitmap(*bmNumMul,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_LOGI_NUMDIV)     dc.DrawBitmap(*bmNumDiv,object->getFlowPos().x,object->getFlowPos().y,false);
//          else if (flow->data.type==flowObject::FLOW_TYPE_LOGI_NUMNOP)     dc.DrawBitmap(*bmNumNOP,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_MATH_DIGICMP)    dc.DrawBitmap(*bmDigiCmp,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_MATH_NUMCMP)     dc.DrawBitmap(*bmNumCmp,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_MATH_DIGICTR)    dc.DrawBitmap(*bmDigiCtr,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_MATH_NUMCTR)     dc.DrawBitmap(*bmNumCtr,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_MATH_CHARCTR)    dc.DrawBitmap(*bmCharCtr,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_MATH_BINCTR)     dc.DrawBitmap(*bmBinCtr,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_MATH_CHARCMP)    dc.DrawBitmap(*bmCharCmp,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_FLOW_START)      dc.DrawBitmap(*bmFlowStart,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_FLOW_STOP)       dc.DrawBitmap(*bmFlowStop,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_FLOW_DELAY)      dc.DrawBitmap(*bmFlowDelay,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_FLOW_TIMER)      dc.DrawBitmap(*bmFlowTimer,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_FLOW_NUMTGATE)   dc.DrawBitmap(*bmFlowNumTGate,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_FLOW_CHARTGATE)  dc.DrawBitmap(*bmFlowCharTGate,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_FLOW_BINTGATE)   dc.DrawBitmap(*bmFlowBinTGate,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_FLOW_DIGITGATE)  dc.DrawBitmap(*bmFlowDigiTGate,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_FLOW_NUMGATE)    dc.DrawBitmap(*bmFlowNumGate,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_FLOW_CHARGATE)   dc.DrawBitmap(*bmFlowCharGate,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_FLOW_BINGATE)    dc.DrawBitmap(*bmFlowBinGate,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_FLOW_DIGIGATE)   dc.DrawBitmap(*bmFlowDigiGate,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_MISC_LOG)        dc.DrawBitmap(*bmMiscLog,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_MISC_ISCONNECT)  dc.DrawBitmap(*bmMiscISCO,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_MISC_LOG_REC)    dc.DrawBitmap(*bmMiscLogRec,object->getFlowPos().x,object->getFlowPos().y,false);
          else if (flow->data.type==flowObject::FLOW_TYPE_MISC_LOGIN)      dc.DrawBitmap(*bmMiscLogIn,object->getFlowPos().x,object->getFlowPos().y,false);
          else
           dc.DrawBitmap(*bmunknown,object->getFlowPos().x,object->getFlowPos().y,false);
       }
       else if ((object->data.stdIN==OAPC_DIGI_IO0) &&
    	          (object->data.stdOUT==OAPC_DIGI_IO0))
        dc.DrawBitmap(*bm1in1out,object->getFlowPos().x,object->getFlowPos().y,false);

       else if ((object->data.stdIN==(OAPC_DIGI_IO0|OAPC_DIGI_IO1)) &&
    	 	    (object->data.stdOUT==(OAPC_DIGI_IO0|OAPC_DIGI_IO1)))
        dc.DrawBitmap(*bm2in2out,object->getFlowPos().x,object->getFlowPos().y,false);

       else if ((object->data.stdIN==(OAPC_DIGI_IO0|OAPC_CHAR_IO3)) &&
    	          (object->data.stdOUT==OAPC_DIGI_IO0))
        dc.DrawBitmap(*bm1in1out1cin,object->getFlowPos().x,object->getFlowPos().y,false);

       else if ((object->data.stdIN==(OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_CHAR_IO3)) &&
    	 	    (object->data.stdOUT==(OAPC_DIGI_IO0|OAPC_DIGI_IO1)))
        dc.DrawBitmap(*bm2in2out1cin,object->getFlowPos().x,object->getFlowPos().y,false);

       else if ((object->data.stdIN==(OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_NUM_IO7)) &&
               (object->data.stdOUT==(OAPC_DIGI_IO0|OAPC_NUM_IO6|OAPC_NUM_IO7)))
        dc.DrawBitmap(*bm2in1out1nin2nout,object->getFlowPos().x,object->getFlowPos().y,false);
       else if ((object->data.stdIN==(OAPC_DIGI_IO0|OAPC_NUM_IO7)) &&
               (object->data.stdOUT==(OAPC_DIGI_IO0|OAPC_NUM_IO6|OAPC_NUM_IO7)))
        dc.DrawBitmap(*bm1in1out1nin2nout,object->getFlowPos().x,object->getFlowPos().y,false);
       else if ((object->data.stdIN==(OAPC_DIGI_IO0|OAPC_NUM_IO7)) &&
                (object->data.stdOUT==(OAPC_DIGI_IO0|OAPC_NUM_IO7)))
        dc.DrawBitmap(*bm1in1out1nin1nout,object->getFlowPos().x,object->getFlowPos().y,false);

       else if ((object->data.stdIN==(OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_CHAR_IO7)) &&
                (object->data.stdOUT==(OAPC_DIGI_IO0|OAPC_CHAR_IO6|OAPC_CHAR_IO7)))
        dc.DrawBitmap(*bm2in1out1cin2cout,object->getFlowPos().x,object->getFlowPos().y,false);

       else if ((object->data.stdIN==(OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2|
                                      OAPC_NUM_IO3|OAPC_NUM_IO4|OAPC_NUM_IO5)) &&
                (object->data.stdOUT==OAPC_DIGI_IO0))
        dc.DrawBitmap(*bmSelEnDigiSelEnDisNum,object->getFlowPos().x,object->getFlowPos().y,false);
       else if ((object->data.stdIN==(OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2)) &&
                (object->data.stdOUT==OAPC_DIGI_IO0))
        dc.DrawBitmap(*bmSelEnDigi,object->getFlowPos().x,object->getFlowPos().y,false);

       else if (object->data.type==HMI_TYPE_IMAGEBUTTON)
        dc.DrawBitmap(*bmimgbutton,object->getFlowPos().x,object->getFlowPos().y,false);
       else if (object->data.type==HMI_TYPE_IMAGE)
        dc.DrawBitmap(*bmimg,object->getFlowPos().x,object->getFlowPos().y,false);

       else if (/*(object->data.type==OAPC_TYPE_EXTERNAL_LIB) &&*/ (object->flowBitmap))
        dc.DrawBitmap(*object->flowBitmap,object->getFlowPos().x,object->getFlowPos().y,false);

       else
		dc.DrawBitmap(*bmunknown,object->getFlowPos().x,object->getFlowPos().y,false);

       node=m_drawObjectList->getDrawableFlowObject(node);

       textSize=dc.GetTextExtent(object->name);
       dc.DrawRotatedText(object->name,object->getFlowPos().x-textSize.y-1,object->getFlowPos().y+(object->getFlowHeight()/2)+(textSize.x/2),90);
    }

    if (currentFlowCon) // draw a single connection highlighted
    {
       if (inputObject)
       {
          if (g_flowObjects.isAtInConnector(inputObject,lastMousePos))
          {
             dc.SetPen(*wxGREEN_PEN);
             currentFlowCon->drawFlowConnection(&dc,wxGREEN_PEN,0,0);
          }
          else
          {
             dc.SetPen(*wxRED_PEN);
             currentFlowCon->drawFlowConnection(&dc,wxRED_PEN,0,0);
          }
       }
       else
       {
           if (currentFlowCon->data.targetID>0)
           {
              dc.SetPen(*wxGREEN_PEN);
              currentFlowCon->drawFlowConnection(&dc,wxGREEN_PEN,0,0);
           }
           else
           {
              dc.SetPen(*wxRED_PEN);
              currentFlowCon->drawFlowConnection(&dc,wxRED_PEN,0,0);
           }
       }
    }
    else if (currentOutConnector) // draw all connections that belong to an objects out-connector highlighted
    {
       wxNode         *node;
       FlowConnection *connection;

       node=currentObject->getFlowConnection(NULL);
       while (node)
       {
          connection=(FlowConnection*)node->GetData();
          if (connection->data.sourceOutput!=currentOutConnector)
          {
             if (connection->data.targetID>0) connection->drawFlowConnection(&dc,wxBLACK_PEN,0,0);
             else connection->drawFlowConnection(&dc,wxLIGHT_GREY_PEN,0,0);
          }
          else
          {
             if (connection->data.targetID>0) connection->drawFlowConnection(&dc,wxGREEN_PEN,0,0);
             else connection->drawFlowConnection(&dc,wxRED_PEN,0,0);
          }
          node=currentObject->getFlowConnection(node);
       }
    }
    else if (currentInConnector) // draw all connections that belong to an objects in-connector highlighted
    {
       // to be optimized: this part draws over lines tha already have been drawn
       drawInLines(currentObject,m_drawObjectList,&dc);
       if (m_currentGroupObject) drawInLines(currentObject,m_currentGroupObject->getINObjectList(),&dc);
    }

    if ((isMouseDrag) && (currentFlowCon))
    {
       switch (drawDir)
       {
          case DRAW_DIR_VERT_DOWN:
          case DRAW_DIR_VERT:
             lastMousePos.x=currentFlowCon->getLastPos().x;
             dc.DrawLine(currentFlowCon->getLastPos().x,currentFlowCon->getLastPos().y,lastMousePos.x,lastMousePos.y);
             break;
          case DRAW_DIR_HORIZONTAL_RIGHT:
          case DRAW_DIR_HORIZONTAL:
             lastMousePos.y=currentFlowCon->getLastPos().y;
             dc.DrawLine(currentFlowCon->getLastPos().x,currentFlowCon->getLastPos().y,lastMousePos.x,lastMousePos.y);
             break;
          default:
             break;
       }
    }
    if ((rectStart.x>=0) && (rectEnd.x>=0))
    {
       dc.SetBrush(*wxTRANSPARENT_BRUSH);
       dc.SetPen(BLUE_PEN_DOT);
       dc.DrawRectangle(rectStart.x,rectStart.y,rectEnd.x-rectStart.x,rectEnd.y-rectStart.y);
    }
}



void FlowCanvas::OnPutControl(wxCommandEvent &WXUNUSED(event))
{
    flowObject         *object;
    DlgControlSelector  dlg(g_mainWin,_T("Select a Control"),DLGCONTROLSELECTOR_FLAG_SHOWFLOWSTATE|DLGCONTROLSELECTOR_FLAG_ALLOWSORT); //1,false);
    wxPoint             setFlowPos;

    setFlowPos=lastMousePos; // avoids that the first mouse-up event of the closing window is misinterpreted as new position
    dlg.ShowModal();
    object=dlg.getSelectedControl();
    if (object)
    {
        g_mainWin->addUndoStep(_T("Put Control"));
        if (object->getFlowPos().x>-1)
        {
           if (wxMessageBox(_("This control already exists as flow element.\nDo you really want to put it at this new position?"),_("Question"), wxICON_QUESTION|wxYES_NO)==wxNO) return;
        }
        object->setFlowPos(m_drawObjectList,setFlowPos,0,1);
        g_isSaved=false;
        Refresh();
    }
}



void FlowCanvas::OnDelConnection(wxCommandEvent &WXUNUSED(event))
{    
    wxASSERT(currentObject);
    g_mainWin->addUndoStep(_T("Delete Connection"));
    if (currentObject) currentObject->deleteFlowConnection(currentFlowCon);
    currentFlowCon=NULL;
    g_isSaved=false;
    Refresh();
}



void FlowCanvas::OnDelInput(wxCommandEvent &WXUNUSED(event))
{
   wxASSERT(currentFlowCon);
   if (currentFlowCon)
   {
      g_mainWin->addUndoStep(_T("Disconnect Input"));
      currentFlowCon->data.targetID=0;
      currentFlowCon->data.targetInput=0;
      currentFlowCon->removeLastPos();
      g_isSaved=false;
      Refresh();
   }
}



void FlowCanvas::OnDelElem(wxCommandEvent &WXUNUSED(event))
{
   wxNode              *node,*flowNode;
   flowObject          *object;
   IOElementDefinition *IODef;
   FlowConnection      *connection;
   wxInt32              i;

   wxASSERT(currentObject);
   if (currentObject==g_flowLog) g_flowLog=NULL;
   g_mainWin->addUndoStep(_T("Delete Element"));
   if (currentObject)
   {
      // disable the flow element
      if (currentObject->data.type==hmiObject::FLOW_TYPE_GROUP_IO_DEFINITION)
      {
         ((IOElementDefinition*)currentObject)->setConnectorPos(m_drawObjectList,wxPoint(-1,-1),1,1);

         // set incoming connections to not connected
         node=m_drawObjectList->getObject((wxNode*)NULL);
         for (i=0; i<2; i++)
         {
            while (node)
            {
               IODef=(IOElementDefinition*)node->GetData();
               if (IODef->data.id!=((IOElementDefinition*)currentObject)->data.id)
               {
                  flowNode=IODef->getFlowConnection(NULL);
                  while (flowNode)
                  {
                     connection=(FlowConnection*)flowNode->GetData();
                     if (connection->data.targetID==((IOElementDefinition*)currentObject)->data.id)
                     {
                        connection->data.targetID=0;
                        connection->data.targetInput=0;
                     }
                     flowNode=IODef->getFlowConnection(flowNode);
                  }
               }
               node=m_drawObjectList->getObject(node);
            }
            node=m_currentGroupObject->getINObjectList()->getObject((wxNode*)NULL);
         }
         if (!m_currentGroupObject->getParentObject()) node=g_objectList.getObject((wxNode*)NULL);
         else node=m_currentGroupObject->getParentObject()->getObject((wxNode*)NULL);

         while (node)
         {
            object=(flowObject*)node->GetData();
            if (object->data.id!=((IOElementDefinition*)currentObject)->data.id)
            {
               flowNode=object->getFlowConnection(NULL);
               while (flowNode)
               {
                  connection=(FlowConnection*)flowNode->GetData();
                  if (connection->data.targetInput==((IOElementDefinition*)currentObject)->data.id)
                  {
                     connection->data.targetID=0;
                     connection->data.targetInput=0;
                  }
                  flowNode=object->getFlowConnection(flowNode);
               }
            }
            node=m_drawObjectList->getObject(node);
         }
         // delete the object completely in case it doesn't has a corresponding HMI element
         m_currentGroupObject->getOUTObjectList()->DeleteObject(currentObject);
         m_currentGroupObject->getINObjectList()->DeleteObject(currentObject);
         m_currentGroupObject->createFlowBitmap();
         delete currentObject;
      }
      else
      {
         currentObject->setFlowPos(m_drawObjectList,wxPoint(-1,-1),1,1);
         node=m_drawObjectList->getObject((wxNode*)NULL);

         for (wxByte i=0; i<2; i++)
         {
            // set incoming connections to not connected

            while (node)
            {
               object=(flowObject*)node->GetData();
               if (object->data.id!=currentObject->data.id)
               {
                  flowNode=object->getFlowConnection(NULL);
                  while (flowNode)
                  {
                     connection=(FlowConnection*)flowNode->GetData();
                     if (connection->data.targetID==currentObject->data.id)
                     {
                        connection->data.targetID=0;
                        connection->data.targetInput=0;
                     }
                     flowNode=object->getFlowConnection(flowNode);
                  }
               }
               node=m_drawObjectList->getObject(node);
            }
            if (m_currentGroupObject)
            {
               node=m_currentGroupObject->getINObjectList()->getObject((wxNode*)NULL); // also disconnect incomming connectors
            }
            else break;
         }
         // delete the object completely in case it doesn't has a corresponding HMI element
         if (currentObject->data.type & FLOW_TYPE_MASK)
         {
            m_drawObjectList->DeleteObject(currentObject);
            delete currentObject;
         }
         else // elsewhere delete only the outgoing connections
         {
            FlowConnection *connection;
            wxNode         *node;

            node=currentObject->getFlowConnection((wxNode*)NULL);
            while (node)
            {
               connection=(FlowConnection*)node->GetData();
               currentObject->deleteFlowConnection(connection);
               node=currentObject->getFlowConnection((wxNode*)NULL);
            }
         }
      }
      
      currentObject=NULL;
      g_isSaved=false;
      Refresh();
   }
}



void FlowCanvas::OnEditElem(wxCommandEvent &WXUNUSED(event))
{
   flowObject *editObject;

   editObject=currentObject;
   wxASSERT(currentObject);
   if (!currentObject) return;
   
   g_mainWin->addUndoStep(_T("Edit Element"));
   editObject->doDataFlowDialog(false);
   if (editObject->data.type==hmiObject::FLOW_TYPE_GROUP_IO_DEFINITION) m_currentGroupObject->createFlowBitmap();
   else if (editObject->data.type==hmiObject::FLOW_TYPE_GROUP) ((flowGroup*)editObject)->createFlowBitmap();
   Refresh();
}



void FlowCanvas::OnEnterGroup(wxCommandEvent &WXUNUSED(event))
{
   wxASSERT(currentObject);
   wxASSERT(currentObject->data.type & hmiObject::FLOW_TYPE_GROUP);
   if ((currentObject) && (currentObject->data.type & hmiObject::FLOW_TYPE_GROUP))
   {
      m_drawObjectList=((flowGroup*)currentObject)->getObjectList();
      m_currentGroupObject=(flowGroup*)currentObject;
   }
   Refresh();
}



void FlowCanvas::loadGroup(wxString filepath,bool display)
{
   wxFile      *FHandle;
   char         rawData[5];
   wxInt32      chunkSize,l;
   size_t       readSize;
   wxUint32     minID=0,maxID=0;

   rawData[4]=0;
   FHandle=new wxFile(filepath,wxFile::read);
   if (FHandle->IsOpened())
   {
      g_mainWin->addUndoStep(_T("Load Group"));
      FHandle->Read(rawData,4);
      if (strncmp(rawData,CHUNK_FORM,4))
      {
         FHandle->Close();
         delete FHandle;
         wxMessageBox(filepath+_T(":\n")+_("No valid IFF file!"),_("Error"),wxICON_ERROR|wxOK|wxCENTRE);
         return;
      }
      FHandle->Seek(4,wxFromCurrent); // TODO: evaluate length information

      FHandle->Read(rawData,4);
      if (strncmp(rawData,CHUNK_APCG,4))
      {
         FHandle->Close();
         delete FHandle;
         wxMessageBox(filepath+_T(":\n")+_("No valid APCP file!"),_("Error"),wxICON_ERROR|wxOK|wxCENTRE);
         return;
      }
      g_isEmpty=false;
      g_isSaved=false;
      while (1)
      {
         readSize=FHandle->Read(rawData,4);
         readSize+=FHandle->Read(&chunkSize,4);
         if (readSize!=8) break;
         chunkSize=ntohl(chunkSize);

         if (chunkSize>0)
         {
            if (!strncmp(rawData,CHUNK_NAME,4))
            {
            }
            else if (!strncmp(rawData,CHUNK_ANNO,4))
            {
            }
            else if (!strncmp(rawData,CHUNK_VERS,4))
            {
            }
            else if (!strncmp(rawData,CHUNK_GPRJ,4))
            {
               struct hmiGroupProjectData grpData;

               chunkSize-=FHandle->Read(&grpData,sizeof(struct hmiGroupProjectData));
               grpData.flowW          =ntohl(grpData.flowW);
               grpData.flowH          =ntohl(grpData.flowH);
               if (g_objectList.m_projectData->flowW<grpData.flowW) g_objectList.m_projectData->flowW=grpData.flowW;
               if (g_objectList.m_projectData->flowH<grpData.flowH) g_objectList.m_projectData->flowH=grpData.flowH;
               refreshProject(0);
            }
            else if (!strncmp(rawData,CHUNK_GRMM,4))
            {
               chunkSize-=FHandle->Read(&l,4); minID=htonl(l);
               chunkSize-=FHandle->Read(&l,4); maxID=htonl(l);
            }
            else if (!strncmp(rawData,CHUNK_FLOW,4))
            {
               flowGroup *loadedGroup=NULL;
               wxUint32   loadedGroupID;
               bool       hasISCOElement; //unused here but required by method

               chunkSize=m_drawObjectList->loadFlowObjectData(FHandle,chunkSize,minID,&loadedGroupID,display,false,&g_objectList,OBJECT_MODE_FLOWELEMENT,&hasISCOElement);
               loadedGroup=(flowGroup*)g_objectList.getObject(loadedGroupID);
               if (loadedGroup) loadedGroup->setFlowPos(NULL,lastMousePos,1,0);
               wxASSERT(chunkSize==0);
            }
            else wxASSERT(0);
            FHandle->Seek(chunkSize,wxFromCurrent);
         }
      }
      FHandle->Close();
      g_objectList.updateUniqueID(g_objectList.currentUniqueID()+maxID-minID);
   }
   else
   {
      delete FHandle;
      wxMessageBox(filepath+_T(":\n")+_("Could not open file!"),_("Error"),wxICON_ERROR|wxOK|wxCENTRE);
      return;
   }
   delete FHandle;
}



void FlowCanvas::OnLoadGroup(wxCommandEvent &WXUNUSED(event))
{
   wxFileDialog* openFileDialog=new wxFileDialog( this, _("Load Group"),m_groupDir,m_groupFile,GROUP_FILETYPES,wxFD_OPEN, wxDefaultPosition);

   if ( openFileDialog->ShowModal() == wxID_OK )
   {
      wxString path;

      path=openFileDialog->GetPath();
      oapc_path_split(&path,&m_groupDir,&m_groupFile,wxEmptyString);
      loadGroup(path,true);
      delete openFileDialog;
   }
   else delete openFileDialog;
   Refresh();
}



void FlowCanvas::OnSaveGroup(wxCommandEvent &WXUNUSED(event))
{
   wxUint32    minID=0xFFFFFFFF,maxID=0;
   flowGroup  *saveObject;

   saveObject=(flowGroup*)currentObject;
   wxASSERT(currentObject);
   if (!currentObject) return;
   wxASSERT(currentObject->data.type==hmiObject::FLOW_TYPE_GROUP);
   if (currentObject->data.type!=hmiObject::FLOW_TYPE_GROUP) return;

   saveObject->getMinMaxIDs(minID,maxID);
   wxFileDialog* saveFileDialog=new wxFileDialog( this,_("Save group as")+_T("..."),m_groupDir,m_groupFile,GROUP_FILETYPES,wxFD_SAVE|wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
   if (saveFileDialog->ShowModal() == wxID_OK )
   {
      wxFile  *FHandle=new wxFile();
      wxString data,path;
      wxInt32  l,length,lastPos;

      path=saveFileDialog->GetPath();
      oapc_path_split(&path,&m_groupDir,&m_groupFile,_T(".apcg"));
      delete saveFileDialog;
      delete m_externalMacros;
      m_externalMacros=NULL;

      FHandle->Create(path,true);
      if (FHandle->IsOpened())
      {
         struct hmiGroupProjectData grpData;

         FHandle->Write(CHUNK_FORM"    "CHUNK_APCG,12); // ********************************************************

         data=_T("ControlRoom Functional Group"); length=htonl(data.Length());
         FHandle->Write(CHUNK_ANNO,4);  // ****************************************************************
         FHandle->Write(&length,4); FHandle->Write(wxConvUTF8.cWC2MB(data),data.Length());

         FHandle->Write(CHUNK_VERS,4);  // ****************************************************************
         length=htonl(4); FHandle->Write(&length,4);
         l=htonl(1);      FHandle->Write(&l,4);

         FHandle->Write(CHUNK_GPRJ,4);  /****************************************************************/
         length=htonl(sizeof(struct hmiGroupProjectData));
         FHandle->Write(&length,4);
         grpData.version        =htonl(1);
         grpData.flowW          =htonl(g_objectList.m_projectData->flowW);
         grpData.flowH          =htonl(g_objectList.m_projectData->flowH);
         grpData.res1=0;         
         grpData.res2=0;
         grpData.res3=0;         
         grpData.res4=0;
         grpData.store_descr[0]=0;
         FHandle->Write(&grpData,sizeof(struct hmiGroupProjectData));

         FHandle->Write(CHUNK_GRMM,4); // group minimum and maximum IDs ***************************************************
         length=htonl(8); FHandle->Write(&length,4);
         l=htonl(minID);  FHandle->Write(&l,4);
         l=htonl(maxID);  FHandle->Write(&l,4);

         FHandle->Write(CHUNK_FLOW,4); //flow connections list ***************************************************
         length=htonl(0); FHandle->Write(&length,4);
         lastPos=FHandle->Tell()-4;

         saveObject->saveFGRP(FHandle,0);

         l=htonl(FHandle->Tell()-lastPos-4); // write length of chunk
         FHandle->Seek(lastPos,wxFromStart);
         FHandle->Write(&l,4);
         FHandle->Seek(0,wxFromEnd);

         l=htonl(FHandle->Tell()-8); // total size of file data
         FHandle->Seek(4,wxFromStart);
         FHandle->Write(&l,4);

         FHandle->Close();
         m_externalMacros=new ExternalMacros();
      }
      delete FHandle;
   }
   else delete saveFileDialog;
}



void FlowCanvas::OnLeaveGroup(wxCommandEvent &WXUNUSED(event))
{
   wxASSERT(m_currentGroupObject);
   if (m_currentGroupObject)
   {
      m_currentGroupObject=m_currentGroupObject->getParentObject();
      if (m_currentGroupObject) m_drawObjectList=m_currentGroupObject->getObjectList();
      else m_drawObjectList=&g_objectList;
   }
   Refresh();
}



void FlowCanvas::OnPopupMenu(wxCommandEvent &event)
{
   wxByte foundID=1;

   switch (event.GetId())
   {
      case POPUP_ADD_CONV_DIGI2NUM:
      case POPUP_ADD_CONV_DIGI2CHAR:
      case POPUP_ADD_CONV_NUM2DIGI:
      case POPUP_ADD_CONV_NUM2BITS:
      case POPUP_ADD_CONV_NUM2CHAR:
      case POPUP_ADD_CONV_CHAR2DIGI:
      case POPUP_ADD_CONV_CHAR2NUM:
      case POPUP_ADD_CONV_MIXED2CHAR:
      case POPUP_ADD_LOGI_DIGIRSFF:
      case POPUP_ADD_LOGI_DIGITFF:
      case POPUP_ADD_LOGI_DIGINOT:
      case POPUP_ADD_LOGI_NUMNOT:
      case POPUP_ADD_LOGI_DIGINOP:
      case POPUP_ADD_LOGI_NUMNOP:
      case POPUP_ADD_LOGI_CHARNOP:
      case POPUP_ADD_LOGI_BINNOP:
      case POPUP_ADD_LOGI_DIGISHIFT:
      case POPUP_ADD_LOGI_NUMSHIFT:
      case POPUP_ADD_LOGI_CHARSHIFT:
      case POPUP_ADD_LOGI_BINSHIFT:
      case POPUP_ADD_LOGI_DIGIOR:
      case POPUP_ADD_LOGI_NUMOR:
      case POPUP_ADD_LOGI_DIGIXOR:
      case POPUP_ADD_LOGI_NUMXOR:
      case POPUP_ADD_LOGI_DIGIAND:
      case POPUP_ADD_LOGI_NUMAND:
      case POPUP_ADD_LOGI_NUMADD:
      case POPUP_ADD_LOGI_NUMSUB:
      case POPUP_ADD_LOGI_NUMMUL:
      case POPUP_ADD_LOGI_NUMDIV:
      case POPUP_ADD_MATH_DIGICMP:
      case POPUP_ADD_MATH_NUMCMP:
      case POPUP_ADD_MATH_CHARCMP:
      case POPUP_ADD_MATH_DIGICTR:
      case POPUP_ADD_MATH_NUMCTR:
      case POPUP_ADD_MATH_CHARCTR:
      case POPUP_ADD_MATH_BINCTR:
      case POPUP_ADD_FLOW_START:
      case POPUP_ADD_FLOW_STOP:
      case POPUP_ADD_FLOW_DELAY:
      case POPUP_ADD_FLOW_TIMER:
      case POPUP_ADD_FLOW_NUMTGATE:
      case POPUP_ADD_FLOW_NUMGATE:
      case POPUP_ADD_FLOW_CHARTGATE:
      case POPUP_ADD_FLOW_CHARGATE:
      case POPUP_ADD_FLOW_BINTGATE:
      case POPUP_ADD_FLOW_BINGATE:
      case POPUP_ADD_FLOW_DIGITGATE:
      case POPUP_ADD_FLOW_DIGIGATE:
      case POPUP_ADD_MISC_LOG:
      case POPUP_ADD_MISC_LOG_REC:
      case POPUP_ADD_MISC_LOGIN:
      case POPUP_ADD_MISC_ISCO:
      case POPUP_ADD_CONV_DIGI2PAIR:
      case POPUP_ADD_CONV_NUM2PAIR:
      case POPUP_ADD_CONV_CHAR2PAIR:
      case POPUP_ADD_CONV_PAIR2CHAR:
      case POPUP_ADD_CONV_PAIR2NUM:
      case POPUP_ADD_CONV_PAIR2DIGI:
      case POPUP_ADD_CONV_PAIR2BIN:
      case POPUP_ADD_CONV_BIN2PAIR:
         {
         flowObject *object=NULL;

         g_mainWin->addUndoStep(_T("Add Control"));
         m_lastElementID=event.GetId();
         g_isEmpty=false;
         g_isSaved=false;
         switch (event.GetId())
         {
            case POPUP_ADD_CONV_DIGI2NUM:
               object=new flowConverterDigi2Num();
               break;
            case POPUP_ADD_CONV_DIGI2CHAR:
               object=new flowConverterDigi2Char();
               break;
            case POPUP_ADD_CONV_NUM2DIGI:
               object=new flowConverterNum2Digi();
               break;
            case POPUP_ADD_CONV_NUM2BITS:
               object=new flowConverterNum2Bits();
               break;
            case POPUP_ADD_CONV_NUM2CHAR:
               object=new flowConverterNum2Char();
               break;
            case POPUP_ADD_CONV_CHAR2DIGI:
               object=new flowConverterChar2Digi();
               break;
            case POPUP_ADD_CONV_CHAR2NUM:
               object=new flowConverterChar2Num();
               break;
            case POPUP_ADD_CONV_MIXED2CHAR:
               object=new flowConverterMixed2Char();
               break;
            case POPUP_ADD_CONV_DIGI2PAIR:
               object=new flowConverterDigi2Pair();
               break;
            case POPUP_ADD_CONV_NUM2PAIR:
               object=new flowConverterNum2Pair();
               break;
            case POPUP_ADD_CONV_CHAR2PAIR:
               object=new flowConverterChar2Pair();
               break;
            case POPUP_ADD_CONV_PAIR2DIGI:
               object=new flowConverterPair2Digi();
               break;
            case POPUP_ADD_CONV_PAIR2NUM:
               object=new flowConverterPair2Num();
               break;
            case POPUP_ADD_CONV_PAIR2CHAR:
               object=new flowConverterPair2Char();
               break;
            case POPUP_ADD_CONV_PAIR2BIN:
               object=new flowConverterPair2Bin();
               break;
            case POPUP_ADD_CONV_BIN2PAIR:
               object=new flowConverterBin2Pair();
               break;
            case POPUP_ADD_LOGI_DIGIRSFF:
               object=new flowLogicDigiRSFF();
               break;
            case POPUP_ADD_LOGI_DIGITFF:
               object=new flowLogicDigiTFF();
               break;
            case POPUP_ADD_LOGI_DIGINOT:
               object=new flowLogicDigiNOT();
               break;
            case POPUP_ADD_LOGI_NUMNOT:
               object=new flowLogicNumNOT();
               break;
            case POPUP_ADD_LOGI_DIGINOP:
               object=new flowLogicDigiNOP();
               break;
            case POPUP_ADD_LOGI_NUMNOP:
               object=new flowLogicNumNOP();
               break;
            case POPUP_ADD_LOGI_CHARNOP:
               object=new flowLogicCharNOP();
               break;
            case POPUP_ADD_LOGI_BINNOP:
               object=new flowLogicBinNOP();
               break;
            case POPUP_ADD_LOGI_DIGISHIFT:
               object=new flowLogicDigiShift();
               break;
            case POPUP_ADD_LOGI_NUMSHIFT:
               object=new flowLogicNumShift();
               break;
            case POPUP_ADD_LOGI_CHARSHIFT:
               object=new flowLogicCharShift();
               break;
            case POPUP_ADD_LOGI_BINSHIFT:
               object=new flowLogicBinShift();
               break;
            case POPUP_ADD_LOGI_DIGIOR:
               object=new flowLogicDigiConcat(hmiObject::FLOW_TYPE_LOGI_DIGIOR);
               break;
            case POPUP_ADD_LOGI_NUMOR:
               object=new flowLogicNumConcat(hmiObject::FLOW_TYPE_LOGI_NUMOR);
               break;
            case POPUP_ADD_LOGI_DIGIXOR:
               object=new flowLogicDigiConcat(hmiObject::FLOW_TYPE_LOGI_DIGIXOR);
               break;
            case POPUP_ADD_LOGI_NUMXOR:
               object=new flowLogicNumConcat(hmiObject::FLOW_TYPE_LOGI_NUMXOR);
               break;
            case POPUP_ADD_LOGI_DIGIAND:
               object=new flowLogicDigiConcat(hmiObject::FLOW_TYPE_LOGI_DIGIAND);
               break;
            case POPUP_ADD_LOGI_NUMAND:
               object=new flowLogicNumConcat(hmiObject::FLOW_TYPE_LOGI_NUMAND);
               break;
            case POPUP_ADD_LOGI_NUMADD:
               object=new flowLogicNumConcat(hmiObject::FLOW_TYPE_LOGI_NUMADD);
               break;
            case POPUP_ADD_LOGI_NUMSUB:
               object=new flowLogicNumConcat(hmiObject::FLOW_TYPE_LOGI_NUMSUB);
               break;
            case POPUP_ADD_LOGI_NUMMUL:
               object=new flowLogicNumConcat(hmiObject::FLOW_TYPE_LOGI_NUMMUL);
               break;
            case POPUP_ADD_LOGI_NUMDIV:
               object=new flowLogicNumConcat(hmiObject::FLOW_TYPE_LOGI_NUMDIV);
               break;
            case POPUP_ADD_MATH_DIGICMP:
               object=new flowMathDigiCmp();
               break;
            case POPUP_ADD_MATH_NUMCMP:
               object=new flowMathNumCmp();
               break;
            case POPUP_ADD_MATH_CHARCMP:
               object=new flowMathCharCmp();
               break;
            case POPUP_ADD_MATH_DIGICTR:
               object=new flowMathDigiCtr();
               break;
            case POPUP_ADD_MATH_NUMCTR:
               object=new flowMathNumCtr();
               break;
            case POPUP_ADD_MATH_CHARCTR:
               object=new flowMathCharCtr();
               break;
            case POPUP_ADD_MATH_BINCTR:
               object=new flowMathBinCtr();
               break;
            case POPUP_ADD_FLOW_STOP:
               object=new flowStop();
               break;
            case POPUP_ADD_FLOW_START:
               object=new flowStart();
               break;
            case POPUP_ADD_FLOW_DELAY:
               object=new flowDelay();
               break;
            case POPUP_ADD_FLOW_TIMER:
               object=new flowTimer();
               break;
            case POPUP_ADD_FLOW_NUMTGATE:
               object=new flowNumTrigGate();
               break;
            case POPUP_ADD_FLOW_NUMGATE:
               object=new flowNumGate();
               break;
            case POPUP_ADD_FLOW_CHARTGATE:
               object=new flowCharTrigGate();
               break;
            case POPUP_ADD_FLOW_CHARGATE:
               object=new flowCharGate();
               break;
            case POPUP_ADD_FLOW_BINTGATE:
               object=new flowBinTrigGate();
               break;
            case POPUP_ADD_FLOW_BINGATE:
               object=new flowBinGate();
               break;
            case POPUP_ADD_FLOW_DIGITGATE:
               object=new flowDigiTrigGate();
               break;
            case POPUP_ADD_FLOW_DIGIGATE:
               object=new flowDigiGate();
               break;
            case POPUP_ADD_MISC_LOG:
               if (g_flowLog==NULL) 
               {
                  object=new flowLog();
                  g_flowLog=(flowLog*)object;
               }
               break;
            case POPUP_ADD_MISC_LOG_REC:
               object=new flowLogRecorder();
               break;
            case POPUP_ADD_MISC_LOGIN:
               object=new flowLogInUser();
               break;
            case POPUP_ADD_MISC_ISCO:
               object=new flowISConnect();
               break;
            default:
               wxASSERT(0);
               break;
         }
         if (!object) return;
         object->setFlowPos(m_drawObjectList,lastMousePos,1,1);
         if (m_currentGroupObject)
         {
            // TODO: make getUniqueID() global and check if we can remove this assignment here
            if (object->data.id<=0) object->data.id=g_objectList.getUniqueID();
            m_currentGroupObject->getObjectList()->addObject(object,true,false);
         }
         else g_objectList.addObject(object,true,false);
         Refresh();
         break;
         }
      case POPUP_ADD_INCONNECTOR_DIGI:
      case POPUP_ADD_INCONNECTOR_NUM:
      case POPUP_ADD_INCONNECTOR_CHAR:
      case POPUP_ADD_INCONNECTOR_BIN:
         {
         IOElementDefinition *IODef;
         wxString             result;

         m_lastElementID=event.GetId();
         g_isEmpty=false;
         g_isSaved=false;
         if (!m_currentGroupObject) return;
         g_mainWin->addUndoStep(_T("Add Connector"));
         if (m_currentGroupObject->getParentObject()) IODef=new IOElementDefinition(1,m_currentGroupObject->getParentObject()->getObjectList(),lastMousePos);
         else IODef=new IOElementDefinition(1,&g_objectList,lastMousePos);

         if (event.GetId()==POPUP_ADD_INCONNECTOR_DIGI) IODef->data.IOFlag=OAPC_DIGI_IO_MASK; // use mask to set all bits, the bit number doesn't matters for a group
         else if (event.GetId()==POPUP_ADD_INCONNECTOR_NUM) IODef->data.IOFlag=OAPC_NUM_IO_MASK; // use mask to set all bits, the bit number doesn't matters for a group
         else if (event.GetId()==POPUP_ADD_INCONNECTOR_CHAR) IODef->data.IOFlag=OAPC_CHAR_IO_MASK; // use mask to set all bits, the bit number doesn't matters for a group
         else if (event.GetId()==POPUP_ADD_INCONNECTOR_BIN) IODef->data.IOFlag=OAPC_BIN_IO_MASK; // use mask to set all bits, the bit number doesn't matters for a group
         else wxASSERT(0);
         IODef->data.posx=7+(m_currentGroupObject->getINObjectList()->GetCount()*13);
         IODef->data.posy=0;
         IODef->data.conPosx=lastMousePos.x;
         IODef->data.conPosy=lastMousePos.y;
         IODef->data.id=g_objectList.getUniqueID();

         result=wxString::Format(_T("IN%d"),m_currentGroupObject->getINObjectList()->GetCount());
         result=wxGetTextFromUser(_("I/O Definition"),_("Name"),result,NULL);
         if (result.Length()>0) IODef->name=result;
         if (IODef->name.Length()>10) IODef->name=IODef->name.Mid(0,10);

         m_currentGroupObject->getINObjectList()->addObject(IODef,true,false);
         m_currentGroupObject->createFlowBitmap();

         Refresh();
         break;
         }
      case POPUP_ADD_OUTCONNECTOR_DIGI:
      case POPUP_ADD_OUTCONNECTOR_NUM:
      case POPUP_ADD_OUTCONNECTOR_CHAR:
      case POPUP_ADD_OUTCONNECTOR_BIN:
         {
         IOElementDefinition *IODef;
         wxString             result;

         m_lastElementID=event.GetId();
         g_isEmpty=false;
         g_isSaved=false;
         if (!m_currentGroupObject) return;
         g_mainWin->addUndoStep(_T("Add Connector"));
         if (m_currentGroupObject->getParentObject()) IODef=new IOElementDefinition(0,m_currentGroupObject->getParentObject()->getObjectList(),lastMousePos);
         else IODef=new IOElementDefinition(0,&g_objectList,lastMousePos);

         if (event.GetId()==POPUP_ADD_OUTCONNECTOR_DIGI) IODef->data.IOFlag=OAPC_DIGI_IO_MASK; // use mask to set all bits, the bit number doesn't matters for a group
         else if (event.GetId()==POPUP_ADD_OUTCONNECTOR_NUM) IODef->data.IOFlag=OAPC_NUM_IO_MASK; // use mask to set all bits, the bit number doesn't matters for a group
         else if (event.GetId()==POPUP_ADD_OUTCONNECTOR_CHAR) IODef->data.IOFlag=OAPC_CHAR_IO_MASK; // use mask to set all bits, the bit number doesn't matters for a group
         else if (event.GetId()==POPUP_ADD_OUTCONNECTOR_BIN) IODef->data.IOFlag=OAPC_BIN_IO_MASK; // use mask to set all bits, the bit number doesn't matters for a group
         else wxASSERT(0);
         IODef->data.posx=7+(m_currentGroupObject->getOUTObjectList()->GetCount()*13);
         IODef->data.posy=m_currentGroupObject->getFlowHeight();
         IODef->data.conPosx=lastMousePos.x;
         IODef->data.conPosy=lastMousePos.y;
         IODef->data.id=g_objectList.getUniqueID();

         result=wxString::Format(_T("OUT%d"),m_currentGroupObject->getOUTObjectList()->GetCount());
         result=wxGetTextFromUser(_("I/O Definition"),_("Name"),result,NULL);
         if (result.Length()>0) IODef->name=result;

         m_currentGroupObject->getOUTObjectList()->addObject(IODef,true,false);
         m_currentGroupObject->createFlowBitmap();

         Refresh();
         break;
         }
      case POPUP_EXTEND_TO_LEFT:
         g_mainWin->addUndoStep(_T("To Left"));
         m_drawObjectList->moveFlowPos(wxPoint(160,0),wxPoint(-1,-1));
         if (m_currentGroupObject)
         {
            m_currentGroupObject->getOUTObjectList()->moveFlowPos(wxPoint(160,0),wxPoint(-1,-1));
            m_currentGroupObject->getINObjectList()->moveFlowPos(wxPoint(160,0),wxPoint(-1,-1));
         }
      case POPUP_EXTEND_TO_RIGHT:
         g_mainWin->addUndoStep(_T("To Right"));
         g_objectList.m_projectData->flowW+=160;
         refreshProject(0);
    	 break;
      case POPUP_EXTEND_TO_TOP:
         g_mainWin->addUndoStep(_T("To Top"));
         m_drawObjectList->moveFlowPos(wxPoint(0,100),wxPoint(-1,-1));
         if (m_currentGroupObject)
         {
//            m_currentGroupObject->getObjectList()->moveFlowPos(wxPoint(0,100));
            m_currentGroupObject->getOUTObjectList()->moveFlowPos(wxPoint(0,100),wxPoint(-1,-1));
            m_currentGroupObject->getINObjectList()->moveFlowPos(wxPoint(0,100),wxPoint(-1,-1));
         }
      case POPUP_EXTEND_TO_BOTTOM:
         g_mainWin->addUndoStep(_T("To Bottom"));
         g_objectList.m_projectData->flowH+=100;
         refreshProject(0);
    	 break;
      case POPUP_EXTEND_INSERT_ROW:
         g_mainWin->addUndoStep(_T("Insert Row"));
     	 g_objectList.m_projectData->flowH+=100;
         m_drawObjectList->moveFlowPos(wxPoint(0,100),wxPoint(-1,lastMousePos.y));
         if (m_currentGroupObject)
         {
            m_currentGroupObject->getOUTObjectList()->moveFlowPos(wxPoint(0,100),wxPoint(-1,lastMousePos.y));
            m_currentGroupObject->getINObjectList()->moveFlowPos(wxPoint(0,100),wxPoint(-1,lastMousePos.y));
         }
    	 refreshProject(0);
    	 break;
      case POPUP_EXTEND_INSERT_COL:
         g_mainWin->addUndoStep(_T("Insert Column"));
         g_objectList.m_projectData->flowW+=160;
         m_drawObjectList->moveFlowPos(wxPoint(160,0),wxPoint(lastMousePos.x,-1));
         if (m_currentGroupObject)
         {
            m_currentGroupObject->getOUTObjectList()->moveFlowPos(wxPoint(160,0),wxPoint(lastMousePos.x,-1));
            m_currentGroupObject->getINObjectList()->moveFlowPos(wxPoint(160,0),wxPoint(lastMousePos.x,-1));
         }
    	   refreshProject(0);
    	   break;
      case POPUP_GROUP_FLOWOBJECTS:
         {
         wxNode     *node;
         flowObject *object;
         wxRect      selRect,objRect;
         ObjectList *groupList=new ObjectList();
	      wxInt32     cnt=0;

         g_isEmpty=false;
         g_isSaved=false;
         selRect=wxRect(rectStart.x,rectStart.y,rectEnd.x-rectStart.x,rectEnd.y-rectStart.y);
         node=m_drawObjectList->getObject((wxNode*)NULL);
         if (!node) wxMessageBox(_("No Flow Elements selected"),_("Error"),wxICON_ERROR);
         else g_mainWin->addUndoStep(_T("Group Elements"));
         while (node)
         {
            object=(flowObject*)node->GetData();
            objRect=wxRect(object->data.flowposx,object->data.flowposy,object->getFlowWidth(),object->getFlowHeight());
            if (selRect.Intersects(objRect))
            {
               if (object->data.type & HMI_TYPE_MASK)
               {
                  wxMessageBox(object->name+_T(":\n")+_("Controls can't be part of a Flow Element group"),_("Error"),wxICON_ERROR);
                  groupList->deleteAll(0);
				      delete groupList;
                  return;
               }
               else if (object->data.type!=flowObject::FLOW_TYPE_GROUP_IO_DEFINITION)
			   {
                  groupList->addObject(object,true,false); // putting outputs to top of list is done in flowGroups sort function
				  cnt++;
			   }
            }
            node=m_drawObjectList->getObject(node);
         }
		 if (cnt==0) return;
         flowGroup *group;
         m_drawObjectList->deleteObjects(*groupList,0,0);
         group=new flowGroup(m_currentGroupObject,groupList,rectStart);
         wxASSERT(group);
         wxASSERT(group->returnOK);
         if ((!group) || (!group->returnOK)) return;
         m_drawObjectList->Insert(group);
         group->createFlowBitmap();

         rectStart.x=-1;
         rectEnd.x=-1;
         Refresh();
         break;
         }
      case POPUP_DEL_UNUSED_CONN:
         {
            wxNode         *connectionNode=NULL;
            FlowConnection *connection;

            wxASSERT(currentObject);
            if (!currentObject) return;
            g_mainWin->addUndoStep(_T("Delete unused Connections"));
            connectionNode=currentObject->getFlowConnection((wxNode*)NULL);
            while (connectionNode)
            {
               connection=(FlowConnection*)connectionNode->GetData();
               if ((connection->data.targetID==0) || (!g_objectList.getObject(connection->data.targetID)))
               {
                  if (currentObject->removeFlowConnection(connection))
                   connectionNode=currentObject->getFlowConnection((wxNode*)NULL);
                  else
                  {
                     wxASSERT(0);
                     connection->data.targetID=0;
                     return;
//                     connectionNode=currentObject->getFlowConnection(connectionNode);
                  }
               }
               else connectionNode=currentObject->getFlowConnection(connectionNode);
            }
         }         
         break;
      default:
         // check macro list
         {
            ExternalMacro *macro;


            macro=m_externalMacros->getMacro(true);
            while (macro)
            {
               if (macro->m_uiID==event.GetId())
               {
                  loadGroup(_T(MACRO_PATH)+macro->m_filename,true);
                  break;
               }
               macro=m_externalMacros->getMacro(false);
            }
         }
         foundID=0;
         break;
   }

   if (!foundID)
   {
      ExternalIOLib *extIOLib;

      extIOLib=g_externalIOLibs->getLib(true);
      while (extIOLib)
      {
         if ((extIOLib->uiID==event.GetId()) || (extIOLib->foldUiID==event.GetId()))
         {
            flowExternalIOLib   *object;

            g_mainWin->addUndoStep(_T("Add External Control"));
            g_isEmpty=false;
            g_isSaved=false;
            object=new flowExternalIOLib(extIOLib,0);
            if (!object->returnOK)
            {
               delete object;
               return;
            }
            if (object->data.stdOUT!=0) // has outputs (=receives) so we put it at the beginning ofthe list to ensure that it is initialized before objects with inputs (=sends)
             m_drawObjectList->Insert(object);
            else
             m_drawObjectList->addObject(object,true,false);
            object->setFlowPos(m_drawObjectList,lastMousePos,1,1);
            Refresh();
            break;
         }
         extIOLib=g_externalIOLibs->getLib(false);
      }
   }
   g_isSaved=false;
}


