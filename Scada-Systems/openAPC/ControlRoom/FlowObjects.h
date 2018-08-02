#ifndef FLOWOBJECTS_H
#define FLOWOBJECTS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define FLOW_OBJECT_WIDTH  106
#define FLOW_OBJECT_HEIGHT 50

class flowObject;

class FlowObjects:wxObject
{
public:
	FlowObjects();
	virtual ~FlowObjects();

    wxByte   isAtOutConnector(flowObject *object,wxPoint pos);
    wxPoint  getOutConnectorPos(flowObject *object,wxPoint pos);
    wxUint64 getOutConnectorType(flowObject *object,wxPoint pos);
    wxByte   isAtInConnector(flowObject *object,wxPoint pos);
    wxPoint  getInConnectorPos(flowObject *object,wxPoint pos);
    wxUint32 getInConnectorType(flowObject *object,wxPoint pos);


private:
    wxPoint in0Pos,out0Pos,
            in1Pos,out1Pos,
            in2Pos,out2Pos,
            in3Pos,out3Pos,
            in4Pos,out4Pos,
            in5Pos,out5Pos,
            in6Pos,out6Pos,
            in7Pos,out7Pos;
};

#endif
