#pragma once
/********************************************************************
	Создан:	        2005/02/07
	Создан:	        7.2.2005   14:06
	Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Library\ObjInput.h
	Директория:	    d:\Work\Projects\М-Немо\Source\Library
	Имя файла:	    ObjInput
	Расширение:	    h
	Автор(ы):	    Алексей, Михаил
    Класс(ы):       CObjInput
	
	  Объект "Окно ввода". Связывается с какой-либо переменной. При нажатии
	пользователем на кнопку (в правой части контролла) генерируется сообщение
	OnBtnDown, которое может быть обработано в наследниках.
*********************************************************************/

#include "object.h"

class CObjInput
	: public CObject
{
public:
	CObjInput(void);
	~CObjInput(void);

	//Прорисовка объекта
	virtual void Draw(HDC dc);

	//Сохранение/загрузка 
	virtual void Serialize(CMemFile& file, BOOL IsLoading=TRUE);
	
	//Установить контрольную переменную
	inline void SetControlVariable(wstring Var){_VariableName = Var;};
	
	inline wstring GetControlVariable(){return _VariableName;};

	//Установить формат вывода значения переменной 
	//(формат задается по принципу printf. Например: %d, %.3f)
	inline void SetFormat(wstring Format){ _Format = Format; }
	//Получить текущее значение фориата
	inline wstring GetFormat(){ return _Format; }

	//Установить цвет текста
	inline void SetTextColor(sRGB Color){SetColor(Color);};
	//Получить цвет текста
	inline sRGB GetTextColor(){return GetColor();};

	//Установить размер(высоту) шрифта
	inline int GetHeight(){ return _Font.Height; }
	//Получить размер(высоту) шрифта
	inline void SetHeight(int Height){ _Font.Height = Height;}

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

	//Установить сразу все параметры шрифта
	void SetFont(sFont Font){_Font = Font;};
	//Получить ссылку на структуру, описывающую используемый шрифт
	sFont& GetFont(){return _Font;};

	//Сообщения
	virtual BOOL OnMouseLButtonUp(sVector Point);
	virtual BOOL OnBtnDown(){return FALSE;};

protected:
	wstring	_VariableName;
	sFont	_Font;
	CRect	_ButtonRect;
	wstring	_Format;
};
