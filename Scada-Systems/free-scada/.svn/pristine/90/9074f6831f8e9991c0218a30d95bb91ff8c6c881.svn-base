#if !defined(__SCATTER_H__)
#define __SCATTER_H__

#include "graphics.h"
#include <time.h>

//////////////////////////////////////////////////////////////////////////////////
////// struct CTPoint
template <class T>
struct CTPoint
{
        T x;
        T y;

        CTPoint()                                                       { x = 0; y = 0; }
        CTPoint(T initX, T initY)                       { x = initX; y = initY; }
        void operator = (const CTPoint& pt)     { x = pt.x;     y = pt.y; }
};


//////////////////////////////////////////////////////////////////////////////////
////// class CLinear
template <class T>
class CLinear : public CGraphics
{
public:
        virtual bool SetRange(double xmin, double ymin, double xmax, double ymax);
        virtual POINT GetPoint(T x, T y);
        virtual void GetValue(POINT& pt, T& x, T& y);

        void Lines(T* x, T* y, int n, COLORREF cr, int Index = 0, const wchar_t * Name = NULL, int LineWidth = 0, int LineStyle = PS_SOLID);
        void Markers(T* x, T* y, int n, COLORREF cr, int mode, int Index = 0, const wchar_t* Name = NULL, int size = 6);
        void Polygon(T* x, T* y, int n);
        void Polygon(CTPoint<T>* pt, int n);
        void Lines(CTPoint<T>* pt, int n, COLORREF cr, int Index = 0, const wchar_t * Name = NULL, int LineWidth = 0, int LineStyle = PS_SOLID);
        void Markers(CTPoint<T>* pt, int n, COLORREF cr, int mode, int Index = 0, const char* Name = NULL, int size = 6);
        bool FilledPolygon(CTPoint<T>* pt, int n);
        void FilledRectangle(T x1, T y1, T x2, T y2);
        bool FilledPolygon(T* x, T* y, int n);
        void FilledRectangle(CTPoint<T>&, CTPoint<T>&, COLORREF cr);
        bool FilledPolygon(CTPoint<T>*, int n, COLORREF cr);
        void FilledRectangle(T x1, T y1, T x2, T y2, COLORREF cr);
        bool FilledPolygon(T* x, T* y, int n, COLORREF cr);

        void MoveTo(T x, T y)                                                                   { GetPoint(x, y); ::MoveToEx(m_hDC, m_Pt.x, m_Pt.y, NULL); }
        void LineTo(T x, T y)                                                                   { GetPoint(x, y); ::LineTo(m_hDC, m_Pt.x, m_Pt.y); }
        void Line(T x1, T y1, T x2, T y2)                                               { MoveTo(x1, y1); LineTo(x2, y2); }
        void Line(CTPoint<T>& pt1, CTPoint<T>& pt2)                             { Line(pt1.x, pt1.y, pt2.x, pt2.y); }
        void Rectangle(T x1, T y1, T x2, T y2)                                  { MoveTo(x1, y1); LineTo(x2, y1); LineTo(x2, y2); LineTo(x1, y2); LineTo(x1, y1); }
        void Rectangle(CTPoint<T>& pt1, CTPoint<T>& pt2)                { Rectangle(pt1.x, pt1.y, pt2.x, pt2.y); }
        void MoveTo(CTPoint<T>& pt)                                                             { MoveTo(pt.x, pt.y); }
        void LineTo(CTPoint<T>& pt)                                                             { LineTo(pt.x, pt.y); }
        void Marker(T x, T y, int mode, int size = 6)                   { GetPoint(x, y); DrawMarker(m_Pt.x, m_Pt.y, mode, size); };
        void Marker(CTPoint<T>& pt, int mode, int size = 6)             { Marker(pt.x, pt.y, mode, size); }
        void FilledRectangle(CTPoint<T>& pt1, CTPoint<T>& pt2)  { FilledRectangle(pt1.x, pt1.y, pt2.x, pt2.y); }
        POINT GetTPoint(CTPoint<T>& pt)                                                 { return GetPoint(pt.x, pt.y); }
        CTPoint<T> GetTValue(POINT& pt)                                                 { CTPoint<T> t; GetValue(pt, t.x, t.y); return t; }

protected:
        void MarkerLegend(COLORREF cr, int Index, const wchar_t* Name, int mode, int size);
};

