#include "PapyrusXInput.h"
#include "skse64/GameInput.h"
#include "Hooks_XInput.h"

#include "InputShimHook.h"
#include "gpadconf.h"
#include "Logger.h"

namespace papyrusXInput
{
	UInt32 GetMappedKeyAuto(StaticFunctionTag* thisInput, UInt32 dxKeycode)
	{
#if DISABLE_GAMEPAD_KEY_TRANSLATION
		return dxKeycode; // don't translate, return the keycode unmodified
#else
		return papyrusXInput::GetMappedKey(thisInput,papyrusXInput::GetMappedControl(thisInput,dxKeycode),kDeviceType_Auto);
#endif
	}

	bool IsKeyPressed(StaticFunctionTag* thisInput, UInt32 dxKeycode)
	{
		UInt32 __dxKeycode = GetMappedKeyAuto(thisInput,dxKeycode);
		if(__dxKeycode == 0xffffffff){ __dxKeycode = dxKeycode; }
		if(XInputHookControl::UsingGamepad())
		{
			if(XInputHookControl::GetSingleton().IsGamepadKey(__dxKeycode))
			{
				return XInputHookControl::GetSingleton().IsKeyPressed(__dxKeycode);
			}
		}
		return InputShimHook::GetSingleton().IsKeyPressed(dxKeycode, true);
	}

	void TapKey(StaticFunctionTag* thisInput, UInt32 dxKeycode)
	{
		UInt32 __dxKeycode = GetMappedKeyAuto(thisInput,dxKeycode);
		if(__dxKeycode == 0xffffffff){ __dxKeycode = dxKeycode; }
		if(XInputHookControl::UsingGamepad())
		{
			if(!XInputHookControl::GetSingleton().BufferedKeyTap(__dxKeycode))
			{
				Log::VerboseDebug("papyrusXInput::TapKey [dxKeycode=%i] -- XInputHookControl failed BufferedKeyTap", dxKeycode);
				InputShimHook::GetSingleton().BufferedKeyTap(dxKeycode, true);
			}
		}
		else
		{
			InputShimHook::GetSingleton().BufferedKeyTap(dxKeycode, true);
		}
		//InputShimHook::GetSingleton().BufferedKeyTap(dxKeycode,true);
	}

	void HoldKey(StaticFunctionTag* thisInput, UInt32 dxKeycode)
	{
		UInt32 __dxKeycode = GetMappedKeyAuto(thisInput,dxKeycode);
		if(__dxKeycode == 0xffffffff){ __dxKeycode = dxKeycode; }
		if(XInputHookControl::UsingGamepad())
		{
			if(!XInputHookControl::GetSingleton().HoldKey(__dxKeycode))
			{
				//DIHookControl::GetSingleton().SetKeyHeldState(__dxKeycode, true);
				InputShimHook::GetSingleton().BufferedKeyPress(dxKeycode, true);
			}
		}
		else
		{
			InputShimHook::GetSingleton().BufferedKeyPress(dxKeycode, true);
		}
	}

	void ReleaseKey(StaticFunctionTag* thisInput, UInt32 dxKeycode)
	{
		UInt32 __dxKeycode = GetMappedKeyAuto(thisInput,dxKeycode);
		if(__dxKeycode == 0xffffffff){ __dxKeycode = dxKeycode; }
		if(XInputHookControl::UsingGamepad())
		{
			if(!XInputHookControl::GetSingleton().ReleaseKey(__dxKeycode))
			{
				//DIHookControl::GetSingleton().SetKeyHeldState(__dxKeycode, false);
				InputShimHook::GetSingleton().BufferedKeyRelease(dxKeycode, true);
			}
		}
		else
		{
			InputShimHook::GetSingleton().BufferedKeyRelease(dxKeycode, true);
		}
	}

	bool IsKeyHeld(StaticFunctionTag* thisInput, UInt32 dxKeycode)
	{
		UInt32 __dxKeycode = GetMappedKeyAuto(thisInput,dxKeycode);
		if(__dxKeycode == 0xffffffff){ __dxKeycode = dxKeycode; }
		if(XInputHookControl::UsingGamepad())
		{
			return XInputHookControl::GetSingleton().IsKeyHeld(__dxKeycode);
		}
		else
		{
			return InputShimHook::GetSingleton().IsKeyHeld(dxKeycode, true);
		}
	}

