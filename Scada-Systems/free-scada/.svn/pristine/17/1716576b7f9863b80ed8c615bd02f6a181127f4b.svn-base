#pragma once
/********************************************************************
        Создан:         2005/02/09
        Создан:         9.2.2005   12:58
        Путь к файлу:   c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\Primitives\Rectangle.h
        Директория:         c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\Primitives
        Имя файла:          Rectangle
        Расширение:         h
        Автор(ы):           Alex Dovgan
    Класс(ы):       CGraph

Описание:       Класс реализующий график в дизайнере
*********************************************************************/

#include "../PropertyWindow/ObjectPropertyWindow.h"
#include "BaseObject.h"

namespace Primitives
{
        class CGraph
                : public CObjGraph
                ,public CBaseObject<CGraph>
        {
        public:
                CGraph(void);
                ~CGraph(void);

                //Обработчик измененния объекта
                virtual void OnEdit();
                //Рисование прямоугольника
                virtual void Draw(HDC dc_);
                //Обработчики сообщений
                virtual void OnSetFocus();
                virtual void OnLostFocus();
                virtual BOOL OnMouseLButtonDown(sVector Point);
                virtual BOOL OnMouseRButtonDown(sVector Point);
                virtual BOOL OnMouseLButtonDblClk(sVector Point);
                virtual BOOL OnMouseLButtonUp(sVector Point);
                virtual BOOL OnMouseMove(sVector Point);
                virtual BOOL OnKeyDown(WPARAM wParam, LPARAM lParam);

                //Проверка попадает ли вектор point на объект
                virtual bool HitTest(sVector point);
                //Получить параметр с названием Name категории Category
                virtual bool GetParamValue(enCatID Category, wstring Name, sParam& Param);
                //Установить параметр с названием Name категории Category
                virtual bool SetParamValue(enCatID Category, wstring Name, sParam Param);

        protected:

        };
}
