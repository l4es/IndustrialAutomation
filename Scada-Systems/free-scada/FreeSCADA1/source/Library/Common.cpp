#include "Common.h"
#include "ObjectMenager.h"
#include "VariableMenager.h"
#include "ActionMenager.h"
#include "MemFile.h"

//////////////////////////////////////////////////////////////////////////
// OPC
//////////////////////////////////////////////////////////////////////////
#include "../3rdParty/opc/opccomn_i.c"
#include "../3rdParty/opc/opcda_i.c"


float AngleBetweenVector(sVector v1, sVector v2)
{
        sVector mul = v1*v2;

        return acos((mul.x+mul.y)/(v1.Length()*v2.Length()));
}


float AngleBetweenVectorEx(sVector V1, sVector V2)
{
        float COS1 = V1.x/V1.Length();
        float COS2 = V2.x/V2.Length();

        float angle = 0;
        float angle1 = acos(COS1);
        float angle2 = acos(COS2);;

        if((V2.y>=0)&&(V1.y>=0))
                angle = angle2 - angle1;
        if((V2.y<=0)&&(V1.y<=0))
                angle = angle1 - angle2;
        if((V2.y>=0)&&(V1.y<=0)&&(V2.x<0)&&(V1.x<0))
                angle = (float)(PI-angle2 + PI-angle1);
        if((V2.y>=0)&&(V1.y<=0)&&(V2.x>=0)&&(V1.x>=0))
                angle = angle2 + angle1;
        if((V2.y<=0)&&(V1.y>=0)&&(V2.x>=0)&&(V1.x>=0))
                angle = -(angle2 + angle1);
        if((V2.y<=0)&&(V1.y>=0)&&(V2.x<0)&&(V1.x<0))
                angle = (float)(-(PI-angle2 + PI-angle1));

        return angle;
}

wstring GenerateUniqueObjectName(wstring Prefix)
{
        wstring tmp;
        DWORD i=0;
        CObjectMenagerPtr mngr;
        wchar_t sufix[32];
        do
        {
                swprintf(sufix, L"%u",++i);
                tmp = Prefix;
                tmp += sufix;
        }while(mngr->FindObject(tmp) != NULL);
        return tmp;
}
wstring GenerateUniqueActionName(wstring Prefix)
{
        wstring tmp;
        DWORD i=0;
        CActionMenagerPtr mngr;
        wchar_t sufix[32];
        do
        {
                swprintf(sufix, L"%u",++i);
                tmp = Prefix;
                tmp += sufix;
        }while(mngr->FindAction(tmp) != NULL);
        return tmp;
}

void sFont::Serialize(CMemFile& file, BOOL IsLoading)
{
        if(IsLoading)
        {
                file>>Name;
                file>>Bold;
                file>>Height;
                file>>Italic;
                file>>StrikeOut;
                file>>Underline;
        }
        else
        {
                file<<Name;
                file<<Bold;
                file<<Height;
                file<<Italic;
                file<<StrikeOut;
                file<<Underline;
        }
};


bool sCompOrderPos::operator()(CObject* first, CObject* second)
{
        return first->GetOrderPos() > second->GetOrderPos();
}


bool sFindByName::operator()(CObject* obj)
{
        return obj->GetName() == Name;
}