#pragma once

#include <vector>
#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>

#include "Global.h"

namespace HelpersLib{

	///	Base class for custom types saving and loading. It also contains several helpers functions.
	class HELPERS_LIB_API CCfgHandlerBase
	{
	public:
		CCfgHandlerBase(){};
		virtual ~CCfgHandlerBase(){};

		/// Called when config class needs to load a variable. 
		/** Client should return restored value. It's recommended to use helpers function to manipulate with xml node.
			\param node xml node which holds variable information. 
			\return Loaded variable of boost::any type.*/
		virtual boost::any				Load(void* node) const = 0;

		/// Called when config class needs to save a variable. 
		/** Client should save value into xml node. It's recommended to use helpers function to manipulate with xml node.
			\param val Value to be saved.
			\param node XML node for this value.*/
		virtual void					Save(const boost::any& val, void* node) const = 0;

		//! Client should return textual name for custom type.
		virtual tstring					GetTypeName() const = 0;

		//! Client should return std::type_info for custom type. Usually: "return typeid(T);"
		virtual const std::type_info&	GetType() const = 0;

	protected:
		//! @name Helper functions
		//@{
		//! Get content of specified XML node.
		tstring GetNodeContent(void* node) const;
		//! Set content for specified XML node.
		void SetNodeContent(void* node, const tstring& content) const;

		//! Add property for specified XML node. Adds new sub tag to the node.
		void AddProperty(void* node, const tstring& name, const tstring& content) const;
		//! Get property of specified XML node.
		tstring GetProperty(void* node, const tstring& name) const;
		//! Set property for specified XML node. Adds new sub tag to the node or changes existing one.
		void SetProperty(void* node, const tstring& name, const tstring& content) const;

		//! Get property of specified XML node.
		boost::any GetAnyProperty(void* node, const tstring& name, const tstring& value_type) const;

		//! Get property of specified XML node of any type with default value
		template<typename T> T GetAnyProperty(void* node, const tstring& name, const tstring& value_type, const T& def_value=T()) const
		{
			try
			{
				return boost::any_cast<T>(GetAnyProperty(node, name, value_type));
			}catch(const boost::bad_any_cast&)
			{
				return def_value;
			}
		}
		//! Set property for specified XML node. Adds new sub tag to the node or changes existing one.
		bool SetAnyProperty(void* node, const tstring& name, const boost::any& content) const;

		//! Get attribute of specified XML node.
		tstring GetAttribute(void* node, const tstring& name) const;
		//! Set attribute for specified XML node.
		void SetAttribute(void* node, const tstring& name, const tstring& content) const;

		//! Find all sub nodes by name.
		std::vector<void*> FindNodes(void* node, const tstring& name) const;
		//@}
	};

	//! smart pointer for CCfgHandlerBase class. Used by CCfgXml.
	typedef boost::shared_ptr<CCfgHandlerBase> CCfgHandlerBasePtr;
}
