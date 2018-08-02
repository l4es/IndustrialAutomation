#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define _WIN32_DCOM
#define _WTL_NO_CSTRING


//STD
#include <math.h>

//ATL/WTL
#include <atlstr.h>
#include <atlbase.h>
#include <atlcom.h>
#include <AtlFile.h>
#include <AtlApp.h>
#include <AtlGDI.h>
#include <WTypes.h>
#include <atlapp.h>
#include <atlmisc.h>

//STL
#include <vector>
#include <functional>
#include <algorithm>
#include <list>
#include <map>
#include <stack>
#include <queue>
#include <boost/spirit/core.hpp>

//OPC
#include "../3rdParty/opc/opc_types.h"
#include "../3rdParty/opc/opccomn.h"
#include "../3rdParty/opc/opcda.h"
#include "../3rdParty/opc/opcerror.h"
#include "../3rdParty/opc/opcquality.h"
#include "../3rdParty/opc/OpcEnum.h"

//Other
#include "../3rdParty/FreeImage/FreeImage.h"
#include "../HelpersLib/HelpersLib.h"
#include <Windows.h>


using namespace std;
using namespace boost;
using namespace WTL;

#define MOVE_BLOCK_SIZE         7
#define PI                                      3.14159268f
#define LCID_RUSSIAN            MAKELCID(MAKELANGID(LANG_RUSSIAN, SUBLANG_NEUTRAL),SORT_DEFAULT)

#define RadToDeg(rad)((rad*180)/PI)
#define DegToRad(deg)((deg*PI)/180)

typedef vector<wstring> TStrings;

enum enTypeID
{
        _SELECTION = 0,
        _RECTANGLE = 1,
        _POLYLINE = 2,
        _ELLIPSE = 3,
        _IMAGE = 4,
        _ACTION = 5,
        _VARIABLE = 6,
        _TEXT = 7,
        _BUTTON = 8,
        _INPUT = 9,
        _SOUND = 10,
        _GRAPH = 11,
};

struct sRGB
{
        sRGB(){r=g=b=0;};
        sRGB(BYTE r_, BYTE g_, BYTE b_):r(r_),g(g_),b(b_){};
        sRGB(COLORREF val){r = GetRValue(val); g = GetGValue(val); b = GetBValue(val);};
        sRGB(const sRGB& src){r = src.r;g = src.g;b = src.b;};

        BYTE r,g,b;

        inline COLORREF AsCOLORREF(){return RGB(r,g,b);};
        inline sRGB& operator=(COLORREF val){r = GetRValue(val); g = GetGValue(val); b = GetBValue(val);return *this;};
        inline sRGB& operator=(const sRGB& src){r = src.r;g = src.g;b = src.b;return *this;};
        inline bool operator!=(const sRGB &rhs)
        {
                return r != rhs.r || g != rhs.g || b != rhs.b;
        }
};

struct sVector
{
        float x,y;

        sVector(){x=y=0;};
        sVector(float x_, float y_):x(x_),y(y_){};
        sVector(float val){x=y=val;};
        sVector(CPoint val){x=(float)val.x; y=(float)val.y;};

        inline void Normalize()
        {
                const float alpha = 1/sqrt(pow(x,2)+pow(y,2));
                x *= alpha;
                y *= alpha;
        };

        inline void Rotate(float Angle)
        {
                float x1,y1;
                x1 = cos(Angle)*x - sin(Angle)*y;
                y1 = sin(Angle)*x + cos(Angle)*y;
                x = x1;
                y = y1;
        }

        inline float Length()
        {
                return sqrt(x*x + y*y);
        }

        inline sVector& operator+=(sVector val)
        {
                x += val.x;
                y += val.y;
                return *this;
        }

        inline sVector& operator-=(sVector val)
        {
                x -= val.x;
                y -= val.y;
                return *this;
        }

        inline sVector& operator/=(sVector val)
        {
                x /= val.x;
                y /= val.y;
                return *this;
        }

        inline sVector operator-(sVector val)
        {
                sVector v;
                v.x = x - val.x;
                v.y = y - val.y;
                return v;
        }

        inline sVector operator/(sVector val)
        {
                sVector v;
                v.x = x/val.x;
                v.y = y/val.y;
                return v;
        }

        inline sVector operator+(sVector val)
        {
                sVector v;
                v.x = x + val.x;
                v.y = y + val.y;
                return v;
        }
        inline sVector operator/(float val)
        {
                sVector v;
                v.x = x / val;
                v.y = y / val;
                return v;
        }

        inline sVector operator*(float val)
        {
                sVector v;
                v.x = x * val;
                v.y = y * val;
                return v;
        }
        inline sVector operator*(sVector val)
        {
                return sVector(x * val.x, y * val.y);
        }
        inline sVector& operator*=(sVector val)
        {
                x *= val.x;
                y *= val.y;
                return *this;
        }

        inline sVector operator-()
        {
                sVector tmp;
                tmp.x = -x;
                tmp.y = -y;
                return tmp;
        }
};

class CMemFile;
struct sFont
{
        wstring Name;
        bool    Bold;
        bool    Italic;
        bool    Underline;
        bool    StrikeOut;
        long    Height;

        void Serialize(CMemFile& file, BOOL IsLoading);
};


class CObject;
//функтор для сравнения объектов по позиции
struct sCompOrderPos
{
        bool operator()(CObject* first, CObject* second);
};

//функтор для поиска объекта по имени
struct  sFindByName
        : unary_function<CObject, bool>
{
        wstring Name;
        sFindByName(wstring Name_): Name(Name_){}
        bool operator()(CObject* obj);
};


//Угол между двумя векторами
float AngleBetweenVector(sVector v1, sVector v2);
//Угол между двумя векторами с учетом знака
float AngleBetweenVectorEx(sVector V1, sVector V2);
//Генерировать уникальное имя объекта с учетом префикса
wstring GenerateUniqueObjectName(wstring Prefix);
wstring GenerateUniqueActionName(wstring Prefix);