template <class T>
class CTimeGraph : public CLinear<T>
{
public:
        virtual void XAxis();
		virtual bool SetRangeY(double ymin, double ymax);
};



//////////////////////////////////////////////////////////////////////////////////
////// class CXLogYLinear
template <class T>
class CXLogYLinear : public CLinear<T>
{
public:
        virtual bool SetRange(double xmin, double ymin, double xmax, double ymax);
        virtual void XAxis();
        virtual void Grid();
        virtual POINT GetPoint(T x, T y);
        virtual void GetValue(POINT& pt, T& x, T& y);
};


//////////////////////////////////////////////////////////////////////////////////
////// class CXLiearYLog
template <class T>
class CXLinearYLog : public CLinear<T>
{
public:
        virtual bool SetRange(double xmin, double ymin, double xmax, double ymax);
        virtual void YAxis();
        virtual void Grid();
        virtual POINT GetPoint(T x, T y);
        virtual void GetValue(POINT& pt, T& x, T& y);
};


//////////////////////////////////////////////////////////////////////////////////
////// class CXLogYLog
template <class T>
class CXLogYLog : public CLinear<T>
{
public:
        virtual bool SetRange(double xmin, double ymin, double xmax, double ymax);
        virtual void XAxis();
        virtual void YAxis();
        virtual void Grid();
        virtual POINT GetPoint(T x, T y);
        virtual void GetValue(POINT& pt, T& x, T& y);
};


///////////////////////////////////////////////////////////////////////////////
// CLinear operations
template<class T>
bool CLinear<T>::SetRange(double xmin, double ymin, double xmax, double ymax)
{
        m_nAxesType             = XY;
    m_Scale.xmin        = xmin;
    m_Scale.ymin        = ymin;
    m_Scale.xmax        = xmax;
    m_Scale.ymax        = ymax;

        return true;
}

template<class T>
POINT CLinear<T>::GetPoint(T x, T y)
{
        m_Pt.x = (LONG)((x - m_Scale.xmin) / m_Scale.dx) + GL;
    m_Pt.y = (LONG)(GB - (y - m_Scale.ymin) / m_Scale.dy);
        return m_Pt;
}

template<class T>
void CLinear<T>::GetValue(POINT& pt, T& x, T& y)
{
    x = (T)(m_Scale.xmin + (pt.x - GL) * m_Scale.dx);
        y = (T)(m_Scale.ymin + (GB - pt.y) * m_Scale.dy);
}

template<class T>
void CLinear<T>::MarkerLegend(COLORREF cr, int Index, const wchar_t* Name, int mode, int size)
{
        m_LogFont.lfHeight = (int)(m_Size.cx / -25.0);
        if (m_LogFont.lfHeight > -10)
                m_LogFont.lfHeight = -10;
        m_LogFont.lfWeight         = 500;
        m_LogFont.lfOrientation= 0;
        m_LogFont.lfEscapement = 0;
        m_Font = ::CreateFontIndirect(&m_LogFont);
        if (m_Font)
        {
                int n  = (m_Rect.right - GR) / 20 + 1;
                int xb = GR + 2 * n;
                int xe = xb + 4 * n;
                int y  = GT - 3 * Index * m_LogFont.lfHeight / 2;
                DrawMarker(GR + 4 * n, y, mode, size);
                int bm  = ::SetBkMode(m_hDC, TRANSPARENT);
                HFONT hOldFont = (HFONT)::SelectObject(m_hDC, m_Font);
                ::SetTextColor(m_hDC, cr);
                SetStringAlign(LEFT, CENTER);
                PrintString(xe + n, y, 0, Name);
                ::SelectObject(m_hDC, hOldFont);
                ::DeleteObject(m_Font);
                ::SetBkMode(m_hDC, bm);
        }
}

