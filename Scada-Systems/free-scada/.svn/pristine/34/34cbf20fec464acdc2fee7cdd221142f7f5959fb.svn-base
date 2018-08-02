#include "tut.h"
#include <algorithm>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#define WIN32_LEAN_AND_MEAN
#include <atlpath.h>
#include <Windows.h>

#include "../HelpersLib.h"

namespace tut
{
	struct CCfgXmlInit
	{
		hl::tstring m_filename;

		CCfgXmlInit()
		{
			ATL::CPath filename = HelpersLib::GetTempPath().c_str();
			filename.Append(_T("xml_config_test.xml"));
			m_filename = (LPCTSTR)filename;
		};
		~CCfgXmlInit()
		{
			delete_test_file();
		};

		hl::tstring generate_filename(size_t file_num)
		{
			ATL::CPath filename = HelpersLib::GetTempPath().c_str();
			hl::tstringstream fmt;
			fmt<<_T("xml_config_test_")<<file_num<<_T(".xml");
			filename.Append(fmt.str().c_str());
			return (LPCTSTR)filename;
		}
		void delete_test_file()
		{
			DeleteFile(m_filename.c_str());
		}

		void delete_file(const hl::tstring& filename)
		{
			DeleteFile(filename.c_str());
		}

	};

	typedef test_group<CCfgXmlInit> factory;
	typedef factory::object object;
}

namespace
{
	tut::factory tf("CCfgXml");
}

#define ENSURE(cond) {char tmp[0x0FFF];sprintf(tmp,"%s(%u):%s",__FILE__,__LINE__, #cond );ensure(tmp, cond);}

namespace tut
{
	class custom_handler_t: public HelpersLib::CCfgHandlerBase
	{
	public:
		typedef std::list<hl::tstring> tstrings_t;

		virtual boost::any Load(void* node) const
		{
			tstrings_t res;

			std::vector<void*> nodes = FindNodes(node, _T("item"));
			for(size_t i=0;i<nodes.size();i++)
				res.push_back(GetNodeContent(nodes[i]));

			hl::tstringstream fmt;
			fmt<<res.size();
			ENSURE(GetProperty(node, _T("count")) == fmt.str());
			ENSURE(GetAttribute(node, _T("item_count")) == fmt.str());

			boost::any val = GetAnyProperty(node, _T("count_as_int"), _T("int"));
			ENSURE(boost::any_cast<int>(val) == (int)res.size());
			return res;
		}
		void Save(const boost::any& val, void* node) const
		{
			tstrings_t strings = boost::any_cast<tstrings_t>(val);
			for(tstrings_t::const_iterator i=strings.begin();i!=strings.end();i++)
				AddProperty(node, _T("item"), *i);

			hl::tstringstream fmt;
			fmt<<strings.size();
			SetProperty(node, _T("count"), fmt.str());
			SetAttribute(node, _T("item_count"), fmt.str());

			SetAnyProperty(node, _T("count_as_int"), (int)strings.size());
		}
		hl::tstring GetTypeName() const
		{
			return _T("CustomType");
		}
		const std::type_info& GetType() const
		{
			return typeid(tstrings_t);
		};
	};

	using namespace HelpersLib;

	//Integer types
	template<>
	template<>
	void object::test<1>()
	{
		CCfgXml cfg1;
		cfg1.SetValue(_T("val1_str"),1);
		cfg1.SetValue(_T("group1.val1_str"),2l);
		cfg1.SetValue(_T("group2.val1_str"),3ul);
		cfg1.SetValue(_T("group1.val2_str"),4u);
		cfg1.Save(m_filename);

		CCfgXml cfg2;
		cfg2.Load(m_filename);

		ENSURE(cfg2.HasValue(_T("val1_str")));
		ENSURE(cfg2.HasValue(_T("group1.val1_str")));
		ENSURE(cfg2.HasValue(_T("group2.val1_str")));
		ENSURE(cfg2.HasValue(_T("group1.val2_str")));

		ENSURE(cfg2.GetValue<int>(_T("val1_str"))					== 1);
		ENSURE(cfg2.GetValue<long>(_T("group1.val1_str"))			== 2l);
		ENSURE(cfg2.GetValue<unsigned long>(_T("group2.val1_str"))	== 3ul);
		ENSURE(cfg2.GetValue<unsigned int>(_T("group1.val2_str"))	== 4u);
		delete_test_file();
	}

