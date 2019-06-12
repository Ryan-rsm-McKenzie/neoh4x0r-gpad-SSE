#include "Hooks_XInput.h"
#include "xinput.h"
#include "skse64_common/SafeWrite.h"
#include "skse64_common/Utilities.h"
#include "windows.h"
#include "skse64/GameInput.h"
#include "skse64/NiExtraData.h"
#include "skse64/InputMap.h"

#include "IATUtils.h"
#include "PapyrusXInput.h"
#include "InputShimHook.h"
#include "papyrusController.h"
#include "Logger.h"

#include "gpadconf.h"
#include "vectors.h"
#include "CameraRotation.h"

#define XINPUT_1_3_DLL "xinput1_3.dll"

static DWORD _stdcall Hook_XInputGetState_Execute(DWORD dwUserIndex, XINPUT_STATE *pState)
{
	return XInputHookControl::GetSingleton().ProcessBufferedData(dwUserIndex, pState);
}


XInputHookControl* XInputHookControl::g_xiHookData = NULL;


XInputHookControl& XInputHookControl::GetSingleton(void)
{
	if (!g_xiHookData) {
		g_xiHookData = new XInputHookControl();
	}
	return *g_xiHookData;
}


void Hooks_XInput_Commit(void)
{
	uintptr_t XInputGetState = (uintptr_t)GetIATAddrEX((UInt8*)GetModuleHandle(NULL), XINPUT_1_3_DLL, "XInputGetState");

	if (XInputGetState) {
		XInputGetState_RealFunc = (XInputGetStateProc)*(uintptr_t*)XInputGetState;
		uintptr_t patch = (uintptr_t)Hook_XInputGetState_Execute;
		SafeWrite64(XInputGetState, patch);
		Log::Debug("hooked XInputGetState [0x%08x]->[0x%08x]", XInputGetState, patch);
	} else {
		Log::Error("Failed to hook XInputGetState");
	}
}


XInputHookControl::XInputHookControl()
{
	Log::Debug("Initializing XInputHookControl");
	this->g_padbuffer = new gPadBuffer();
	Log::Debug("Initialized XInputHookControl");
}


void XInputHookControl::ToggleGamepad()
{
	Setting * setting = GetINISetting("bGamepadEnable");
	if (!setting || setting->GetType() != Setting::kType_Bool) {
		Log::Error("XInputHookControl::ToggleGamepad() [bGamepadEnable not found]");
		return;
	}
	setting->data.u8 = setting->data.u8 >= 1 ? false : true;
}
bool XInputHookControl::UsingGamepad()
{
	return (InputEventDispatcher::GetSingleton() && InputEventDispatcher::GetSingleton()->IsGamepadEnabled());
}


#if ENABLE_MOUSE_MOVEMENT
void XInputHookControl::MoveMouseToPoint(float const& x, float const& y, float const& width, float const& height)
{
	this->g_padbuffer->MoveMouseToPoint(x, y, width, height);
}
#endif


bool XInputHookControl::IsGamepadKey(UInt32 key)
{
	return this->g_padbuffer->IsGamepadKey(key);
}


bool XInputHookControl::HoldKey(UInt32 key)
{
	if (!XInputHookControl::UsingGamepad()) {
		return false;
	} else {
		return this->g_padbuffer->push(key, true);
	}
}


bool XInputHookControl::ReleaseKey(UInt32 key)
{
	if (!XInputHookControl::UsingGamepad()) {
		return false;
	} else {
		return this->g_padbuffer->pop(key, true);
	}
}


bool XInputHookControl::DisableKey(UInt32 key)
{
	if (!XInputHookControl::UsingGamepad()) {
		return false;
	} else {
		return this->g_padbuffer->disable(key);
	}
}


bool XInputHookControl::EnableKey(UInt32 key)
{
	if (!XInputHookControl::UsingGamepad()) {
		return false;
	} else {
		return this->g_padbuffer->enable(key);
	}
}


bool XInputHookControl::IsKeyDisabled(UInt32 keycode)
{
	if (!XInputHookControl::UsingGamepad()) {
		return false;
	} else {
		return this->g_padbuffer->GetKeyDisabledState(keycode);
	}
}


bool XInputHookControl::IsKeyHeld(UInt32 keycode)
{
	if (!XInputHookControl::UsingGamepad()) {
		return false;
	} else {
		return this->g_padbuffer->GetKeyState(keycode);
	}
}


UInt32 XInputHookControl::GetNumKeysPressed()
{
#if 0
	//return InputShimHook::GetSingleton().GetNumKeysPressed();
	UInt32 numkeys = InputShimHook::GetSingleton().GetNumKeysPressed();
	for (int i = InputMap::kMacro_GamepadOffset; i < InputMap::kMaxMacros; i++) {
		if (this->g_padbuffer->GetKeyState(i)) {
			numkeys++;
		}
	}
	return numkeys;
#endif
	return InputShimHook::GetSingleton().GetNumKeysPressed();
}