template<class T>
void CLinear<T>::Markers(T* x, T* y, int n, COLORREF cr, int mode, int Index, const wchar_t * Name, int size)
{
        switch(mode)
        {
                case CROSS:
                case XCROSS:
                case STAR:
                case CIRCLE:
                case TRIANGLE:
                case DIAMOND:
                case SQUARE:
                {
                        HPEN hPen = ::CreatePen(PS_SOLID, 0, cr);
                        HPEN hOldPen  = (HPEN)::SelectObject(m_hDC, hPen);
                        for(int i = 0; i < n; i ++)
                                Marker(x[i], y[i], mode, size);
                        if (m_bEnableLegend && Index > 0 && Name)
                                MarkerLegend(cr, Index, Name, mode, size);
                        ::SelectObject(m_hDC, hOldPen);
                        ::DeleteObject(hPen);
                        break;
                }
                case FCIRCLE:
                case FTRIANGLE:
                case FDIAMOND:
                case FSQUARE:
                {
                        HBRUSH hBrush = ::CreateSolidBrush(cr);
                        HBRUSH hBrold = (HBRUSH)::SelectObject(m_hDC, hBrush);
                        for(int i = 0; i < n; i ++)
                                Marker(x[i], y[i], mode, size);
                        if (m_bEnableLegend && Index > 0 && Name)
                                MarkerLegend(cr, Index, Name, mode, size);
                        ::SelectObject(m_hDC, hBrold);
                        ::DeleteObject(hBrush);
                        break;
                }
        }
}

template<class T>
void CLinear<T>::Markers(CTPoint<T>* pt, int n, COLORREF cr, int mode, int Index, const char* Name, int size)
{
        switch(mode)
        {
                case CROSS:
                case XCROSS:
                case STAR:
                case CIRCLE:
                case TRIANGLE:
                case DIAMOND:
                case SQUARE:
                {
                        HPEN hPen = ::CreatePen(PS_SOLID, 1, cr);
                        HPEN hOldPen  = (HPEN)::SelectObject(m_hDC, hPen);
                        for(int i = 0; i < n; i ++)
                                Marker(pt[i].x, pt[i].y, mode, size);
                        if (m_bEnableLegend && Index > 0 && Name)
                                MarkerLegend(cr, Index, Name, mode, size);
                        ::SelectObject(m_hDC, hOldPen);
                        ::DeleteObject(hPen);
                        break;
                }
                case FCIRCLE:
                case FTRIANGLE:
                case FDIAMOND:
                case FSQUARE:
                {
                        HBRUSH hBrush = ::CreateSolidBrush(cr);
                        HBRUSH hBrold = (HBRUSH)::SelectObject(m_hDC, hBrush);
                        for(int i = 0; i < n; i ++)
                                Marker(pt[i].x, pt[i].y, mode, size);
                        if (m_bEnableLegend && Index > 0 && Name)
                                MarkerLegend(cr, Index, Name, mode, size);
                        ::SelectObject(m_hDC, hBrold);
                        ::DeleteObject(hBrush);
                        break;
                }
        }
}

template<class T>
void CLinear<T>::FilledRectangle(CTPoint<T>& pt1, CTPoint<T>& pt2, COLORREF cr)
{
    HBRUSH hBrush = ::CreateSolidBrush(cr);
    HBRUSH hBrold = (HBRUSH)::SelectObject(m_hDC, hBrush);
        FilledRectangle(pt1, pt2);
        ::SelectObject(m_hDC, hBrold);
    ::DeleteObject(hBrush);
}

template<class T>
void CLinear<T>::Polygon(CTPoint<T>* pt, int n)
{
        MoveTo(pt[0].x, pt[0].y);
        for(int i = 1; i < n; i ++)
                LineTo(pt[i].x, pt[i].y);
        LineTo(pt[0].x, pt[0].y);
}

template<class T>
bool CLinear<T>::FilledPolygon(CTPoint<T>* pt, int n)
{
        POINT *p = new POINT[n];
        if (!p) return false;
        for(int i = 0; i < n; i ++)
                p[i] = GetTPoint(pt[i]);
        ::Polygon(m_hDC, p, n );
        delete []p;

        return true;
}

template<class T>
bool CLinear<T>::FilledPolygon(CTPoint<T>* pt, int n, COLORREF cr)
{
    HBRUSH hBrush = ::CreateSolidBrush(cr);
    HBRUSH hBrold = (HBRUSH)::SelectObject(m_hDC, hBrush);
        bool bl = FilledPolygon(pt, n);
        ::SelectObject(m_hDC, hBrold);
    ::DeleteObject(hBrush);

        return bl;
}

