#pragma once
#include "object.h"

//Класс для редактирования группы выбранных объектов (2 и более)
//Выбранные объекты перемещаются из CObjectMenager в CSelection при
//при добавлении объектов в CSelection (из CObjectMenager они удаляются)
//После редактирования объекты перемещаются обратно в CObjectMenager
class CSelection :
	public CObject
{
	friend class CObjectMenager;
public:
	enum enPosition {POS_FRONT, POS_BACK, POS_OLD}; //позиция объекте в списке :
	//POS_FRONT-первая, POS_BACK-последняя, POS_OLD-без изменения 
	CSelection(void);
	~CSelection(void);

	virtual void Draw(HDC dc); //Рисование CSelection и объектов входящих в него

	virtual void AddObject(CObject* obj); //Добавление объекта в Selection
	inline void RemoveObject(CObject* obj){_Objects.remove(obj);} //Удаление объекта из Selection

	void ClearObject(); //Очистка списка _Object
	bool IsEmpty(){ return _Objects.empty(); }
	void RelateToAbsolute(CObject*); //Перевод координат объектов из абсолютных в относительные
	void AbsoluteToRelate(CObject*); //Перевод координат объектов из относительных в абсолютные
	void Convert(CObject* obj);		//Перевод размера и позиции объекта в абсолютные координаты
									//с учетом смещения объекта, вызванного поворотом CSelection
	
	void CalcSizeEndPos();			//Расчет размеров и позиции CSelection
	TStrings GetObjects();			//Получить список всех объектов
	CObject* FindObject(wstring Name); //Найти объект по имени

	void SetOrderPos(enPosition Pos){ _OrderPos = Pos; } //Установить порядковую позицию объекта в списке

	

protected:
	sVector	_LastPosition;
	bool	_MLButtonDown;
	list<CObject*>	_Objects;

	CPen	_Pen;
	enPosition _OrderPos;


};
