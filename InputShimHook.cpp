#include "InputShimHook.h"
#include "skse64/Hooks_DirectInput8Create.h"
//#include "skse/PapyrusInput.h"
#include "Hooks_XInput.h"
#include "xinput.h"

InputShimHook* InputShimHook::g_isHookData	= NULL;

InputShimHook& InputShimHook::GetSingleton(void)
{
	if (!g_isHookData)
		g_isHookData = new InputShimHook();
	return *g_isHookData;
}

void InputShimHook::PostKeyEventWait() { PostKeyEventWait(POST_KEY_EVENT_WAIT()); }
void InputShimHook::PostKeyEventWait(UInt32 interval) { Sleep(interval); }

InputShimHook::InputShimHook()
{
	Log::Debug("Initializing InputShimHook");
	#if USE_HWND
		this->m_hWnd = FindWindow(WINDOW_CLASS,WINDOW_TITLE);
		if(this->m_hWnd != NULL)
		{
			Log::Debug("Found skyrim window handle [0x%08x]", this->m_hWnd);
		}
		else
		{
			Log::Error("Could not find skyrim window handle");
		}
	#endif
	memset(this->m_kbkeys,0,GAMEPAD_MAX);
	//bGamepadEnabled = false;
	//bInitGamepad = false;
	Log::Debug("Initialized InputShimHook");
}
//void InputShimHook::InitGamepad()
//{
//	if(bInitGamepad) { return; }
//	bGamepadEnabled = XInputHookControl::UsingGamepad();
//	bInitGamepad = true;
//}
void InputShimHook::SetKBKeyState(UInt32 scancode, bool key_up)
{
	if(scancode < 1 || scancode > GAMEPAD_MAX)
	{
		Log::Error("InputShimHook::SetKBKeyState() invalid key [scancode:%d]", scancode);
		return;
	}
	IScopedCriticalSection lock(&m_keyLock);
	this->m_kbkeys[scancode] = key_up ? _KEY_UP : _KEY_DOWN;

	///*
	//	#define INPUT_MOUSE     0
	//	#define INPUT_KEYBOARD  1
	//	#define INPUT_HARDWARE  2
	//*/
	//DWORD input_type = this->GetInputType(scancode);
	//if(!key_up &&
	//	(XInputHookControl::UsingGamepad() &&
	//	(input_type == INPUT_KEYBOARD) ||
	//	(input_type == INPUT_MOUSE)))
	//{
	//	Log::VerboseDebug("InputShimHook::SetKBKeyState -- disabling xinput for key: %d", scancode);
	//	XInputEnable(false);
	//}
	//else if(key_up && this->bGamepadEnabled)
	//{
	//	Log::VerboseDebug("InputShimHook::SetKBKeyState -- enabling xinput for key: %d", scancode);
	//	XInputEnable(true);
	//}
}

