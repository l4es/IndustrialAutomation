#pragma once
#include <string>
#include "Global.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define INITIALIZE_CLSID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	const CLSID name = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

namespace HelpersLib
{

	HELPERS_LIB_API std::wstring ExtractFilePath(std::wstring FileName);

	//Флаги для проверки операционной системы (поддерживается или нет)
	const DWORD OS_DENY_W311		= 1;
	const DWORD OS_DENY_W9X			= 1<<1;
	const DWORD OS_DENY_NT4			= 1<<2;
	const DWORD OS_UNSUPPORT_W311	= 1<<16;
	const DWORD OS_UNSUPPORT_W9X	= 1<<17;
	const DWORD OS_UNSUPPORT_NT4	= 1<<18;
	const DWORD OS_UNSUPPORT_W2K	= 1<<19;

	//Функция тестирует поддерживается ли данная версия ОС
	// выдает соответствующее сообщение
	HELPERS_LIB_API BOOL TestAcceptingOS(DWORD OS);

	//Функция регистрирует программу-обработчик для заданного типа файла
	HELPERS_LIB_API void RegistryFileExt(std::wstring Ext, std::wstring DocID, std::wstring Cmd, std::wstring Icon, std::wstring Type=L"open");
}
