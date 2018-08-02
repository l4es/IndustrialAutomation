#pragma once

enum enCtrl 
{
	CT_TEXT, 
	CT_INT, 
	CT_FLOAT,  
	CT_FLAG,  
	CT_BOOL, 
	CT_COLOR, 
	CT_IMAGES, 
	CT_FONT, 
	CT_SOUND, 
	CT_POLYLINE, 
	CT_SIDE, 
	CT_VARIABLE,
	CT_ALIGN
};

enum enType {T_NOTYPE = 0, T_FLOAT = 1, T_STRING = 2, T_INTEGER = 3, T_BOOLEAN = 4, T_RGB=5, T_VARIANT = 6};
enum enCatID {CAT_EMPTY = 0, CAT_GEOMETRY, CAT_APPEARANCE, CAT_FONT, CAT_DATA, };



struct sParam 
{
	enType Type;
	CComVariant Value;

	sParam(): Type(T_NOTYPE){  Value.ChangeType(VT_EMPTY);  }
	
	//vector<sRGB>	Vec_RGB;
	//vector<bool>	Vec_Bool;
	enType GetType(){ return Type; }

	bool GetFloat(float& val)	
	{ 
		if(Value.vt == VT_R4)
		{
			val = Value.fltVal;
			return true;
		}
		if(Value.vt == VT_BSTR)
		{
			_bstr_t bstr(Value.bstrVal);
			val = atof((const char*)bstr);
			return true;
		}
		return false;

	}

	bool	GetInt(int& val)	
	{ 
		if(Value.vt == VT_I2)
		{
			val = Value.iVal;
			return true;
		}
		if(Value.vt == VT_BSTR)
		{
			_bstr_t bstr(Value.bstrVal);
			val = atoi((const char*)bstr);
			return true;
		}
		return false; 
	}


	bool GetString(wstring& val) 
	{ 
		if(Value.vt == VT_BSTR)
		{
			_bstr_t bstr(Value.bstrVal);
			wstring s(bstr);
			val = s;
			return true;
		}
		return false;
	}


	bool	GetBool(bool& val)	
	{ 
		if(Value.vt == VT_BOOL)
		{
			if(Value.boolVal == VARIANT_TRUE){val = true; return true;}
			if(Value.boolVal == VARIANT_FALSE){val = false; return true;}
		}
		if(Value.vt == VT_BSTR)
		{
			_bstr_t bstr(Value.bstrVal);
			wstring s(bstr);
			if(s == L"true"){val = true; return true;}
			if(s == L"false"){val = false; return true;}
		}
		return false;
	}

	bool	GetRGB(sRGB& val)	
	{ 
		if(Value.vt == VT_I4)
		{
			COLORREF color = (COLORREF)Value.lVal;
			sRGB rgb;
			rgb.r = GetRValue(color);
			rgb.g = GetGValue(color);
			rgb.b = GetBValue(color);
			val = rgb;
			return true;
		}
		if(Value.vt == VT_BSTR)
		{
			_bstr_t bstr(Value.bstrVal);
			COLORREF color = (COLORREF)atoi((const char*)bstr);
			sRGB rgb;
			rgb.r = GetRValue(color);
			rgb.g = GetGValue(color);
			rgb.b = GetBValue(color);
			val = rgb;
			return true;
		}
		return false; 
	}

	wstring GetAsText()
	{
		TCHAR tmp[0xff];
		wstring str;
		sRGB rgb;
		float f;
		int i;
		bool b;

		switch(Type) 
		{
		case T_FLOAT:
			if(GetFloat(f))
				_stprintf(tmp,L"%.3f", f);
			break;
		case T_STRING:
			if(GetString(str))
				_tcscpy(tmp, str.c_str());
			break;
		case T_INTEGER:
  			if(GetInt(i))
				_stprintf(tmp,L"%d", i);
			break;
		case T_BOOLEAN:
			if(GetBool(b))
				_stprintf(tmp, b?L"true":L"false");
			break;
		case  T_RGB:
			if(GetRGB(rgb))
				_stprintf(tmp, L"%03d %03d %03d", rgb.r, rgb.g, rgb.b);
			break;
		case  T_VARIANT:
			if(GetString(str))
				_stprintf(tmp, str.c_str());
			break;
		default: 
			ATLASSERT(L"Œÿ»¡ ¿"!=NULL);
			_tcscpy(tmp, L"");
		}
		return tmp;
	}

	inline void SetFloat(float Float)
	{
		Type = T_FLOAT;
		Value.ChangeType(VT_R4);
		Value.fltVal = Float;
	}

	inline void SetFloat(wstring StrVal)
	{
		Type = T_FLOAT;
		Value.ChangeType(VT_R4);
		Value.fltVal = _wtof(StrVal.c_str());
	}

