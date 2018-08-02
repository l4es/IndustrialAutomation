#pragma once
/********************************************************************
	Создан:	        2005/02/08
	Создан:	        8.2.2005   13:46
	Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Visualizator\Input.h
	Директория:	    d:\Work\Projects\М-Немо\Source\Visualizator
	Имя файла:	    Input
	Расширение:	    h
	Автор(ы):	    Михаил
    Класс(ы):       CInput
	
Описание:	Перегружается базовый класс CObjInput для обработки
сообщения нажатия на кнопку(кнопка контрола)
*********************************************************************/

#include "InputBox.h"

namespace Primitives
{
	class CInput
		: public CObjInput
	{
	public:
		CInput(){};
		~CInput(){};

		//Сообщения нажатия на кнопку
		virtual BOOL OnBtnDown()
		{
			//Получить значение переменной
			CVariableMenagerPtr vm;
			CComVariant val = vm->GetVariable(_VariableName);

			//Преобразуем значение к строковому виду
			CComVariant tmp = val;
			tmp.ChangeType(VT_BSTR);

			//Открыть диалог ввода
			CInputBox dlg(_VariableName, L"Значение переменной", CString(tmp.bstrVal).GetBuffer());
			if(dlg.DoModal() == IDOK)
			{
				tmp = CComBSTR(dlg.GetValAsText().c_str());
				
				//Пытаемся преобразовать значение из строки к исходному типу
				if(FAILED(tmp.ChangeType(val.vt)))
				{
					//Возможно при вводе фещественного числа использовалась '.' вместо ','
					try
					{
						tmp = (double)_wtof(dlg.GetValAsText().c_str());
						if(FAILED(tmp.ChangeType(val.vt)))
							return TRUE;
					}
					catch(...){};
				}
				//Записываем новое значение.
				CVariableMenagerPtr vm;
				vm->SetVariable(_VariableName, tmp, true);
			}
			return TRUE;
		};
	};
}