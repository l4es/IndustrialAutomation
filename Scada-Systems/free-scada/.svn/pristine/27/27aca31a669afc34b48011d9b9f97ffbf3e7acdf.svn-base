#pragma once

/********************************************************************
Создан:	        2005/2/2005
Создан:	        8.12.2004   9:57
Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Library\ObjText.h
Директория:	    d:\Work\Projects\М-Немо\Source\Library
Имя файла:	    ObjText
Расширение:	    h
Автор(ы):	    Алексей, Михаил
Класс(ы):       CObjText

Описание:	Класс реализующий отображение текстового значения.
			Текстовое значение может быть как задано пользователем, 
			так и считано с ОРС сервера
*********************************************************************/

#include "Object.h"
#include "Equation.h"



class CObjText
	: public CObject
{
public:
	CObjText(void);
	~CObjText(void);
	//Рисовать объект
	virtual void Draw(HDC dc_);
	//Установить строку отображаемого текста
	inline void SetText(wstring Text){_Text = Text;};
	//Получить строку отображаемого текста
	inline wstring GetText(){return _Text;};
	//Установить цвет текста
	inline void SetTextColor(sRGB Color){SetColor(Color);};
	//Получить цвет текста
	inline sRGB GetTextColor(){return GetColor();};
	//Установить выравнивание текста
	inline void SetTextAlign(DWORD Align){_Align = Align;};
	//Получить выравнивание текста
	inline DWORD GetTextAlign(){return _Align;};
	//Установить цвет фона
	inline void SetBGColor(sRGB BackGrdColor){ _BGColor = BackGrdColor; };
	//Получить цвет фона
	inline sRGB GetBGColor(){ return _BGColor; };
	//Установить прозрачность: Transparent = true - прозрачный
    inline void SetBGTransparent(bool Transparent){ _IsBGTransparent = Transparent; };
	//Определить прозрачный ли фон
	inline bool IsBGTransparent(){ return _IsBGTransparent; };

	//Получить шрифт
	inline sFont& GetFont(){return _Font;};
	//Установить шрифт
	inline void SetFont(sFont Font){_Font = Font;};
	//Определить является ли текстовая переменная считываемой с OPC-сервера
	inline bool IsOPC(){ return _IsOPC; }
	//Установить значение текстовой переменной считываемым с ОРС-сервера
	inline void SetOPC(bool OPC = true){ _IsOPC = OPC; }
	//Получить выражение для вычисления значения текстовой переменной при
	//считывании ее с ОРС сервера
	inline wstring GetExpression(){ return _Expression; }
	//Установить выражение для вычисления значения текстовой переменной
	inline void SetExpression(wstring Expression){ _Expression = Expression; }
	//Получить формат отображения текстовой переменной при конветировании ее из 
	//числового значения в текст (при считывании с ОРС сервера)
	inline wstring GetFormat(){ return _Format; }
	//Установить формат отображения текстовой переменной при конветировании ее из 
	//числового значения в текст (при считывании с ОРС сервера)
	inline void SetFormat(wstring Format){ _Format = Format; }
	//Получить высоту текста
	inline int GetHeight(){ return _Font.Height; }
	//Установить высоту текста
	inline void SetHeight(int Height)
	{ 
		_Font.Height = Height;
		if( Height > GetSize().y) 
			SetSize(GetSize().x, (float)Height);
	}
	//Установить начертание текста жирным
	inline void SetBold(bool Bold) { _Font.Bold = Bold; }
	//Установит начертание текста наклонным
	inline void SetItalic(bool Italic) {_Font.Italic	= Italic; }
	//Установить начертание текста зачеркнутым
	inline void SetStrikeOut(bool StrikeOut) { _Font.StrikeOut	= StrikeOut; }
	//Установить начертание текста подчеркнутым
	inline void SetUnderline(bool Underline) {_Font.Underline	= Underline; }

	//Определить является ли начертание текста жирным
	inline bool GetBold() { return _Font.Bold; }
	//Определить является ли начертание текста наклонным
	inline bool GetItalic() { return _Font.Italic; }
	//Определить является ли начертание текста зачеркнутым
	inline bool GetStrikeOut() { return _Font.StrikeOut; }
	//Определить является ли начертание текста подчеркнутым
	inline bool GetUnderline() { return _Font.Underline; }

	//Получить наименование шрифта
	wstring GetFontName(){return _Font.Name;};
	//Установить наименование шрифта
	void SetFontName(wstring FontName){_Font.Name = FontName;};
	//Сохранение-загрузка 
	void Serialize(CMemFile& file, BOOL IsLoading = TRUE);

protected:
	sFont		_Font;
	sRGB		_BGColor;
	wstring		_Text;
	bool		_IsBGTransparent;
	DWORD		_Align;
	bool		_IsOPC;
	wstring		_Expression;
	wstring		_Format;
	CEquation	_Equation;
};
