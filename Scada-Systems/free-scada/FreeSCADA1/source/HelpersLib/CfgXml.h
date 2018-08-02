#pragma once

#include "CfgHandlerBase.h"
#include "Global.h"

#include <map>
#include <boost/any.hpp>

namespace HelpersLib{

	/// \brief Advanced configuration reader for xml-based configs. 
	/** This class provides functionality to read and save xml-based configuration files. As distinct from simple_xml_config_t
		this class can be enriched by custom types. By default it can handle common types like integers, real numbers and strings. 
		Additional types can be added by implementing your own handler which should be inherited from CCfgHandlerBase type.*/
	class HELPERS_LIB_API CCfgXml
	{
		friend CCfgHandlerBase;
	public:
		CCfgXml(void);
		virtual ~CCfgXml(void);

		//! \brief Save the configuration to a xml file
		bool Save(const tstring& filename);
		//! \brief Load the configuration from a xml file
		bool Load(const tstring& filename);

		//! \brief Test if the configuration has a variable.
		//! \param name Variable name. Could be hierarchical separated by '.' (ex. "colors.background")
		//! \return Returns true if the variable is available.
		bool HasValue(const tstring& name) const;

		//! \brief Return value for requested variable.
		//! \param name Variable name. Could be hierarchical separated by '.' (ex. "colors.background")
		//! \param def_value Default value. Will be used in case if variable unavailable.
		//! \return Returns value of boost::any. To get value your type use boost::any_cast<T>(value).
		boost::any GetValue(const tstring& name, const boost::any& def_value=boost::any());

		/// Return value for requested variable and type. It's helper method for common get_value().
		/** \n\b Example:
			\code int my_val = cfg.get_value<int>(_U("my_variable_name")); \endcode
			\code int my_val2 = cfg.get_value<int>(_U("my_variable_name"), 123); \endcode
			\param name Variable name. Could be hierarchical.
			\param def_value Default value. Will be used in case if variable unavailable.
			\return Returns value of requested type.*/
		template<typename T> T GetValue(const tstring& name, const T& def_value=T())
		{
			try
			{
				return boost::any_cast<T>(GetValue(name, boost::any(def_value)));
			}catch(const boost::bad_any_cast&)
			{
				return def_value;
			}
		}

		//! \brief Adds or changes existing value of provided variable.
		//! \param name Variable name. Could be hierarchical.
		//! \param value Value for the variable.
		void SetValue(const tstring& name, const boost::any& value);

	protected:
		typedef std::map<tstring, boost::any> values_map_t;

		values_map_t m_values;

	protected:
		/// Client can override this method to handle load errors.
		virtual void OnLoadValueFailed(const tstring& /*value_name*/, const tstring& /*value_type*/){};

		/// Client can override this method to handle save errors.
		virtual void OnSaveValueFailed(const tstring& /*value_name*/, const std::type_info& /*value_type*/){};

	private:
		void ParseNode(void* node);
	};

	HELPERS_LIB_API void InitializeStandardCfgHandlers();

	/// Adds handler for custom types which are not supported by CCfgXml.
	/** Custom handler should be inherited from CCfgHandlerBase. You should not delete this object manually.
	The object will be destroyed automatically by this class.
	\n\b Example:
	\code 
	class your_own_handler_t: public HelpersLib::CCfgHandlerBase { ... }

	RegisterCfgHandler( new your_own_handler_t() ); \endcode
	\param handler Custom handler. Will be deleted automatically in CCfgXml destructor.
	\param force Replace existing handler if any.
	\return Returns true if the handler was successfully added.
	*/
	HELPERS_LIB_API bool RegisterCfgHandler(CCfgHandlerBasePtr handler, bool force=false);

	HELPERS_LIB_API void CleanupCfgHandlers();
}