template<class T>
void CLinear<T>::Lines(CTPoint<T>* pt, int n, COLORREF cr, int Index, const wchar_t* Name, int LineWidth, int LineStyle)
{
        if (m_bPrinting)        LineWidth *= m_nPrintScale;
        HPEN hPen = ::CreatePen(LineStyle, LineWidth, cr);
        HPEN hOldPen  = (HPEN)::SelectObject(m_hDC, hPen);

        MoveTo(pt[0].x, pt[0].y);
        for(int i = 1; i < n; i ++)
                LineTo(pt[i].x, pt[i].y);

        if (m_bEnableLegend && Index > 0 && Name)
                Legend(cr, Index, Name);

        ::SelectObject(m_hDC, hOldPen);
        ::DeleteObject(hPen);
}

template<class T>
void CLinear<T>::FilledRectangle(T x1, T y1, T x2, T y2)
{
        POINT pt1 = GetPoint(x1, y1);
        POINT pt2 = GetPoint(x2, y2);
        ::Rectangle(m_hDC, pt1.x, pt1.y, pt2.x, pt2.y);
}

template<class T>
void CLinear<T>::FilledRectangle(T x1, T y1, T x2, T y2, COLORREF cr)
{
    HBRUSH hBrush = ::CreateSolidBrush(cr);
    HBRUSH hBrold = (HBRUSH)::SelectObject(m_hDC, hBrush);
        FilledRectangle(x1, y1, x2, y2);
        ::SelectObject(m_hDC, hBrold);
    ::DeleteObject(hBrush);
}

template<class T>
void CLinear<T>::Polygon(T* x, T* y, int n)
{
        MoveTo(x[0], y[0]);
        for(int i = 1; i < n; i ++)
                LineTo(x[i], y[i]);
        LineTo(x[0], y[0]);
}

template<class T>
bool CLinear<T>::FilledPolygon(T* x, T* y, int n)
{
        POINT *pt = new POINT[n];
        if (!pt) return false;
        for(int i = 0; i < n; i ++)
                pt[i] = GetPoint(x[i], y[i]);
        ::Polygon(m_hDC, pt, n );
        delete []pt;

        return true;
}

template<class T>
bool CLinear<T>::FilledPolygon(T* x, T* y, int n, COLORREF cr)
{
    HBRUSH hBrush = ::CreateSolidBrush(cr);
    HBRUSH hBrold = (HBRUSH)::SelectObject(m_hDC, hBrush);
        bool bl = FilledPolygon(x, y, n);
        ::SelectObject(m_hDC, hBrold);
    ::DeleteObject(hBrush);

        return bl;
}

template<class T>
void CLinear<T>::Lines(T* x, T* y, int n, COLORREF cr, int Index, const wchar_t * Name, int LineWidth, int LineStyle)
{
        if (m_bPrinting)        LineWidth *= m_nPrintScale;
        HPEN hPen = ::CreatePen(LineStyle, LineWidth, cr);
        HPEN hOldPen  = (HPEN)::SelectObject(m_hDC, hPen);

        MoveTo(x[0], y[0]);
        for(int i = 1; i < n; i ++)
                LineTo(x[i], y[i]);

        if (m_bEnableLegend && Index > 0 && Name)
                Legend(cr, Index, Name);

        ::SelectObject(m_hDC, hOldPen);
        ::DeleteObject(hPen);
}


///////////////////////////////////////////////////////////////////////////////
// CXLogYLinear operations
template<class T>
bool CXLogYLinear<T>::SetRange(double xmin, double ymin, double xmax, double ymax)
{
        m_nAxesType      = XLOG;
        if (xmin < 0.000001 || xmax < 0.000001)
        {
                ::MessageBox(NULL, "The X minimum value must be larger than 0.000001", "Warning", MB_ICONWARNING);
                return false;
        }

    m_Scale.xmin        = log10(xmin);
    m_Scale.ymin        = ymin;
    m_Scale.xmax        = log10(xmax);
    m_Scale.ymax        = ymax;
        m_nXStep                = (int)(m_Scale.xmax - m_Scale.xmin);

        return true;
}

template<class T>
void CXLogYLinear<T>::XAxis()
{
        int       xb, yb, xe, ye;
    int   i, j, k = 1;
        char  str[32];
        float value;

        yb = GB;
        for(i = 1; i <= m_nXStep; i ++)
        {
                for(j = 1; j <= 10; j ++)
                {
                        if (k == 0)
                                ye = GB + m_bM / 7;
                        else
                                ye = GB + m_bM / 14;
                        xb = xe = (int)(log10((float)j) * PX / m_nXStep + 1.0 * (i-1) * PX / m_nXStep + GL);
                        if (j == 1)
                        {
                                value = (float)(pow(10.0, m_Scale.xmin) * pow(10.0, i - 1));
                                Format(m_nXDecimal, str, value);
                                PrintString(xb, GB + m_bM / 5, 0, str);
                        }
                        if ((i != 1 || j != 1) && (i != m_nXStep || j != 10))
                                DrawLine(xb, yb, xe, ye);
                        k = 1;
                }
                k=0;
        }
        value = (float)(pow(10.0, m_Scale.xmin) * pow(10.0, i - 1));
        Format(m_nXDecimal, str, value);
        PrintString(GR, GB + m_bM / 5, 0, str);
}

