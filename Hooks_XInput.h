#pragma once

#include "skse64/GameTypes.h"
#include "skse64/InputMap.h"
#include <queue>
#include <xinput.h>
#include "skse64/GameMenus.h"
#include "skse64/PluginAPI.h"
#include "Logger.h"

enum
{
	kDeviceType_Auto = 0xff,
};


#define ENABLE_EVENT_PROCESSING 1 // 1=enabled (used by gPadBuffer::ProcessEvents)
#define ENABLE_MOUSE_MOVEMENT 0 //1=enabled allow mouse to move camera


void Hooks_XInput_Commit(void);


typedef DWORD(_stdcall * XInputGetStateProc)(DWORD, XINPUT_STATE *);
static XInputGetStateProc XInputGetState_RealFunc;


namespace XINPUT_TRIGGER_KEYS
{
	typedef enum
	{
		LEFT_TRIGGER = 0x0001,
		RIGHT_TRIGGER = 0x0002,
	};
}


typedef struct _gpad
{
	WORD	wButtons;
	BYTE	bLeftTrigger;
	BYTE	bRightTrigger;
	SHORT	sThumbLX;
	SHORT	sThumbLY;
	SHORT	sThumbRX;
	SHORT	sThumbRY;
}gpad;



struct gPadBuffer
{
public:
	gPadBuffer() : gButtons(0), gButtonsDisabled(0), gLeftTrigger(0), gRightTrigger(0), gTriggersDisabled(0),
		gsThumbLX(0), gsThumbLY(0), gsThumbRX(0), gsThumbRY(0),
		b__gsThumbLX(0), b__gsThumbLY(0),
		b__automove(0), xgpad()
	{
#if ENABLE_MOUSE_MOVEMENT
		b__MoveMouse = 0;
#endif
		Last_LSDirection = 0;
		Last_GamepadButtons = 0;
	}

	bool push(UInt32 key, bool bHeld);
	bool pop(UInt32 key, bool bHeld);

	bool push_extended(const char* control_name, bool bHeld);
	bool pop_extended(const char* control_name, bool bHeld);

	bool disable(UInt32 key);
	bool enable(UInt32 key);

	bool GetKeyDisabledState(UInt32 key);
	bool GetKeyState(UInt32 key);

	bool IsKeyDown(UInt32 lookup_key, UInt32 key_data);
	bool IsGamepadKey(UInt32 key);
	HRESULT ProcessBufferedData(DWORD dwUserIndex, XINPUT_STATE * pState);

#if ENABLE_MOUSE_MOVEMENT
	void MoveMouseToPoint(float const& x, float const& y, float const& width, float const& height);
#endif
	void ProcessEvents();
	void ProcessMovementKeys(XINPUT_STATE const& pState);
	inline XINPUT_STATE GetPadState()
	{
		XINPUT_STATE pState = { 0 };
		HRESULT hr = XInputGetState_RealFunc(0, &pState);
		if (hr != ERROR_SUCCESS) { Log::VerboseDebug("gPadBuffer::GetPadState()::HRESULT %d", hr); }
		return pState;
	}
private:
	const int XINPUT_GAMEPAD_TRIGGER_MAX_THRESHOLD(void) const { return 255; }
	const int POST_KEY_EVENT_WAIT(void) const { return 35; } // time to wait between key events (in milliseconds) to give the game-engine time to process the events
	const int MULTI_SCREENSHOT_COUNT(void) const { return 10; } // 10 screenshots (1s/screenshot*10 = 10 seconds of gameplay captured)
	const int MULTI_SCREENSHOT_DELAY(void) const { return 1000; } // a delay of 1s/screenshot

	const float fMouseSensitivity(void) const { return 1.f; }
	const SHORT THUMB_MAX(void) const { return 32767; }
	const SHORT THUMB_MIN(void) const { return -32767; }
	const float DeadZonePercent(void) const { return 0.75; }
	const float DeadZone(void) const { return (float)THUMB_MAX() * DeadZonePercent(); }

	HRESULT CreatePadState(DWORD dwUserIndex, XINPUT_STATE * pState);
	const char * GetControlNameFromKey(UInt32 key);
	UInt32 GetKeyFromControlName(const char * control_name);
	bool IsLeftTrigger(UInt32 key);
	bool IsRightTrigger(UInt32 key);
	UInt32 GetMappedButton(UInt32 key);
	void PostKeyEventWait();
	void PostKeyEventWait(UInt32 interval);
	bool IsExtendedControlKey(UInt32 key);
	bool IsExtendedControlName(const char* control_name);

	SInt32 Last_LSDirection;
	UInt32 Last_GamepadButtons;

	WORD			gButtons;
	WORD			gButtonsDisabled;
	BYTE			gTriggersDisabled;
	BYTE			gLeftTrigger;
	BYTE			gRightTrigger;

