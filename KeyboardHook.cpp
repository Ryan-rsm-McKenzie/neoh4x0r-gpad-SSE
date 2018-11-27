#include "KeyboardHook.h"
#include "gpadconf.h"
#include "InputShimHook.h"
#include "Hooks_XInput.h"
#include "Logger.h"

static HANDLE hThread;
static DWORD dwThread;
static HHOOK hKeyboardHook;
static HHOOK hMouseHook;
static bool bhookedkeyboardevent;
static bool bhookedmouseevent;

__declspec(dllexport) LRESULT CALLBACK KeyboardEvent (int nCode, WPARAM wParam, LPARAM lParam)
{
	if(!bhookedkeyboardevent)
	{
		Log::Debug("hooked keyboard event");
		bhookedkeyboardevent=true;
	}
	if((nCode == HC_ACTION) &&
		(
		(wParam == WM_SYSKEYDOWN) || (wParam == WM_SYSKEYUP) ||
		(wParam == WM_KEYDOWN) || (wParam == WM_KEYUP)))
	{
		KBDLLHOOKSTRUCT* key = (KBDLLHOOKSTRUCT*)lParam;
		if(key)
		{
			if((wParam == WM_SYSKEYDOWN) || (wParam == WM_KEYDOWN))
			{
				Log::VerboseDebug("KeyDown: [sc:%d] [vk:%d] [f:0x%08x] ", key->scanCode, key->vkCode, key->flags);
				InputShimHook::GetSingleton().SetKBKeyState(key->scanCode,false);
			}
			else if ((wParam == WM_SYSKEYUP) || (wParam == WM_KEYUP))
			{
				Log::VerboseDebug("KeyUp: [sc:%d] [vk:%d] [f:0x%08x] ", key->scanCode, key->vkCode, key->flags);
				InputShimHook::GetSingleton().SetKBKeyState(key->scanCode,true);
			}

		}
	}
    return CallNextHookEx(hKeyboardHook,nCode,wParam,lParam);
}
__declspec(dllexport) LRESULT CALLBACK MouseEvent (int nCode, WPARAM wParam, LPARAM lParam)
{
	if(!bhookedmouseevent)
	{
		Log::Debug("hooked mouse event");
		bhookedmouseevent=true;
	}
	const int LMB = 256;
	const int RMB = 257;
	const int MMB = 258;
	//const int MB3 = 259;
	//const int MB4 = 260;
	//const int MB5 = 261;
	//const int MB6 = 262;
	//const int MB7 = 263;

	MOUSEHOOKSTRUCT * pMouseStruct = (MOUSEHOOKSTRUCT *)lParam;
	if(pMouseStruct)
	{
		LONG dx = pMouseStruct->pt.x;
		LONG dy = pMouseStruct->pt.y;
		//pMouseStruct->dwExtraInfo
		// do something with the data
		if(wParam == WM_MOUSEMOVE)
		{
			InputShimHook::GetSingleton().SetMSPosition(dx,dy);
		}
		if((wParam == WM_LBUTTONDOWN) || (wParam == WM_LBUTTONUP))
		{
			Log::VerboseDebug("MouseEvent: [sc:%d] [down:%s]", LMB, (wParam == WM_LBUTTONDOWN)? "true": "false");
			InputShimHook::GetSingleton().SetMSKeyState(LMB, (wParam == WM_LBUTTONUP));
		}
		else if((wParam == WM_RBUTTONDOWN) || (wParam == WM_RBUTTONUP))
		{
			Log::VerboseDebug("MouseEvent: [sc:%d] [down:%s]", RMB, (wParam == WM_RBUTTONDOWN)? "true": "false");
			InputShimHook::GetSingleton().SetMSKeyState(RMB, (wParam == WM_RBUTTONUP));
		}
		else if((wParam == WM_MBUTTONDOWN) || (wParam == WM_MBUTTONUP))
		{
			Log::VerboseDebug("MouseEvent: [sc:%d] [down:%s]", MMB, (wParam == WM_MBUTTONDOWN)? "true": "false");
			InputShimHook::GetSingleton().SetMSKeyState(MMB, (wParam == WM_MBUTTONUP));
		}
	}
		return CallNextHookEx(hMouseHook,nCode,wParam,lParam);
}
void WINAPIV KeyboardHook::InstallHook()
{
	Log::Debug("installing input hooks");

	hThread = NULL;
	dwThread = NULL;
	hKeyboardHook = NULL;
	hMouseHook = NULL;
	bhookedkeyboardevent=false;
	bhookedmouseevent=false;

	 hThread = CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)
    CreateHook, NULL, NULL, &dwThread);
}

void WINAPIV KeyboardHook::CreateHook()
{
	//HWND hwnd = InputShimHook::GetSingleton().GetWindowHandle();
	DWORD dwThreadId = NULL;

	Log::Debug("KeyboardHook::CreateHook() called");
	HINSTANCE hInstance = GetModuleHandle(NULL);
	if(!(hKeyboardHook = SetWindowsHookEx (
		WH_KEYBOARD_LL,
		(HOOKPROC) KeyboardEvent,
		hInstance,
		dwThreadId)))
	{
		DWORD error = GetLastError();
		Log::Error("KeyboardHook::CreateHook() failed to install keyboard hook [ERROR:0x%08x]", error);
		return;
	}

	if(!(hMouseHook = SetWindowsHookEx (
		WH_MOUSE_LL,
		(HOOKPROC) MouseEvent,
		hInstance,
		dwThreadId)))
	{
		DWORD error = GetLastError();
		Log::Error("KeyboardHook::CreateHook() failed to install mouse hook [ERROR:0x%08x]", error);
		return;
	}

	Log::Debug("KeyboardHook::CreateHook() installed hooks... stating mesage loop");
	MSG message;
    while (GetMessage(&message,NULL,0,0)) {
        TranslateMessage( &message );
        DispatchMessage( &message );
    }
	Log::Debug("KeyboardHook::CreateHook() mesage loop exit");
	UnhookWindowsHookEx(hKeyboardHook);
	UnhookWindowsHookEx(hMouseHook);
}
