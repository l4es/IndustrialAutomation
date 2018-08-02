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
#include <wx/file.h>
#include <wx/dcbuffer.h>

#ifndef __WXMSW__
#include <arpa/inet.h>
#endif

#include "globals.h"
#include "flowConnection.h"

#ifdef ENV_WINDOWS
#ifdef ENV_WINDOWSCE
#include "Winsock2.h"
#endif
#endif



FlowConnection::FlowConnection(wxPoint pos)
{
   init();
   data.linex[0]=pos.x;
   data.liney[0]=pos.y;
#ifdef ENV_PLAYER
   sourceOutputNum=-2000000;
   targetInputNum=-2000000;
#endif
}



FlowConnection::FlowConnection()
{
   init();
}



void FlowConnection::init()
{
#ifdef ENV_PLAYER
   isTempConnection=false;
#endif
   data.sourceOutput=0;
   data.targetID=0;
   data.targetInput=0;
#ifndef ENV_PLAYER
   for (wxInt32 i=1; i<MAX_CONNECTION_EDGES; i++)
   {
      data.linex[i]=-1;
      data.liney[i]=-1;
   }
   data.usedEdges=0;
#endif
}


FlowConnection::~FlowConnection()
{

}



#ifdef ENV_EDITOR
wxPoint FlowConnection::getPosAt(wxInt32 idx)
{
   return wxPoint(data.linex[idx],data.liney[idx]);
}



void FlowConnection::setPosAt(wxInt32 idx,wxPoint pos)
{
   data.linex[idx]=pos.x;
   data.liney[idx]=pos.y;
}



wxPoint FlowConnection::getLastPos()
{
   return getPosAt(data.usedEdges);
}



void FlowConnection::setLastPos(wxPoint pos)
{
   setPosAt(data.usedEdges,pos);
}



wxPoint FlowConnection::getPrevPos()
{
   if (data.usedEdges<=0) return wxPoint(-1000,-1000);
   return getPosAt(data.usedEdges-1);
}



void FlowConnection::setPrevPos(wxPoint pos)
{
   if (data.usedEdges<=0) return;
   setPosAt(data.usedEdges-1,pos);
}



wxByte FlowConnection::addPos(wxPoint pos)
{
   if (data.usedEdges+2>=MAX_CONNECTION_EDGES) return 0;
   data.usedEdges++;
   data.linex[data.usedEdges]=pos.x;
   data.liney[data.usedEdges]=pos.y;
   return 1;
}



wxInt32 FlowConnection::getPosIdx()
{
    return data.usedEdges;
}


/*wxByte flowConnection::isLastPos(wxPoint pos)
{
   if (data.usedEdges+1>=MAX_CONNECTION_EDGES) return 0;
   data.linex[data.usedEdges]=pos.x;
   data.liney[data.usedEdges]=pos.y;
   return 1;
}*/



void FlowConnection::optimizeLineAt(wxInt32 idx)
{
    wxInt32 dropIdx=-1,i;

    if ((idx>=2) && (idx%2==0))
    {
        if ((data.liney[idx-2]-5<=data.liney[idx]) && (data.liney[idx-2]+5>=data.liney[idx]))
        {
           data.liney[idx]=data.liney[idx-2];
           dropIdx=idx-2;
        }
        else if ((data.liney[idx-1]-5<=data.liney[idx+1]) && (data.liney[idx-1]+5>=data.liney[idx+1]))
        {
           data.liney[idx-1]=data.liney[idx+1];
           dropIdx=idx;
        }
    }
    if ((dropIdx==-1) && (idx%2==1) && (idx+1<MAX_CONNECTION_EDGES))
    {
        if ((data.linex[idx-2]-5<=data.linex[idx]) && (data.linex[idx-2]+5>=data.linex[idx]))
        {
           data.linex[idx]=data.linex[idx-2];
           dropIdx=idx-2;
        }
        else if ((data.linex[idx-1]-5<=data.linex[idx+1]) && (data.linex[idx-1]+5>=data.linex[idx+1]))
        {
           data.linex[idx-1]=data.linex[idx+1];
           dropIdx=idx;
        }
    }
    if (dropIdx>-1)
    {
        for (i=dropIdx; i<MAX_CONNECTION_EDGES-2; i++)
        {
           data.linex[i]=data.linex[i+2];
           data.liney[i]=data.liney[i+2];
        }
        data.usedEdges-=2;
    }


}



wxByte FlowConnection::removeLastPos()
{
    if (data.usedEdges<=0) return 0;
    data.usedEdges--;
    return 1;
}