	BYTE			b__automove;

	BYTE			b__gsThumbLX;
	//BYTE			b__gsThumbRX;
	BYTE			b__gsThumbLY;
	//BYTE			b__gsThumbRY;
#if ENABLE_MOUSE_MOVEMENT
	BYTE			b__MoveMouse;
#endif
	// LEFT_STICK:  LEFT/RIGHT
	SHORT			gsThumbLX;
	// RIGHT_STICK: LEFT/RIGHT
	SHORT			gsThumbRX;

	// LEFT_STICK:  UP/DOWN
	SHORT			gsThumbLY;
	// RIGHT_STICK: UP/DOWN
	SHORT			gsThumbRY;

	gpad			xgpad;

#pragma region Help Functions
	inline bool IsMenuOpen(BSFixedString menuName)
	{
		MenuManager	* mm = MenuManager::GetSingleton();
		if (mm) { return mm->IsMenuOpen(&menuName); }
		return false;
	}
	inline bool IsTextInputActive()
	{
		/* check if we are in text input mode
		   if so, then we should not open any menus
		*/
		//Log::VerboseDebug("DEBUG::IsTextInputActive() called");
		InputManager* im = InputManager::GetSingleton();
		if (im) {
			//Log::VerboseDebug("DEBUG::IsTextInputActive() im->allowTextInput = %d", im->allowTextInput);
			return im->allowTextInput > 0 && im->allowTextInput < 0xff;
		}
		//else
		//{
		//	Log::VerboseDebug("DEBUG::IsTextInputActive() InputManager == null");
		//}
		return false;
	}


	inline void OpenMenu(BSFixedString menuName)
	{
		if (IsTextInputActive()) { return; }
		if (IsMenuOpen(menuName)) { return; }
		StringCache::Ref menuNameRef;
		CALL_MEMBER_FN(&menuNameRef, ctor)(menuName.data);
		CALL_MEMBER_FN(UIManager::GetSingleton(), AddMessage)(&menuNameRef, UIMessage::kMessage_Open, NULL);
		CALL_MEMBER_FN(&menuNameRef, Release)();
	}


	inline void CloseMenu(BSFixedString menuName)
	{
		//if (IsTextInputActive()) { return; }
		if (!IsMenuOpen(menuName)) { return; }
		StringCache::Ref menuNameRef;
		CALL_MEMBER_FN(&menuNameRef, ctor)(menuName.data);
		CALL_MEMBER_FN(UIManager::GetSingleton(), AddMessage)(&menuNameRef, UIMessage::kMessage_Close, NULL);
		CALL_MEMBER_FN(&menuNameRef, Release)();
	}
#pragma endregion

#pragma region extened control names
	const char* Forward(void) const { return "Forward"; }
	const char* Back(void) const { return "Back"; }
	const char* StrafeLeft(void) const { return "Strafe Left"; }
	const char* StrafeRight(void) const { return "Strafe Right"; }
	const char* QuickInventory(void) const { return "Quick Inventory"; }
	const char* QuickMagic(void) const { return "Quick Magic"; }
	const char* QuickMap(void) const { return "Quick Map"; }
	const char* QuickStats(void) const { return "Quick Stats"; }
	const char* Console(void) const { return "Console"; }
	const char* Screenshot(void) const { return "Screenshot"; }
	const char* MultiScreenshot(void) const { return "Multi-Screenshot"; }
	const char* AutoMove(void) const { return "Auto-Move"; }
#pragma endregion
};


class XInputHookControl //: public mISingleton <XInputHookControl>
{
public:
	static XInputHookControl& GetSingleton(void);

	XInputHookControl();
	bool HoldKey(UInt32 key);
	bool ReleaseKey(UInt32 key);

	bool DisableKey(UInt32 key);
	bool EnableKey(UInt32 key);

	bool IsKeyDisabled(UInt32 keycode);
	bool IsKeyHeld(UInt32 keycode);
	UInt32 GetNumKeysPressed();
	SInt32 GetNthKeyPressed(UInt32 which);

	bool BufferedKeyTap(UInt32 key);
	bool BufferedKeyPress(UInt32 key);
	bool BufferedKeyRelease(UInt32 key);
	bool IsKeyPressed(UInt32 dxKeycode);
	static bool UsingGamepad();
	static void ToggleGamepad();

	bool IsGamepadKey(UInt32 key);


	XINPUT_STATE GetPadState();

#if ENABLE_MOUSE_MOVEMENT
	void MoveMouseToPoint(float const& x, float const& y, float const& width, float const& height);
#endif

	HRESULT ProcessBufferedData(DWORD dwUserIndex, XINPUT_STATE * pState);
private:
	static XInputHookControl* g_xiHookData;
	gPadBuffer* g_padbuffer;
};