	void DisableKey(StaticFunctionTag* thisInput, UInt32 dxKeycode)
	{
		UInt32 __dxKeycode = GetMappedKeyAuto(thisInput,dxKeycode);
		if(__dxKeycode == 0xffffffff){ __dxKeycode = dxKeycode; }
		if(XInputHookControl::UsingGamepad())
		{
			if(!XInputHookControl::GetSingleton().DisableKey(__dxKeycode))
			{
				InputShimHook::GetSingleton().SetKeyDisableState(dxKeycode, true, 0);
			}
		}
		else
		{
			InputShimHook::GetSingleton().SetKeyDisableState(dxKeycode, true, 0);
		}
	}

	void EnableKey(StaticFunctionTag* thisInput, UInt32 dxKeycode)
	{
		UInt32 __dxKeycode = GetMappedKeyAuto(thisInput,dxKeycode);
		if(__dxKeycode == 0xffffffff){ __dxKeycode = dxKeycode; }
		if(XInputHookControl::UsingGamepad())
		{
			if(!XInputHookControl::GetSingleton().EnableKey(__dxKeycode))
			{
				InputShimHook::GetSingleton().SetKeyDisableState(dxKeycode, false, 0);
			}
		}
		else
		{
			InputShimHook::GetSingleton().SetKeyDisableState(dxKeycode, false, 0);
		}
	}

	bool IsKeyDisabled(StaticFunctionTag* thisInput, UInt32 dxKeycode)
	{
		UInt32 __dxKeycode = GetMappedKeyAuto(thisInput,dxKeycode);
		if(__dxKeycode == 0xffffffff){ __dxKeycode = dxKeycode; }
		if(XInputHookControl::UsingGamepad())
		{
			return XInputHookControl::GetSingleton().IsKeyDisabled(__dxKeycode);
		}
		else
		{
			return InputShimHook::GetSingleton().IsKeyDisabled(dxKeycode);
		}
	}

	UInt32 GetNumKeysPressed(StaticFunctionTag* thisInput)
	{
		if(XInputHookControl::UsingGamepad())
		{
			return XInputHookControl::GetSingleton().GetNumKeysPressed();
		}
		return InputShimHook::GetSingleton().GetNumKeysPressed();
	}

	SInt32 GetNthKeyPressed(StaticFunctionTag* thisInput, UInt32 which)
	{
		if(XInputHookControl::UsingGamepad())
		{
			return XInputHookControl::GetSingleton().GetNthKeyPressed(which);
		}
		return InputShimHook::GetSingleton().GetNthKeyPressed(which);
	}

	//UInt32 GetNumMouseButtonsPressed(StaticFunctionTag* thisInput);
	//UInt32 GetMouseButtonPressed(StaticFunctionTag* thisInput, UInt32 which);

	//void DisableMouse(StaticFunctionTag* thisInput);
	//void EnableMouse(StaticFunctionTag* thisInput);
	//bool IsMouseDisabled(StaticFunctionTag* thisInput);

	SInt32 GetMappedKey(StaticFunctionTag* thisInput, BSFixedString name, UInt32 deviceType)
	{
		InputManager * inputManager = InputManager::GetSingleton();
		if (!inputManager)
		{
			Log::VerboseDebug("PapyrusXInput::GetMappedKey - inputManager is null");
			return -1;
		}

		UInt32 key = 0xFF;

		// Manual device selection
		if (deviceType != 0xFF)
		{
			key = inputManager->GetMappedKey(name, deviceType, InputManager::kContext_Gameplay);
		}
		// Auto-selected device
		else
		{
			// Gamepad
			if (InputEventDispatcher::GetSingleton() && InputEventDispatcher::GetSingleton()->IsGamepadEnabled())
			{
				deviceType = kDeviceType_Gamepad;
				key = inputManager->GetMappedKey(name, kDeviceType_Gamepad, InputManager::kContext_Gameplay);
			}
			// Mouse + Keyboard
			else
			{
				deviceType = kDeviceType_Keyboard;
				key = inputManager->GetMappedKey(name, deviceType, InputManager::kContext_Gameplay);
				if (key == 0xFF)
				{
					deviceType = kDeviceType_Mouse;
					key = inputManager->GetMappedKey(name, deviceType, InputManager::kContext_Gameplay);
				}
			}
		}

		if (key == 0xFF && deviceType != kDeviceType_Gamepad)
			return -1;

		if (key == 0xFF && deviceType == kDeviceType_Gamepad)
		{
			deviceType = kDeviceType_Keyboard; // default to keyboard so we can get the keyboard key
			key = inputManager->GetMappedKey(name, deviceType, InputManager::kContext_Gameplay);
			if(key == 0xFF)
				return -1;
		}

		// Map to common value space
		if (deviceType == kDeviceType_Mouse)
		{
			return key + InputMap::kMacro_MouseButtonOffset;
		}
		else if (deviceType == kDeviceType_Gamepad)
		{
			UInt32 mapped = InputMap::GamepadMaskToKeycode(key);
			return (mapped != InputMap::kMaxMacros ? mapped : -1);
		}
		else
		{
			return key;
		}
	}

