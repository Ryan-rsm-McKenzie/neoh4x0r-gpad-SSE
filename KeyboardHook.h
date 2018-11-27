#pragma once
#include <windows.h>

class KeyboardHook
{
public:
	static void WINAPIV InstallHook();

private:
	static void WINAPIV CreateHook();
//__declspec(dllexport) LRESULT CALLBACK KeyboardEvent (int nCode, WPARAM wParam, LPARAM lParam);
//__declspec(dllexport) LRESULT CALLBACK MouseEvent (int nCode, WPARAM wParam, LPARAM lParam);

};
