#include "StdAfx.h"
#include "CfgHandlerBase.h"
#include "CfgXml.h"
#include "StrConv.h"

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <list>

namespace HelpersLib{

	typedef std::list<CCfgHandlerBasePtr> handlers_t;
	extern handlers_t g_cfg_handlers;

	tstring CCfgHandlerBase::GetNodeContent(void* node_) const
	{
		xmlNodePtr node = reinterpret_cast<xmlNodePtr>(node_);
		xmlChar* tmp = xmlNodeListGetString(node->doc, node->children, 1);
		tstring node_content = conv_to_tstring(std::string((const char*)tmp));
		if(tmp)
			xmlFree(tmp);
		return node_content;
	}

	void CCfgHandlerBase::SetNodeContent(void* node, const tstring& content) const
	{
		std::string str_val = conv_to_utf8(content);
		xmlNodeAddContentLen((xmlNodePtr)node, BAD_CAST(str_val.c_str()), (int)str_val.size());
	}

	void CCfgHandlerBase::AddProperty(void* node, const tstring& name, const tstring& content) const
	{
		xmlNewChild((xmlNodePtr)node, NULL, BAD_CAST(conv_to_utf8(name).c_str()), BAD_CAST(conv_to_utf8(content).c_str()));
	}

	tstring CCfgHandlerBase::GetProperty(void* node, const tstring& name) const
	{
		std::vector<void*> nodes = FindNodes(node, name);
		if(nodes.empty())
			return tstring();
		else
			return GetNodeContent(nodes.front());
	}

	void CCfgHandlerBase::SetProperty(void* node, const tstring& name, const tstring& content) const
	{
		std::vector<void*> nodes = FindNodes(node, name);
		if(nodes.empty())
			AddProperty(node, name, content);
		else
			SetNodeContent(nodes.front(), content);
	}

	boost::any CCfgHandlerBase::GetAnyProperty(void* node, const tstring& name, const tstring& value_type) const
	{
		std::vector<void*> nodes = FindNodes(node, name);
		if(!nodes.empty())
		{
			handlers_t::const_iterator it=g_cfg_handlers.end();
			for(it=g_cfg_handlers.begin();it!=g_cfg_handlers.end();it++)
			{
				if((*it)->GetTypeName() == value_type)
					return (*it)->Load(nodes.front());
			}
		}
		return boost::any();
	}

	bool CCfgHandlerBase::SetAnyProperty(void* node, const tstring& name, const boost::any& content) const
	{
		std::vector<void*> nodes = FindNodes(node, name);
		xmlNodePtr node_to_save;
		if(!nodes.empty())
			node_to_save = reinterpret_cast<xmlNodePtr>(nodes.front());
		else
			node_to_save = xmlNewChild((xmlNodePtr)node, NULL, BAD_CAST(conv_to_utf8(name).c_str()), NULL);
		
		handlers_t::const_iterator it=g_cfg_handlers.end();
		for(it=g_cfg_handlers.begin();it!=g_cfg_handlers.end();it++)
		{
			if((*it)->GetType() == content.type())
			{
				(*it)->Save(content, node_to_save);
				return true;
			}
		}
		return false;
	}

	tstring CCfgHandlerBase::GetAttribute(void* node, const tstring& name) const
	{
		if(xmlHasProp((xmlNodePtr)node, BAD_CAST(conv_to_utf8(name).c_str())))
		{
			xmlChar* tmp = xmlGetProp((xmlNodePtr)node, BAD_CAST(conv_to_utf8(name).c_str()));
			tstring res(conv_to_tstring(std::string((const char*)tmp)));
			if(tmp)
				xmlFree(tmp);
			return res;
		}
		else
			return tstring();
	}

	void CCfgHandlerBase::SetAttribute(void* node, const tstring& name, const tstring& content) const
	{
		xmlSetProp((xmlNodePtr)node, BAD_CAST(conv_to_utf8(name).c_str()), BAD_CAST(conv_to_utf8(content).c_str()));
	}

	std::vector<void*> CCfgHandlerBase::FindNodes(void* node_, const tstring& name) const
	{
		xmlNodePtr node = reinterpret_cast<xmlNodePtr>(node_);
		std::vector<void*> res;

		xmlXPathInit();
		xmlXPathContextPtr xpath_context = xmlXPathNewContext(node->doc);
		xpath_context->node = node;

		xmlXPathObjectPtr xpath_obj = xmlXPathEval(BAD_CAST(conv_to_utf8(name).c_str()), xpath_context);
		if(xpath_obj->type == XPATH_NODESET && xpath_obj->nodesetval->nodeNr > 0)
		{
			for(int i=0;i<xpath_obj->nodesetval->nodeNr;i++)
				res.push_back(xpath_obj->nodesetval->nodeTab[i]);
		}
		if(xpath_obj)
			xmlXPathFreeObject(xpath_obj);

		xmlXPathFreeContext(xpath_context);
		return res;
	}
}
