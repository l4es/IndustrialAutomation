#pragma once

using namespace System;

namespace libe1803_dot_net
{
	private class StringToPointer
	{
	public:
		StringToPointer(String ^s);
		~StringToPointer();
		void Free();
		const char *pointer = NULL;
	};
}