void FlowConnection::drawFlowConnection(wxAutoBufferedPaintDC *dc,const wxPen *pen,wxInt32 x, wxInt32 y)
{
    wxInt32 i;

    if (data.usedEdges==0) return;
    if (data.targetID>0) for (i=1; i<=data.usedEdges; i++) if (data.linex[i]>-1)
    {
        dc->SetPen(*wxWHITE_PEN);
        dc->DrawLine(data.linex[i-1]+x+1,data.liney[i-1]+y+1,data.linex[i]+x+1,data.liney[i]+y+1);
        dc->DrawLine(data.linex[i-1]+x-1,data.liney[i-1]+y-1,data.linex[i]+x-1,data.liney[i]+y-1);
    }
    if (!pen)
    {
       if (data.targetID>0) dc->SetPen(*wxBLACK_PEN);
       else dc->SetPen(*wxLIGHT_GREY_PEN);
    }
    else dc->SetPen(*pen);
    for (i=1; i<=data.usedEdges; i++) if (data.linex[i]>-1)
    {
        dc->DrawLine(data.linex[i-1]+x,data.liney[i-1]+y,data.linex[i]+x,data.liney[i]+y);
    }
}



wxByte FlowConnection::isAtEnd(wxPoint pos,wxInt32 tolerance)
{
   if (data.usedEdges>MAX_CONNECTION_EDGES)
   {
      wxASSERT(0);
      return 0;
   }
   if ((data.linex[data.usedEdges]-tolerance<=pos.x) && (data.linex[data.usedEdges]+tolerance>=pos.x) && 
       (data.liney[data.usedEdges]-tolerance<=pos.y) && (data.liney[data.usedEdges]+tolerance>=pos.y))
    return 1;
   return 0;
}



wxByte FlowConnection::isLastLine(wxPoint pos,wxInt32 tolerance)
{
   while (data.linex[data.usedEdges]<0) data.usedEdges--;

   if (getFlowLine(pos,tolerance)>=data.usedEdges) return 1;
   return 0;
}



wxInt32 FlowConnection::getFlowLine(wxPoint pos,wxInt32 tolerance)
{
    wxInt32 i,xmin,xmax,ymin,ymax;

    if (data.usedEdges==0) return -1;
    for (i=1; i<=data.usedEdges; i++) if (data.linex[i]>-1)
    {
       if (data.linex[i-1]<data.linex[i])
       {
           xmin=data.linex[i-1];
           xmax=data.linex[i];
       }
       else
       {
           xmin=data.linex[i];
           xmax=data.linex[i-1];
       }
       if (data.liney[i-1]<data.liney[i])
       {
           ymin=data.liney[i-1];
           ymax=data.liney[i];
       }
       else
       {
           ymin=data.liney[i];
           ymax=data.liney[i-1];
       }

       if ((i==1) || (i==data.usedEdges))
       {
          if ((xmin-tolerance<=pos.x) && (xmax+tolerance>=pos.x) && (ymin<=pos.y) && (ymax>=pos.y))
           return i;
       }
       else
       {
          if ((xmin-tolerance<=pos.x) && (xmax+tolerance>=pos.x) && (ymin-tolerance<=pos.y) && (ymax+tolerance>=pos.y))
           return i;
       }
    }
    return -1;
}



wxInt32 FlowConnection::saveFlow(wxFile *FHandle,bool isCompiled)
{
   struct flowConnectionData convData;
   wxInt32                   i;

   if (FHandle==NULL) return 0;

   convData.usedEdges=            htonl(data.usedEdges);
   convData.sourceOutput         =oapc_util_htonll(data.sourceOutput);
   convData.targetID=             htonl(data.targetID);
   convData.targetInput          =oapc_util_htonll(data.targetInput);
   if (!isCompiled)
   {
      for (i=0; i<=data.usedEdges; i++)
      {
         convData.linex[i]=htonl(data.linex[i]);
         convData.liney[i]=htonl(data.liney[i]);
      }
      return FHandle->Write(&convData,sizeof(struct flowConnectionData));
   }
   else return FHandle->Write(&convData,FLOW_CONNECTION_HEAD_SIZE); // only first four elements of flowConnectionData structure
}

#else


wxByte FlowConnection::getDigiIndexFromFlag(wxUint64 flag)
{
   switch (flag & OAPC_DIGI_IO_MASK)
   {
      default: // in case of a group it is a number that can't be mapped to an index value
      case OAPC_DIGI_IO0: 
         return 0;
      case OAPC_DIGI_IO1: 
         return 1;
      case OAPC_DIGI_IO2: 
         return 2;
      case OAPC_DIGI_IO3: 
         return 3;
      case OAPC_DIGI_IO4: 
         return 4;
      case OAPC_DIGI_IO5: 
         return 5;
      case OAPC_DIGI_IO6: 
         return 6;
      case OAPC_DIGI_IO7: 
         return 7;
   }
}



wxByte FlowConnection::getNumIndexFromFlag(wxUint64 flag)
{
   switch (flag & OAPC_NUM_IO_MASK)
   {
      default: // in case of a group it is a number that can't be mapped to an index value
      case OAPC_NUM_IO0: 
         return 0;
      case OAPC_NUM_IO1: 
         return 1;
      case OAPC_NUM_IO2: 
         return 2;
      case OAPC_NUM_IO3: 
         return 3;
      case OAPC_NUM_IO4: 
         return 4;
      case OAPC_NUM_IO5: 
         return 5;
      case OAPC_NUM_IO6: 
         return 6;
      case OAPC_NUM_IO7: 
         return 7;
   }
}