SInt32 XInputHookControl::GetNthKeyPressed(UInt32 which)
{
#if 0
	return InputShimHook::GetSingleton().GetNthKeyPressed(which);
	UInt32 pressed_count = this->GetNumKeysPressed();
	if (pressed_count == 0) { return -1; }
	int KEY_COUNT = KEY_MAX + InputMap::kMacro_NumGamepadButtons;
	UINT pressed[KEY_MAX + InputMap::kMacro_NumGamepadButtons];
	memset(pressed, 0, KEY_COUNT);
	if (which < KEY_COUNT) {
		UInt32 current_key = 0;
		for (int i = 1; i < KEY_MAX; i++) {
			if (InputShimHook::GetSingleton().IsKeyPressed(i, false)) {
				pressed[current_key] = i;
				current_key++;
			}
		}
		for (int i = InputMap::kMacro_GamepadOffset; i < InputMap::kMaxMacros; i++) {
			if (this->g_padbuffer->GetKeyState(i)) {
				pressed[current_key] = i;
				current_key++;
			}
		}
		return which > pressed_count ? -1 : pressed[which];
	}
	return -1;
#endif
	return InputShimHook::GetSingleton().GetNthKeyPressed(which);
}


bool XInputHookControl::BufferedKeyTap(UInt32 key)
{
	if (!XInputHookControl::UsingGamepad()) {
		return false;
	} else if (XInputHookControl::BufferedKeyPress(key)) {
		return XInputHookControl::BufferedKeyRelease(key);
	} else {
		return false;
	}
}


bool XInputHookControl::BufferedKeyPress(UInt32 key)
{
	if (!XInputHookControl::UsingGamepad()) {
		return false;
	} else {
		return this->g_padbuffer->push(key, false);
	}
}


bool XInputHookControl::BufferedKeyRelease(UInt32 key)
{
	if (!XInputHookControl::UsingGamepad()) {
		return false;
	} else {
		return this->g_padbuffer->pop(key, false);
	}
}


HRESULT XInputHookControl::ProcessBufferedData(DWORD dwUserIndex, XINPUT_STATE * pState)
{
	return this->g_padbuffer->ProcessBufferedData(dwUserIndex, pState);
}


bool XInputHookControl::IsKeyPressed(UInt32 dxKeycode)
{
	if (!XInputHookControl::UsingGamepad()) {
		return false;
	} else {
		return this->g_padbuffer->GetKeyState(dxKeycode);
	}
}


XINPUT_STATE XInputHookControl::GetPadState()
{
	if (this->g_padbuffer) {
		return this->g_padbuffer->GetPadState();
	} else {
		return XINPUT_STATE();
	}
}


bool gPadBuffer::GetKeyDisabledState(UInt32 key)
{
	if (!IsGamepadKey(key)) {
		return false;
	}

	if (IsLeftTrigger(key)) {
		return (gTriggersDisabled & XINPUT_TRIGGER_KEYS::LEFT_TRIGGER) == XINPUT_TRIGGER_KEYS::LEFT_TRIGGER;
	} else if (IsRightTrigger(key)) {
		return (gTriggersDisabled & XINPUT_TRIGGER_KEYS::RIGHT_TRIGGER) == XINPUT_TRIGGER_KEYS::RIGHT_TRIGGER;
	} else {
		return (gButtonsDisabled & this->GetMappedButton(key)) == this->GetMappedButton(key);
	}
}


void gPadBuffer::PostKeyEventWait()
{
	PostKeyEventWait(POST_KEY_EVENT_WAIT());
}


void gPadBuffer::PostKeyEventWait(UInt32 interval)
{
	Sleep(interval);
}


bool gPadBuffer::IsKeyDown(UInt32 lookup_key, UInt32 key_data)
{
	if (lookup_key == 0xFFFFFFFF) {
		return false;
	}

	if (IsLeftTrigger(lookup_key) || IsRightTrigger(lookup_key)) {
		if ((IsLeftTrigger(lookup_key)) && (key_data > 0)) {
			return true;
		}
		if ((IsRightTrigger(lookup_key)) && (key_data > 0)) {
			return true;
		}
	}
	return (key_data & this->GetMappedButton(lookup_key)) == this->GetMappedButton(lookup_key);
}


