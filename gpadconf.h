#pragma once

#include <string>

const std::string & local_GetConfigPath(bool bwritepath);
std::string local_GetConfigOption(const char * section, const char * key);
bool local_GetConfigOption_UInt32(const char * section, const char * key, UInt32 * dataOut);

#if _DEBUG
#define FORCE_DEBUG_LOGGING 1
#define FORCE_VERBOSE_DEBUG 1
#else
#define FORCE_DEBUG_LOGGING 0
#define FORCE_VERBOSE_DEBUG 0
#endif

class gpadconf
{
	public:
		//static gpadconf& GetSingleton(void);

		static void Init(const char* plugin_name);
		static bool ExtendedControlsEnabled(void);
		static bool PluginEnabled(void);
		static bool SendMovementKeys(void);
		static bool SendMajorMovementKeysOnly(void);
		static bool DebugEnabled(void);
		static bool VerboseDebugEnabled(void);
private:
	//static gpadconf* g_gpconf;
	static bool bInit;
	static UInt32 opt_useextendedcontrols;
	static UInt32 opt_enableplugin;
	static UInt32 opt_enabledebug;
	static UInt32 opt_enableverbosedebug;
	static UInt32 opt_sendmovementkeys;
	static UInt32 opt_sendmajormovementkeysonly;
};
