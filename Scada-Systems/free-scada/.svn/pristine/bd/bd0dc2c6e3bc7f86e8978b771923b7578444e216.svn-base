#pragma once
/********************************************************************
	Создан:	        2005/02/07
	Создан:	        7.2.2005   15:48
	Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Library\ObjButton.h
	Директория:	    d:\Work\Projects\М-Немо\Source\Library
	Имя файла:	    ObjButton
	Расширение:	    h
	Автор(ы):	    Алексей, Михаил
    Класс(ы):       CObjButton
	
	Объект "Button". Связывается с какой-либо переменной. При нажатии
	пользователем на кнопку переменная меняет свое значение на противоположное.
*********************************************************************/

#include "object.h"

class CObjButton
	: public CObject
{
public:
	CObjButton(void);
	~CObjButton(void);

	//Прорисовка объекта
	virtual void Draw(HDC dc);

	//Сохранение/загрузка 
	virtual void Serialize(CMemFile& file, BOOL IsLoading=TRUE);

	//Задать заголовок кнопки
	inline void SetCaption(wstring Caption){_Caption = Caption;};
	inline wstring GetCaption(){return _Caption;};

	//Установить контрольную переменную
	inline void SetControlVariable(wstring Var){_VariableName = Var;};
	//Получить имя контрольной переменной
	inline wstring GetControlVariable(){return _VariableName;};

	//Установить цвет текста
	inline void SetTextColor(sRGB Color){_TextColor = Color;};
	//Получить цвет текста
	inline sRGB GetTextColor(){return _TextColor;};

	//Установить размер(высоту) шрифта
	inline void SetHeight(int Height){ _Font.Height = Height;}
	//Получить размер(высоту) шрифта
	inline int GetHeight(){ return _Font.Height; }

	//Установить атрибуты шрифта (Жирный, Курсив, Перечеркнутый, Подчеркнутый)
	inline void SetBold(bool Bold) { _Font.Bold = Bold; }
	inline void SetItalic(bool Italic) {_Font.Italic	= Italic; }
	inline void SetStrikeOut(bool StrikeOut) { _Font.StrikeOut	= StrikeOut; }
	inline void SetUnderline(bool Underline) {_Font.Underline	= Underline; }

	//Получить атрибуты шрифта (Жирный, Курсив, Перечеркнутый, Подчеркнутый)
	inline bool GetBold() { return _Font.Bold; }
	inline bool GetItalic() { return _Font.Italic; }
	inline bool GetStrikeOut() { return _Font.StrikeOut; }
	inline bool GetUnderline() { return _Font.Underline; }

	//Установить имя шрифта (Например: Arial, Times New Roman)
	void SetFontName(wstring FontName){_Font.Name = FontName;};
	//Получить имя шрифта
	wstring GetFontName(){return _Font.Name;};

	//Сообщения
	virtual BOOL OnMouseLButtonDown(sVector Point);

protected:
	bool		_IsDown;
	wstring		_VariableName;
	wstring		_Caption;
	sFont		_Font;
	sRGB		_TextColor;
};
