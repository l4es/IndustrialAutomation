#include "StdAfx.h"
#include "CfgXml.h"
#include "StrConv.h"

#include <atlstr.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <list>

namespace HelpersLib{

	namespace _internal
	{
		template<typename T>
		class generic_handler_t: public CCfgHandlerBase
		{
			const tstring m_type_name;
		public:
			generic_handler_t(tstring type_name):m_type_name(type_name){};
			boost::any	Load(void* node) const
			{
				try
				{
					return boost::lexical_cast<T>(GetNodeContent(node));	
				}
				catch(boost::bad_lexical_cast&)
				{
					return boost::any();
				}
			}
			void Save(const boost::any& val, void* node) const
			{
				tstringstream fmt;
				try
				{
					fmt<<boost::any_cast<T>(val);
				}catch(const boost::bad_any_cast&)
				{
					return;
				}
				
				SetNodeContent(node, fmt.str());
			}
			tstring GetTypeName() const
			{
				return m_type_name;
			}
			const std::type_info& GetType() const
			{
				return typeid(T);
			};
		};
	};
//////////////////////////////////////////////////////////////////////////
	typedef std::list<CCfgHandlerBasePtr> handlers_t;
	handlers_t g_cfg_handlers;

	void InitializeStandardCfgHandlers()
	{
		RegisterCfgHandler( CCfgHandlerBasePtr(new _internal::generic_handler_t<int>(_T("int"))) );
		RegisterCfgHandler( CCfgHandlerBasePtr(new _internal::generic_handler_t<long>(_T("long"))) );
		RegisterCfgHandler( CCfgHandlerBasePtr(new _internal::generic_handler_t<unsigned long>(_T("unsigned long"))) );
		RegisterCfgHandler( CCfgHandlerBasePtr(new _internal::generic_handler_t<unsigned int>(_T("unsigned int"))) );

		RegisterCfgHandler( CCfgHandlerBasePtr(new _internal::generic_handler_t<float>(_T("float"))) );
		RegisterCfgHandler( CCfgHandlerBasePtr(new _internal::generic_handler_t<double>(_T("double"))) );

		RegisterCfgHandler( CCfgHandlerBasePtr(new _internal::generic_handler_t<tstring>(_T("string"))) );
	}

	bool RegisterCfgHandler(CCfgHandlerBasePtr handler, bool force)
	{
		handlers_t::iterator it=g_cfg_handlers.end();
		for(it=g_cfg_handlers.begin();it!=g_cfg_handlers.end();it++)
		{
			if((*it)->GetTypeName() == handler->GetTypeName() || (*it)->GetType() == handler->GetType())
			{
				if(force==true)
				{
					g_cfg_handlers.erase(it);
					break;
				}
				else
					return false;
			}
		}
		
		g_cfg_handlers.push_back(handler);
		return true;
	}

	void CleanupCfgHandlers()
	{
		g_cfg_handlers.clear();
	}

//////////////////////////////////////////////////////////////////////////
	CCfgXml::CCfgXml(void)
	{
		
	}

	CCfgXml::~CCfgXml(void)
	{
	}

	bool CCfgXml::Save(const tstring& filename)
	{
		//general idea: search for available nodes in the document tree. If some part of a variable is available, it will be
		//used as insert point for the new variable. For example two variables: "base.var1", "base.var2". "base" should
		//be used as insert point for the second variable. Do not add new tag for "base".

		xmlDocPtr doc = xmlNewDoc(BAD_CAST("1.0"));
		xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "root");
		xmlDocSetRootElement(doc, root_node);

		xmlXPathInit();
		xmlXPathContextPtr xpath_context = xmlXPathNewContext(doc);
		xpath_context->node = root_node;