	inline void SetInt(int Int)
	{
		Type = T_INTEGER;
		Value.ChangeType(VT_I2);
		Value.iVal = Int;
	}

	inline void SetInt(wstring StrVal)
	{
		Type = T_INTEGER;
		Value.ChangeType(VT_I2);
		Value.iVal = _wtoi(StrVal.c_str());
	}

	inline void SetBool(wstring StrVal)
	{
		Type = T_BOOLEAN;
		Value.ChangeType(VT_BOOL);
		_tcslwr(&*StrVal.begin());
		if(StrVal == L"true")
			Value.boolVal = VARIANT_TRUE;
		if(StrVal == L"false")
			Value.boolVal = VARIANT_FALSE;
	}

	inline void SetBool(bool Bool)
	{
		Type = T_BOOLEAN;
		Value.ChangeType(VT_BOOL);
		if(Bool)
			Value.boolVal = VARIANT_TRUE;
		else
			Value.boolVal = VARIANT_FALSE;
	}

	inline void SetString(wstring StrVal)
	{
		Type = T_STRING;
		Value.ChangeType(VT_BSTR);
		Value.bstrVal = _bstr_t(StrVal.c_str());
	}

	inline void SetRGB(wstring StrVal)
	{
		Type = T_RGB;
		BYTE r,g,b;
		int n = 0;
		for(int i = 0; i < 3; i++)
		{
			wstring s;
			for(int j = 0; j < 3; j++)
			{
				s += StrVal[n];
				n++;
			}
			
			if(i == 0) r = _wtoi(s.c_str());
			if(i == 1) g = _wtoi(s.c_str());
			if(i == 2) b = _wtoi(s.c_str());
			n++;
		}
		COLORREF color = RGB(r, g, b);
		Value.ChangeType(VT_I4);
		Value.lVal = (LONG)color;

	}


	inline void SetRGB(sRGB color)
	{
		Type = T_RGB;
		Value.ChangeType(VT_I4);
		Value.lVal = (LONG)color.AsCOLORREF();
	}

	inline void SetRGB(COLORREF color)
	{
		Type = T_RGB;
		Value.ChangeType(VT_I4);
		Value.lVal = (LONG)color;
	}

	inline void SetVariant(_variant_t var)
	{
		Type = T_VARIANT;
		Value = CComVariant(var); 
	}

	
	void Clear()
	{
		Type		= T_NOTYPE;
		Value.Clear();
	}
};

struct sCtrlParam
{
	enCtrl Ctrl;
	wstring Param;
	sCtrlParam(enCtrl Ctrl_, wstring Param_): Ctrl(Ctrl_), Param(Param_){}
	bool operator == (sCtrlParam& Param_)
	{ 
		return Ctrl == Param_.Ctrl && _tcscmp(Param.c_str(), Param_.Param.c_str()) == 0;
	}
};

struct sFindCtrl
	: public unary_function<wstring, bool>
{
	wstring Param;
	sFindCtrl(wstring Param_): Param(Param_){}
	bool operator ()(sCtrlParam CtrlParam){ return CtrlParam.Param == Param; }	
};


struct sFindCategory
	: public unary_function<enCatID, bool>
{
	enCatID Param_;
	sFindCategory(enCatID Param): Param_(Param){}
	bool operator ()(enCatID Param){ return Param == Param_; }	
};

struct sFindCtrlParam
	: public unary_function<sCtrlParam, bool>
{
	sCtrlParam Param_;
	sFindCtrlParam(sCtrlParam Param): Param_(Param){}
	bool operator ()(sCtrlParam Param){ return Param == Param_; }	
};
/*
struct sSumCat
{
	int _Val;
	sSumCat(int& Val): _Val(Val){}
	void operator()(enCatID){ _Val++; }
};
*/

struct sCompareName 
{
	bool operator()(const sCtrlParam& first, const sCtrlParam& second)
	{
		return first.Param < second.Param;
	}
};


typedef list<sCtrlParam>				TCtrlParamList;
typedef list<sCtrlParam>::iterator		TCtrlParamListIter;

typedef list<wstring>					TStringList;
typedef list<wstring>::iterator			TStringListIter;
//typedef map<wstring, sParam>		TParamMap;

typedef map<enCatID, TCtrlParamList>	TCategoryMap;
typedef map<enCatID, TCtrlParamList>::iterator TCategoryMapIter;

typedef list<enCatID>					TCategoryList;
typedef list<enCatID>::iterator			TCategoryListIter;

typedef map<enActionType, TCtrlParamList>	TActionsMap;
typedef map<enActionType, TCtrlParamList>::iterator TActionsMapIter;

