#pragma once

class CMemFile;
class CManipulator;



class CObject
{
	friend class CObjectMenager;
public:
	//Constructor/Destructor
	CObject(bool AutoDelete = true);
	virtual ~CObject(void);

	//Прорисовка объекта. В качестве параметра DC 
	//на котором "рисоваться" объекту
	virtual void Draw(HDC dc) = 0;
	//Загрузка/Сохранение объекта в поток(файл)
	virtual void Serialize(CMemFile& file, BOOL IsLoading=TRUE);
	
	//Переместить объект в позицию
	void MoveTo(const sVector Dest);
	void MoveTo(const float x, const float y);
	
	//Переместить объект на сколько-то единиц (вектор)
	void MoveBy(const sVector Offset);
	void MoveBy(const float x, const float y);

	//Установить размер объекта
	void SetSize(const sVector Size);
	void SetSize(const float cx, const float cy);

	//Установить коэффициент масштабирования объекта
	//коэффициент используется только динамически 
	//(т.е. связан с каким то выражением)
	void SetScale(const sVector Scale);
	void SetScale(const float x, const float y);

	//Задать основной цвет объекта
	void SetColor(sRGB Color);
	void SetColor(BYTE r, BYTE g, BYTE b);
	void SetColor(COLORREF Color);

	//Повернуть объект в заданный угол
	inline void Rotate(float Angle){_Angle = Angle; CalculateBounds();};
	//Повернуть объект на заданный угол
	inline void RotateBy(float Angle){_Angle += Angle;if(_Angle>2*PI) _Angle -= 2*PI;if(_Angle<-2*PI) _Angle += 2*PI; CalculateBounds();};

	//Показать объект
	inline void		Show(){_Visible = true;};
	//Скрыть объект
	inline void		Hide(){_Visible = false;};
	inline void		SetVisible(bool Visible){_Visible = Visible;};
	inline sVector	GetPosition(){return _Position;}; 
	inline sVector	GetSize(){return _Size;};
	inline sVector	GetScale(){return _Scale;};
	inline sRGB		GetColor(){return _Color;};
	virtual float	GetAngle(){return _Angle;};
	inline bool		IsVisible(){return _Visible;};
	inline void		SetName(wstring Name){_Name = Name;};
	inline wstring	GetName(){return _Name;};
	inline void		SetTypeID(UINT TypeID){_TypeID = TypeID;};
	inline UINT		GetTypeID(){return _TypeID;};
	inline BYTE		GetActionFlags(){ return _ActionFlags; }
	inline void		SetActionFlags(BYTE ActionFlag){ _ActionFlags |= ActionFlag; }
	inline DWORD	GetOrderPos(){ return _OrderPos; }
	inline void		SetOrderPos(DWORD OrderPos){ _OrderPos = OrderPos; }

	//Получить верхнюю левую граничную точку объекта
	sVector GetLTBound(){CalculateBounds(); return _BoundLT; }	
	//Получить нижнюю правую граничную точку объекта
	sVector GetRBBound(){CalculateBounds(); return _BoundRB; }	
	//Попадает ли точка на объект
	virtual bool HitTest(sVector point);

	


	//Messages
	virtual BOOL OnMouseLButtonDown(sVector Point);
	virtual BOOL OnMouseRButtonDown(sVector Point);
	virtual BOOL OnMouseLButtonDblClk(sVector Point);
	virtual BOOL OnMouseLButtonUp(sVector Point);
	virtual BOOL OnMouseMove(sVector Point);
	virtual BOOL OnKeyDown(WPARAM wParam, LPARAM lParam);
	virtual void OnSetFocus();
	virtual void OnLostFocus();
//	virtual void OnEndEdit();
	virtual void OnEdit();

protected:
	void CalculateBounds(); //расчет верхней и нижней правой граничных точек объекта

	
	bool			_AutoDelete; //Вызывать ли delete в функции 
								 //CObjectMenager::RemoveObject и CObjectMenager::RemoveAll

private:
	DWORD	_TypeID;		//ID объекта (_RECTANGLE=1, _POLYLINE=2, _ELLIPSE=3) 
	wstring	_Name;			//Имя
//	bool	_Visual;		//Визуальный (прямоугольник) или невизуальный (звук)
	sRGB	_Color;			//Цвет
	sVector	_Position;		//Позиция 
	sVector	_Size;			//Размер 
	sVector	_Scale;			//Масштаб 
	bool	_Visible;		//Видимый или невидимый
	sVector	_LastPosition;	//
	bool	_MLButtonDown;	//Нажата ли левая кнопка мыши
	float	_Angle;			//Угол поворота

	sVector _BoundLT;		//Верхняя левая граничная точка объекта
	sVector _BoundRB;		//Нижняя правая граничная точка объекта
	BYTE	_ActionFlags;	//Флаг возможных действий для объекта
	DWORD	_OrderPos;		//Позиция в списке объектов
};