		typedef boost::tokenizer<boost::char_separator<TCHAR>, tstring::const_iterator, tstring > tokenizer;
		boost::char_separator<TCHAR> separators(_T("."));
		for(values_map_t::iterator i=m_values.begin();i!=m_values.end();i++)
		{
			tokenizer tokens(i->first,separators);
			xmlNodePtr value_node = NULL;
			xmlNodePtr insert_node = root_node;

			//Split variable name on tokens and search for available nodes in the tree.
			tstring xpath_search;
			for(tokenizer::iterator token=tokens.begin();token!=tokens.end();token++)
			{
				if(!xpath_search.empty())
					xpath_search += _T("/");
				xpath_search += *token;
				xmlXPathObjectPtr xpath_obj = xmlXPathEval(BAD_CAST(conv_to_utf8(xpath_search).c_str()), xpath_context);

				if(xpath_obj->type == XPATH_NODESET && xpath_obj->nodesetval->nodeNr > 0)
				{
					insert_node = xpath_obj->nodesetval->nodeTab[0]; //Get the first node
					if(xpath_obj)
						xmlXPathFreeObject(xpath_obj);
				}
				else
				{
					if(xpath_obj)
						xmlXPathFreeObject(xpath_obj);

					//New insert points haven't found. Complete the tree with rest parts of the variable name.
					for(tokenizer::iterator k=token;k!=tokens.end();k++)
					{
						insert_node = xmlNewChild(insert_node, NULL, BAD_CAST(conv_to_utf8(*k).c_str()), NULL);
						value_node = insert_node;
					}

					handlers_t::iterator it=g_cfg_handlers.end();
					for(it=g_cfg_handlers.begin();it!=g_cfg_handlers.end();it++)
					{
						if((*it)->GetType() == i->second.type())
						{
							xmlNewProp(value_node, BAD_CAST("type"),BAD_CAST(conv_to_utf8((*it)->GetTypeName()).c_str()));
							(*it)->Save(i->second, value_node);
							break;
						}
					}
					if(it == g_cfg_handlers.end())
						OnSaveValueFailed(i->first,i->second.type());

					break;
				}
			}
		}

		xmlXPathFreeContext(xpath_context);
		const bool res = xmlSaveFormatFileEnc(conv_to_utf8(filename).c_str(), doc, "UTF-8", 1) > 0;
		xmlFreeDoc(doc);
		return res;
	};

	bool CCfgXml::Load(const tstring& filename)
	{
		xmlDocPtr doc = xmlParseFile(conv_to_utf8(filename).c_str());
		if(doc == NULL)
			return false;

		m_values.clear();
		ParseNode(xmlDocGetRootElement(doc));

		xmlFreeDoc(doc);

		return true;
	};

	void CCfgXml::ParseNode(void* node_)
	{
		xmlNodePtr node = reinterpret_cast<xmlNodePtr>(node_);
		if(node == NULL)
			return;

		if(xmlHasProp(node, BAD_CAST("type")))
		{
			tstring value_name;
			tstring value_type;

			xmlChar* tmp = xmlGetProp(node, BAD_CAST("type"));
			value_type = conv_to_tstring(std::string((const char*)tmp));
			if(tmp)
				xmlFree(tmp);

			//Do not add root into the path
			for(xmlNodePtr cur=node;cur && cur!=xmlDocGetRootElement(node->doc);cur=cur->parent)
			{
				if(value_name.empty())
					value_name = conv_to_tstring(std::string((const char*)cur->name));
				else
					value_name = conv_to_tstring(std::string((const char*)cur->name)) + _T('.') + value_name;
			}

			handlers_t::iterator it=g_cfg_handlers.end();
			for(it=g_cfg_handlers.begin();it!=g_cfg_handlers.end();it++)
			{
				if((*it)->GetTypeName() == value_type)
				{
					m_values[value_name] = (*it)->Load(node);
					break;
				}
			}
			if(it == g_cfg_handlers.end())
				OnLoadValueFailed(value_name,value_type);
		}
		else
		{
			for(xmlNodePtr cur=node->children;cur;cur=cur->next)
				ParseNode(cur);
		}
	}

	bool CCfgXml::HasValue(const tstring& name) const
	{
		return (m_values.find(conv_to_lower(name)) != m_values.end());
	}

	boost::any CCfgXml::GetValue(const tstring& name, const boost::any& def_value)
	{
		values_map_t::const_iterator it = m_values.find(conv_to_lower(name));
		if(it == m_values.end())
			return def_value;
		else
			return it->second;
	}

	void CCfgXml::SetValue(const tstring& name, const boost::any& value)
	{
		m_values[conv_to_lower(name)] = value;
	}
}
