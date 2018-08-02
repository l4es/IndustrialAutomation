#pragma once
/********************************************************************
	Создан:	        2005/02/09
	Создан:	        9.2.2005   10:19
	Путь к файлу: 	c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\Primitives\ManipulatorVertex.h
	Директория:	    c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\Primitives
	Имя файла:	    ManipulatorVertex
	Расширение:	    h
	Автор(ы):	    Алексей
    Класс(ы):       CManipulatorVertex
	
Описание:	Класс предназначен для добавления и перемещения вершин полилинии
			с помощью мыши
*********************************************************************/

#include "manipulator.h"

namespace Primitives
{

	class CManipulatorVertex :
		public CManipulator
	{
	public:
		CManipulatorVertex(CObject* Obj);
		~CManipulatorVertex(void);
		//Рисовние манипулятора
		virtual void Draw(HDC dc);
		//Обработчики сообщений
		virtual BOOL OnMouseLButtonDown(sVector Point);
		virtual BOOL OnMouseLButtonUp(sVector Point);
		virtual BOOL OnMouseMove(sVector Point);
		virtual BOOL OnMouseRButtonDown(sVector Point);
		//Проверка попадает ли вектор point на одну из вершин или линию
		virtual bool HitTest(sVector point);
		//проверка попадает ли вектор point на лингию
		bool HitLine(sVector point, vector< sVector >::iterator& vertex);

	protected:
		sVector _BrickSize;
		CBrush	_Brush;
		CPen	_Pen;
		sVector	_LastPosition;
		bool	_MLButtonDown;
		vector< sVector >::iterator _CurrentVertex;//Итератор, указывающий на 
													//текущую вершину
	};
 
}