template<class T>
void CXLogYLinear<T>::Grid()
{
        int i, j, k;

        HPEN hPen       = ::CreatePen(PS_DOT, 0, m_nGridColor);
        HPEN hOldPen = (HPEN)::SelectObject(m_hDC, hPen);

        for(i = 1; i < m_nXStep; i ++)
                for(k = 2; k <= 10; k ++)
        {
                j = (int)(log10(k) * PX / m_nXStep + 1.0 * (i - 1) * PX / m_nXStep + GL);
                DrawLine(j, GT + 1, j, GB - 1);
        }
        for(k = 2; k < 10; k ++)
        {
                j = (int)(log10(k) * PX / m_nXStep + 1.0 * (m_nXStep - 1) * PX / m_nXStep + GL);
                DrawLine(j, GT + 1, j, GB - 1);
        }

        for(i = 1; i < YGridTicks; i ++)
        {
                j = GT + (int)(1.0 * i * (GB-GT) / YGridTicks);
                DrawLine(GL + 1, j, GR - 1, j);
        }

        ::SelectObject(m_hDC, hOldPen);
        ::DeleteObject(hPen);
}

template<class T>
POINT CXLogYLinear<T>::GetPoint(T x, T y)
{
        m_Pt.x = (LONG)((log10(x) - m_Scale.xmin) / m_Scale.dx) + GL;
    m_Pt.y = (LONG)(GB - (y - m_Scale.ymin) / m_Scale.dy);
        return m_Pt;
}

template<class T>
void CXLogYLinear<T>::GetValue(POINT& pt, T& x, T& y)
{
    x = (T)pow(10, m_Scale.xmin + (pt.x - GL) * m_Scale.dx);
        y = (T)(m_Scale.ymin + (GB - pt.y) * m_Scale.dy);
}


///////////////////////////////////////////////////////////////////////////////
// CXLinearYLog operations
template<class T>
bool CXLinearYLog<T>::SetRange(double xmin, double ymin, double xmax, double ymax)
{
        m_nAxesType      = YLOG;
        if (ymin < 0.000001 || ymax < 0.000001)
        {
                ::MessageBox(NULL, "The Y minimum value must be larger than 0.000001", "Warning", MB_ICONWARNING);
                return false;
        }

    m_Scale.xmin        = xmin;
    m_Scale.ymin        = log10(ymin);
    m_Scale.xmax        = xmax;
    m_Scale.ymax        = log10(ymax);
        m_nYStep                = (int)(m_Scale.ymax - m_Scale.ymin);

        return true;
}

template<class T>
void CXLinearYLog<T>::YAxis()
{
        int       xb, yb, xe, ye;
    int   i, j, k = 1;
        char  str[32];
        float value;

        xe = GL;
        for(i = 1; i <= m_nYStep; i ++)
        {
                for(j = 1; j <= 10; j ++)
                {
                        if (k == 0)
                                xb = GL - m_lM / 10;
                        else
                                xb = GL - m_lM / 20;
                        yb = ye = GB - (int)(log10(j) * PY / m_nYStep + 1.0 * (i - 1) * PY / m_nYStep);
                        if (j == 1)
                        {
                                value = (float)(pow(10.0, m_Scale.ymin) * pow(10.0, i - 1));
                                Format(m_nYDecimal, str, value);
                                PrintString(GL - m_lM / 6, yb, 0, str);
                        }
                        if ((i != 1 || j != 1) && (i != m_nYStep || j != 10))
                                DrawLine(xb, yb, xe, ye);
                        k = 1;
                }
                k=0;
        }

        value = (float)(pow(10.0, m_Scale.ymin) * pow(10.0, i - 1));
        Format(m_nYDecimal, str, value);
        PrintString(GL - m_lM / 6, yb, 0, str);
}

