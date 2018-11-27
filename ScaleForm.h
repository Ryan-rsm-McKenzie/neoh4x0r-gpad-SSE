#pragma once

#include "skse64/ScaleformCallbacks.h"
#include "skse64/ScaleformMovie.h"
#include "skse64/GameEvents.h"
#include "skse64/GameInput.h"

namespace scaleform
{
	bool RegisterFuncs(GFxMovieView * view, GFxValue * root);
}
#pragma region old-code
//bool RegisterFuncs(GFxMovieView * view, GFxValue * root);
//
//class SKSEScaleform_Test : public GFxFunctionHandler
//{
//	public:
//		virtual void Invoke(Args * args);
//};
//class SKSEScaleform_GetMappedKey : public GFxFunctionHandler
//{
//	public:
//		virtual void Invoke(Args * args);
//};
//class SKSEScaleform_GetLastControl : public GFxFunctionHandler
//{
//	public:
//		virtual void	Invoke(Args * args);
//};
//class SKSEScaleform_GetLastKeycode : public GFxFunctionHandler
//{
//	public:
//		virtual void	Invoke(Args * args);
//};
#pragma endregion
