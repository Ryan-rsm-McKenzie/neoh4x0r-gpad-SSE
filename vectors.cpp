#include "vectors.h"

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#pragma region  NiPoint4
NiPoint4::NiPoint4()
{
	a = 0.0f;
	b = 0.0f;
	c = 0.0f;
	d = 0.0f;
}
NiPoint4 NiPoint4::operator- () const
{
	return NiPoint4(-a, -b, -c, -d);
}
NiPoint4 NiPoint4::operator+ (const NiPoint4& pt) const
{
	return NiPoint4(a + pt.a, b + pt.b, c + pt.c, d + pt.d);
}
NiPoint4 NiPoint4::operator- (const NiPoint4& pt) const
{
	return NiPoint4(a - pt.a, b - pt.b, c - pt.c, d - pt.d);
}
NiPoint4& NiPoint4::operator+= (const NiPoint4& pt)
{
	a += pt.a;
	b += pt.b;
	c += pt.c;
	d += pt.d;
	return *this;
}
NiPoint4& NiPoint4::operator-= (const NiPoint4& pt)
{
	a -= pt.a;
	b -= pt.b;
	c -= pt.c;
	d -= pt.d;
	return *this;
}
NiPoint4 NiPoint4::operator* (float scalar) const
{
	return NiPoint4(scalar * a, scalar * b, scalar * c, scalar * d);
}
NiPoint4 NiPoint4::operator/ (float scalar) const
{		
	float invScalar = 1.0f / scalar;
	return NiPoint4(invScalar * a, invScalar * b, invScalar * c, invScalar * d);
}
NiPoint4& NiPoint4::operator*= (float scalar)
{
	a *= scalar;
	b *= scalar;
	c *= scalar;
	d *= scalar;
	return *this;
}
NiPoint4& NiPoint4::operator/= (float scalar)
{
	float invScalar = 1.0f / scalar;
	a *= invScalar;
	b *= invScalar;
	c *= invScalar;
	d *= invScalar;
	return *this;
}
#pragma endregion
#pragma region NiMatrix44
void NiMatrix44::Identity()
{
	data[0][0] = 1.0f;
	data[0][1] = 0.0f;
	data[0][2] = 0.0f;
	data[0][3] = 0.0f;

	data[1][0] = 0.0f;
	data[1][1] = 1.0f;
	data[1][2] = 0.0f;
	data[1][3] = 0.0f;

	data[2][0] = 0.0f;
	data[2][1] = 0.0f;
	data[2][2] = 1.0f;
	data[2][3] = 0.0f;

	data[3][0] = 0.0f;
	data[3][1] = 0.0f;
	data[3][2] = 0.0f;
	data[3][3] = 1.0f;
}
//NiMatrix44 NiMatrix44::operator+(const NiMatrix44& mat) const
//{
//}
//NiMatrix44 NiMatrix44::operator-(const NiMatrix44& mat) const
//{
//}
NiMatrix44 NiMatrix44::operator*(const NiMatrix44& rhs) const
{
	NiMatrix44 tmp;
	tmp.data[0][0] =
		data[0][0]*rhs.data[0][0]+
		data[0][1]*rhs.data[1][0]+
		data[0][2]*rhs.data[2][0]+
		data[0][3]*rhs.data[3][0];
	tmp.data[1][0] =
		data[1][0]*rhs.data[0][0]+
		data[1][1]*rhs.data[1][0]+
		data[1][2]*rhs.data[2][0]+
		data[1][3]*rhs.data[3][0];
	tmp.data[2][0] =
		data[2][0]*rhs.data[0][0]+
		data[2][1]*rhs.data[1][0]+
		data[2][2]*rhs.data[2][0]+
		data[2][3]*rhs.data[3][0];
	tmp.data[0][1] =
		data[0][0]*rhs.data[0][1]+
		data[0][1]*rhs.data[1][1]+
		data[0][2]*rhs.data[2][1]+
		data[0][3]*rhs.data[3][1];
	tmp.data[1][1] =
		data[1][0]*rhs.data[0][1]+
		data[1][1]*rhs.data[1][1]+
		data[1][2]*rhs.data[2][1]+
		data[1][3]*rhs.data[3][1];
	tmp.data[2][1] =
		data[2][0]*rhs.data[0][1]+
		data[2][1]*rhs.data[1][1]+
		data[2][2]*rhs.data[2][1]+
		data[2][3]*rhs.data[3][1];
	tmp.data[0][2] =
		data[0][0]*rhs.data[0][2]+
		data[0][1]*rhs.data[1][2]+
		data[0][2]*rhs.data[2][2]+
		data[0][3]*rhs.data[3][3];
	tmp.data[1][2] =
		data[1][0]*rhs.data[0][2]+
		data[1][1]*rhs.data[1][2]+
		data[1][2]*rhs.data[2][2]+
		data[1][3]*rhs.data[3][2];
	tmp.data[2][2] =
		data[2][0]*rhs.data[0][2]+
		data[2][1]*rhs.data[1][2]+
		data[2][2]*rhs.data[2][2]+
		data[2][3]*rhs.data[3][2];
	tmp.data[3][3] =
		data[3][0]*rhs.data[0][3]+
		data[3][1]*rhs.data[1][3]+
		data[3][2]*rhs.data[2][3]+
		data[3][3]*rhs.data[3][3];
	return tmp;
}
NiPoint4 NiMatrix44::operator*(const NiPoint4& pt) const
{
	return NiPoint4
	(
	data[0][0]*pt.a+data[0][1]*pt.b+data[0][2]*pt.c+data[0][3]*pt.d,
	data[1][0]*pt.a+data[1][1]*pt.b+data[1][2]*pt.c+data[1][3]*pt.d,
	data[2][0]*pt.a+data[2][1]*pt.b+data[2][2]*pt.c+data[2][3]*pt.d,
	data[3][0]*pt.a+data[3][1]*pt.b+data[3][3]*pt.c+data[3][3]*pt.d
	);
}
NiMatrix44 NiMatrix44::operator*(float scalar) const
{
	NiMatrix44 result;
	result.data[0][0] = data[0][0] * scalar;
	result.data[0][1] = data[0][1] * scalar;
	result.data[0][2] = data[0][2] * scalar;
	result.data[0][3] = data[0][3] * scalar;

	result.data[1][0] = data[1][0] * scalar;
	result.data[1][1] = data[1][1] * scalar;
	result.data[1][2] = data[1][2] * scalar;
	result.data[1][3] = data[1][3] * scalar;

	result.data[2][0] = data[2][0] * scalar;
	result.data[2][1] = data[2][1] * scalar;
	result.data[2][2] = data[2][2] * scalar;
	result.data[2][3] = data[2][3] * scalar;

	result.data[3][0] = data[3][0] * scalar;
	result.data[3][1] = data[3][1] * scalar;
	result.data[3][2] = data[3][2] * scalar;
	result.data[3][3] = data[3][3] * scalar;
	return result;
}
#pragma endregion
#pragma region rotations
PointMatrix4 Rotation::CreateMatrixRot3D_X(float angle_x)
{
	float radians = angle_x * (M_PI/180.0);
	float x_rot[4][4] = 
	{
		{1,		0,				0,				0},
		{0,		cos(radians),	-sin(radians),	0},
		{0,		sin(radians),	cos(radians),	0},
		{0,		0,				0,				1}
	};
	NiPoint4 p[4];
	for(int i=0;i<4;i++)
	{
		p[i] = NiPoint4(x_rot[i][0],x_rot[i][1],x_rot[i][2],x_rot[i][3]);		
	}
	PointMatrix4 pm;
	pm.r0 = p[0];
	pm.r1 = p[1];
	pm.r2 = p[2];
	pm.r3 = p[3];
	return pm;
}
PointMatrix3 Rotation::CreateMatrixRot2D_X(float angle_x)
{
	float radians = angle_x * (M_PI/180.0);
	float x_rot[3][3] = 
	{
		{1,		0,				0},
		{0,		cos(radians),	-sin(radians)},
		{0,		sin(radians),	cos(radians)}
	};
	NiMatrix33 xr_rot;
	for(int i = 0;i<3;i++) { for(int j = 0;j<3;j++) { xr_rot.data[i][j] = x_rot[i][j]; } }
	NiPoint3 p[3];
	for(int i=0;i<3;i++)
	{
		p[i] = NiPoint3(xr_rot.data[i][0],xr_rot.data[i][1],xr_rot.data[i][2]);		
	}
	PointMatrix3 pm;
	pm.r0 = p[0];
	pm.r1 = p[1];
	pm.r2 = p[2];
	return pm;
}
PointMatrix4 Rotation::CreateMatrixRot3D_Y(float angle_y)
{
	float radians = angle_y * (M_PI/180.0);
	float y_rot[4][4] = 
	{
		{cos(radians),		0,	sin(radians),	0},
		{0,					1,	0,				0},
		{-sin(radians),		0,	cos(radians),	0},
		{0,					0,	0,				1}
	};
	NiPoint4 p[4];
	for(int i=0;i<4;i++)
	{
		p[i] = NiPoint4(y_rot[i][0],y_rot[i][1],y_rot[i][2],y_rot[i][3]);		
	}
	PointMatrix4 pm;
	pm.r0 = p[0];
	pm.r1 = p[1];
	pm.r2 = p[2];
	pm.r3 = p[3];
	return pm;
}
PointMatrix3 Rotation::CreateMatrixRot2D_Y(float angle_y)
{
	float radians = angle_y * (M_PI/180.0);
	float y_rot[3][3] = 
	{
		{cos(radians),	0,		-sin(radians)},
		{0,				1,		0},
		{sin(radians),	0,		cos(radians)}
	};
	NiMatrix33 yr_rot;
	for(int i = 0;i<3;i++) { for(int j = 0;j<3;j++) { yr_rot.data[i][j] = y_rot[i][j]; } }
	NiPoint3 p[3];
	for(int i=0;i<3;i++)
	{
		p[i] = NiPoint3(yr_rot.data[i][0],yr_rot.data[i][1],yr_rot.data[i][2]);		
	}
	PointMatrix3 pm;
	pm.r0 = p[0];
	pm.r1 = p[1];
	pm.r2 = p[2];
	return pm;
}
PointMatrix4 Rotation::CreateMatrixRot3D_Z(float angle_z)
{
	float radians = angle_z * (M_PI/180.0);
	float z_rot[4][4] = 
	{
		{cos(radians),		-sin(radians),	0,	0},
		{sin(radians),		cos(radians),	0,	0},
		{0,					0,				1,	0},
		{0,					0,				0,	1}
	};
	NiPoint4 p[4];
	for(int i=0;i<4;i++)
	{
		p[i] = NiPoint4(z_rot[i][0],z_rot[i][1],z_rot[i][2],z_rot[i][3]);		
	}
	PointMatrix4 pm;
	pm.r0 = p[0];
	pm.r1 = p[1];
	pm.r2 = p[2];
	pm.r3 = p[3];
	return pm;
}
PointMatrix3 Rotation::CreateMatrixRot2D_Z(float angle_z)
{
	float radians = angle_z * (M_PI/180.0);
	float z_rot[3][3] = 
	{
		{cos(radians),	-sin(radians),	0},
		{sin(radians),	cos(radians),	0},
		{0,				0,				1}
	};
	NiMatrix33 zr_rot;
	for(int i = 0;i<3;i++) { for(int j = 0;j<3;j++) { zr_rot.data[i][j] = z_rot[i][j]; } }

	NiPoint3 p[3];
	for(int i=0;i<3;i++)
	{
		p[i] = NiPoint3(zr_rot.data[i][0],zr_rot.data[i][1],zr_rot.data[i][2]);		
	}
	PointMatrix3 pm;
	pm.r0 = p[0];
	pm.r1 = p[1];
	pm.r2 = p[2];
	return pm;
}

PointMatrix3 Rotation::RotateMatrix2D_X(float angle_x)
{
	PointMatrix3 pm_x = CreateMatrixRot2D_X(angle_x);	
	return pm_x;
}
PointMatrix3 Rotation::RotateMatrix2D_Y(float angle_y)
{
	PointMatrix3 pm_y = CreateMatrixRot2D_Y(angle_y);
	return pm_y;
}
PointMatrix3 Rotation::RotateMatrix2D_Z(float angle_z)
{
	PointMatrix3 pm_z = CreateMatrixRot2D_Z(angle_z);
	return pm_z;
}

PointMatrix4 Rotation::RotateMatrix3D_X(float angle_x)
{
	PointMatrix4 pm_x = CreateMatrixRot3D_X(angle_x);
	return pm_x;
}
PointMatrix4 Rotation::RotateMatrix3D_Y(float angle_y)
{
	PointMatrix4 pm_y = CreateMatrixRot3D_Y(angle_y);
	return pm_y;
}
PointMatrix4 Rotation::RotateMatrix3D_Z(float angle_z)
{
	PointMatrix4 pm_z = CreateMatrixRot3D_Z(angle_z);
	return pm_z;
}
#pragma endregion