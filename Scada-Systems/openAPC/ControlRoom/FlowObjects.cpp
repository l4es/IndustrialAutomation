#include <wx/wx.h>



#include "hmiObject.h"
#include "flowGroup.h"
#include "FlowObjects.h"
#include "IOElementDefinition.h"


FlowObjects::FlowObjects()
{
   in0Pos=wxPoint( 7,0);   out0Pos=wxPoint( 7,50);
   in1Pos=wxPoint(20,0);   out1Pos=wxPoint(20,50);
   in2Pos=wxPoint(33,0);   out2Pos=wxPoint(33,50);
   in3Pos=wxPoint(46,0);   out3Pos=wxPoint(46,50);
   in4Pos=wxPoint(59,0);   out4Pos=wxPoint(59,50);
   in5Pos=wxPoint(72,0);   out5Pos=wxPoint(72,50);
   in6Pos=wxPoint(85,0);   out6Pos=wxPoint(85,50);
   in7Pos=wxPoint(98,0);   out7Pos=wxPoint(98,50);
}



FlowObjects::~FlowObjects()
{

}



wxByte FlowObjects::isAtOutConnector(flowObject *object,wxPoint pos)
{
   wxUint32 type;

   type=getOutConnectorType(object,pos);
   if (type) return 1;
   return 0;
}



wxPoint FlowObjects::getOutConnectorPos(flowObject *object,wxPoint pos)
{
    if (object->data.type==hmiObject::FLOW_TYPE_GROUP) return ((flowGroup*)object)->getOutConnectorPos(pos);
    else if (object->data.type==hmiObject::FLOW_TYPE_GROUP_IO_DEFINITION) return ((IOElementDefinition*)object)->getOutConnectorPos(pos);
    if (((object->data.stdOUT & OAPC_DIGI_IO0)==OAPC_DIGI_IO0) ||
        ((object->data.stdOUT & OAPC_NUM_IO0) ==OAPC_NUM_IO0) ||
        ((object->data.stdOUT & OAPC_BIN_IO0) ==OAPC_BIN_IO0) ||
        ((object->data.stdOUT & OAPC_CHAR_IO0)==OAPC_CHAR_IO0))
    {
        if ((pos.x-object->getFlowPos().x>=out0Pos.x-5)  && (pos.x-object->getFlowPos().x<=out0Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out0Pos.y-10) && (pos.y-object->getFlowPos().y<=out0Pos.y)) return out0Pos;
    }
    if (((object->data.stdOUT & OAPC_DIGI_IO1)==OAPC_DIGI_IO1) ||
        ((object->data.stdOUT & OAPC_NUM_IO1) ==OAPC_NUM_IO1) ||
        ((object->data.stdOUT & OAPC_BIN_IO1) ==OAPC_BIN_IO1) ||
        ((object->data.stdOUT & OAPC_CHAR_IO1)==OAPC_CHAR_IO1))
    {
        if ((pos.x-object->getFlowPos().x>=out1Pos.x-5)  && (pos.x-object->getFlowPos().x<=out1Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out1Pos.y-10) && (pos.y-object->getFlowPos().y<=out1Pos.y)) return out1Pos;
    }
    if (((object->data.stdOUT & OAPC_DIGI_IO2)==OAPC_DIGI_IO2) ||
        ((object->data.stdOUT & OAPC_NUM_IO2) ==OAPC_NUM_IO2) ||
        ((object->data.stdOUT & OAPC_BIN_IO2) ==OAPC_BIN_IO2) ||
        ((object->data.stdOUT & OAPC_CHAR_IO2)==OAPC_CHAR_IO2))
    {
        if ((pos.x-object->getFlowPos().x>=out2Pos.x-5)  && (pos.x-object->getFlowPos().x<=out2Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out2Pos.y-10) && (pos.y-object->getFlowPos().y<=out2Pos.y)) return out2Pos;
    }
    if (((object->data.stdOUT & OAPC_DIGI_IO3)==OAPC_DIGI_IO3) ||
        ((object->data.stdOUT & OAPC_NUM_IO3) ==OAPC_NUM_IO3) ||
        ((object->data.stdOUT & OAPC_BIN_IO3) ==OAPC_BIN_IO3) ||
        ((object->data.stdOUT & OAPC_CHAR_IO3)==OAPC_CHAR_IO3))
    {
        if ((pos.x-object->getFlowPos().x>=out3Pos.x-5)  && (pos.x-object->getFlowPos().x<=out3Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out3Pos.y-10) && (pos.y-object->getFlowPos().y<=out3Pos.y)) return out3Pos;
    }
    if (((object->data.stdOUT & OAPC_DIGI_IO4)==OAPC_DIGI_IO4) ||
        ((object->data.stdOUT & OAPC_NUM_IO4) ==OAPC_NUM_IO4) ||
        ((object->data.stdOUT & OAPC_BIN_IO4) ==OAPC_BIN_IO4) ||
        ((object->data.stdOUT & OAPC_CHAR_IO4)==OAPC_CHAR_IO4))
    {
        if ((pos.x-object->getFlowPos().x>=out4Pos.x-5)  && (pos.x-object->getFlowPos().x<=out4Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out4Pos.y-10) && (pos.y-object->getFlowPos().y<=out4Pos.y)) return out4Pos;
    }
    if (((object->data.stdOUT & OAPC_DIGI_IO5)==OAPC_DIGI_IO5) ||
        ((object->data.stdOUT & OAPC_NUM_IO5) ==OAPC_NUM_IO5) ||
        ((object->data.stdOUT & OAPC_BIN_IO5) ==OAPC_BIN_IO5) ||
        ((object->data.stdOUT & OAPC_CHAR_IO5)==OAPC_CHAR_IO5))
    {
        if ((pos.x-object->getFlowPos().x>=out5Pos.x-5)  && (pos.x-object->getFlowPos().x<=out5Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out5Pos.y-10) && (pos.y-object->getFlowPos().y<=out5Pos.y)) return out5Pos;
    }
    
    if (((object->data.stdOUT & OAPC_DIGI_IO6)==OAPC_DIGI_IO6) ||
        ((object->data.stdOUT & OAPC_NUM_IO6) ==OAPC_NUM_IO6) ||
        ((object->data.stdOUT & OAPC_BIN_IO6) ==OAPC_BIN_IO6) ||
        ((object->data.stdOUT & OAPC_CHAR_IO6)==OAPC_CHAR_IO6))
    {
        if ((pos.x-object->getFlowPos().x>=out6Pos.x-5)  && (pos.x-object->getFlowPos().x<=out6Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out6Pos.y-10) && (pos.y-object->getFlowPos().y<=out6Pos.y)) return out6Pos;
    }
    if (((object->data.stdOUT & OAPC_DIGI_IO7)==OAPC_DIGI_IO7) ||
        ((object->data.stdOUT & OAPC_NUM_IO7) ==OAPC_NUM_IO7) ||
        ((object->data.stdOUT & OAPC_BIN_IO7) ==OAPC_BIN_IO7) ||
        ((object->data.stdOUT & OAPC_CHAR_IO7)==OAPC_CHAR_IO7))
    {
        if ((pos.x-object->getFlowPos().x>=out7Pos.x-5)  && (pos.x-object->getFlowPos().x<=out7Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out7Pos.y-10) && (pos.y-object->getFlowPos().y<=out7Pos.y)) return out7Pos;
    }

    if (((object->data.stdOUT & FLOW_TYPE_FLAG_DIGI_OVERFLOW_OUT)==FLOW_TYPE_FLAG_DIGI_OVERFLOW_OUT) ||
        ((object->data.stdOUT & FLOW_TYPE_FLAG_NUM_OVERFLOW_OUT)==FLOW_TYPE_FLAG_NUM_OVERFLOW_OUT) ||
        ((object->data.stdOUT & FLOW_TYPE_FLAG_CHAR_OVERFLOW_OUT)==FLOW_TYPE_FLAG_CHAR_OVERFLOW_OUT) ||
        ((object->data.stdOUT & FLOW_TYPE_FLAG_BIN_OVERFLOW_OUT)==FLOW_TYPE_FLAG_BIN_OVERFLOW_OUT))
    {
       if ((pos.x-object->getFlowPos().x>=object->getFlowWidth()-10)      && (pos.x-object->getFlowPos().x<=object->getFlowWidth()) &&
           (pos.y-object->getFlowPos().y>=(object->getFlowHeight()/2)-5) && (pos.y-object->getFlowPos().y<=(object->getFlowHeight()/2)+5))
        return wxPoint(object->getFlowWidth(),(object->getFlowHeight()/2)-1);
    }
    
    return wxPoint(-1,-1);
}



