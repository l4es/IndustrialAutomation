#pragma once

#include "../exports.h"
#include "../strings/strings.h"
#include "../file_system/path.h"

#pragma warning(push)
#include "../disable_warnings.h"

namespace gen_helpers1{

    //! \brief Configuration manager for configs
    class GEN_HELPERS_API config_manager_t
    {
    public:
        config_manager_t(const tstring& configuration_path, const tstring& application_name);
        virtual ~config_manager_t(void);

		/// Open config. First tries to load from user folder then from default
		template<typename T>
		bool open_config(const tstring& name, T& cfg_object)
		{
			tstring filename = get_user_config(name);
			bool try_load_default = true;
			if(filename.is_exists())
				try_load_default = !cfg_object.load(filename);

			if(try_load_default)
				return cfg_object.load(get_default_config(name));
			else
				return true;
		}

		/// Save config. Save config file into user specific folder.
		template<typename T>
		bool save_config(const tstring& name, T& cfg_object)
		{
			return cfg_object.save(get_user_config(name));
		}

	private:
        tstring                 m_configuration_path;
        tstring                 m_application_name;

		tstring get_user_config(const tstring& cfg_name);
		tstring get_default_config(const tstring& cfg_name);
    };
}

#pragma warning(pop)
