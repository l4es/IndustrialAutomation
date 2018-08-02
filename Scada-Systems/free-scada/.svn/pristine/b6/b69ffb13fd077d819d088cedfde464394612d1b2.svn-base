#include "config_manager.h"
#include "../file_system/stdpaths.h"

namespace gen_helpers1{

	static const uchar company_name[] = _U("Intel");

    config_manager_t::config_manager_t(const tstring& configuration_path, const tstring& application_name)
		:m_configuration_path(configuration_path)
		,m_application_name(application_name)
    {
    }
    
    config_manager_t::~config_manager_t(void)
    {
    }


	tstring config_manager_t::get_user_config(const tstring& cfg_name)
	{
		tstring file_path(stdpaths::get_user_config_dir(company_name));
		file_path.append(m_application_name);
		file_path.append(cfg_name + _U(".xml"));
		return file_path;
	}

	tstring config_manager_t::get_default_config(const tstring& cfg_name)
	{
		tstring file_path(m_configuration_path);
		file_path.append(cfg_name + _U(".xml"));
		return file_path;
	}
}
