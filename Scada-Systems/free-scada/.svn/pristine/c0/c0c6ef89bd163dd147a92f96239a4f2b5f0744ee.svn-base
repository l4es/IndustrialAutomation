#include "common.h"
#include ".\objtext.h"
#include "ObjectMenager.h"
#include "Action.h"

CObjText::CObjText(void)
{
	//Генерируем уникальное имя для объекта
	SetName(GenerateUniqueObjectName(L"Text_"));
	SetTypeID(_TEXT);
	//Устанавливем флаги действий(Action) все возможные кроме вращения
	SetActionFlags(AT_ALL_NOROTATE);

	_Text = L"Simple Text";
	_IsBGTransparent	= false;
	_Align				= TA_LEFT;

	_Font.Name		= L"Arial";
	_Font.Height	= 14;
	_Font.Bold		= false;
	_Font.Italic	= false;
	_Font.StrikeOut	= false;
	_Font.Underline	= false;

	_IsOPC = false;
	_Format = L"%0.3f";
	_Expression = L"0";
}

CObjText::~CObjText(void)
{
}

//Рисовать объект
void CObjText::Draw(HDC dc_)
{
	if(!IsVisible())
		return;

	CDCHandle dc(dc_);

	//Получить инстанцию менеджера объектов
	CObjectMenagerPtr mng;
	CPoint pt;
	//Перевод из глобальных координат в экранные
	pt = mng->GlobalToScreen(GetPosition());

	CFont font;
	//создание шрифта
	font.CreateFont(	_Font.Height,
						0,
						0,
						0,
						_Font.Bold?FW_BOLD:FW_NORMAL,
						_Font.Italic?TRUE:FALSE,
						_Font.Underline?TRUE:FALSE,
						_Font.StrikeOut?TRUE:FALSE,
						DEFAULT_CHARSET,
						OUT_DEFAULT_PRECIS,
						CLIP_DEFAULT_PRECIS,
						DEFAULT_QUALITY,
						DEFAULT_PITCH,
						_Font.Name.c_str());
	if(font.IsNull())
		return;

	//Устанавливаем прозрачный фон или нет
	dc.SetBkMode(_IsBGTransparent?TRANSPARENT:OPAQUE);
	//Устанавливаем цвет фона
	dc.SetBkColor(_BGColor.AsCOLORREF());
	//Устанавливаем цвет текста
	dc.SetTextColor(GetTextColor().AsCOLORREF());

	//Выбираем шрифт в контекст устройства
	CFontHandle OldFont = dc.SelectFont(font);
	//Устанавливаем флаги: воспринимать /t как табуляцию,
	//центрировать по вертикали, переносить текст на следующую строку
	//если не помещается в одну линию
	DWORD flags = DT_EXPANDTABS|DT_VCENTER|DT_WORDBREAK;
	//Устанавлваем выравнивание
	switch(_Align)
	{
	case TA_LEFT:	flags |= DT_LEFT;	break;
	case TA_CENTER:	flags |= DT_CENTER;	break;
	case TA_RIGHT:	flags |= DT_RIGHT;	break;
	}

	CRect rc(pt, CPoint(pt.x+GetSize().x, pt.y+GetSize().y));
    
	//Если фон не прозрачный создаем заливку (Brush)
	if(!_IsBGTransparent)
	{
		CBrush brush;
		brush.CreateSolidBrush(GetBGColor().AsCOLORREF());
		dc.FillRect(rc, brush);
		brush.DeleteObject();
	}

	wstring Text = _Text;
	//Если отображаем ОРС переменную, рассчитываем значение выражения, 
	//преобразуем в текст в соответствии с форматом
	if(_IsOPC)
	{
		double val = _Expression.empty() ? NULL : _Equation.CalcDouble(_Expression);
		wchar_t buffer[0xff];
		swprintf(buffer, _Format.c_str(), val);
		Text = buffer;
	}

	wstring buffer;

	//выделяем символы конца строки табуляции
	wstring::iterator it = Text.begin();
	while(it != Text.end())
	{
		if((*it) == '\\'  && (it+1) != Text.end())
		{
			if((*(it+1)) == 'n')
			{
				buffer.push_back('\n');
				it++; 	it++;
				continue;
			}
			if((*(it+1)) == 't')
			{
				buffer.push_back('\t');
				it++; 	it++;
				continue;
			}
		}


		buffer.push_back(*it);
		it++;
	}
	
	//Выводим текст на экран
	dc.DrawText(buffer.c_str(), (int)buffer.size(), &rc, flags);
	//Выбираем старый шрифт в контекст устройства
	dc.SelectFont(OldFont);
}


//Сохранение - загрузка объекта
void CObjText::Serialize(CMemFile& file, BOOL IsLoading)
{
	CObject::Serialize(file, IsLoading);

	if(IsLoading)
	{
		_Font.Serialize(file, IsLoading);

		file>>_BGColor;
		file>>_Text;
		file>>_IsBGTransparent;
		file>>_Align;
		file>>_IsOPC;
		file>>_Expression;
		file>>_Format;
	}
	else
	{
		_Font.Serialize(file, IsLoading);

		file<<_BGColor;
		file<<_Text;
		file<<_IsBGTransparent;
		file<<_Align;
		file<<_IsOPC;
		file<<_Expression;
		file<<_Format;
	}
}