bool gPadBuffer::GetKeyState(UInt32 key)
{
	if (key == 0xFFFFFFFF) {
		return false;
	}
	if (!XInputHookControl::UsingGamepad()) {
		return false;
	}

	const char* control_name = this->GetControlNameFromKey(key);
	if (this->IsExtendedControlName(control_name)) {
		const float p_zone = this->DeadZone();
		const SHORT sThumbLY = this->GetPadState().Gamepad.sThumbLY;
		const SHORT sThumbLX = this->GetPadState().Gamepad.sThumbLX;
		if ((sThumbLY >= -p_zone && sThumbLY <= p_zone) &&
			(sThumbLX >= -DeadZone() && sThumbLX <= p_zone)) {
			return false;
		}
		if (strcmp(control_name, Forward()) == 0) {
			if (sThumbLY > p_zone) {
				return true;
			} else {
				return false;
			}
		} else if (strcmp(control_name, Back()) == 0) {
			if (sThumbLY < -p_zone) {
				return true;
			} else {
				return false;
			}
		} else if (strcmp(control_name, StrafeLeft()) == 0) {
			if (sThumbLX < -p_zone) {
				return true;
			} else {
				return false;
			}
		} else if (strcmp(control_name, StrafeRight()) == 0) {
			if (sThumbLX > p_zone) {
				return true;
			} else {
				return false;
			}
		} else {
			return false;
		}
	}
	if (IsLeftTrigger(key) || IsRightTrigger(key)) {
		if ((IsLeftTrigger(key)) && (this->GetPadState().Gamepad.bLeftTrigger > 0)) {
			return true;
		}
		if ((IsRightTrigger(key)) && (this->GetPadState().Gamepad.bRightTrigger > 0)) {
			return true;
		}
	}
	return (this->GetPadState().Gamepad.wButtons & this->GetMappedButton(key)) == this->GetMappedButton(key);
}


bool gPadBuffer::disable(UInt32 key)
{
	if (!IsGamepadKey(key)) {
		return false;
	}

	if (IsLeftTrigger(key)) {
		gTriggersDisabled |= XINPUT_TRIGGER_KEYS::LEFT_TRIGGER;
	} else if (IsRightTrigger(key)) {
		gTriggersDisabled |= XINPUT_TRIGGER_KEYS::RIGHT_TRIGGER;
	} else {
		gButtonsDisabled |= this->GetMappedButton(key);
	}
	return true;
}


bool gPadBuffer::enable(UInt32 key)
{
	if (!IsGamepadKey(key)) {
		return false;
	}

	if (IsLeftTrigger(key)) {
		gTriggersDisabled &= ~XINPUT_TRIGGER_KEYS::LEFT_TRIGGER;
	} else if (IsRightTrigger(key)) {
		gTriggersDisabled &= ~XINPUT_TRIGGER_KEYS::RIGHT_TRIGGER;
	} else {
		gButtonsDisabled &= ~this->GetMappedButton(key);
	}
	return true;
}


bool gPadBuffer::push_extended(const char* control_name, bool bHeld)
{
	if (!gpadconf::ExtendedControlsEnabled()) {
		return false;
	}
	/*
		forward: gPadState.Gamepad.sThumbLY > DeadZone()
		back: gPadState.Gamepad.sThumbLY < -DeadZone()
		left: gPadState.Gamepad.sThumbLX < -DeadZone()
		right: gPadState.Gamepad.sThumbLX > DeadZone()
	*/
	UIStringHolder* ui_stringholder = UIStringHolder::GetSingleton();

	if (!XInputHookControl::UsingGamepad()) {
		return false;
	}
	if (strcmp(control_name, Forward()) == 0) {
		if (!this->IsTextInputActive()) {
			b__gsThumbLY = 1; gsThumbLY = THUMB_MAX();
		} return true;
	} else if (strcmp(control_name, Back()) == 0) {
		if (!this->IsTextInputActive()) {
			b__gsThumbLY = 1; gsThumbLY = THUMB_MIN();
		} return true;
	} else if (strcmp(control_name, StrafeLeft()) == 0) {
		if (!this->IsTextInputActive()) {
			b__gsThumbLX = 1; gsThumbLX = THUMB_MIN();
		} return true;
	} else if (strcmp(control_name, StrafeRight()) == 0) {
		if (!this->IsTextInputActive()) {
			b__gsThumbLX = 1; gsThumbLX = THUMB_MAX();
		} return true;
	} else if (strcmp(control_name, QuickInventory()) == 0 && ui_stringholder) {
		BSFixedString menuname = ui_stringholder->inventoryMenu;
		IsMenuOpen(menuname) ? CloseMenu(menuname) : OpenMenu(menuname);
		return true;
	} else if (strcmp(control_name, QuickMagic()) == 0 && ui_stringholder) {
		BSFixedString menuname = ui_stringholder->magicMenu;
		IsMenuOpen(menuname) ? CloseMenu(menuname) : OpenMenu(menuname);
		return true;
	} else if (strcmp(control_name, QuickMap()) == 0 && ui_stringholder) {
		BSFixedString menuname = ui_stringholder->mapMenu;
		IsMenuOpen(menuname) ? CloseMenu(menuname) : OpenMenu(menuname);
		return true;
	} else if (strcmp(control_name, QuickStats()) == 0 && ui_stringholder) {
		UInt32 key = this->GetKeyFromControlName(QuickStats());
		Log::VerboseDebug("gPadBuffer::push_extended [control_name=%s] [key=%d]", control_name, key);
		BSFixedString menuname = ui_stringholder->statsMenu;
		IsMenuOpen(menuname) ? CloseMenu(menuname) : OpenMenu(menuname);
		return true;
	} else if (strcmp(control_name, Console()) == 0 && ui_stringholder) {
		UInt32 key = this->GetKeyFromControlName(Console());
		Log::VerboseDebug("gPadBuffer::push_extended [control_name=%s] [key=%d]", control_name, key);
		BSFixedString menuname = ui_stringholder->console;
		IsMenuOpen(menuname) ? CloseMenu(menuname) : OpenMenu(menuname);
		return true;
	} else if (strcmp(control_name, Screenshot()) == 0) {
		UInt32 key = this->GetKeyFromControlName(Screenshot());
		Log::VerboseDebug("gPadBuffer::push_extended [control_name=%s] [key=%d]", control_name, key);
		if (key != -1) {
			if (!this->IsTextInputActive()) {
				!bHeld ?
					InputShimHook::GetSingleton().BufferedKeyTap(key, true) :
					InputShimHook::GetSingleton().BufferedKeyPress(key, true);
			}
			return true;
		} else {
			return false;
		}
	} else if (strcmp(control_name, MultiScreenshot()) == 0) {
		for (int i = 0; i < MULTI_SCREENSHOT_COUNT(); i++) {
			this->push_extended(Screenshot(), false); // bHeld will not work for taking multiple screenshots -- force to false
			Sleep(MULTI_SCREENSHOT_DELAY()); // wait 0.5 sec between taking screenshots
		}
		return true;
	} else if (strcmp(control_name, AutoMove()) == 0) {
		if (!this->IsTextInputActive()) {
			if (this->b__automove) {
				this->b__automove = 0; this->pop_extended(Forward(), bHeld);
			} else {
				this->b__automove = 1; this->push_extended(Forward(), bHeld);
			}
		}
		return true;
	} else {
		return false;
	}
}