wxUint64 FlowObjects::getOutConnectorType(flowObject *object,wxPoint pos)
{
    if (object->data.type==hmiObject::FLOW_TYPE_GROUP) return ((flowGroup*)object)->getOutConnectorType(pos);
    else if (object->data.type==hmiObject::FLOW_TYPE_GROUP_IO_DEFINITION) return ((IOElementDefinition*)object)->getOutConnectorType(pos);
    if ((object->data.stdOUT & OAPC_DIGI_IO0)==OAPC_DIGI_IO0)
    {
        if ((pos.x-object->getFlowPos().x>=out0Pos.x-5)  && (pos.x-object->getFlowPos().x<=out0Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out0Pos.y-10) && (pos.y-object->getFlowPos().y<=out0Pos.y)) return OAPC_DIGI_IO0;
    }
    if ((object->data.stdOUT & OAPC_DIGI_IO1)==OAPC_DIGI_IO1)
    {
        if ((pos.x-object->getFlowPos().x>=out1Pos.x-5)  && (pos.x-object->getFlowPos().x<=out1Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out1Pos.y-10) && (pos.y-object->getFlowPos().y<=out1Pos.y)) return OAPC_DIGI_IO1;
    }
    if ((object->data.stdOUT & OAPC_DIGI_IO2)==OAPC_DIGI_IO2)
    {
        if ((pos.x-object->getFlowPos().x>=out2Pos.x-5)  && (pos.x-object->getFlowPos().x<=out2Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out2Pos.y-10) && (pos.y-object->getFlowPos().y<=out2Pos.y)) return OAPC_DIGI_IO2;
    }
    if ((object->data.stdOUT & OAPC_DIGI_IO3)==OAPC_DIGI_IO3)
    {
        if ((pos.x-object->getFlowPos().x>=out3Pos.x-5)  && (pos.x-object->getFlowPos().x<=out3Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out3Pos.y-10) && (pos.y-object->getFlowPos().y<=out3Pos.y)) return OAPC_DIGI_IO3;
    }
    if ((object->data.stdOUT & OAPC_DIGI_IO4)==OAPC_DIGI_IO4)
    {
        if ((pos.x-object->getFlowPos().x>=out4Pos.x-5)  && (pos.x-object->getFlowPos().x<=out4Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out4Pos.y-10) && (pos.y-object->getFlowPos().y<=out4Pos.y)) return OAPC_DIGI_IO4;
    }
    if ((object->data.stdOUT & OAPC_DIGI_IO5)==OAPC_DIGI_IO5)
    {
        if ((pos.x-object->getFlowPos().x>=out5Pos.x-5)  && (pos.x-object->getFlowPos().x<=out5Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out5Pos.y-10) && (pos.y-object->getFlowPos().y<=out5Pos.y)) return OAPC_DIGI_IO5;
    }
    if ((object->data.stdOUT & OAPC_DIGI_IO6)==OAPC_DIGI_IO6)
    {
        if ((pos.x-object->getFlowPos().x>=out6Pos.x-5)  && (pos.x-object->getFlowPos().x<=out6Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out6Pos.y-10) && (pos.y-object->getFlowPos().y<=out6Pos.y)) return OAPC_DIGI_IO6;
    }
    if ((object->data.stdOUT & OAPC_DIGI_IO7)==OAPC_DIGI_IO7)
    {
        if ((pos.x-object->getFlowPos().x>=out7Pos.x-5)  && (pos.x-object->getFlowPos().x<=out7Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out7Pos.y-10) && (pos.y-object->getFlowPos().y<=out7Pos.y)) return OAPC_DIGI_IO7;
    }

    if ((object->data.stdOUT & OAPC_NUM_IO0) ==OAPC_NUM_IO0)
    {
        if ((pos.x-object->getFlowPos().x>=out0Pos.x-5)  && (pos.x-object->getFlowPos().x<=out0Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out0Pos.y-10) && (pos.y-object->getFlowPos().y<=out0Pos.y)) return OAPC_NUM_IO0;
    }
    if ((object->data.stdOUT & OAPC_NUM_IO1) ==OAPC_NUM_IO1)
    {
        if ((pos.x-object->getFlowPos().x>=out1Pos.x-5)  && (pos.x-object->getFlowPos().x<=out1Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out1Pos.y-10) && (pos.y-object->getFlowPos().y<=out1Pos.y)) return OAPC_NUM_IO1;
    }
    if ((object->data.stdOUT & OAPC_NUM_IO2) ==OAPC_NUM_IO2)
    {
        if ((pos.x-object->getFlowPos().x>=out2Pos.x-5)  && (pos.x-object->getFlowPos().x<=out2Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out2Pos.y-10) && (pos.y-object->getFlowPos().y<=out2Pos.y)) return OAPC_NUM_IO2;
    }
    if ((object->data.stdOUT & OAPC_NUM_IO3) ==OAPC_NUM_IO3)
    {
        if ((pos.x-object->getFlowPos().x>=out3Pos.x-5)  && (pos.x-object->getFlowPos().x<=out3Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out3Pos.y-10) && (pos.y-object->getFlowPos().y<=out3Pos.y)) return OAPC_NUM_IO3;
    }
    if ((object->data.stdOUT & OAPC_NUM_IO4) ==OAPC_NUM_IO4)
    {
        if ((pos.x-object->getFlowPos().x>=out4Pos.x-5)  && (pos.x-object->getFlowPos().x<=out4Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out4Pos.y-10) && (pos.y-object->getFlowPos().y<=out4Pos.y)) return OAPC_NUM_IO4;
    }
    if ((object->data.stdOUT & OAPC_NUM_IO5) ==OAPC_NUM_IO5)
    {
        if ((pos.x-object->getFlowPos().x>=out5Pos.x-5)  && (pos.x-object->getFlowPos().x<=out5Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out5Pos.y-10) && (pos.y-object->getFlowPos().y<=out5Pos.y)) return OAPC_NUM_IO5;
    }
    if ((object->data.stdOUT & OAPC_NUM_IO6) ==OAPC_NUM_IO6)
    {
        if ((pos.x-object->getFlowPos().x>=out6Pos.x-5)  && (pos.x-object->getFlowPos().x<=out6Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out6Pos.y-10) && (pos.y-object->getFlowPos().y<=out6Pos.y)) return OAPC_NUM_IO6;
    }
    if ((object->data.stdOUT & OAPC_NUM_IO7) ==OAPC_NUM_IO7)
    {
        if ((pos.x-object->getFlowPos().x>=out7Pos.x-5)  && (pos.x-object->getFlowPos().x<=out7Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out7Pos.y-10) && (pos.y-object->getFlowPos().y<=out7Pos.y)) return OAPC_NUM_IO7;
    }

    if ((object->data.stdOUT & OAPC_CHAR_IO0)==OAPC_CHAR_IO0)
    {
        if ((pos.x-object->getFlowPos().x>=out0Pos.x-5)  && (pos.x-object->getFlowPos().x<=out0Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out0Pos.y-10) && (pos.y-object->getFlowPos().y<=out0Pos.y)) return OAPC_CHAR_IO0;
    }
    if ((object->data.stdOUT & OAPC_CHAR_IO1)==OAPC_CHAR_IO1)
    {
        if ((pos.x-object->getFlowPos().x>=out1Pos.x-5)  && (pos.x-object->getFlowPos().x<=out1Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out1Pos.y-10) && (pos.y-object->getFlowPos().y<=out1Pos.y)) return OAPC_CHAR_IO1;
    }
    if ((object->data.stdOUT & OAPC_CHAR_IO2)==OAPC_CHAR_IO2)
    {
        if ((pos.x-object->getFlowPos().x>=out2Pos.x-5)  && (pos.x-object->getFlowPos().x<=out2Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out2Pos.y-10) && (pos.y-object->getFlowPos().y<=out2Pos.y)) return OAPC_CHAR_IO2;
    }
    if ((object->data.stdOUT & OAPC_CHAR_IO3)==OAPC_CHAR_IO3)
    {
        if ((pos.x-object->getFlowPos().x>=out3Pos.x-5)  && (pos.x-object->getFlowPos().x<=out3Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out3Pos.y-10) && (pos.y-object->getFlowPos().y<=out3Pos.y)) return OAPC_CHAR_IO3;
    }
    if ((object->data.stdOUT & OAPC_CHAR_IO4)==OAPC_CHAR_IO4)
    {
        if ((pos.x-object->getFlowPos().x>=out4Pos.x-5)  && (pos.x-object->getFlowPos().x<=out4Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out4Pos.y-10) && (pos.y-object->getFlowPos().y<=out4Pos.y)) return OAPC_CHAR_IO4;
    }
    if ((object->data.stdOUT & OAPC_CHAR_IO5)==OAPC_CHAR_IO5)
    {
        if ((pos.x-object->getFlowPos().x>=out5Pos.x-5)  && (pos.x-object->getFlowPos().x<=out5Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out5Pos.y-10) && (pos.y-object->getFlowPos().y<=out5Pos.y)) return OAPC_CHAR_IO5;
    }
    if ((object->data.stdOUT & OAPC_CHAR_IO6)==OAPC_CHAR_IO6)
    {
        if ((pos.x-object->getFlowPos().x>=out6Pos.x-5)  && (pos.x-object->getFlowPos().x<=out6Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out6Pos.y-10) && (pos.y-object->getFlowPos().y<=out6Pos.y)) return OAPC_CHAR_IO6;
    }
    if ((object->data.stdOUT & OAPC_CHAR_IO7)==OAPC_CHAR_IO7)
    {
        if ((pos.x-object->getFlowPos().x>=out7Pos.x-5)  && (pos.x-object->getFlowPos().x<=out7Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out7Pos.y-10) && (pos.y-object->getFlowPos().y<=out7Pos.y)) return OAPC_CHAR_IO7;
    }

    if ((object->data.stdOUT & OAPC_BIN_IO0) ==OAPC_BIN_IO0)
    {
        if ((pos.x-object->getFlowPos().x>=out0Pos.x-5)  && (pos.x-object->getFlowPos().x<=out0Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out0Pos.y-10) && (pos.y-object->getFlowPos().y<=out0Pos.y)) return OAPC_BIN_IO0;
    }
    if ((object->data.stdOUT & OAPC_BIN_IO1) ==OAPC_BIN_IO1)
    {
        if ((pos.x-object->getFlowPos().x>=out1Pos.x-5)  && (pos.x-object->getFlowPos().x<=out1Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out1Pos.y-10) && (pos.y-object->getFlowPos().y<=out1Pos.y)) return OAPC_BIN_IO1;
    }
    if ((object->data.stdOUT & OAPC_BIN_IO2) ==OAPC_BIN_IO2)
    {
        if ((pos.x-object->getFlowPos().x>=out2Pos.x-5)  && (pos.x-object->getFlowPos().x<=out2Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out2Pos.y-10) && (pos.y-object->getFlowPos().y<=out2Pos.y)) return OAPC_BIN_IO2;
    }
    if ((object->data.stdOUT & OAPC_BIN_IO3) ==OAPC_BIN_IO3)
    {
        if ((pos.x-object->getFlowPos().x>=out3Pos.x-5)  && (pos.x-object->getFlowPos().x<=out3Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out3Pos.y-10) && (pos.y-object->getFlowPos().y<=out3Pos.y)) return OAPC_BIN_IO3;
    }
    if ((object->data.stdOUT & OAPC_BIN_IO4) ==OAPC_BIN_IO4)
    {
        if ((pos.x-object->getFlowPos().x>=out4Pos.x-5)  && (pos.x-object->getFlowPos().x<=out4Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out4Pos.y-10) && (pos.y-object->getFlowPos().y<=out4Pos.y)) return OAPC_BIN_IO4;
    }
    if ((object->data.stdOUT & OAPC_BIN_IO5) ==OAPC_BIN_IO5)
    {
        if ((pos.x-object->getFlowPos().x>=out5Pos.x-5)  && (pos.x-object->getFlowPos().x<=out5Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out5Pos.y-10) && (pos.y-object->getFlowPos().y<=out5Pos.y)) return OAPC_BIN_IO5;
    }
    if ((object->data.stdOUT & OAPC_BIN_IO6) ==OAPC_BIN_IO6)
    {
        if ((pos.x-object->getFlowPos().x>=out6Pos.x-5)  && (pos.x-object->getFlowPos().x<=out6Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out6Pos.y-10) && (pos.y-object->getFlowPos().y<=out6Pos.y)) return OAPC_BIN_IO6;
    }
    if ((object->data.stdOUT & OAPC_BIN_IO7) ==OAPC_BIN_IO7)
    {
        if ((pos.x-object->getFlowPos().x>=out7Pos.x-5)  && (pos.x-object->getFlowPos().x<=out7Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=out7Pos.y-10) && (pos.y-object->getFlowPos().y<=out7Pos.y)) return OAPC_BIN_IO7;
    }

    if ((object->data.stdOUT & FLOW_TYPE_FLAG_DIGI_OVERFLOW_OUT)==FLOW_TYPE_FLAG_DIGI_OVERFLOW_OUT)
    {
       if ((pos.x-object->getFlowPos().x>=object->getFlowWidth()-5)      && (pos.x-object->getFlowPos().x<=object->getFlowWidth()) &&
           (pos.y-object->getFlowPos().y>=(object->getFlowHeight()/2)-3) && (pos.y-object->getFlowPos().y<=(object->getFlowHeight()/2)+3))
        return FLOW_TYPE_FLAG_DIGI_OVERFLOW_OUT;
    }
    else if ((object->data.stdOUT & FLOW_TYPE_FLAG_NUM_OVERFLOW_OUT)==FLOW_TYPE_FLAG_NUM_OVERFLOW_OUT)
    {
       if ((pos.x-object->getFlowPos().x>=object->getFlowWidth()-10)     && (pos.x-object->getFlowPos().x<=object->getFlowWidth()) &&
           (pos.y-object->getFlowPos().y>=(object->getFlowHeight()/2)-5) && (pos.y-object->getFlowPos().y<=(object->getFlowHeight()/2)+5))
        return FLOW_TYPE_FLAG_NUM_OVERFLOW_OUT;
    }
    else if ((object->data.stdOUT & FLOW_TYPE_FLAG_CHAR_OVERFLOW_OUT)==FLOW_TYPE_FLAG_CHAR_OVERFLOW_OUT)
    {
       if ((pos.x-object->getFlowPos().x>=object->getFlowWidth()-10)     && (pos.x-object->getFlowPos().x<=object->getFlowWidth()) &&
           (pos.y-object->getFlowPos().y>=(object->getFlowHeight()/2)-5) && (pos.y-object->getFlowPos().y<=(object->getFlowHeight()/2)+5))
        return FLOW_TYPE_FLAG_CHAR_OVERFLOW_OUT;
    }
    else if ((object->data.stdOUT & FLOW_TYPE_FLAG_BIN_OVERFLOW_OUT)==FLOW_TYPE_FLAG_BIN_OVERFLOW_OUT)
    {
       if ((pos.x-object->getFlowPos().x>=object->getFlowWidth()-10)     && (pos.x-object->getFlowPos().x<=object->getFlowWidth()) &&
           (pos.y-object->getFlowPos().y>=(object->getFlowHeight()/2)-5) && (pos.y-object->getFlowPos().y<=(object->getFlowHeight()/2)+5))
        return FLOW_TYPE_FLAG_BIN_OVERFLOW_OUT;
    }
    return 0;
}



