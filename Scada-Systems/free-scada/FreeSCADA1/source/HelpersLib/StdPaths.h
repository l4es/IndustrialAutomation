#pragma once

#include <atlstr.h>
#include "Global.h"

#ifdef GetTempPath
#	undef GetTempPath
#endif

namespace HelpersLib
{
	//! \brief Returns path to shared configuration directory.
	//!
	//! Default locations:
	//!	\n Windows: All Users\\Application Data\\\<application_name\>
	//! \param app_name Optional application name which could be added into the path.
	HELPERS_LIB_API tstring GetSharedConfigPath(const tstring& AppName=tstring());

	//! \brief Returns path to user's configuration directory.
	//!
	//! Default locations:
	//!	\n Windows: \<user name\>\\Application Data\\\<application_name\>
	//! \param app_name Optional application name which could be added into the path.
	HELPERS_LIB_API tstring GetUserConfigPath(const tstring& AppName=tstring());

	//! \brief Returns path to user's documents directory.
	//!
	//! Default locations:
	//!	\n Windows: \<user name\>\\My Documents
	HELPERS_LIB_API tstring GetDocumentsPath();

	//! \brief Returns path to temporary directory.
	//!
	//! Default locations:
	//!	\n Linux: /tmp
	//!	\n Windows: $TEMP or $TMP system variables
	HELPERS_LIB_API tstring GetTempPath();

	HELPERS_LIB_API tstring GetExecutablePath();
}
