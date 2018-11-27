#include "gpadconf.h"
#include "main.h"
#include "skse64_common/Utilities.h"
#include <shlobj.h>				// CSIDL_MYCODUMENTS
#include "Logger.h"
#define GPAD_PLUGIN_PATH "Data\\SKSE\\Plugins\\"
//gpadconf* gpadconf::g_gpconf	= NULL;

#if FORCE_DEBUG_LOGGING
_W("FORCE_DEBUG_LOGGING is enabled -- disable it before release");
#endif
#if FORCE_VERBOSE_DEBUG
_W("FORCE_VERBOSE_DEBUG is enabled -- disable it before release");
#endif
bool gpadconf::bInit = false;
UInt32 gpadconf::opt_useextendedcontrols = 1;
UInt32 gpadconf::opt_enableplugin = 1;
UInt32 gpadconf::opt_enabledebug = 0;
UInt32 gpadconf::opt_enableverbosedebug = 0;
UInt32 gpadconf::opt_sendmovementkeys =1;
UInt32 gpadconf::opt_sendmajormovementkeysonly =0;

bool gpadconf::DebugEnabled(void)
{
#if DEBUG
	return true;
#else
	#if FORCE_DEBUG_LOGGING
		return true;
	#else
		return opt_enabledebug == 1 ? true : false;
	#endif
#endif
}
bool gpadconf::VerboseDebugEnabled(void)
{
	if(DebugEnabled())
	{
#if FORCE_VERBOSE_DEBUG
		return true;
#else
		return opt_enableverbosedebug == 1 ? true : false;
#endif
	}
	else
	{
		return false;
	}
}
bool gpadconf::ExtendedControlsEnabled(void)
{
	return opt_useextendedcontrols == 1 ? true : false;
}
bool gpadconf::PluginEnabled(void)
{
	return opt_enableplugin == 1 ? true : false;
}
bool gpadconf::SendMovementKeys(void)
{
	return opt_sendmovementkeys == 1 ? true : false;
}
bool gpadconf::SendMajorMovementKeysOnly(void)
{
	return opt_sendmajormovementkeysonly == 1 ? true : false;
}

const std::string & local_GetConfigPath(bool bwritepath)
{
	static std::string s_configPath;

	if(s_configPath.empty())
	{
		std::string	runtimePath = GetRuntimeDirectory();
		if(!runtimePath.empty())
		{
			//s_configPath = runtimePath + "Data\\SKSE\\Plugins\\" + PLUGIN_CONFIG;
			s_configPath = runtimePath;
			s_configPath.append(GPAD_PLUGIN_PATH);
			s_configPath.append(GPAD_PLUGIN_NAME);
			s_configPath.append(".ini");

			if(bwritepath)
			{
				_MESSAGE("config path = %s", s_configPath.c_str());
			}
		}
	}

	return s_configPath;
}
std::string local_GetConfigOption(const char * section, const char * key)
{
	std::string	result;

	const std::string & configPath = local_GetConfigPath(false);
	if(!configPath.empty())
	{
		char	resultBuf[256];
		resultBuf[0] = 0;

		UInt32	resultLen = GetPrivateProfileString(section, key, NULL, resultBuf, sizeof(resultBuf), configPath.c_str());

		result = resultBuf;
	}

	return result;
}

bool local_GetConfigOption_UInt32(const char * section, const char * key, UInt32 * dataOut)
{
	std::string	data = local_GetConfigOption(section, key);
	if(data.empty())
		return false;

	return (sscanf_s(data.c_str(), "%u", dataOut) == 1);
}
//gpadconf& gpadconf::GetSingleton(void)
//{
//	if (!g_gpconf)
//		g_gpconf = new gpadconf();
//	return *g_gpconf;
//}
void gpadconf::Init(const char* plugin_name)
{
	if(bInit) { return; }

	std::string log("\\My Games\\Skyrim\\SKSE\\");
	log.append(plugin_name);
	log.append(".log");
	gLog.OpenRelative(CSIDL_MYDOCUMENTS, log.c_str());
	gLog.SetPrintLevel(IDebugLog::kLevel_Error);
	gLog.SetLogLevel(IDebugLog::kLevel_Message);


	local_GetConfigPath(true);
	// read config options

	_MESSAGE("reading option: Globals::enable_debug");
	if(!local_GetConfigOption_UInt32("Globals", "enable_debug", &opt_enabledebug))
	{
		_WARNING("option: Globals::enable_debug enabled not found using defaults (disabled)");
		opt_enabledebug = FALSE;
	}

	if(DebugEnabled())
	{
		_WARNING("option: Globals::enable_debug enabled -- will log debug messages");
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);
	}
	_MESSAGE("reading option: Globals::enable_verbose_debug");
	if(!local_GetConfigOption_UInt32("Globals", "enable_verbose_debug", &opt_enableverbosedebug))
	{
		_WARNING("option: Globals::enable_verbose_debug enabled not found using defaults (disabled)");
		opt_enabledebug = FALSE;
	}

	if(DebugEnabled() && VerboseDebugEnabled())
	{
		_WARNING("option: Globals::enable_verbose_debug enabled -- will log verbose debug messages");
	}

	_MESSAGE("reading option: Globals::enable_plugin");
	if(!local_GetConfigOption_UInt32("Globals", "enable_plugin", &opt_enableplugin))
	{
		// default to false
		_WARNING("option: Globals::enable_plugin not found using defaults (enabled)");
		opt_enableplugin = TRUE;
	}
	else
	{
		if(!PluginEnabled())
		{
			Log::Warn("option: Globals::enable_plugin disabled -- disabling plugin");
		}
	}

	_MESSAGE("reading option: Features::use_extended_controls");
	if(!local_GetConfigOption_UInt32("Features", "use_extended_controls", &opt_useextendedcontrols))
	{
		// default to false
		_WARNING("option: Features::use_extended_controls not found using defaults (enabled)");
		opt_useextendedcontrols = TRUE;
	}
	else
	{
		if(!ExtendedControlsEnabled())
		{
			_WARNING("option: Features::use_extended_controls -- disabling extened control support ");
		}
	}

	_MESSAGE("reading option: Features::send_movement_keys");
	if(!local_GetConfigOption_UInt32("Features", "send_movement_keys", &opt_sendmovementkeys))
	{
		// default to false
		_WARNING("option: Features::send_movement_keys not found using defaults (enabled)");
		opt_sendmovementkeys = TRUE;
	}
	else
	{
		if(!SendMovementKeys())
		{
			_WARNING("option: Features::send_movement_keys -- will not send movement keys");
		}
	}

	_MESSAGE("reading option: Features::send_major_movement_keys_only");
	if(!local_GetConfigOption_UInt32("Features", "send_major_movement_keys_only", &opt_sendmajormovementkeysonly))
	{
		// default to false
		_WARNING("option: Features::send_major_movement_keys_only not found using defaults (disabled)");
		opt_sendmajormovementkeysonly = FALSE;
	}
	else
	{
		if(SendMajorMovementKeysOnly())
		{
			_WARNING("option: Features::send_major_movement_keys_only -- will not send diagonal movement keys");
		}
	}
	bInit=true;
}