wxByte FlowObjects::isAtInConnector(flowObject *object,wxPoint pos)
{
   wxUint32 type;

   type=getInConnectorType(object,pos);
   if (type) return 1;
   return 0;
}



wxPoint FlowObjects::getInConnectorPos(flowObject *object,wxPoint pos)
{
    if (object->data.type==hmiObject::FLOW_TYPE_GROUP) return ((flowGroup*)object)->getInConnectorPos(pos);
    else if (object->data.type==hmiObject::FLOW_TYPE_GROUP_IO_DEFINITION) return ((IOElementDefinition*)object)->getInConnectorPos(pos);
    if (((object->data.stdIN & OAPC_DIGI_IO0)==OAPC_DIGI_IO0) ||
        ((object->data.stdIN & OAPC_NUM_IO0) ==OAPC_NUM_IO0) ||
        ((object->data.stdIN & OAPC_BIN_IO0) ==OAPC_BIN_IO0) ||
        ((object->data.stdIN & OAPC_CHAR_IO0)==OAPC_CHAR_IO0))
    {
        if ((pos.x-object->getFlowPos().x>=in0Pos.x-5)  && (pos.x-object->getFlowPos().x<=in0Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in0Pos.y) && (pos.y-object->getFlowPos().y<=in0Pos.y+10)) return in0Pos;
    }
    if (((object->data.stdIN & OAPC_DIGI_IO1)==OAPC_DIGI_IO1) ||
        ((object->data.stdIN & OAPC_NUM_IO1) ==OAPC_NUM_IO1) ||
        ((object->data.stdIN & OAPC_BIN_IO1) ==OAPC_BIN_IO1) ||
        ((object->data.stdIN & OAPC_CHAR_IO1)==OAPC_CHAR_IO1))
    {
        if ((pos.x-object->getFlowPos().x>=in1Pos.x-5)  && (pos.x-object->getFlowPos().x<=in1Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in1Pos.y) && (pos.y-object->getFlowPos().y<=in1Pos.y+10)) return in1Pos;
    }
    if (((object->data.stdIN & OAPC_DIGI_IO2)==OAPC_DIGI_IO2) ||
        ((object->data.stdIN & OAPC_NUM_IO2) ==OAPC_NUM_IO2) ||
        ((object->data.stdIN & OAPC_BIN_IO2) ==OAPC_BIN_IO2) ||
        ((object->data.stdIN & OAPC_CHAR_IO2)==OAPC_CHAR_IO2))
    {
        if ((pos.x-object->getFlowPos().x>=in2Pos.x-5)  && (pos.x-object->getFlowPos().x<=in2Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in2Pos.y) && (pos.y-object->getFlowPos().y<=in2Pos.y+10)) return in2Pos;
    }
    if (((object->data.stdIN & OAPC_DIGI_IO3)==OAPC_DIGI_IO3) ||
        ((object->data.stdIN & OAPC_NUM_IO3) ==OAPC_NUM_IO3) ||
        ((object->data.stdIN & OAPC_BIN_IO3) ==OAPC_BIN_IO3) ||
        ((object->data.stdIN & OAPC_CHAR_IO3)==OAPC_CHAR_IO3))
    {
        if ((pos.x-object->getFlowPos().x>=in3Pos.x-5)  && (pos.x-object->getFlowPos().x<=in3Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in3Pos.y) && (pos.y-object->getFlowPos().y<=in3Pos.y+10)) return in3Pos;
    }
    if (((object->data.stdIN & OAPC_DIGI_IO4)==OAPC_DIGI_IO4) ||
        ((object->data.stdIN & OAPC_NUM_IO4) ==OAPC_NUM_IO4) ||
        ((object->data.stdIN & OAPC_BIN_IO4) ==OAPC_BIN_IO4) ||
        ((object->data.stdIN & OAPC_CHAR_IO4)==OAPC_CHAR_IO4))
    {
        if ((pos.x-object->getFlowPos().x>=in4Pos.x-5)  && (pos.x-object->getFlowPos().x<=in4Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in4Pos.y) && (pos.y-object->getFlowPos().y<=in4Pos.y+10)) return in4Pos;
    }
    if (((object->data.stdIN & OAPC_DIGI_IO5)==OAPC_DIGI_IO5) ||
        ((object->data.stdIN & OAPC_NUM_IO5) ==OAPC_NUM_IO5) ||
        ((object->data.stdIN & OAPC_BIN_IO5) ==OAPC_BIN_IO5) ||
        ((object->data.stdIN & OAPC_CHAR_IO5)==OAPC_CHAR_IO5))
    {
        if ((pos.x-object->getFlowPos().x>=in5Pos.x-5)  && (pos.x-object->getFlowPos().x<=in5Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in5Pos.y) && (pos.y-object->getFlowPos().y<=in5Pos.y+10)) return in5Pos;
    }
    
    if (((object->data.stdIN & OAPC_DIGI_IO6)==OAPC_DIGI_IO6) ||
        ((object->data.stdIN & OAPC_NUM_IO6) ==OAPC_NUM_IO6) ||
        ((object->data.stdIN & OAPC_BIN_IO6) ==OAPC_BIN_IO6) ||
        ((object->data.stdIN & OAPC_CHAR_IO6)==OAPC_CHAR_IO6))
    {
        if ((pos.x-object->getFlowPos().x>=in6Pos.x-5)  && (pos.x-object->getFlowPos().x<=in6Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in6Pos.y) && (pos.y-object->getFlowPos().y<=in6Pos.y+10)) return in6Pos;
    }
    if (((object->data.stdIN & OAPC_DIGI_IO7)==OAPC_DIGI_IO7) ||
        ((object->data.stdIN & OAPC_NUM_IO7) ==OAPC_NUM_IO7) ||
        ((object->data.stdIN & OAPC_BIN_IO7) ==OAPC_BIN_IO7) ||
        ((object->data.stdIN & OAPC_CHAR_IO7)==OAPC_CHAR_IO7))
    {
        if ((pos.x-object->getFlowPos().x>=in7Pos.x-5)  && (pos.x-object->getFlowPos().x<=in7Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in7Pos.y) && (pos.y-object->getFlowPos().y<=in7Pos.y+10)) return in7Pos;
    }
    return wxPoint(-1,-1);
}