void InputShimHook::SetMSPosition(LONG dx, LONG dy)
{
	#if !ENABLE_MOUSE_MOVEMENT
		return;
	#else
	if(!XInputHookControl::UsingGamepad()) { return; }
	RECT rect;
	if(GetWindowRect(this->m_hWnd, &rect))
	{
	//	//LONG width = rect.right - rect.left;
	//	//LONG height = rect.bottom - rect.top;
	//	//double px = ((double)dx / (double)width);
	//	//double py = ((double)dy / (double)height);
	//	//Log::VerboseDebug("nputShimHook::SetMSPosition(%d,%d) [w:%d] [h:%d] [px:%f] [py:%f]",dx,dy, width, height, px, py);
		const float fScreenWidth = (float)(rect.right - rect.left);
		const float fScreenHeight = (float)(rect.bottom - rect.top);


		const LONG cx = abs(dx);
		const LONG cy = abs(dy);
		//const float px = (float)((float)cx / (float)fScreenWidth);
		//const float py = (float)((float)cy / (float)fScreenHeight);
		const float px = ((float)cx - (fScreenWidth / 2.f)) / (fScreenWidth / 2.f);
		const float py = (-((float)cy - (fScreenHeight / 2.f))) / (fScreenHeight / 2.f);
		XInputHookControl::GetSingleton().MoveMouseToPoint((float)px,(float)py, fScreenWidth ,fScreenHeight);
	}
	else
	{
		Log::VerboseDebug("InputShimHook::SetMSPosition(%d,%d) -- could not get window rect",dx,dy);
	}
#endif
}
void InputShimHook::SetMSKeyState(UInt32 mousekey, bool key_up)
{
	this->SetKBKeyState(mousekey,key_up);
}
DWORD InputShimHook::GetInputType(UInt32 dxKeycode)
{
	/*
		#define INPUT_MOUSE     0
		#define INPUT_KEYBOARD  1
		#define INPUT_HARDWARE  2
	*/
	DWORD input_type = INPUT_KEYBOARD;
#if !FORCE_KEYBOARD_INPUT
	if(dxKeycode > KEY_MIN && dxKeycode < KEY_MAX) { input_type = INPUT_KEYBOARD; }
	else if(dxKeycode >= MOUSE_MIN && dxKeycode < MOUSE_MAX) { input_type = INPUT_MOUSE; }
	else if(dxKeycode >= GAMEPAD_MIN && dxKeycode < GAMEPAD_MAX) { input_type = INPUT_HARDWARE; }
	else
	{
		Log::VerboseDebug("InputShimHook::GetInputType() -- could not determine input type for [key:0x%08x] defaulting to INPUT_KEYBOARD", dxKeycode);
	}
#endif

	Log::VerboseDebug("InputShimHook::GetInputType(dxKeycode:%i) INPUT_TYPE = %s", dxKeycode, input_type == INPUT_KEYBOARD ? "INPUT_KEYBOARD" : input_type == INPUT_MOUSE ? "INPUT_MOUSE" : input_type == INPUT_HARDWARE ? "INPUT_HARDWARE" : "UNKNOWN");
return input_type;
}
#if USE_SENDINPUT
UInt32 InputShimHook::GetVirtualKey(UInt32 dxKeycode)
{
	UINT vk = MapVirtualKey(dxKeycode,MAPVK_VSC_TO_VK_EX);
	return vk;
}
UInt32 InputShimHook::GetScanCode(UInt32 vkKey)
{
	UInt32 scan = MapVirtualKey(vkKey,MAPVK_VK_TO_VSC);
	return scan;
}
DWORD InputShimHook::GetInputFlags(UInt32 dxKeycode, bool key_up)
{
	Log::VerboseDebug("InputShimHook::GetInputFlags(dxKeycode:%i,key_up:%s)", dxKeycode, key_up ? "true" : "false");
	DWORD flags = 0;
#if !FORCE_KEYBOARD_INPUT
	DWORD input_type = this->GetInputType(dxKeycode);
	switch(input_type)
	{
	case INPUT_KEYBOARD:
		//flags = key_up ? KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE : KEYEVENTF_KEYDOWN | KEYEVENTF_SCANCODE;
		flags |= KEYEVENTF_SCANCODE;
		flags |= key_up ? KEYEVENTF_KEYUP : KEYEVENTF_KEYDOWN;
		break;
	case INPUT_MOUSE:
		switch(dxKeycode)
		{
			case BUTTON_MOUSE_LEFT: flags |= key_up ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_LEFTDOWN; break;
			case BUTTON_MOUSE_RIGHT: flags |= key_up ? MOUSEEVENTF_RIGHTUP : MOUSEEVENTF_RIGHTDOWN; break;
			case BUTTON_MOUSE_WHEEL: flags |= key_up ? MOUSEEVENTF_MIDDLEUP : MOUSEEVENTF_MIDDLEDOWN; break;
			case MOUSE_WHEEL_UP:
				Log::Warn("InputShimHook::GetInputFlags() -- [key:0x%08x] [MOUSE_WHEEL_UP] is not supported yet", dxKeycode);
				flags |= key_up ? KEYEVENTF_KEYUP : KEYEVENTF_KEYDOWN;
				break;
			case MOUSE_WHEEL_DOWN:
				Log::Warn("InputShimHook::GetInputFlags() -- [key:0x%08x] [MOUSE_WHEEL_DOWN] is not supported yet", dxKeycode);
				flags |= key_up ? KEYEVENTF_KEYUP : KEYEVENTF_KEYDOWN;
				break;
			default:
				Log::Warn("InputShimHook::GetInputFlags() -- [key:0x%08x] not handled for INPUT_MOUSE", dxKeycode);
				flags |= key_up ? KEYEVENTF_KEYUP : KEYEVENTF_KEYDOWN;
		}
		break;
	case INPUT_HARDWARE:
		Log::VerboseDebug("InputShimHook::GetInputFlags() -- INPUT_HARDWARE is not supported [key:0x%08x]", dxKeycode);
		//flags |= key_up ? KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE : KEYEVENTF_KEYDOWN | KEYEVENTF_SCANCODE;
		flags |= key_up ? WM_KEYUP : WM_KEYDOWN;
		break;
	default:
		//flags |= key_up ? KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE : KEYEVENTF_KEYDOWN | KEYEVENTF_SCANCODE;
		flags |= key_up ? KEYEVENTF_KEYUP : KEYEVENTF_KEYDOWN;
	}
#else
	//flags |= key_up ? KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE : KEYEVENTF_KEYDOWN | KEYEVENTF_SCANCODE;
	flags |= key_up ? KEYEVENTF_KEYUP : KEYEVENTF_KEYDOWN;
#endif
	return flags;
}
#endif
bool InputShimHook::IsKeyPressed(UInt32 keycode, bool use_fallback)
{

	//Log::Warn("STUB :: InputShimHook::IsKeyPressed(%d) -- not implemented", keycode);

	DWORD input_type = this->GetInputType(keycode);

	if (input_type == INPUT_KEYBOARD)
	{
		//UInt32 vk = this->GetVirtualKey(keycode);
		//if(vk == 0)
		//{
		//	Log::VerboseDebug("STUB :: InputShimHook::IsKeyPressed(%d) -- does not map to a virtual key", keycode);
		//	return false;
		//}
		if(keycode < 1 || keycode > KEY_MAX)
		{
			Log::Error("InputShimHook::IsKeyPressed() invalid key [keycode:%d]", keycode);
			return false;
		}
		IScopedCriticalSection lock(&m_keyLock);
		bool down = this->m_kbkeys[keycode] == _KEY_DOWN;
		Log::VerboseDebug("InputShimHook::IsKeyPressed() [sc:%d] [down:%s]", keycode, down ? "true" : "false");
		return down;
	}
	else if (input_type == INPUT_MOUSE)
	{
		//UInt32 index = keycode - MOUSE_MIN;
		//if(index < 0 || index > InputMap::kMacro_NumMouseButtons)
		//{
		//	Log::Error("InputShimHook::IsKeyPressed() invalid key [mouse button:%d]", keycode);
		//	return false;
		//}
		IScopedCriticalSection lock(&m_keyLock);
		bool down = this->m_kbkeys[keycode] == _KEY_DOWN;
		Log::VerboseDebug("InputShimHook::IsKeyPressed() [sc:%d] [down:%s]", keycode, down ? "true" : "false");
		return down;
	}
	else
	{
		Log::VerboseDebug("InputShimHook::IsKeyPressed() -- unsupported input_type:%d", input_type);
		return false;
	}
}
bool InputShimHook::IsKeyDisabled(UInt32 keycode)
{
	Log::VerboseDebug("STUB :: InputShimHook::IsKeyDisabled(%d) -- ignored", keycode);
	return false;
}
bool InputShimHook::IsKeyHeld(UInt32 keycode, bool use_fallback)
{
	//Log::Warn("STUB :: InputShimHook::IsKeyHeld(%d) -- not implemented", keycode);
	//return false;
	return this->IsKeyPressed(keycode, use_fallback);
}
UInt32 InputShimHook::GetNumKBKeysPressed()
{
	UInt32 numkeys = 0;
	IScopedCriticalSection lock(&m_keyLock);
	for(int i=0;i<GAMEPAD_MAX;i++)
	{
		if(this->m_kbkeys[i] == _KEY_DOWN)
		{
			numkeys++;
		}
	}
	return numkeys;
}
UInt32 InputShimHook::GetNumMSKeysPressed()
{
	UInt32 numkeys = 0;
	IScopedCriticalSection lock(&m_keyLock);
	for(int i=0;i<MAX_MOUSE_BUTTONS;i++)
	{
		if(this->m_kbkeys[i+InputMap::kMacro_MouseButtonOffset] == _KEY_DOWN)
		{
			numkeys++;
		}
	}
	return numkeys;
}
UInt32 InputShimHook::GetNumKeysPressed()
{
	//Log::Warn("STUB :: InputShimHook::GetNumKeysPressed() -- not implemented");
	UInt32 numkeys = 0;
	numkeys = numkeys + this->GetNumKBKeysPressed();
	//numkeys = numkeys + this->GetNumMSKeysPressed();
	return numkeys;
}
SInt32 InputShimHook::GetNthKeyPressed(UInt32 n)
{
	//Log::Warn("STUB :: InputShimHook::GetNthKeyPressed(%d) -- not implemented", n);
	// return the dxscancode at index n
	// if nothing is pressed return -1

	IScopedCriticalSection lock(&m_keyLock);

	UInt32 pressed_count = this->GetNumKBKeysPressed();
	if(pressed_count == 0) { return -1; }

	BYTE temp[GAMEPAD_MAX];
	for(int i=0;i<GAMEPAD_MAX;i++) { temp[i] = this->m_kbkeys[i]; }

	BYTE pressed[GAMEPAD_MAX];
	memset(pressed,0,GAMEPAD_MAX);
	if(n < KEY_MAX)
	{
		UInt32 current_key=0;
		// get all presed keys
		for(int i=0;i<GAMEPAD_MAX;i++)
		{
			if(temp[i] == _KEY_DOWN)
			{
				pressed[current_key] = i; // store the index (scancode) 0 to 256
				current_key++;
			}
		}

		//UInt32 scancode = this->GetScanCode(pressed[n]);
		return n > pressed_count ? -1 : pressed[n];
	}
	// skipping check of mouse keys
	return -1;
}
void InputShimHook::SetKeyDisableState(UInt32 keycode, bool bDisable, UInt32 mask)
{
	Log::Warn("STUB :: InputShimHook::SetKeyDisableState(%d,%s,%d) -- ignored", keycode, bDisable ? "true" : "false", mask);
}
void InputShimHook::SetKeyHeldState(UInt32 keycode, bool bHold, bool use_fallback)
{
	!bHold ? this->BufferedKeyRelease(keycode,use_fallback) : this->BufferedKeyPress(keycode,use_fallback);
}
void InputShimHook::TapKey(UInt32 keycode, bool use_fallback)
{
	this->BufferedKeyTap(keycode,use_fallback);
}
void InputShimHook::BufferedKeyTap(UInt32 key, bool use_fallback)
{
	Log::VerboseDebug("InputShimHook::BufferedKeyTap [keycode=%d]", key);
	UInt32 keys[1];
	keys[0] = key;
	this->BufferedKeyTapEx(keys,1,use_fallback);
}
void InputShimHook::BufferedKeyPress(UInt32 key, bool use_fallback)
{
	Log::VerboseDebug("InputShimHook::BufferedKeyPress [keycode=%d]", key);
	UInt32 keys[1];
	keys[0] = key;
	this->BufferedKeyPressEx(keys,1,use_fallback);
}
void InputShimHook::BufferedKeyRelease(UInt32 key, bool use_fallback)
{
	Log::VerboseDebug("InputShimHook::BufferedKeyRelease [keycode=%d]", key);
	UInt32 keys[1];
	keys[0] = key;
	this->BufferedKeyReleaseEx(keys,1,use_fallback);
}
void InputShimHook::TapKeyEx(UInt32 key[], int count, bool use_fallback)
{
	this->BufferedKeyTapEx(key,count,use_fallback);
}
void InputShimHook::BufferedKeyTapEx(UInt32 key[], int count, bool use_fallback)
{
#if USE_SENDINPUT
	this->BufferedKeyPressEx(key,count,use_fallback);
	this->PostKeyEventWait();
	this->BufferedKeyReleaseEx(key,count,use_fallback);
#else
	for(int i=0;i<count;i++)
	{
		UInt32 scan = key[i];
		DIHookControl::GetSingleton().BufferedKeyTap(scan);
	}
#endif
}
void InputShimHook::BufferedKeyPressEx(UInt32 key[], int count, bool use_fallback)
{
#if USE_SENDINPUT
	INPUT* ip = new INPUT[count];
	memset(ip,0,count);
	for(int i=0;i<count;i++)
	{
		UInt32 scan = key[i];
		if(scan < KEY_MIN || scan > GAMEPAD_MAX)
		{
			Log::VerboseDebug("InputShimHook::BufferedKeyPressEx ingored invalid key [keycode=%d] [i:%d]", scan, i);
			continue;
		}
		Log::VerboseDebug("InputShimHook::BufferedKeyPressEx [keycode=%d] [i:%d]", scan, i);
		DWORD input_type = this->GetInputType(scan);
		//if( input_type != INPUT_KEYBOARD)
		//{
		//	Log::Warn("InputShimHook::BufferedKeyPressEx() -- input type [%d] for key: [%d] is not supported", input_type,scan );
		//	continue;
		//}
		DWORD input_flags = this->GetInputFlags(scan, false);
		//keybd_event(0,(BYTE)scan,input_flags,0);
		ip[i].type = input_type;
		switch(input_type)
		{
			case INPUT_KEYBOARD:
				ip[i].ki.time = 0;
				ip[i].ki.wVk = 0;
				ip[i].ki.wScan = scan;
				ip[i].ki.dwExtraInfo = GetMessageExtraInfo();
				ip[i].ki.dwFlags = input_flags;
				break;
			case INPUT_MOUSE:
				ip[i].mi.time = 0;
				ip[i].mi.mouseData = 0;
				ip[i].mi.dx = 0;
				ip[i].mi.dy = 0;
				ip[i].mi.dwExtraInfo = GetMessageExtraInfo();
				ip[i].mi.dwFlags = input_flags;
				break;
			case INPUT_HARDWARE:
				//ip[i].hi.wParamL = 1;
				//ip[i].hi.wParamH = 0;
				//ip[i].hi.uMsg = input_flags;
				Log::Warn("InputShimHook::BufferedKeyPressEx() -- INPUT_HARDWARE is unsupported");
				break;
		}
	}
	SendInput(count, ip, sizeof(INPUT));
	delete[] ip;
#else
	for(int i=0;i<count;i++)
	{
		UInt32 scan = key[i];
		DIHookControl::GetSingleton().BufferedKeyPress(scan);
	}
#endif
}
void InputShimHook::BufferedKeyReleaseEx(UInt32 key[], int count, bool use_fallback)
{
#if USE_SENDINPUT
	INPUT* ip = new INPUT[count];
	memset(ip,0,count);
	for(int i=0;i<count;i++)
	{
		UInt32 scan = key[i];
		if(scan < KEY_MIN || scan > GAMEPAD_MAX)
		{
			Log::VerboseDebug("InputShimHook::BufferedKeyReleaseEx ingored invalid key [keycode=%d] [i:%d]", scan, i);
			continue;
		}
		Log::VerboseDebug("InputShimHook::BufferedKeyReleaseEx [keycode=%d] [i:%d]", scan, i);
		DWORD input_type = this->GetInputType(scan);
		//if( input_type != INPUT_KEYBOARD)
		//{
		//	Log::Warn("InputShimHook::BufferedKeyReleaseEx() -- input type [%d] for key: [%d] is not supported", input_type,scan );
		//	continue;
		//}
		DWORD input_flags = this->GetInputFlags(scan, true);
		//keybd_event(0,(BYTE)scan,input_flags,0);
		ip[i].type = input_type;
		switch(input_type)
		{
			case INPUT_KEYBOARD:
				ip[i].ki.time = 0;
				ip[i].ki.wVk = 0;
				ip[i].ki.wScan = scan;
				ip[i].ki.dwExtraInfo = GetMessageExtraInfo();
				ip[i].ki.dwFlags = input_flags;
				break;
			case INPUT_MOUSE:
				ip[i].mi.time = 0;
				ip[i].mi.mouseData = 0;
				ip[i].mi.dx = 0;
				ip[i].mi.dy = 0;
				ip[i].mi.dwExtraInfo = GetMessageExtraInfo();
				ip[i].mi.dwFlags = input_flags;
				break;
			case INPUT_HARDWARE:
				//ip[i].hi.wParamL = 1;
				//ip[i].hi.wParamH = 0;
				//ip[i].hi.uMsg = input_flags;
				Log::Warn("InputShimHook::BufferedKeyPressEx() -- INPUT_HARDWARE is unsupported");
				break;
		}
	}
	SendInput(count, ip, sizeof(INPUT));
	delete[] ip;
#else
	for(int i=0;i<count;i++)
	{
		UInt32 scan = key[i];
		DIHookControl::GetSingleton().BufferedKeyRelease(scan);
	}
#endif

}
