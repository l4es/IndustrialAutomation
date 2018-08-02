#include "common.h"
#include ".\objinput.h"

#include "ObjectMenager.h"
#include "VariableMenager.h"
#include "MemFile.h"
#include "Action.h"
#include "Graphic.h"

CObjInput::CObjInput(void)
{
	//Инициализация объекта
	_Format = L"%.3f";
	_Font.Name		= L"Arial";
	_Font.Height	= 14;
	_Font.Bold		= false;
	_Font.Italic	= false;
	_Font.StrikeOut	= false;
	_Font.Underline	= false;
	SetSize(60, 10);
	SetName(GenerateUniqueObjectName(L"Edit_"));

	SetTypeID(_INPUT);
	SetActionFlags(AT_ALL_NOROTATE);
}

CObjInput::~CObjInput(void)
{

}

void CObjInput::Draw(HDC dc_)
{
	if(!IsVisible())
		return; //Если объект невидимый, то ничего не рисуем

	CDCHandle dc(dc_);

	//Получить инстанцию менеджера объектов
	CObjectMenager* mng = CObjectMenager::Instance();
	CPoint ptBase;
	//Перевод из глобальных координат в экранные
	ptBase = mng->GlobalToScreen(GetPosition());

	//Стандартные кисти
	CBrush	brWindow			= GetSysColorBrush(COLOR_WINDOW);
	CBrush	brFace			= GetSysColorBrush(COLOR_3DFACE);

	//Формирование стандартных карандашей
	CPen	penShadowHavyDark,
			penShadowDark,
			penShadowLight,
			penShadowHighLight;
	penShadowHavyDark.CreatePen(	PS_SOLID, 1, GetSysColor(COLOR_3DDKSHADOW));
	penShadowDark.CreatePen(		PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
	penShadowLight.CreatePen(		PS_SOLID, 1, GetSysColor(COLOR_3DLIGHT));
	penShadowHighLight.CreatePen(	PS_SOLID, 1, GetSysColor(COLOR_3DHIGHLIGHT));

	CPenHandle penOld;
	CRect rcWindow;
	CRect rcButton;

	//Расчет общего размера окна
	rcWindow.left	= ptBase.x;
	rcWindow.right	= ptBase.x + GetSize().x;
	rcWindow.top	= ptBase.y;
	rcWindow.bottom	= ptBase.y + GetSize().y;

	//Прорисовка окна
	{
		CRect &rc = rcWindow;
		CPoint pt = ptBase;

		//Закрашиваем фон
		dc.FillRect(rc, brWindow);

		//Темная линия: верх-лево (выше)
		penOld = dc.SelectPen(penShadowDark);
		dc.MoveTo(pt.x + GetSize().x, pt.y);
		dc.LineTo(pt);
		dc.LineTo(pt.x, pt.y + GetSize().y);
		dc.SelectPen(penOld);

		//Очень темная линия: верх-лево (ниже)
		penOld = dc.SelectPen(penShadowHavyDark);
		dc.MoveTo(pt.x + GetSize().x-1, pt.y+1);
		dc.LineTo(pt.x+1,pt.y+1);
		dc.LineTo(pt.x+1, pt.y + GetSize().y -1);
		dc.SelectPen(penOld);

		//Очень светлая линия: низ-право (выше)
		penOld = dc.SelectPen(penShadowHighLight);
		dc.MoveTo(pt.x + GetSize().x, pt.y);
		dc.LineTo(pt.x + GetSize().x, pt.y + GetSize().y);
		dc.LineTo(pt.x-1, pt.y + GetSize().y);
		dc.SelectPen(penOld);

		//Светлая линия: низ-право (ниже)
		penOld = dc.SelectPen(penShadowLight);
		dc.MoveTo(pt.x + GetSize().x-1, pt.y+1);
		dc.LineTo(pt.x + GetSize().x-1, pt.y + GetSize().y-1);
		dc.LineTo(pt.x, pt.y + GetSize().y-1);
		dc.SelectPen(penOld);

		//Дорисовываем последний пиксель
		dc.SetPixel(pt.x + GetSize().x-1, pt.y+1, GetSysColor(COLOR_3DDKSHADOW));
	}

	//Прорисовка кнопки
	{
		//Расчитывается размер кнопки 
		CPoint pt;
		CRect &rc = rcButton;
		rc = rcWindow;
		rc.top += 2;
		rc.bottom -= 1;
		rc.right -= 1;
		
		//ширина кнопки не может быть больше половины ширины окна
		if(rc.Height() < rc.Width()/2)
			rc.left = rc.right - rc.Height();
		else
			rc.left = rc.right - rc.Width()/2;

		//_ButtonRect - требуется для проверки попал ли 
		//клик в область кнопки
		_ButtonRect = rc;

		pt.x = rc.left;
		pt.y = rc.top;

		//Рисуем фон
		dc.FillRect(rc, brFace);

		//Светлая линия: верх-лево (ниже)
		penOld = dc.SelectPen(penShadowLight);
		dc.MoveTo(pt.x + rc.Width(), pt.y);
		dc.LineTo(pt.x, pt.y);
		dc.LineTo(pt.x, pt.y + rc.Height());
		dc.SelectPen(penOld);

		//Очень светлая линия: верх-лево (выше)
		penOld = dc.SelectPen(penShadowHighLight);
		dc.MoveTo(pt.x + rc.Width()-1, pt.y+1);
		dc.LineTo(pt.x+1, pt.y+1);
		dc.LineTo(pt.x+1, pt.y + rc.Height()-1);
		dc.SelectPen(penOld);

		//Очень темная линия: низ-право (ниже)
		penOld = dc.SelectPen(penShadowHavyDark);
		dc.MoveTo(pt.x + rc.Width(), pt.y);
		dc.LineTo(pt.x + rc.Width(), pt.y + rc.Height());
		dc.LineTo(pt.x-1, pt.y + rc.Height());
		dc.SelectPen(penOld);

		//Темная линия: низ-право (выше)
		penOld = dc.SelectPen(penShadowDark);
		dc.MoveTo(pt.x + rc.Width()-1, pt.y+1);
		dc.LineTo(pt.x + rc.Width()-1, pt.y + rc.Height()-1);
		dc.LineTo(pt.x, pt.y + rc.Height()-1);
		dc.SelectPen(penOld);
	}
	
	//Вывод текста
	{
		//Создаем шрифт для поля ввода
		CFont font;
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

		//Создаем шрифт для кнопки
		CFont btn_font;
		btn_font.CreateFont(	_Font.Height,
								0,
								0,
								0,
								FW_BOLD,
								FALSE,
								FALSE,
								FALSE,
								DEFAULT_CHARSET,
								OUT_DEFAULT_PRECIS,
								CLIP_DEFAULT_PRECIS,
								DEFAULT_QUALITY,
								DEFAULT_PITCH,
								_Font.Name.c_str());
		if(btn_font.IsNull())
			return;

		//Отступы для текста
		const int MariginLeft = 5;
		const int MariginTop = 3;

		//Расчитываем область вывода текста
		CRect rcText = rcWindow;
		rcText.right -= rcButton.Width();
		rcText.left += MariginLeft;
		rcText.top += MariginTop;

		wstring Text;
		if(_VariableName.empty())
			Text = L"No data";
		else
		{
			//Получаем значение переменной
			CVariableMenagerPtr vm;
			CComVariant val = vm->GetVariable(_VariableName);

			//Вывод на DC с учетом формата
			val.ChangeType(VT_R8);
			wchar_t buffer[0xff];
			swprintf(buffer, _Format.c_str(), val.dblVal);
			Text = buffer;
		}

		//Выводим текст для окна
		CFontHandle OldFont = dc.SelectFont(font);
		int bkmOld = dc.SetBkMode(TRANSPARENT);
		COLORREF clOld = dc.SetTextColor(GetColor().AsCOLORREF());
		dc.DrawText(Text.c_str(), -1, &rcText, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
		dc.SetTextColor(clOld);
		dc.SelectFont( OldFont );

		//Выводим текст кнопки
		OldFont = dc.SelectFont( btn_font );
		rcText = rcButton;
		dc.DrawText(_T("..."), -1, &rcText, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		dc.SelectFont( OldFont );
		dc.SetBkMode(bkmOld);
	}
}


void CObjInput::Serialize(CMemFile& file, BOOL IsLoading)
{
	CObject::Serialize(file, IsLoading);
	if(IsLoading)
	{
		//Это загрузка
		file>>_VariableName;
		file>>_Format;
		
		_Font.Serialize(file, IsLoading);
	}
	else
	{
		//Это сохранение
		file<<_VariableName;
		file<<_Format;

		_Font.Serialize(file, IsLoading);
	}
}

BOOL CObjInput::OnMouseLButtonUp(sVector Point)
{
	//Проверяем не была ли нажата кнопка
	//если нажата, генерируется сообщение OnBtnDown()
	CGraphic graph(NULL);
	TRect rc;
	CObjectMenagerPtr om;
	rc[0] = om->ScreenToGlobal(CPoint(_ButtonRect.left, _ButtonRect.top));
	rc[1] = om->ScreenToGlobal(CPoint(_ButtonRect.right, _ButtonRect.top));
	rc[2] = om->ScreenToGlobal(CPoint(_ButtonRect.right, _ButtonRect.bottom));
	rc[3] = om->ScreenToGlobal(CPoint(_ButtonRect.left, _ButtonRect.bottom));

	if(graph.TestPtInRect(Point, rc))
	{
		if(OnBtnDown())
			return TRUE;
	}
	return CObject::OnMouseLButtonUp(Point);
}