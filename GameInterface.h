#pragma once

#include "skse64/PluginAPI.h"
#include "skse64_common/skse_version.h"

class g_gameinterface
{
	public:
		static bool Init(const SKSEInterface * skse, PluginHandle gHandle);
private:
		static bool bMsgInit;
		static void RecieveMessage(SKSEMessagingInterface::Message* msg);
};
