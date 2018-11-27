#pragma once

#include "skse64/GameTypes.h"

struct StaticFunctionTag;
class VMClassRegistry;

#define thumb_max 32767.0
#define dead_zone_scalar 0.50 // 0.50 gets major/minor directions, 0.75 gets only major directions
#define dead_zone (thumb_max * dead_zone_scalar)
#define direction_scalar 32768.0f

namespace papyrusController
{
	enum
	{
		kLeftStick = 0,
		kRightStick = 1,
	};
	enum
	{
		kDiretion_none		= 0,
		kDiretion_up		= 1,
		kDiretion_upright	= 2,
		kDiretion_right		= 3,
		kDiretion_downright	= 4,
		kDiretion_down		= 5,
		kDiretion_downleft	= 6,
		kDiretion_left		= 7,
		kDiretion_upleft	= 8,
	};
	enum
	{
		kDiretion_Idle		= 0x00,
		kDiretion_Up		= 0x01,
		kDiretion_Down		= 0x02,
		kDiretion_Left		= 0x04,
		kDiretion_Right		= 0x08,
	};

	void ToggleGamepad(StaticFunctionTag* thisInput);

	float GetLX(StaticFunctionTag* thisInput);
	float GetLY(StaticFunctionTag* thisInput);
	float GetRX(StaticFunctionTag* thisInput);
	float GetRY(StaticFunctionTag* thisInput);

	SInt32 GetLSDirection(StaticFunctionTag* thisInput);
	SInt32 GetRSDirection(StaticFunctionTag* thisInput);

	// non-native functions
	SInt32 GetDirection(int stick);
	bool HasDirectionFlag(UInt32 flags, BYTE direction);

	bool RegisterFuncs(VMClassRegistry* registry);
}