wxByte FlowConnection::getCharIndexFromFlag(wxUint64 flag)
{
   switch (flag & OAPC_CHAR_IO_MASK)
   {
      default: // in case of a group it is a number that can't be mapped to an index value
      case OAPC_CHAR_IO0: 
         return 0;
      case OAPC_CHAR_IO1: 
         return 1;
      case OAPC_CHAR_IO2: 
         return 2;
      case OAPC_CHAR_IO3: 
         return 3;
      case OAPC_CHAR_IO4: 
         return 4;
      case OAPC_CHAR_IO5: 
         return 5;
      case OAPC_CHAR_IO6: 
         return 6;
      case OAPC_CHAR_IO7: 
         return 7;
   }
}



wxByte FlowConnection::getBinIndexFromFlag(wxUint64 flag)
{
   switch (flag & OAPC_BIN_IO_MASK)
   {
      default: // in case of a group it is a number that can't be mapped to an index value
      case OAPC_BIN_IO0: 
         return 0;
      case OAPC_BIN_IO1: 
         return 1;
      case OAPC_BIN_IO2: 
         return 3;
      case OAPC_BIN_IO3: 
         return 3;
      case OAPC_BIN_IO4: 
         return 4;
      case OAPC_BIN_IO5: 
         return 5;
      case OAPC_BIN_IO6: 
         return 6;
      case OAPC_BIN_IO7: 
         return 7;
   }
}
#endif



#ifdef ENV_PLAYER
void FlowConnection::setFlowIONumbers()
{
   if (data.sourceOutput & OAPC_DIGI_IO_MASK)      sourceOutputNum=getDigiIndexFromFlag(data.sourceOutput);
   else if (data.sourceOutput & OAPC_NUM_IO_MASK)  sourceOutputNum=getNumIndexFromFlag(data.sourceOutput);
   else if (data.sourceOutput & OAPC_CHAR_IO_MASK) sourceOutputNum=getCharIndexFromFlag(data.sourceOutput);
   else if (data.sourceOutput & OAPC_BIN_IO_MASK)  sourceOutputNum=getBinIndexFromFlag(data.sourceOutput);
   else sourceOutputNum=MAX_NUM_IOS; // overflow outputs
   wxASSERT(sourceOutputNum<MAX_NUM_IOS+1);

   if (data.targetInput & OAPC_DIGI_IO_MASK)      targetInputNum=getDigiIndexFromFlag(data.targetInput);
   else if (data.targetInput & OAPC_NUM_IO_MASK)  targetInputNum=getNumIndexFromFlag(data.targetInput);
   else if (data.targetInput & OAPC_CHAR_IO_MASK) targetInputNum=getCharIndexFromFlag(data.targetInput);
   else if (data.targetInput & OAPC_BIN_IO_MASK)  targetInputNum=getBinIndexFromFlag(data.targetInput);
   else targetInputNum=MAX_NUM_IOS; // overflow outputs
   wxASSERT(targetInputNum<MAX_NUM_IOS+1);
}
#endif



wxInt32 FlowConnection::loadFlow(wxFile *FHandle,struct hmiFlowData *WXUNUSED(flowData),wxUint32 IDOffset,wxByte &successful,bool isCompiled) // later used with other versions of flowdata
{
   wxInt32                   i,length;
   struct flowConnectionData convData;

   successful=0;
   if (FHandle==NULL) return 0;

   if (isCompiled) length=FHandle->Read(&convData,FLOW_CONNECTION_HEAD_SIZE);
   else length=FHandle->Read(&convData,sizeof(struct flowConnectionData));
   data.usedEdges                =ntohl(convData.usedEdges);
   data.sourceOutput             =oapc_util_ntohll(convData.sourceOutput);
   if (IDOffset==0) data.targetID=ntohl(convData.targetID);
   else data.targetID            =ntohl(convData.targetID)-IDOffset+g_objectList.currentUniqueID()+1;
   data.targetInput              =oapc_util_ntohll(convData.targetInput);
   wxASSERT(data.targetInput);
   if (data.usedEdges>MAX_CONNECTION_EDGES) data.usedEdges=MAX_CONNECTION_EDGES;
   else if (data.usedEdges<=0)
   {
      if (data.targetID<=0) successful=0;
      else successful=1;
#ifdef ENV_PLAYER
      setFlowIONumbers();
#endif
      return length;
   }
   if (!isCompiled) for (i=0; i<=data.usedEdges; i++)
   {
      data.linex[i]=ntohl(convData.linex[i]);
      data.liney[i]=ntohl(convData.liney[i]);
      wxASSERT(data.linex[i]>=0);
      wxASSERT(data.liney[i]>=0);
      if ((data.linex[i]<0) || (data.liney[i]<0)) return length;
   }
   while (data.linex[data.usedEdges]<0) data.usedEdges--;
   if (data.targetID<=0) successful=0;
   else successful=1;
#ifdef ENV_PLAYER
   setFlowIONumbers();
#endif
   return length;
}