bool gPadBuffer::push(UInt32 key, bool bHeld)
{
	if (!XInputHookControl::UsingGamepad()) {
		return false;
	}
	if (!this->IsGamepadKey(key)) {
		return false;
	}
	const char* control_name = this->GetControlNameFromKey(key);
	if (IsExtendedControlName(control_name)) {
		return this->push_extended(control_name, bHeld);
	}
	if (IsLeftTrigger(key)) {
		// have LT
		gLeftTrigger = 1;
		PostKeyEventWait(); // this must be called here, otherwise it will not work
	} else if (IsRightTrigger(key)) {
		// have RT
		gRightTrigger = 1;
		PostKeyEventWait(); // this must be called here, otherwise it will not work
	} else {
		// have button
		gButtons |= this->GetMappedButton(key);
		PostKeyEventWait(); // this must be called here, otherwise it will not work
	}
	return true;
}


bool gPadBuffer::pop_extended(const char* control_name, bool bHeld)
{
	if (!gpadconf::ExtendedControlsEnabled()) {
		return false;
	}
	/*
		forward: gPadState.Gamepad.sThumbLY > DeadZone()
		back: gPadState.Gamepad.sThumbLY < -DeadZone()
		left: gPadState.Gamepad.sThumbLX < -DeadZone()
		right: gPadState.Gamepad.sThumbLX > DeadZone()
	*/
	if (!XInputHookControl::UsingGamepad()) {
		return false;
	}
	if (strcmp(control_name, Forward()) == 0) {
		if (!this->IsTextInputActive()) {
			b__gsThumbLY = 0; gsThumbLY = 0;
		}return true;
	} else if (strcmp(control_name, Back()) == 0) {
		if (!this->IsTextInputActive()) {
			b__gsThumbLY = 0; gsThumbLY = 0;
		} return true;
	} else if (strcmp(control_name, StrafeLeft()) == 0) {
		if (!this->IsTextInputActive()) {
			b__gsThumbLX = 0; gsThumbLX = 0;
		} return true;
	} else if (strcmp(control_name, StrafeRight()) == 0) {
		if (!this->IsTextInputActive()) {
			b__gsThumbLX = 0; gsThumbLX = 0;
		} return true;
	} else if (strcmp(control_name, QuickInventory()) == 0) {
		return true;
	} else if (strcmp(control_name, QuickMagic()) == 0) {
		return true;
	} else if (strcmp(control_name, QuickMap()) == 0) {
		return true;
	} else if (strcmp(control_name, QuickStats()) == 0) {
		return true;
	} else if (strcmp(control_name, Console()) == 0) {
		return true;
	} else if (strcmp(control_name, Screenshot()) == 0) {
		return true;
	} else if (strcmp(control_name, MultiScreenshot()) == 0) {
		return true;
	} else if (strcmp(control_name, AutoMove()) == 0) {
		return true;
	} else {
		return false;
	}
}


