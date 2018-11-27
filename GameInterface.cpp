#include <time.h>
#include "GameInterface.h"
#include "PapyrusXInput.h"
#include "PapyrusController.h"
#include "KeyboardHook.h"
#include "InputShimHook.h"
#include "Hooks_XInput.h"
#include "ScaleForm.h"
#include "gpad_version.h"
#include "Logger.h"

bool g_gameinterface::bMsgInit =false;

static SKSEPapyrusInterface* g_papyrusInterface = NULL;
static SKSEMessagingInterface* g_messageInterface = NULL;
static SKSEScaleformInterface* g_scaleformInterface = NULL;

bool g_gameinterface::Init(const SKSEInterface *skse, PluginHandle gHandle)
{
	bool bRegistered = false;
	g_papyrusInterface = (SKSEPapyrusInterface *)skse->QueryInterface(kInterface_Papyrus);
	g_messageInterface = (SKSEMessagingInterface *)skse->QueryInterface(kInterface_Messaging);
	g_scaleformInterface = (SKSEScaleformInterface *)skse->QueryInterface(kInterface_Scaleform);

#pragma region papyus func reg
	Log::Info("Registering papyrus functions");
	if(!g_papyrusInterface)
	{
		Log::Error("Unable to register papyrus functions");
		return false;
	}
	else if(g_papyrusInterface->interfaceVersion < SKSEPapyrusInterface::kInterfaceVersion)
	{
		Log::Error("Unable to register papyrus functions -- expected version [%d] found [%d] ",
		SKSEPapyrusInterface::kInterfaceVersion,
		g_papyrusInterface->interfaceVersion);
		return false;
	}
	else
	{
		bRegistered = g_papyrusInterface->Register(papyrusXInput::RegisterFuncs);
		if(!bRegistered) { Log::Error("Failed to register papyrus functions"); return false;}
		bRegistered = g_papyrusInterface->Register(papyrusController::RegisterFuncs);
		Log::Info("Succefully registered papyrus functions");
	}
#pragma endregion

#pragma region mesg func reg
	Log::Info("Registering message functions");
	if(!g_messageInterface)
	{
		Log::Error("Unable to register message functions");
		return false;
	}
	else if(g_messageInterface->interfaceVersion < SKSEMessagingInterface::kInterfaceVersion)
	{
		Log::Error("Unable to register message functions -- expected version [%d] found [%d] ",
		SKSEMessagingInterface::kInterfaceVersion,
		g_messageInterface->interfaceVersion);
		return false;
	}
	else
	{
		bRegistered = g_messageInterface->RegisterListener(gHandle, "SKSE", RecieveMessage);
		if (bRegistered) {
			srand(time(NULL));
			Log::Info("Succefully registered message functions");
		}
		else
		{
			Log::Error("Failed to register message functions");
			return false;
		}
	}
#pragma endregion

	#pragma region scaleform func reg
	Log::Info("Registering message functions");
	if(!g_scaleformInterface)
	{
		Log::Error("Unable to register scaleform functions");
		return false;
	}
	else if(g_scaleformInterface->interfaceVersion < SKSEScaleformInterface::kInterfaceVersion)
	{
		Log::Error("Unable to register scaleform functions -- expected version [%d] found [%d] ",
		SKSEScaleformInterface::kInterfaceVersion,
		g_scaleformInterface->interfaceVersion);
		return false;
	}
	else
	{
		bRegistered = g_scaleformInterface->Register(GPAD_PLUGIN_NAME, scaleform::RegisterFuncs);
		if (bRegistered) {
			srand(time(NULL));
			Log::Info("Succefully scaleform message functions");
		}
		else
		{
			Log::Error("Failed to scaleform message functions");
			return false;
		}
	}
#pragma endregion
	return true;
}

void g_gameinterface::RecieveMessage(SKSEMessagingInterface::Message* msg)
{
	#pragma region g_gameinterface::RecieveMessage
	if (msg->type == SKSEMessagingInterface::kMessage_PostLoadGame)
	{
		//SetupTimer();
		if(!bMsgInit)
		{
			KeyboardHook::InstallHook();
			Hooks_XInput_Commit();
			//InputShimHook::GetSingleton().InitGamepad();

			//if((*g_inputEventDispatcher))
			//{
			//	float unk1 = 0.f;
			//	float unk2 = 0x01b8fed0;
			//	(*g_inputEventDispatcher)->gamepad->Unk_02(unk2);
			//	//Log::VerboseDebug("g_gameinterface::RecieveMessage -- g_inputEventDispatcher is active");
			//	UInt32 key_unk01 = (*g_inputEventDispatcher)->keyboard->Unk_01();
			//	Log::VerboseDebug("g_gameinterface::RecieveMessage -- (*g_inputEventDispatcher)->keyboard->Unk_01() = 0x%08x", key_unk01);
			//	//(*g_inputEventDispatcher)->keyboard->Unk_02(12.f);
			//	//(*g_inputEventDispatcher)->keyboard->Unk_03();
			//	//(*g_inputEventDispatcher)->keyboard->Unk_06();
			//	UInt32 mouse_unk01 = (*g_inputEventDispatcher)->mouse->Unk_01();
			//	Log::VerboseDebug("g_gameinterface::RecieveMessage -- (*g_inputEventDispatcher)->mouse->Unk_01() = 0x%08x", mouse_unk01);
			//	//(*g_inputEventDispatcher)->mouse->Unk_02(unk1);
			//	//(*g_inputEventDispatcher)->mouse->Unk_03();
			//	//(*g_inputEventDispatcher)->mouse->Unk_06();
			//	UInt32 gamepad_unk01 = (*g_inputEventDispatcher)->gamepad->Unk_01();
			//	Log::VerboseDebug("g_gameinterface::RecieveMessage -- (*g_inputEventDispatcher)->gamepad->Unk_01() = 0x%08x", gamepad_unk01);
			//	//(*g_inputEventDispatcher)->gamepad->Unk_03();
			//	//(*g_inputEventDispatcher)->gamepad->Unk_06();
			//}

			bMsgInit = true;
		}
	}
	#pragma endregion
}