wxUint32 FlowObjects::getInConnectorType(flowObject *object,wxPoint pos)
{
    if (object->data.type==hmiObject::FLOW_TYPE_GROUP) return ((flowGroup*)object)->getInConnectorType(pos);
    else if (object->data.type==hmiObject::FLOW_TYPE_GROUP_IO_DEFINITION) return ((IOElementDefinition*)object)->getInConnectorType(pos);
    if ((object->data.stdIN & OAPC_DIGI_IO0)==OAPC_DIGI_IO0)
    {
        if ((pos.x-object->getFlowPos().x>=in0Pos.x-5)  && (pos.x-object->getFlowPos().x<=in0Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in0Pos.y) && (pos.y-object->getFlowPos().y<=in0Pos.y+10)) return OAPC_DIGI_IO0;
    }
    if ((object->data.stdIN & OAPC_DIGI_IO1)==OAPC_DIGI_IO1)
    {
        if ((pos.x-object->getFlowPos().x>=in1Pos.x-5)  && (pos.x-object->getFlowPos().x<=in1Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in1Pos.y) && (pos.y-object->getFlowPos().y<=in1Pos.y+10)) return OAPC_DIGI_IO1;
    }
    if ((object->data.stdIN & OAPC_DIGI_IO2)==OAPC_DIGI_IO2)
    {
        if ((pos.x-object->getFlowPos().x>=in2Pos.x-5)  && (pos.x-object->getFlowPos().x<=in2Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in2Pos.y) && (pos.y-object->getFlowPos().y<=in2Pos.y+10)) return OAPC_DIGI_IO2;
    }
    if ((object->data.stdIN & OAPC_DIGI_IO3)==OAPC_DIGI_IO3)
    {
        if ((pos.x-object->getFlowPos().x>=in3Pos.x-5)  && (pos.x-object->getFlowPos().x<=in3Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in3Pos.y) && (pos.y-object->getFlowPos().y<=in3Pos.y+10)) return OAPC_DIGI_IO3;
    }
    if ((object->data.stdIN & OAPC_DIGI_IO4)==OAPC_DIGI_IO4)
    {
        if ((pos.x-object->getFlowPos().x>=in4Pos.x-5)  && (pos.x-object->getFlowPos().x<=in4Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in4Pos.y) && (pos.y-object->getFlowPos().y<=in4Pos.y+10)) return OAPC_DIGI_IO4;
    }
    if ((object->data.stdIN & OAPC_DIGI_IO5)==OAPC_DIGI_IO5)
    {
        if ((pos.x-object->getFlowPos().x>=in5Pos.x-5)  && (pos.x-object->getFlowPos().x<=in5Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in5Pos.y) && (pos.y-object->getFlowPos().y<=in5Pos.y+10)) return OAPC_DIGI_IO5;
    }
    if ((object->data.stdIN & OAPC_DIGI_IO6)==OAPC_DIGI_IO6)
    {
        if ((pos.x-object->getFlowPos().x>=in6Pos.x-5)  && (pos.x-object->getFlowPos().x<=in6Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in6Pos.y) && (pos.y-object->getFlowPos().y<=in6Pos.y+10)) return OAPC_DIGI_IO6;
    }
    if ((object->data.stdIN & OAPC_DIGI_IO7)==OAPC_DIGI_IO7)
    {
        if ((pos.x-object->getFlowPos().x>=in7Pos.x-5)  && (pos.x-object->getFlowPos().x<=in7Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in7Pos.y) && (pos.y-object->getFlowPos().y<=in7Pos.y+10)) return OAPC_DIGI_IO7;
    }

    if ((object->data.stdIN & OAPC_NUM_IO0) ==OAPC_NUM_IO0)
    {
        if ((pos.x-object->getFlowPos().x>=in0Pos.x-5)  && (pos.x-object->getFlowPos().x<=in0Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in0Pos.y) && (pos.y-object->getFlowPos().y<=in0Pos.y+10)) return OAPC_NUM_IO0;
    }
    if ((object->data.stdIN & OAPC_NUM_IO1) ==OAPC_NUM_IO1)
    {
        if ((pos.x-object->getFlowPos().x>=in1Pos.x-5)  && (pos.x-object->getFlowPos().x<=in1Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in1Pos.y) && (pos.y-object->getFlowPos().y<=in1Pos.y+10)) return OAPC_NUM_IO1;
    }
    if ((object->data.stdIN & OAPC_NUM_IO2) ==OAPC_NUM_IO2)
    {
        if ((pos.x-object->getFlowPos().x>=in2Pos.x-5)  && (pos.x-object->getFlowPos().x<=in2Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in2Pos.y) && (pos.y-object->getFlowPos().y<=in2Pos.y+10)) return OAPC_NUM_IO2;
    }
    if ((object->data.stdIN & OAPC_NUM_IO3) ==OAPC_NUM_IO3)
    {
        if ((pos.x-object->getFlowPos().x>=in3Pos.x-5)  && (pos.x-object->getFlowPos().x<=in3Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in3Pos.y) && (pos.y-object->getFlowPos().y<=in3Pos.y+10)) return OAPC_NUM_IO3;
    }
    if ((object->data.stdIN & OAPC_NUM_IO4) ==OAPC_NUM_IO4)
    {
        if ((pos.x-object->getFlowPos().x>=in4Pos.x-5)  && (pos.x-object->getFlowPos().x<=in4Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in4Pos.y) && (pos.y-object->getFlowPos().y<=in4Pos.y+10)) return OAPC_NUM_IO4;
    }
    if ((object->data.stdIN & OAPC_NUM_IO5) ==OAPC_NUM_IO5)
    {
        if ((pos.x-object->getFlowPos().x>=in5Pos.x-5)  && (pos.x-object->getFlowPos().x<=in5Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in5Pos.y) && (pos.y-object->getFlowPos().y<=in5Pos.y+10)) return OAPC_NUM_IO5;
    }
    if ((object->data.stdIN & OAPC_NUM_IO6) ==OAPC_NUM_IO6)
    {
        if ((pos.x-object->getFlowPos().x>=in6Pos.x-5)  && (pos.x-object->getFlowPos().x<=in6Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in6Pos.y) && (pos.y-object->getFlowPos().y<=in6Pos.y+10)) return OAPC_NUM_IO6;
    }
    if ((object->data.stdIN & OAPC_NUM_IO7) ==OAPC_NUM_IO7)
    {
        if ((pos.x-object->getFlowPos().x>=in7Pos.x-5)  && (pos.x-object->getFlowPos().x<=in7Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in7Pos.y) && (pos.y-object->getFlowPos().y<=in7Pos.y+10)) return OAPC_NUM_IO7;
    }

    if ((object->data.stdIN & OAPC_CHAR_IO0)==OAPC_CHAR_IO0)
    {
        if ((pos.x-object->getFlowPos().x>=in0Pos.x-5)  && (pos.x-object->getFlowPos().x<=in0Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in0Pos.y) && (pos.y-object->getFlowPos().y<=in0Pos.y+10)) return OAPC_CHAR_IO0;
    }
    if ((object->data.stdIN & OAPC_CHAR_IO1)==OAPC_CHAR_IO1)
    {
        if ((pos.x-object->getFlowPos().x>=in1Pos.x-5)  && (pos.x-object->getFlowPos().x<=in1Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in1Pos.y) && (pos.y-object->getFlowPos().y<=in1Pos.y+10)) return OAPC_CHAR_IO1;
    }
    if ((object->data.stdIN & OAPC_CHAR_IO2)==OAPC_CHAR_IO2)
    {
        if ((pos.x-object->getFlowPos().x>=in2Pos.x-5)  && (pos.x-object->getFlowPos().x<=in2Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in2Pos.y) && (pos.y-object->getFlowPos().y<=in2Pos.y+10)) return OAPC_CHAR_IO2;
    }
    if ((object->data.stdIN & OAPC_CHAR_IO3)==OAPC_CHAR_IO3)
    {
        if ((pos.x-object->getFlowPos().x>=in3Pos.x-5)  && (pos.x-object->getFlowPos().x<=in3Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in3Pos.y) && (pos.y-object->getFlowPos().y<=in3Pos.y+10)) return OAPC_CHAR_IO3;
    }
    if ((object->data.stdIN & OAPC_CHAR_IO4)==OAPC_CHAR_IO4)
    {
        if ((pos.x-object->getFlowPos().x>=in4Pos.x-5)  && (pos.x-object->getFlowPos().x<=in4Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in4Pos.y) && (pos.y-object->getFlowPos().y<=in4Pos.y+10)) return OAPC_CHAR_IO4;
    }
    if ((object->data.stdIN & OAPC_CHAR_IO5)==OAPC_CHAR_IO5)
    {
        if ((pos.x-object->getFlowPos().x>=in5Pos.x-5)  && (pos.x-object->getFlowPos().x<=in5Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in5Pos.y) && (pos.y-object->getFlowPos().y<=in5Pos.y+10)) return OAPC_CHAR_IO5;
    }
    if ((object->data.stdIN & OAPC_CHAR_IO6)==OAPC_CHAR_IO6)
    {
        if ((pos.x-object->getFlowPos().x>=in6Pos.x-5)  && (pos.x-object->getFlowPos().x<=in6Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in6Pos.y) && (pos.y-object->getFlowPos().y<=in6Pos.y+10)) return OAPC_CHAR_IO6;
    }
    if ((object->data.stdIN & OAPC_CHAR_IO7)==OAPC_CHAR_IO7)
    {
        if ((pos.x-object->getFlowPos().x>=in7Pos.x-5)  && (pos.x-object->getFlowPos().x<=in7Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in7Pos.y) && (pos.y-object->getFlowPos().y<=in7Pos.y+10)) return OAPC_CHAR_IO7;
    }

    if ((object->data.stdIN & OAPC_BIN_IO0) ==OAPC_BIN_IO0)
    {
        if ((pos.x-object->getFlowPos().x>=in0Pos.x-5)  && (pos.x-object->getFlowPos().x<=in0Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in0Pos.y) && (pos.y-object->getFlowPos().y<=in0Pos.y+10)) return OAPC_BIN_IO0;
    }
    if ((object->data.stdIN & OAPC_BIN_IO1) ==OAPC_BIN_IO1)
    {
        if ((pos.x-object->getFlowPos().x>=in1Pos.x-5)  && (pos.x-object->getFlowPos().x<=in1Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in1Pos.y) && (pos.y-object->getFlowPos().y<=in1Pos.y+10)) return OAPC_BIN_IO1;
    }
    if ((object->data.stdIN & OAPC_BIN_IO2) ==OAPC_BIN_IO2)
    {
        if ((pos.x-object->getFlowPos().x>=in2Pos.x-5)  && (pos.x-object->getFlowPos().x<=in2Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in2Pos.y) && (pos.y-object->getFlowPos().y<=in2Pos.y+10)) return OAPC_BIN_IO2;
    }
    if ((object->data.stdIN & OAPC_BIN_IO3) ==OAPC_BIN_IO3)
    {
        if ((pos.x-object->getFlowPos().x>=in3Pos.x-5)  && (pos.x-object->getFlowPos().x<=in3Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in3Pos.y) && (pos.y-object->getFlowPos().y<=in3Pos.y+10)) return OAPC_BIN_IO3;
    }
    if ((object->data.stdIN & OAPC_BIN_IO4) ==OAPC_BIN_IO4)
    {
        if ((pos.x-object->getFlowPos().x>=in4Pos.x-5)  && (pos.x-object->getFlowPos().x<=in4Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in4Pos.y) && (pos.y-object->getFlowPos().y<=in4Pos.y+10)) return OAPC_BIN_IO4;
    }
    if ((object->data.stdIN & OAPC_BIN_IO5) ==OAPC_BIN_IO5)
    {
        if ((pos.x-object->getFlowPos().x>=in5Pos.x-5)  && (pos.x-object->getFlowPos().x<=in5Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in5Pos.y) && (pos.y-object->getFlowPos().y<=in5Pos.y+10)) return OAPC_BIN_IO5;
    }
    if ((object->data.stdIN & OAPC_BIN_IO6) ==OAPC_BIN_IO6)
    {
        if ((pos.x-object->getFlowPos().x>=in6Pos.x-5)  && (pos.x-object->getFlowPos().x<=in6Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in6Pos.y) && (pos.y-object->getFlowPos().y<=in6Pos.y+10)) return OAPC_BIN_IO6;
    }
    if ((object->data.stdIN & OAPC_BIN_IO7) ==OAPC_BIN_IO7)
    {
        if ((pos.x-object->getFlowPos().x>=in7Pos.x-5)  && (pos.x-object->getFlowPos().x<=in7Pos.x+5) &&
            (pos.y-object->getFlowPos().y>=in7Pos.y) && (pos.y-object->getFlowPos().y<=in7Pos.y+10)) return OAPC_BIN_IO7;
    }

    return 0;
}


