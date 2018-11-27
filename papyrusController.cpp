#include "papyrusController.h"
#include "skse64/GameInput.h"
#include "Hooks_XInput.h"
#include "gpadconf.h"
#include "Logger.h"

namespace papyrusController
{
	void ToggleGamepad(StaticFunctionTag* thisInput)
	{
		if(!gpadconf::PluginEnabled()) { return; }
		XInputHookControl::ToggleGamepad();
	}
	float GetLX(StaticFunctionTag* thisInput)
	{
		if(!gpadconf::PluginEnabled()) { return 0; }
		if(!XInputHookControl::UsingGamepad()) { return 0; }
		return XInputHookControl::GetSingleton().GetPadState().Gamepad.sThumbLX;
	}
	float GetLY(StaticFunctionTag* thisInput)
	{
		if(!gpadconf::PluginEnabled()) { return 0; }
		if(!XInputHookControl::UsingGamepad()) { return 0; }
		return XInputHookControl::GetSingleton().GetPadState().Gamepad.sThumbLY;
	}
	float GetRX(StaticFunctionTag* thisInput)
	{
		if(!gpadconf::PluginEnabled()) { return 0; }
		if(!XInputHookControl::UsingGamepad()) { return 0; }
		return XInputHookControl::GetSingleton().GetPadState().Gamepad.sThumbRX;
	}
	float GetRY(StaticFunctionTag* thisInput)
	{
		if(!gpadconf::PluginEnabled()) { return 0; }
		if(!XInputHookControl::UsingGamepad()) { return 0; }
		return XInputHookControl::GetSingleton().GetPadState().Gamepad.sThumbRY;
	}
	SInt32 GetLSDirection(StaticFunctionTag* thisInput)
	{
		return GetDirection(kLeftStick);
	}
	SInt32 GetRSDirection(StaticFunctionTag* thisInput)
	{
		return GetDirection(kRightStick);
	}

	bool HasDirectionFlag(UInt32 flags, BYTE direction)
	{
		return (flags & direction) == direction;
	}
	SInt32 GetDirection(int stick)
	{
		if(!gpadconf::PluginEnabled()) { return kDiretion_none; }
		if(!XInputHookControl::UsingGamepad()) { return kDiretion_none; }
		float dX=0;
		float dY=0;
		switch(stick)
		{
			case kLeftStick:
				dX=(float)XInputHookControl::GetSingleton().GetPadState().Gamepad.sThumbLX;
				dY=(float)XInputHookControl::GetSingleton().GetPadState().Gamepad.sThumbLY;
				break;
			case kRightStick:
				dX=(float)XInputHookControl::GetSingleton().GetPadState().Gamepad.sThumbRX;
				dY=(float)XInputHookControl::GetSingleton().GetPadState().Gamepad.sThumbRY;
				break;
			default:
				Log::VerboseDebug("papyrusController::GetDirection(%d) -- unknown joystick index", stick);
				break;
		}
		//Log::VerboseDebug("[dX:%f] [dY:%f]", dX, dY);
		UInt32 flags = kDiretion_Idle;
		if(dY >= dead_zone) {
			//Log::VerboseDebug("Adding Direction: kDiretion_Up");
			flags |= kDiretion_Up;
		}
		if(dY <= -dead_zone) {
			//Log::VerboseDebug("Adding Direction: kDiretion_Down");
			flags |= kDiretion_Down;
		}
		if(dX >= dead_zone) {
			//Log::VerboseDebug("Adding Direction: kDiretion_Right");
			flags |= kDiretion_Right;
		}
		if(dX <= -dead_zone) {
			//Log::VerboseDebug("Adding Direction: kDiretion_Left");
			flags |= kDiretion_Left;
		}

		if(HasDirectionFlag(flags, kDiretion_Up) &&
			HasDirectionFlag(flags, kDiretion_Down))
		{
			Log::VerboseDebug("papyrusController::GetDirection(%d) -- have both up and down directions!", stick);
		}
		if(HasDirectionFlag(flags, kDiretion_Right) &&
			HasDirectionFlag(flags, kDiretion_Left))
		{
			Log::VerboseDebug("papyrusController::GetDirection(%d) -- have both left and right directions!", stick);
		}

		if(HasDirectionFlag(flags, kDiretion_Up) &&
			HasDirectionFlag(flags, kDiretion_Right))
		{
			//Log::VerboseDebug("Joystick Direction: kDiretion_upright");
			return kDiretion_upright;
		}
		else if(HasDirectionFlag(flags, kDiretion_Up) &&
			HasDirectionFlag(flags, kDiretion_Left))
		{
			//Log::VerboseDebug("Joystick Direction: kDiretion_upleft");
			return kDiretion_upleft;
		}
		else if(HasDirectionFlag(flags, kDiretion_Down) &&
			HasDirectionFlag(flags, kDiretion_Right))
		{
			//Log::VerboseDebug("Joystick Direction: kDiretion_downright");
			return kDiretion_downright;
		}
		else if(HasDirectionFlag(flags, kDiretion_Down) &&
			HasDirectionFlag(flags, kDiretion_Left))
		{
			//Log::VerboseDebug("Joystick Direction: kDiretion_downleft");
			return kDiretion_downleft;
		}
		else if(HasDirectionFlag(flags, kDiretion_Down))
		{
			//Log::VerboseDebug("Joystick Direction: kDiretion_down");
			return kDiretion_down;
		}
		else if(HasDirectionFlag(flags, kDiretion_Up))
		{
			//Log::VerboseDebug("Joystick Direction: kDiretion_up");
			return kDiretion_up;
		}
		else if(HasDirectionFlag(flags, kDiretion_Left))
		{
			//Log::VerboseDebug("Joystick Direction: kDiretion_left");
			return kDiretion_left;
		}
		else if(HasDirectionFlag(flags, kDiretion_Right))
		{
			//Log::VerboseDebug("Joystick Direction: kDiretion_right");
			return kDiretion_right;
		}
		else
		{
			//Log::VerboseDebug("Joystick Direction: kDiretion_none");
			return kDiretion_none;
		}
	}
}

#include "skse64/PapyrusVM.h"
#include "skse64/PapyrusNativeFunctions.h"
bool papyrusController::RegisterFuncs(VMClassRegistry* registry)
{
	Log::Debug("papyrusController::RegisterFuncs called");
	registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, void> ("ToggleGamepad", "Controller", papyrusController::ToggleGamepad, registry));
	registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, float> ("GetLX", "Controller", papyrusController::GetLX, registry));
	registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, float> ("GetLY", "Controller", papyrusController::GetLY, registry));
	registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, float> ("GetRX", "Controller", papyrusController::GetRX, registry));
	registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, float> ("GetRY", "Controller", papyrusController::GetRY, registry));
	registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, SInt32> ("GetLSDirection", "Controller", papyrusController::GetLSDirection, registry));
	registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, SInt32> ("GetRSDirection", "Controller", papyrusController::GetRSDirection, registry));

	/* don't set function flags: kFunctionFlag_NoWait because it is not thread-safe */
	//registry->SetFunctionFlags("Controller", "GetLX", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->SetFunctionFlags("Controller", "GetLY", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->SetFunctionFlags("Controller", "GetRX", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->SetFunctionFlags("Controller", "GetRY", VMClassRegistry::kFunctionFlag_NoWait);

	//registry->SetFunctionFlags("Controller", "GetLSDirection", VMClassRegistry::kFunctionFlag_NoWait);
	//registry->SetFunctionFlags("Controller", "GetRSDirection", VMClassRegistry::kFunctionFlag_NoWait);
	return true;
}
