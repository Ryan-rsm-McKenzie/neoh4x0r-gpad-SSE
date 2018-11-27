#pragma once
//#include "skse/Hooks_DirectInput8Create.h"
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "skse64/InputMap.h"
#include "common/ICriticalSection.h"

#define _KEY_DOWN				1
#define _KEY_UP					0

#define KEYEVENTF_KEYDOWN		0
#pragma region mouse button definitions
#define BUTTON_MOUSE_LEFT		256
#define BUTTON_MOUSE_RIGHT		257
#define BUTTON_MOUSE_WHEEL		258
#define BUTTON_MOUSE_NUM3		259
#define BUTTON_MOUSE_NUM4		260
#define BUTTON_MOUSE_NUM5		261
#define BUTTON_MOUSE_NUM6		262
#define BUTTON_MOUSE_NUM7		263
#define MOUSE_WHEEL_UP			264
#define MOUSE_WHEEL_DOWN		265
#pragma endregion

#define ALLOW_DISABLE_KEYS		0
#define USE_SENDINPUT			1 // otherwise use DIHookControl (which doesn't seem to work)
#define USE_HWND				1
#define FORCE_KEYBOARD_INPUT	0
#define MAX_MOUSE_BUTTONS		InputMap::kMacro_NumMouseButtons
#define KEY_MIN					InputMap::kMacro_KeyboardOffset // 1
#define KEY_MAX					InputMap::kMacro_NumKeyboardKeys //211
#define MOUSE_MIN				InputMap::kMacro_MouseButtonOffset //256
#define MOUSE_MAX				MOUSE_MIN + (InputMap::kMacro_NumMouseButtons + InputMap::kMacro_MouseWheelDirections) //265
#define GAMEPAD_MIN				InputMap::kMacro_GamepadOffset //266
#define GAMEPAD_MAX				InputMap::kMaxMacros //281
#define WINDOW_TITLE			"Skyrim Special Edition"
#define WINDOW_CLASS			"Skyrim Special Edition"

class InputShimHook
{
	public:
		static InputShimHook& GetSingleton(void);
		InputShimHook();

		//void test();

		//void	InitGamepad();


		bool	IsKeyHeld(UInt32 keycode, bool use_fallback);
		void	SetKeyHeldState(UInt32 keycode, bool bHold, bool use_fallback);

		UInt32	GetNumKeysPressed();
		UInt32	GetNumKBKeysPressed();
		UInt32	GetNumMSKeysPressed();
		SInt32	GetNthKeyPressed(UInt32 n);

		bool	IsKeyDisabled(UInt32 keycode); // unused
		void	SetKeyDisableState(UInt32 keycode, bool bDisable, UInt32 mask);  // unused

		bool	IsKeyPressed(UInt32 keycode, bool use_fallback);
		void	TapKey(UInt32 keycode, bool use_fallback);
		void	BufferedKeyTap(UInt32 key, bool use_fallback);
		void	BufferedKeyPress(UInt32 key, bool use_fallback);
		void	BufferedKeyRelease(UInt32 key, bool use_fallback);

		void	TapKeyEx(UInt32 key[], int count, bool use_fallback);
		void	BufferedKeyTapEx(UInt32 key[], int count, bool use_fallback);
		void	BufferedKeyPressEx(UInt32 key[], int count, bool use_fallback);
		void	BufferedKeyReleaseEx(UInt32 key[], int count, bool use_fallback);

		void	SetKBKeyState(UInt32 vkKey, bool key_up);
		void	SetMSKeyState(UInt32 mousekey, bool key_up);
		void	SetMSPosition(LONG dx, LONG dy);

		inline HWND const GetWindowHandle() {
			#if USE_HWND
				return this->m_hWnd;
			#else
				return NULL;
			#endif
		}
	private:
		static InputShimHook* g_isHookData;
		//bool bInitGamepad;
		//bool bGamepadEnabled;
		ICriticalSection	m_keyLock;
		BYTE m_kbkeys[GAMEPAD_MAX];
		DWORD GetInputType(UInt32 dxKeycode);
#if USE_SENDINPUT
		DWORD GetInputFlags(UInt32 dxKeycode, bool key_up);
		UInt32 GetVirtualKey(UInt32 dxKeycode);
		UInt32 GetScanCode(UInt32 vkKey);
#endif
		const int POST_KEY_EVENT_WAIT(void) const { return 35; }
		void PostKeyEventWait();
		void PostKeyEventWait(UInt32 interval);

#if USE_HWND
		HWND m_hWnd; /* unused ATM */
#endif
};
