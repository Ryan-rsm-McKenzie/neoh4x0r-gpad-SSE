#include <vector>

#include "skse64/PapyrusNativeFunctions.h"
#include "ScaleForm.h"
#include "skse64_common/SafeWrite.h"
#include "skse64/Hooks_ScaleForm.cpp"
#include "papyrusController.h"

namespace scaleform
{
//	class CustomSKSEScaleform_StartRemapMode : public GFxFunctionHandler
//{
//	class RemapHandler : public BSTEventSink<InputEvent>
//	{
//	public:
//		virtual EventResult ReceiveEvent(InputEvent ** evns, InputEventDispatcher * dispatcher)
//		{
//			Log::VerboseDebug("CustomSKSEScaleform_StartRemapMode::RemapHandler::ReceiveEvent called");
//			ButtonEvent * e = (ButtonEvent*) *evns;
//
//			// Make sure this is really a button event
//			if (!e || e->eventType != InputEvent::kEventType_Button)
//				return kEvent_Continue;
//
//			UInt32 deviceType = e->deviceType;
//
//			//if ((dispatcher->IsGamepadEnabled() ^ (deviceType == kDeviceType_Gamepad)) || e->flags == 0 || e->timer != 0.0)
//			//	return kEvent_Continue;
//			if (e->flags == 0 || e->timer != 0.0)
//				return kEvent_Continue;
//
//			UInt32 keyMask = e->keyMask;
//			UInt32 keyCode;
//
//			// Mouse
//			if (deviceType == kDeviceType_Mouse)
//				keyCode = InputMap::kMacro_MouseButtonOffset + keyMask;
//			// Gamepad
//			else if (deviceType == kDeviceType_Gamepad)
//				keyCode = InputMap::GamepadMaskToKeycode(keyMask);
//			// Keyboard
//			else
//				keyCode = keyMask;
//
//			// Valid scancode?
//			if (keyCode >= InputMap::kMaxMacros)
//				keyCode = -1;
//
//			GFxValue arg;
//			arg.SetNumber(keyCode);
//			scope.Invoke("EndRemapMode", NULL, &arg, 1);
//
//			MenuControls::GetSingleton()->remapMode = false;
//			PlayerControls::GetSingleton()->remapMode = false;
//
//			dispatcher->RemoveEventSink(this);
//			return kEvent_Continue;
//		}
//
//		GFxValue scope;
//	};
//
//	RemapHandler	remapHandler;
//
//public:
//	virtual void	Invoke(Args * args)
//	{
//		Log::VerboseDebug("CustomSKSEScaleform_StartRemapMode::Invoke called");
//		ASSERT(args->numArgs >= 1);
//
//		remapHandler.scope = args->args[0];
//
//		PlayerControls	* playerControls = PlayerControls::GetSingleton();
//		if (!playerControls)
//			return;
//
//		MenuControls	* menuControls = MenuControls::GetSingleton();
//		if (!menuControls)
//			return;
//
//		if (! (*g_inputEventDispatcher))
//			return;
//
//		(*g_inputEventDispatcher)->AddEventSink(&remapHandler);
//		menuControls->remapMode = true;
//		playerControls->remapMode = true;
//	}
//};

	class SKSEScaleform_GetLX : public GFxFunctionHandler
	{
	public:
		virtual void	Invoke(Args * args)
		{
			StaticFunctionTag* tag = new StaticFunctionTag();
			float p = papyrusController::GetLX(tag);
			args->result->SetNumber(p);
		}
	};
	class SKSEScaleform_GetLY : public GFxFunctionHandler
	{
	public:
		virtual void	Invoke(Args * args)
		{
			StaticFunctionTag* tag = new StaticFunctionTag();
			float p = papyrusController::GetLY(tag);
			args->result->SetNumber(p);
		}
	};
	class SKSEScaleform_GetRX : public GFxFunctionHandler
	{
	public:
		virtual void	Invoke(Args * args)
		{
			StaticFunctionTag* tag = new StaticFunctionTag();
			float p = papyrusController::GetRX(tag);
			args->result->SetNumber(p);
		}
	};
	class SKSEScaleform_GetRY : public GFxFunctionHandler
	{
	public:
		virtual void	Invoke(Args * args)
		{
			StaticFunctionTag* tag = new StaticFunctionTag();
			float p = papyrusController::GetRY(tag);
			args->result->SetNumber(p);
		}
	};
	class SKSEScaleform_GetLSDirection : public GFxFunctionHandler
	{
	public:
		virtual void	Invoke(Args * args)
		{
			StaticFunctionTag* tag = new StaticFunctionTag();
			SInt32 p = papyrusController::GetLSDirection(tag);
			args->result->SetNumber(p);
		}
	};
	class SKSEScaleform_GetRSDirection : public GFxFunctionHandler
	{
	public:
		virtual void	Invoke(Args * args)
		{
			StaticFunctionTag* tag = new StaticFunctionTag();
			SInt32 p = papyrusController::GetRSDirection(tag);
			args->result->SetNumber(p);
		}
	};

	bool RegisterFuncs(GFxMovieView * view, GFxValue * root)
	{

		//Log::VerboseDebug("scaleform::RegisterFuncs called");
		RegisterFunction <SKSEScaleform_GetLX>(root, view, "GetLX");
		RegisterFunction <SKSEScaleform_GetLY>(root, view, "GetLY");
		RegisterFunction <SKSEScaleform_GetRX>(root, view, "GetRX");
		RegisterFunction <SKSEScaleform_GetRY>(root, view, "GetRY");
		RegisterFunction <SKSEScaleform_GetLSDirection>(root, view, "GetLSDirection");
		RegisterFunction <SKSEScaleform_GetRSDirection>(root, view, "GetRSDirection");

		//GFxValue	globals;
		//bool	result = view->GetVariable(&globals, "_global");
		//if(!result)
		//{
		//	Log::VerboseDebug("scaleform::RegisterFuncs() -- couldn't get _global");
		//}
		//else
		//{
		//	GFxValue	skse;
		//	view->CreateObject(&skse);
		//	RegisterFunction <CustomSKSEScaleform_StartRemapMode>(&skse, view, "StartRemapMode");
		//	globals.SetMember("skse", &skse);
		//}
		//PlayerControls	* playerControls = PlayerControls::GetSingleton();
		//MenuControls	* menuControls = MenuControls::GetSingleton();
		//if(playerControls && playerControls->remapMode)
		//{
		//	Log::VerboseDebug("scaleform::RegisterFuncs -> playerControls are in remapMode");
		//}
		//if(menuControls && menuControls->remapMode)
		//{
		//	Log::VerboseDebug("scaleform::RegisterFuncs -> menuControls are in remapMode");
		//}
		//if ((*g_inputEventDispatcher))
		//{
		//	(*g_inputEventDispatcher)->gamepad
		//}

		return true;
	}
}
