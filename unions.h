#pragma once

typedef union _u_uint32_float_t
{
	_u_uint32_float_t() : i(0) { }
	explicit _u_uint32_float_t(UInt32 _i) : i(_i) { }
	explicit _u_uint32_float_t(float _f) : f(_f) { }
	UInt32 i;
	float f;
} _u_uint32_float_s;
