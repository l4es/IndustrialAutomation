#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "StringToPointer.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace libe1803_dot_net
{

	StringToPointer::StringToPointer(String ^s)
	{
		IntPtr^ ip = Marshal::StringToHGlobalAnsi(s);
		if (ip != IntPtr::Zero)
			pointer = reinterpret_cast<const char *>(ip->ToPointer());
	}
	StringToPointer::~StringToPointer()
	{
		Free();
	}
	void StringToPointer::Free()
	{
		if (pointer != NULL)
		{
			IntPtr ip = IntPtr((void *)pointer);
			Marshal::FreeHGlobal(ip);
			pointer = NULL;
		}
	}

}