	//Real types
	template<>
	template<>
	void object::test<2>()
	{
		CCfgXml cfg1;
		cfg1.SetValue(_T("group1.val1"),123.456f);
		cfg1.SetValue(_T("group1.val2"),-123.456);
		cfg1.Save(m_filename);

		CCfgXml cfg2;
		cfg2.Load(m_filename);

		ENSURE(cfg2.HasValue(_T("group1.val1")));
		ENSURE(cfg2.HasValue(_T("group1.val2")));

		ENSURE(cfg2.GetValue<float>(_T("group1.val1"))		== 123.456f);
		ENSURE(cfg2.GetValue<double>(_T("group1.val2"))	== -123.456);
		delete_test_file();
	}

	//String type
	template<>
	template<>
	void object::test<3>()
	{
		CCfgXml cfg1;
		cfg1.SetValue(_T("group1.val1"),tstring(_T("-123.456")));
		cfg1.Save(m_filename);

		CCfgXml cfg2;
		cfg2.Load(m_filename);

		ENSURE(cfg2.HasValue(_T("group1.val1")));

		ENSURE(cfg2.GetValue<tstring>(_T("group1.val1")) == tstring(_T("-123.456")));
		delete_test_file();
	}

	//Invalid type
	template<>
	template<>
	void object::test<4>()
	{
		CCfgXml cfg1;
		cfg1.SetValue(_T("group1.val1"),boost::any());
		cfg1.Save(m_filename);

		CCfgXml cfg2;
		cfg2.Load(m_filename);

		ENSURE(cfg2.HasValue(_T("group1.val1")) == false);
		delete_test_file();
	}

	//Custom type
	template<>
	template<>
	void object::test<5>()
	{
		custom_handler_t::tstrings_t strings;
		strings.push_back(_T("str1"));
		strings.push_back(_T("str2"));
		strings.push_back(_T("str3"));
		strings.push_back(_T("str4"));

		RegisterCfgHandler(CCfgHandlerBasePtr(new custom_handler_t()));
		CCfgXml cfg1;
		cfg1.SetValue(_T("val1"),strings);
		cfg1.Save(m_filename);

		CCfgXml cfg2;
		cfg2.Load(m_filename);

		ENSURE(cfg2.HasValue(_T("val1")) == true);

		custom_handler_t::tstrings_t loaded_strings;
		loaded_strings = cfg2.GetValue<custom_handler_t::tstrings_t>(_T("val1"));

		bool is_equal = std::equal(strings.begin(),strings.end(),loaded_strings.begin());
		ENSURE(is_equal == true);
		delete_test_file();
	}

	//multithreading
	void thread_func(tstring filename)
	{
		custom_handler_t::tstrings_t strings;
		strings.push_back(_T("str1"));
		strings.push_back(_T("str2"));
		strings.push_back(_T("str3"));
		strings.push_back(_T("str4"));

		RegisterCfgHandler(CCfgHandlerBasePtr(new custom_handler_t()));
		CCfgXml cfg1;
		cfg1.SetValue(_T("val1"),strings);
		cfg1.SetValue(_T("group2.val1_str"),3ul);
		cfg1.SetValue(_T("group1.val2"),-123.456);
		cfg1.Save(filename);

		CCfgXml cfg2;
		cfg2.Load(filename);

		ENSURE(cfg2.HasValue(_T("val1")) == true);
		ENSURE(cfg2.HasValue(_T("group2.val1_str")) == true);
		ENSURE(cfg2.HasValue(_T("group1.val2")) == true);

		custom_handler_t::tstrings_t loaded_strings;
		loaded_strings = cfg2.GetValue<custom_handler_t::tstrings_t>(_T("val1"));

		bool is_equal = std::equal(strings.begin(),strings.end(),loaded_strings.begin());
		ENSURE(is_equal == true);

		ENSURE(cfg2.GetValue<unsigned long>(_T("group2.val1_str"))	== 3ul);
		ENSURE(cfg2.GetValue<double>(_T("group1.val2"))	== -123.456);
	}

	template<>
	template<>
	void object::test<6>()
	{
		boost::thread_group threads;
		const size_t thread_count = 16;
		for (size_t i = 0; i < thread_count; ++i)
		{
			tstring filename = generate_filename(i);
			threads.create_thread(boost::bind(&thread_func,filename));
		}
		threads.join_all();

		for (size_t i = 0; i < thread_count; ++i)
			delete_file(generate_filename(i));
	}
}