template<class T>
void CXLinearYLog<T>::Grid()
{
        int i, j, k;

        HPEN hPen       = ::CreatePen(PS_DOT, 0, m_nGridColor);
        HPEN hOldPen = (HPEN)::SelectObject(m_hDC, hPen);

        for(i = 1; i < XGridTicks; i ++)
        {
                j = GL + (int)(1.0 * i * (GR-GL) / XGridTicks);
                DrawLine(j, GT + 1, j, GB - 1);
        }

        for(i = 1; i < m_nYStep; i ++)
                for(k = 2; k <= 10; k ++)
        {
                j = GB - (int)(log10(k) * PY / m_nYStep + 1.0 * (i - 1) * PY / m_nYStep);
                DrawLine(GL + 1, j, GR - 1, j);
        }
        for(k = 2; k < 10; k ++)
        {
                j = GB - (int)(log10(k) * PY / m_nYStep + 1.0 * (m_nYStep - 1) * PY / m_nYStep);
                DrawLine(GL + 1, j, GR - 1, j);
        }

        ::SelectObject(m_hDC, hOldPen);
        ::DeleteObject(hPen);
}

template<class T>
POINT CXLinearYLog<T>::GetPoint(T x, T y)
{
        m_Pt.x = (LONG)((x - m_Scale.xmin) / m_Scale.dx) + GL;
    m_Pt.y = (LONG)(GB - (log10(y) - m_Scale.ymin) / m_Scale.dy);
        return m_Pt;
}

template<class T>
void CXLinearYLog<T>::GetValue(POINT& pt, T& x, T& y)
{
    x = (T)(m_Scale.xmin + (pt.x - GL) * m_Scale.dx);
        y = (T)pow(10, (m_Scale.ymin + (GB - pt.y) * m_Scale.dy));
}

///////////////////////////////////////////////////////////////////////////////
// CXLogYLog operations
template<class T>
bool CXLogYLog<T>::SetRange(double xmin, double ymin, double xmax, double ymax)
{
        m_nAxesType      = XYLOG;
        if (xmin < 0.000001 || xmax < 0.000001)
        {
                ::MessageBox(NULL, "The X minimum value must be larger than 0.000001", "Warning", MB_ICONWARNING);
                return false;
        }

        if (ymin < 0.000001 || ymax < 0.000001)
        {
                ::MessageBox(NULL, "The Y minimum value must be larger than 0.000001", "Warning", MB_ICONWARNING);
                return false;
        }

    m_Scale.xmin        = log10(xmin);
    m_Scale.ymin        = log10(ymin);
    m_Scale.xmax        = log10(xmax);
    m_Scale.ymax        = log10(ymax);
        m_nXStep                = (int)(m_Scale.xmax - m_Scale.xmin);
        m_nYStep                = (int)(m_Scale.ymax - m_Scale.ymin);

        return true;
}

template<class T>
void CXLogYLog<T>::XAxis()
{
        int       xb, yb, xe, ye;
    int   i, j, k = 1;
        char  str[32];
        float value;

        yb = GB;
        for(i = 1; i <= m_nXStep; i ++)
        {
                for(j = 1; j <= 10; j ++)
                {
                        if (k == 0)
                                ye = GB + m_bM / 7;
                        else
                                ye = GB + m_bM / 14;
                        xb = xe = (int)(log10(j) * PX / m_nXStep + 1.0 * (i-1) * PX / m_nXStep + GL);
                        if (j == 1)
                        {
                                value = (float)(pow(10.0, m_Scale.xmin) * pow(10.0, i - 1));
                                Format(m_nXDecimal, str, value);
                                PrintString(xb, GB + m_bM / 5, 0, str);
                        }
                        if ((i != 1 || j != 1) && (i != m_nXStep || j != 10))
                                DrawLine(xb, yb, xe, ye);
                        k = 1;
                }
                k=0;
        }
        value = (float)(pow(10.0, m_Scale.xmin) * pow(10.0, i - 1));
        Format(m_nXDecimal, str, value);
        PrintString(GR, GB + m_bM / 5, 0, str);
}

