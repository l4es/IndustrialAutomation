#pragma once

#include <assert.h>

#pragma warning(push,3) //Disable unreasonable warnings
#include <boost/spirit/core.hpp>
#include <boost/spirit/utility/confix.hpp>
#include <boost/spirit/utility/escape_char.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>
#pragma warning(pop)

using namespace boost::spirit;
using namespace boost::algorithm;

#include "ScriptParser.h"

namespace ScriptParser
{
	class fAssignString
	{
	public:
		explicit
			fAssignString(std::wstring& ref_,bool ToLower=false)
			: ref(ref_),_ToLower(ToLower) {}

			template <typename T2>
				void operator()(T2 const& val) const
			{ ref = val; }

			template<typename IteratorT>
				void operator()(IteratorT& first, IteratorT& last) const
			{ 
				ref = std::wstring(first,last);
				if(_ToLower)
					boost::algorithm::to_lower(ref);
			}

	private:
		std::wstring& ref;
		bool _ToLower;
	};
}