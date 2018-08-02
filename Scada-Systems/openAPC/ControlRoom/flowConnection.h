#ifndef FLOWCONNECTION_H
#define FLOWCONNECTION_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_CONNECTION_EDGES 32

#include <wx/dcbuffer.h>
#include <wx/file.h>

struct flowConnectionData
{
    wxInt32  usedEdges; // align to 64 bit with next line, unused in compiled structure
    wxUint32 targetID;  // aligned to 64 bit
    wxUint64 sourceOutput,targetInput;
    wxInt32  linex[MAX_CONNECTION_EDGES],liney[MAX_CONNECTION_EDGES];
};

#define FLOW_CONNECTION_HEAD_SIZE 24


class FlowConnection:public wxObject
{
public:
	FlowConnection(wxPoint pos);
	FlowConnection();
	virtual ~FlowConnection();

#ifdef ENV_EDITOR
   wxPoint   getLastPos();
   void      setLastPos(wxPoint pos);
   wxPoint   getPrevPos();
   void      setPrevPos(wxPoint pos);
   wxByte    addPos(wxPoint pos);
   wxPoint   getPosAt(wxInt32 idx);
   void      setPosAt(wxInt32 idx,wxPoint pos);
   wxInt32   getPosIdx();

   wxByte    isLastLine(wxPoint pos,wxInt32 tolerance);
   void      drawFlowConnection(wxAutoBufferedPaintDC *dc,const wxPen *pen,wxInt32 x, wxInt32 y);
   wxByte    removeLastPos();
   wxInt32   getFlowLine(wxPoint pos,wxInt32 tolerance);
   wxByte    isAtEnd(wxPoint pos,wxInt32 tolerance);
   void      optimizeLineAt(wxInt32 idx);
   wxInt32   saveFlow(wxFile *FHandle,bool isCompiled);
#else
          void   setFlowIONumbers();
   static wxByte getDigiIndexFromFlag(wxUint64 flag);
   static wxByte getNumIndexFromFlag(wxUint64 flag);
   static wxByte getCharIndexFromFlag(wxUint64 flag);
   static wxByte getBinIndexFromFlag(wxUint64 flag);
#endif
   wxInt32       loadFlow(wxFile *FHandle,struct hmiFlowData *flowData,wxUint32 IDOffset,wxByte &successful,bool isCompiled);

   struct        flowConnectionData data;
#ifdef ENV_PLAYER
   wxInt32       sourceOutputNum,targetInputNum;
   bool          isTempConnection;
#endif

private:
   void    init();
};

#endif
