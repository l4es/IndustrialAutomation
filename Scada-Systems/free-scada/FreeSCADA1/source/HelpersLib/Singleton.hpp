#pragma once

#if (_ATL_VER>=0x700)
#	include <atlcore.h>
#else
#	include <atlbase.h>
#endif

/*
Пример класса singleton:

class CSome :public CSingleton<CSome>
{
protected:
	CSome(void);
	~CSome(void);
	DEF_SINGLETON(CDevice)
};

==============================================

CAutoSingletonCreator - класс для автоматического создания синглтона
Пример (в любом cpp файле):
	CAutoSingletonCreator<CSome> _AutoCreateSome;

=================================================

CSingletonPtr - smart pointer для автоматического создания и удаления синглона

Пример:
	if(SomeVar == true)
	{
		CSingletonPtr<CSome> some;	//Создается инстанция класса
		some->SomeFunc();
	}								//Освобождается инстанция класса
*/

#define DEF_SINGLETON(Class) protected: Class(const Class&); friend CSingleton<Class>;

template<class T>
class CSingleton
{
public:
	static T* Instance()
	{
		//Initialize the critical section, if needed.
		if(!_cs)
		{
			CComAutoCriticalSection *cs = new CComAutoCriticalSection(); //Always create CS
			InterlockedCompareExchangePointer((PVOID volatile*)&_cs,cs,NULL); //Assign the value, if _cs == NULL
			((CComAutoCriticalSection*)_cs)->Lock();
			if(_cs != cs) delete cs; //Check for other instances of the local CS
			((CComAutoCriticalSection*)_cs)->Unlock();
		}

		((CComAutoCriticalSection*)_cs)->Lock();
		if(!_self)
			_self = new T;

		_refCount++;
		((CComAutoCriticalSection*)_cs)->Unlock();

		return _self;
	}

	unsigned long Release()
	{
		if(_cs)
		{
			((CComAutoCriticalSection*)_cs)->Lock();

			if(_refCount == 0)
				return 0;

			_refCount--;

			if(_refCount == 0)
				delete _self;
			((CComAutoCriticalSection*)_cs)->Unlock();

			if(_refCount == 0)
			{
				delete _cs;
				_cs = NULL;
			}

			return _refCount;
		}
		else
			return 0;
	}

protected:
	CSingleton(){};
	CSingleton(const CSingleton&);
	virtual ~CSingleton(){_self = NULL;};

	static T* _self;
	static LONG _refCount;
	static ATL::CComAutoCriticalSection volatile *_cs;
};

template<class T>	T*  CSingleton<T>::_self = NULL;
template<class T>	LONG  CSingleton<T>::_refCount=0;
template<class T>	ATL::CComAutoCriticalSection volatile *CSingleton<T>::_cs = NULL;

template<class T>	
class CAutoSingletonCreator
{
private:
	T* _obj;
public:
	CAutoSingletonCreator(){_obj = T::Instance();}
	~CAutoSingletonCreator(){if(_obj){_obj->Release();}}
};

template<class T>
class CSingletonPtr
{
	T* _obj;
public:
	CSingletonPtr()
	{
		_obj = T::Instance();
	}
	~CSingletonPtr()
	{
		if(_obj)
			_obj->Release();
	}
	T* operator->()
	{
		ATLASSERT(_obj!=NULL);
		return _obj;
	}

};