bool gPadBuffer::pop(UInt32 key, bool bHeld)
{
	if (!XInputHookControl::UsingGamepad()) {
		return false;
	}

	if (!this->IsGamepadKey(key)) {
		return false;
	}
	const char* control_name = this->GetControlNameFromKey(key);
	if (IsExtendedControlName(control_name)) {
		return this->pop_extended(control_name, bHeld);
	}
	if (IsLeftTrigger(key)) {
		// have LT
		gLeftTrigger = 0;
		PostKeyEventWait(); // this must be called here, otherwise it will not work
	} else if (IsRightTrigger(key)) {
		// have RT
		gRightTrigger = 0;
		PostKeyEventWait(); // this must be called here, otherwise it will not work
	} else {
		// have button
		gButtons &= ~this->GetMappedButton(key);
		PostKeyEventWait(); // this must be called here, otherwise it will not work
	}
	return true;
}


bool gPadBuffer::IsExtendedControlName(const char* control_name)
{
	if (!gpadconf::ExtendedControlsEnabled()) {
		return false;
	}
	if (!control_name) {
		return false;
	}

	Log::VerboseDebug("gPadBuffer::IsExtendedControlName(%s)", control_name);
	if (strcmp(control_name, Forward()) == 0) {
		return true;
	} else if (strcmp(control_name, Back()) == 0) {
		return true;
	} else if (strcmp(control_name, StrafeLeft()) == 0) {
		return true;
	} else if (strcmp(control_name, StrafeRight()) == 0) {
		return true;
	} else if (strcmp(control_name, QuickInventory()) == 0) {
		return true;
	} else if (strcmp(control_name, QuickMagic()) == 0) {
		return true;
	} else if (strcmp(control_name, QuickMap()) == 0) {
		return true;
	} else if (strcmp(control_name, QuickStats()) == 0) {
		return true;
	} else if (strcmp(control_name, Console()) == 0) {
		return true;
	} else if (strcmp(control_name, Screenshot()) == 0) {
		return true;
	} else if (strcmp(control_name, MultiScreenshot()) == 0) {
		return true;
	} else if (strcmp(control_name, AutoMove()) == 0) {
		return true;
	} else {
		return false;
	}
}


UInt32 gPadBuffer::GetKeyFromControlName(const char * control_name)
{
	if (!control_name) {
		return -1;
	}
	BSFixedString name(control_name);
	UInt32 deviceType = kDeviceType_Auto;
	/* copied from: PapyrusInput.cpp::GetMappedKey */
	InputManager * inputManager = InputManager::GetSingleton();
	if (!inputManager) {
		return -1;
	}
	UInt32 key = 0xFF;
	if (XInputHookControl::UsingGamepad()) {
		deviceType = kDeviceType_Gamepad;
		key = inputManager->GetMappedKey(name, kDeviceType_Gamepad, InputManager::kContext_Gameplay);
	} else {
		deviceType = kDeviceType_Keyboard;
		key = inputManager->GetMappedKey(name, deviceType, InputManager::kContext_Gameplay);
		if (key == 0xFF) {
			deviceType = kDeviceType_Mouse;
			key = inputManager->GetMappedKey(name, deviceType, InputManager::kContext_Gameplay);
		}
		if (key == 0xFF) {
			return -1;
		}
	}
	if (key == 0xFF && deviceType == kDeviceType_Gamepad) {
		deviceType = kDeviceType_Keyboard; // default to keyboard so we can get the keyboard key
		key = inputManager->GetMappedKey(name, deviceType, InputManager::kContext_Gameplay);
		if (key == 0xFF) {
			return -1;
		}
	}

	// Map to common value space
	if (deviceType == kDeviceType_Mouse) {
		return key + InputMap::kMacro_MouseButtonOffset;
	} else if (deviceType == kDeviceType_Gamepad) {
		UInt32 mapped = InputMap::GamepadMaskToKeycode(key);
		return (mapped != InputMap::kMaxMacros ? mapped : -1);
	} else {
		return key;
	}
}


const char * gPadBuffer::GetControlNameFromKey(UInt32 key)
{
	BSFixedString name = BSFixedString();
	InputManager * inputManager = InputManager::GetSingleton();
	if (inputManager) {
		UInt32 buttonID;
		UInt32 deviceType;

		if (key >= InputMap::kMacro_GamepadOffset) {
			buttonID = GetMappedButton(key); deviceType = kDeviceType_Gamepad;
		} else if (key >= InputMap::kMacro_MouseButtonOffset) {
			buttonID = key - InputMap::kMacro_MouseButtonOffset; deviceType = kDeviceType_Mouse;
		} else {
			buttonID = key; deviceType = kDeviceType_Keyboard;
		}

		if (deviceType == kDeviceType_Gamepad && buttonID == 0xFF) {
			buttonID = key; deviceType = kDeviceType_Keyboard;
		}

		name = inputManager->GetMappedControl(buttonID, deviceType, InputManager::kContext_Gameplay);
	}
	return name.data;
}