template<class T>
void CXLogYLog<T>::YAxis()
{
        int       xb, yb, xe, ye;
    int   i, j, k = 1;
        char  str[32];
        float value;

        xe = GL;
        for(i = 1; i <= m_nYStep; i ++)
        {
                for(j = 1; j <= 10; j ++)
                {
                        if (k == 0)
                                xb = GL - m_lM / 10;
                        else
                                xb = GL - m_lM / 20;
                        yb = ye = GB - (int)(log10(j) * PY / m_nYStep + 1.0 * (i - 1) * PY / m_nYStep);
                        if (j == 1)
                        {
                                value = (float)(pow(10.0, m_Scale.ymin) * pow(10.0, i - 1));
                                Format(m_nYDecimal, str, value);
                                PrintString(GL - m_lM / 6, yb, 0, str);
                        }
                        if ((i != 1 || j != 1) && (i != m_nYStep || j != 10))
                                DrawLine(xb, yb, xe, ye);
                        k = 1;
                }
                k=0;
        }

        value = (float)(pow(10.0, m_Scale.ymin) * pow(10.0, i - 1));
        Format(m_nYDecimal, str, value);
        PrintString(GL - m_lM / 6, yb, 0, str);
}

template<class T>
void CXLogYLog<T>::Grid()
{
        int i, j, k;

        HPEN hPen       = ::CreatePen(PS_DOT, 0, m_nGridColor);
        HPEN hOldPen = (HPEN)::SelectObject(m_hDC, hPen);

        for(i = 1; i < m_nXStep; i ++)
                for(k = 2; k <= 10; k ++)
        {
                j = (int)(log10(k) * PX / m_nXStep + 1.0 * (i - 1) * PX / m_nXStep + GL);
                DrawLine(j, GT + 1, j, GB - 1);
        }
        for(k = 2; k < 10; k ++)
        {
                j = (int)(log10(k) * PX / m_nXStep + 1.0 * (m_nXStep - 1) * PX / m_nXStep + GL);
                DrawLine(j, GT + 1, j, GB - 1);
        }

        for(i = 1; i < m_nYStep; i ++)
                for(k = 2; k <= 10; k ++)
        {
                j = GB - (int)(log10(k) * PY / m_nYStep + 1.0 * (i - 1) * PY / m_nYStep);
                DrawLine(GL + 1, j, GR - 1, j);
        }
        for(k = 2; k < 10; k ++)
        {
                j = GB - (int)(log10(k) * PY / m_nYStep + 1.0 * (m_nYStep - 1) * PY / m_nYStep);
                DrawLine(GL + 1, j, GR - 1, j);
        }

        ::SelectObject(m_hDC, hOldPen);
        ::DeleteObject(hPen);
}

template<class T>
POINT CXLogYLog<T>::GetPoint(T x, T y)
{
        m_Pt.x = (LONG)((log10(x) - m_Scale.xmin) / m_Scale.dx) + GL;
    m_Pt.y = (LONG)(GB - (log10(y) - m_Scale.ymin) / m_Scale.dy);
        return m_Pt;
}

template<class T>
void CXLogYLog<T>::GetValue(POINT& pt, T& x, T& y)
{
    x = (T)pow(10, (m_Scale.xmin + (pt.x - GL) * m_Scale.dx));
        y = (T)pow(10, (m_Scale.ymin + (GB - pt.y) * m_Scale.dy));
}


template<class T>
void CTimeGraph<T>::XAxis()
{
        int       xb, yb, xe, ye;
		int   i, j;
        wchar_t  str[32];
        

        yb=GB;
        j = XTicks / 10;
        for(i = 0; i <= XTicks; i ++)
        {
                xb = xe = (int)(GL + 1.0 * PX * i / XTicks );
                if((i % j) == 0)
                {
                        ye = GB + m_bM / 7;
                        const __time32_t t = static_cast<__time32_t>((m_Scale.xmin + i * (m_Scale.xmax - m_Scale.xmin) / XTicks));
                        //Format(m_nXDecimal, str, value);
                        struct tm newtime;
                        _localtime32_s( &newtime, &t );   // Convert time to struct tm form.
                        // Print local time as a string.
                        wcsftime(str,32,L"%H:%M:%S",&newtime);
                        PrintString(xb, GB + m_bM / 5, 0, str);
                }
                else
                        ye = GB + m_bM / 14;
                if (i > 0 && i < XTicks)
                        DrawLine(xb, yb, xe, ye);
        }
}

template<class T>
bool CTimeGraph<T>::SetRangeY(double ymin, double ymax)
{
	m_Scale.ymin=ymin;
	m_Scale.ymax=ymax;
	return true;
}

#endif
