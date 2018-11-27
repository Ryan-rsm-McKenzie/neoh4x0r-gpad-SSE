#include <windows.h>
#include <ShlObj.h>  // CSIDL_MYDOCUMENTS

#include "main.h"
#include "skse64/PluginAPI.h"
#include "skse64_common/skse_version.h"
#include "skse64_common/SafeWrite.h"
#include "skse64/GameAPI.h"

#include "gpadconf.h"
#include "GameInterface.h"
#include "Logger.h"
//#include "ScaleForm.h"

#define GPAD_PLUGIN_VERSION 1

static PluginHandle					g_pluginHandle = kPluginHandle_Invalid;

extern "C"
{
bool SKSEPlugin_Query(const SKSEInterface * skse, PluginInfo * info)
{
	// main plugin enty point
	gpadconf::Init(GPAD_PLUGIN_NAME);
	gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim Special Edition\\SKSE\\neoh4x0r-gpad-SSE.log");
	Log::Info("SKSEPlugin_Query (%s.dll v%s)", GPAD_PRODUCTNAME, GPAD_VERSION_VERSTRING);
	// populate info structure
	info->infoVersion =	PluginInfo::kInfoVersion;
	info->name =		GPAD_PLUGIN_NAME;
	info->version =		GPAD_PLUGIN_VERSION;

	// store plugin handle so we can identify ourselves later
	g_pluginHandle = skse->GetPluginHandle();

	if(skse->isEditor)
	{
		Log::Warn("loaded in editor, marking as incompatible");

		return false;
	}
	else if(skse->runtimeVersion != RUNTIME_VERSION_1_5_53)
	{
		Log::Warn("unsupported runtime version %08X", skse->runtimeVersion);

		return false;
	}
	return true;
}

bool SKSEPlugin_Load(const SKSEInterface * skse)
{
	Log::Info("SKSEPlugin_Load");
	if(!gpadconf::PluginEnabled())
	{
		Log::Warn("Plugin has been disabled via (%s.ini) -- set [Globals] enable_plugin=1 to enable", GPAD_PLUGIN_NAME);
		return false;
	}
	//Custom_Hooks_Scaleform_Commit();
	return g_gameinterface::Init(skse,g_pluginHandle);
}
};