bool gPadBuffer::IsExtendedControlKey(UInt32 key)
{
	if (!gpadconf::ExtendedControlsEnabled()) {
		return false;
	}
	if (!XInputHookControl::UsingGamepad()) {
		return false;
	}
	Log::VerboseDebug("gPadBuffer::IsExtendedControlKey(%i)", key);
	const char* control_name = GetControlNameFromKey(key);
	return this->IsExtendedControlName(control_name);
}


bool gPadBuffer::IsGamepadKey(UInt32 key)
{
	if (!XInputHookControl::UsingGamepad()) {
		return false;
	} // will return false if not using a gamepad
	Log::VerboseDebug("gPadBuffer::IsGamepadKey(%i)", key);
	if (key >= InputMap::kMacro_GamepadOffset && key < InputMap::kMaxMacros) {
		return true; // if key is in range return true
	}
	// check if we have an exteneded or 'virtual' key like Forward, Back, etc
	return this->IsExtendedControlKey(key);
}


bool gPadBuffer::IsLeftTrigger(UInt32 key)
{
	if (!XInputHookControl::UsingGamepad()) {
		return false;
	}
	if (key == InputMap::kGamepadButtonOffset_LT) {
		return true;
	}
	return false;
}


bool gPadBuffer::IsRightTrigger(UInt32 key)
{
	if (!XInputHookControl::UsingGamepad()) {
		return false;
	}
	if (key == InputMap::kGamepadButtonOffset_RT) {
		return true;
	}
	return false;
}


UInt32 gPadBuffer::GetMappedButton(UInt32 key)
{
	return InputMap::GamepadKeycodeToMask(key);
}


void gPadBuffer::ProcessMovementKeys(XINPUT_STATE const& pState)
{
	if (!gpadconf::SendMovementKeys()) {
		return;
	}
	if (!XInputHookControl::UsingGamepad()) {
		return;
	}
	SInt32 direction = papyrusController::GetDirection(papyrusController::kLeftStick);
	if (direction == this->Last_LSDirection) {
		return;
	}
	UInt32 keys[2];
	memset(keys, 0, 3);

	if (gpadconf::SendMajorMovementKeysOnly() &&
		(direction == papyrusController::kDiretion_upright ||
		direction == papyrusController::kDiretion_downright ||
		direction == papyrusController::kDiretion_downleft ||
		direction == papyrusController::kDiretion_upleft)) {
		return;
	}
	switch (direction) {
	case papyrusController::kDiretion_up:
		Log::VerboseDebug("gPadBuffer::ProcessMovementKeys -- Up");
		InputShimHook::GetSingleton().BufferedKeyTap(this->GetKeyFromControlName(Forward()), false);
		break;
	case papyrusController::kDiretion_upright:
		Log::VerboseDebug("gPadBuffer::ProcessMovementKeys -- Up/Right");
		keys[0] = this->GetKeyFromControlName(Forward());
		keys[1] = this->GetKeyFromControlName(StrafeRight());
		InputShimHook::GetSingleton().BufferedKeyTapEx(keys, 2, false);
		break;
	case papyrusController::kDiretion_right:
		Log::VerboseDebug("gPadBuffer::ProcessMovementKeys -- Right");
		InputShimHook::GetSingleton().BufferedKeyTap(this->GetKeyFromControlName(StrafeRight()), false);
		break;
	case papyrusController::kDiretion_downright:
		Log::VerboseDebug("gPadBuffer::ProcessMovementKeys -- Down/Right");
		keys[0] = this->GetKeyFromControlName(Back());
		keys[1] = this->GetKeyFromControlName(StrafeRight());
		InputShimHook::GetSingleton().BufferedKeyTapEx(keys, 2, false);
		break;
	case papyrusController::kDiretion_down:
		Log::VerboseDebug("gPadBuffer::ProcessMovementKeys -- Down");
		InputShimHook::GetSingleton().BufferedKeyTap(this->GetKeyFromControlName(Back()), false);
		break;
	case papyrusController::kDiretion_downleft:
		Log::VerboseDebug("gPadBuffer::ProcessMovementKeys -- Down/Left");
		keys[0] = this->GetKeyFromControlName(Back());
		keys[1] = this->GetKeyFromControlName(StrafeLeft());
		InputShimHook::GetSingleton().BufferedKeyTapEx(keys, 2, false);
		break;
	case papyrusController::kDiretion_left:
		Log::VerboseDebug("gPadBuffer::ProcessMovementKeys -- Left");
		InputShimHook::GetSingleton().BufferedKeyTap(this->GetKeyFromControlName(StrafeLeft()), false);
		break;
	case papyrusController::kDiretion_upleft:
		Log::VerboseDebug("gPadBuffer::ProcessMovementKeys -- Up/Left");
		keys[0] = this->GetKeyFromControlName(Forward());
		keys[1] = this->GetKeyFromControlName(StrafeLeft());
		InputShimHook::GetSingleton().BufferedKeyTapEx(keys, 2, false);
		break;
	}
	this->Last_LSDirection = direction;
}