	BSFixedString GetMappedControl(StaticFunctionTag* thisInput, SInt32 keyCode)
	{
		if (keyCode < 0 || keyCode >= InputMap::kMaxMacros)
		{
			Log::VerboseDebug("PapyrusXInput::GetMappedControl - [keyCode=0x%08x (out of range)]", keyCode);
			return BSFixedString();
		}

		InputManager * inputManager = InputManager::GetSingleton();
		if (!inputManager) {
			Log::VerboseDebug("PapyrusXInput::GetMappedControl - inputManager is null");
			return BSFixedString();
		}

		UInt32 buttonID;
		UInt32 deviceType;

		if (keyCode >= InputMap::kMacro_GamepadOffset)
		{
			buttonID = InputMap::GamepadKeycodeToMask(keyCode);
			deviceType = kDeviceType_Gamepad;
		}
		else if (keyCode >= InputMap::kMacro_MouseButtonOffset)
		{
			buttonID = keyCode - InputMap::kMacro_MouseButtonOffset;
			deviceType = kDeviceType_Mouse;
		}
		else
		{
			buttonID = keyCode;
			deviceType = kDeviceType_Keyboard;
		}

		return inputManager->GetMappedControl(buttonID, deviceType, InputManager::kContext_Gameplay);
	}
}

#include "skse64/PapyrusVM.h"
#include "skse64/PapyrusNativeFunctions.h"

bool papyrusXInput::RegisterFuncs(VMClassRegistry* registry)
{
	Log::Debug("papyrusXInput::RegisterFuncs called");
	registry->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, bool, UInt32> ("IsKeyPressed", "Input", papyrusXInput::IsKeyPressed, registry));

	registry->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, void, UInt32> ("TapKey", "Input", papyrusXInput::TapKey, registry));

	registry->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, void, UInt32> ("HoldKey", "Input", papyrusXInput::HoldKey, registry));

	registry->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, void, UInt32> ("ReleaseKey", "Input", papyrusXInput::ReleaseKey, registry));

	registry->RegisterFunction(
		new NativeFunction0 <StaticFunctionTag, UInt32> ("GetNumKeysPressed", "Input", papyrusXInput::GetNumKeysPressed, registry));

	registry->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, SInt32, UInt32> ("GetNthKeyPressed", "Input", papyrusXInput::GetNthKeyPressed, registry));


	//registry->RegisterFunction(
	//	new NativeFunction1 <StaticFunctionTag, bool, UInt32> ("IsKeyHeld", "Input", papyrusXInput::IsKeyHeld, registry));

	//registry->RegisterFunction(
	//	new NativeFunction1 <StaticFunctionTag, bool, UInt32> ("IsKeyDisabeld", "Input", papyrusXInput::IsKeyDisabled, registry));

	//registry->RegisterFunction(
	//	new NativeFunction1 <StaticFunctionTag, void, UInt32> ("DisableKey", "Input", papyrusXInput::DisableKey, registry));

	//registry->RegisterFunction(
	//	new NativeFunction1 <StaticFunctionTag, void, UInt32> ("EnableKey", "Input", papyrusXInput::EnableKey, registry));

	registry->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, SInt32, BSFixedString, UInt32> ("GetMappedKey", "Input", papyrusXInput::GetMappedKey, registry));

	registry->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, BSFixedString, SInt32> ("GetMappedControl", "Input", papyrusXInput::GetMappedControl, registry));

	registry->SetFunctionFlags("Input", "IsKeyPressed", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("Input", "TapKey", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("Input", "HoldKey", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("Input", "ReleaseKey", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("Input", "GetNumKeysPressed", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("Input", "GetNthKeyPressed", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("Input", "GetMappedKey", VMClassRegistry::kFunctionFlag_NoWait);
	registry->SetFunctionFlags("Input", "GetMappedControl", VMClassRegistry::kFunctionFlag_NoWait);

	return true;
}
