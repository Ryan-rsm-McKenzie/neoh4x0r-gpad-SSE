#pragma once
#pragma once

#include "skse64/NiExtraData.h"
#include "skse64/GameCamera.h"
#include "skse64/NiNodes.h"
#include "skse64/GameReferences.h"
#include "vectors.h"

namespace CameraRotation
{
	float Length_NiPoint3(NiPoint3 const& point);
	NiPoint3 Normalize_NiPoint3(NiPoint3 const& point);
	void TestCameraRotation();
	void RotateCamera(float const& angleX, float const& angleY, float const& angleZ);
}