void gPadBuffer::ProcessEvents()
{
	/* disabled for now */
#if !ENABLE_EVENT_PROCESSING
	return;
#else
	if (!XInputHookControl::UsingGamepad()) {
		return;
	}
	XINPUT_STATE state = this->GetPadState(); // create instance of the current state
	this->ProcessMovementKeys(state);
#pragma region old-code
	//	WORD buttons = state.Gamepad.wButtons;
	//	BYTE bleft = state.Gamepad.bLeftTrigger;
	//	BYTE bright = state.Gamepad.bRightTrigger;
	//	SHORT lx = state.Gamepad.sThumbLX;
	//	SHORT ly = state.Gamepad.sThumbLY;
	//	SHORT rx = state.Gamepad.sThumbRX;
	//	SHORT ry = state.Gamepad.sThumbRY;
	//
	//	if(buttons != this->xgpad.wButtons)
	//	{
	//		bool b = this->IsKeyDown(InputMap::kGamepadButtonOffset_B, buttons);
	//		//if(b) {
	//		//	InputShimHook::GetSingleton().BufferedKeyPress(InputMap::kGamepadButtonOffset_B);
	//		//}
	//		//else {
	//		//	InputShimHook::GetSingleton().BufferedKeyRelease(InputMap::kGamepadButtonOffset_B);
	//		//}
	//		//b = this->IsKeyDown(InputMap::kGamepadButtonOffset_Y, buttons);
	//		//if(b) {
	//		//	InputShimHook::GetSingleton().BufferedKeyPress(InputMap::kGamepadButtonOffset_Y);
	//		//}
	//		//else {
	//		//	InputShimHook::GetSingleton().BufferedKeyRelease(InputMap::kGamepadButtonOffset_Y);
	//		//}
	//		//b = this->IsKeyDown(InputMap::kGamepadButtonOffset_DPAD_UP, buttons);
	//		//b = this->IsKeyDown(InputMap::kGamepadButtonOffset_DPAD_DOWN, buttons);
	//		//b = this->IsKeyDown(InputMap::kGamepadButtonOffset_DPAD_LEFT, buttons);
	//		//b = this->IsKeyDown(InputMap::kGamepadButtonOffset_DPAD_RIGHT, buttons);
	//		//b = this->IsKeyDown(InputMap::kGamepadButtonOffset_START, buttons);
	//		//b = this->IsKeyDown(InputMap::kGamepadButtonOffset_BACK, buttons);
	//		//b = this->IsKeyDown(InputMap::kGamepadButtonOffset_LEFT_THUMB, buttons);
	//		//b = this->IsKeyDown(InputMap::kGamepadButtonOffset_RIGHT_THUMB, buttons);
	//		//b = this->IsKeyDown(InputMap::kGamepadButtonOffset_LEFT_SHOULDER, buttons);
	//		//b = this->IsKeyDown(InputMap::kGamepadButtonOffset_RIGHT_SHOULDER, buttons);
	//		//b = this->IsKeyDown(InputMap::kGamepadButtonOffset_A, buttons);
	//		//b = this->IsKeyDown(InputMap::kGamepadButtonOffset_X, buttons);
	//
	//	}
	//	if(bleft != this->xgpad.bLeftTrigger)
	//	{
	//		bool b = this->IsKeyDown(InputMap::kGamepadButtonOffset_LT, bleft);
	//		if(b)
	//		{
	//			// fire event LT down
	//		}
	//		else
	//		{
	//			// fire event LT up
	//		}
	//	}
	//	if(bright != this->xgpad.bRightTrigger)
	//	{
	//		bool b = this->IsKeyDown(InputMap::kGamepadButtonOffset_RT, bright);
	//		if(b)
	//		{
	//			// fire event RT down
	//		}
	//		else
	//		{
	//			// fire event RT up
	//		}
	//	}
	//	if(lx != this->xgpad.sThumbLX)
	//	{
	//	}
	//	if(ly != this->xgpad.sThumbLY)
	//	{
	//	}
	//	if(lx != this->xgpad.sThumbRX)
	//	{
	//	}
	//	if(ry != this->xgpad.sThumbRY)
	//	{
	//	}
	//
	//	this->xgpad.wButtons = buttons;
	//
	//	this->xgpad.bLeftTrigger = bleft;
	//	this->xgpad.bRightTrigger = bright;
	//
	//	this->xgpad.sThumbLX = lx;
	//	this->xgpad.sThumbLY = ly;
	//	this->xgpad.sThumbRX = rx;
	//	this->xgpad.sThumbRY = ry;
#pragma endregion
#endif
}


#if ENABLE_MOUSE_MOVEMENT
void gPadBuffer::MoveMouseToPoint(float const& x, float const& y, float const& width, float const& height)
{
	if (!XInputHookControl::UsingGamepad()) { return; }

	//float dx= (x * (float)THUMB_MAX());
	//float dy= (y * (float)THUMB_MAX());
	//Log::Console("MoveMouseToPoint(%f,%f) -> [%f,%f]",x,y,dx,dy);
	//this->b__MoveMouse = 1;
	//this->gsThumbRX = (SHORT)dx;
	//this->gsThumbRY = (SHORT)dy;
	//XINPUT_STATE state = this->GetPadState();
	//SHORT rx = state.Gamepad.sThumbRX;
	//SHORT ry = state.Gamepad.sThumbRY;
	//Log::Console("MoveMouseToPoint [rx:%d] [ry:%d]",rx,ry);

	//const float rad = atan2(y, x);
	//const float deg = rad * (180.f/(float)MATH_PI);
	//const int deg2 = (int)(deg + 360.f) % 360;
	//const float rad_2 = ((float)deg2 * (float)MATH_PI) / 180.f;
	//const float len = sqrt((x*x) + (y*y));
	////const float r = len * DeadZone();
	//const float r = len;
	//const float dx = r*cos(rad_2);
	//const float dy = r*sin(rad_2);
	//

	//const float rx= (dx*(float)THUMB_MAX()) * fMouseSensitivity();
	//const float ry= (dy*(float)THUMB_MAX()) * fMouseSensitivity();
	////Log::Console("MoveMouseToPoint [rx:%d] [ry:%d]",dx,dy);
	//Log::Console("MoveMouseToPoint [%f,%f] ([a:%d] [l:%f] [r:%f]) [%f,%f] -> [%f,%f]",x,y,deg2, len, r, dx, dy,rx,ry);
	//this->b__MoveMouse = 1;
	//this->gsThumbRX = (SHORT)rx;
	//this->gsThumbRY = (SHORT)ry;

	UIStringHolder* ui_stringholder = UIStringHolder::GetSingleton();
	if (ui_stringholder) {
		BSFixedString menuname = ui_stringholder->console;
		if (IsMenuOpen(menuname)) {
			return; // don't move the mouse while in console mode
		}
	}
	CameraRotation::TestCameraRotation();
}
#endif


HRESULT gPadBuffer::CreatePadState(DWORD dwUserIndex, XINPUT_STATE * pState)
{
	HRESULT hr = XInputGetState_RealFunc(dwUserIndex, pState);
	if (hr != ERROR_SUCCESS) {
		Log::VerboseDebug("gPadBuffer::CreatePadState::HRESULT %d", hr); return hr;
	}

	this->ProcessEvents();
	pState->Gamepad.wButtons |= (this->gButtons &= ~this->gButtonsDisabled);

	if ((this->gTriggersDisabled &  XINPUT_TRIGGER_KEYS::LEFT_TRIGGER) != XINPUT_TRIGGER_KEYS::LEFT_TRIGGER) {
		if (this->gLeftTrigger) {
			pState->Gamepad.bLeftTrigger = XINPUT_GAMEPAD_TRIGGER_MAX_THRESHOLD();
		}
	}
	if ((this->gTriggersDisabled &  XINPUT_TRIGGER_KEYS::RIGHT_TRIGGER) != XINPUT_TRIGGER_KEYS::RIGHT_TRIGGER) {
		if (this->gRightTrigger) {
			pState->Gamepad.bRightTrigger = XINPUT_GAMEPAD_TRIGGER_MAX_THRESHOLD();
		}
	}

	/* Setup joystick values */
	//if(!this->IsTextInputActive())
	//{
	if (this->b__gsThumbLX) {
		pState->Gamepad.sThumbLX = this->gsThumbLX;
	}
	if (this->b__gsThumbLY) {
		pState->Gamepad.sThumbLY = this->gsThumbLY;
	}
#if ENABLE_MOUSE_MOVEMENT
	if (this->b__MoveMouse) {
		pState->Gamepad.sThumbRX = this->gsThumbRX;
		pState->Gamepad.sThumbRY = this->gsThumbRY;
		this->b__MoveMouse = 0;
	}
#endif
	//}
	return hr;
}


HRESULT gPadBuffer::ProcessBufferedData(DWORD dwUserIndex, XINPUT_STATE * pState)
{
	return this->CreatePadState(dwUserIndex, pState);
}
