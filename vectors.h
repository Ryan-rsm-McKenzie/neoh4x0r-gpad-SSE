#pragma once
#pragma once
#include "skse64/NiTypes.h"

class NiPoint4
{
public:
	NiPoint4();
	NiPoint4(float A, float B, float C, float D) : a(A), b(B), c(C), d(D) { };
	float	a;
	float	b;
	float	c;
	float	d;

		// Negative
	NiPoint4 operator- () const;

	// Basic operations
	NiPoint4 operator+ (const NiPoint4& pt) const;
	NiPoint4 operator- (const NiPoint4& pt) const;

	NiPoint4& operator+= (const NiPoint4& pt);
	NiPoint4& operator-= (const NiPoint4& pt);

	// Scalar operations
	NiPoint4 operator* (float fScalar) const;
	NiPoint4 operator/ (float fScalar) const;

	NiPoint4& operator*= (float fScalar);
	NiPoint4& operator/= (float fScalar);

};

class NiMatrix44
{
public:
	float	data[4][4];
	void Identity();

	NiMatrix44 operator+(const NiMatrix44& mat) const;
	NiMatrix44 operator-(const NiMatrix44& mat) const;

	// Matric mult
	NiMatrix44 operator*(const NiMatrix44& mat) const;

	// Vector mult
	NiPoint4 operator*(const NiPoint4& pt) const;

	// Scalar multiplier
	NiMatrix44 operator*(float fScalar) const;
};
typedef struct PointMatrix2
{
public:
	NiPoint2 r0;
	NiPoint2 r1;
} PointMatrix2;

typedef struct PointMatrix4
{
public:
	NiPoint4 r0;
	NiPoint4 r1;
	NiPoint4 r2;
	NiPoint4 r3;

	void FromArray(float data[4][4])
	{
		r0 = NiPoint4(data[0][0],data[0][1],data[0][2],data[0][3]);
		r1 = NiPoint4(data[1][0],data[1][1],data[1][2],data[1][3]);
		r2 = NiPoint4(data[2][0],data[2][1],data[2][2],data[2][3]);
		r3 = NiPoint4(data[3][0],data[3][1],data[3][2],data[3][3]);
	}
	PointMatrix4 Inverse()
	{
		PointMatrix4 inverse;
		inverse.r0.a = r0.a;
		inverse.r0.b = r1.a;
		inverse.r0.c = r2.a;
		inverse.r0.d = r3.a;

		inverse.r1.a = r0.b;
		inverse.r1.b = r1.b;
		inverse.r1.c = r2.b;
		inverse.r1.d = r3.b;

		inverse.r2.a = r0.c;
		inverse.r2.b = r1.c;
		inverse.r2.c = r2.c;
		inverse.r2.d = r3.c;

		inverse.r3.a = r0.d;
		inverse.r3.b = r1.d;
		inverse.r3.c = r2.d;
		inverse.r3.d = r3.d;
		return inverse;
	}
	PointMatrix4 Transpose()
	{
		PointMatrix4 transpose;
		transpose.r0 = r0;
		transpose.r1 = r1;
		transpose.r2 = r2;
		transpose.r3 = r3;

		transpose.r0.b = r1.a;
		transpose.r1.a = r0.b;

		transpose.r0.c = r2.a;
		transpose.r2.a = r0.c;

		transpose.r0.d = r3.a;
		transpose.r3.a = r0.d;

		transpose.r1.c = r2.b;
		transpose.r2.b = r1.c;

		transpose.r1.d = r3.b;
		transpose.r3.b = r1.d;

		transpose.r2.d = r3.c;
		transpose.r3.c = r2.d;
		return transpose;
	}
	void FromNiMatrix44(NiMatrix44 rot)
	{
		FromArray(rot.data);
	}
	NiMatrix44 ToNiMatrix44()
	{
		NiMatrix44 rot;
		rot.data[0][0] = r0.a;
		rot.data[0][1] = r0.b;
		rot.data[0][2] = r0.c;
		rot.data[0][3] = r0.d;

		rot.data[1][0] = r1.a;
		rot.data[1][1] = r1.b;
		rot.data[1][2] = r1.c;
		rot.data[1][3] = r1.d;

		rot.data[2][0] = r2.a;
		rot.data[2][1] = r2.b;
		rot.data[2][2] = r2.c;
		rot.data[2][3] = r2.d;

		rot.data[3][0] = r3.a;
		rot.data[3][1] = r3.b;
		rot.data[3][2] = r3.c;
		rot.data[3][3] = r3.d;
		return rot;
	}
} PointMatrix4;

typedef struct PointMatrix3
{
	public:
	NiPoint3 r0;
	NiPoint3 r1;
	NiPoint3 r2;

	PointMatrix3 Inverse()
	{
		PointMatrix3 inverse;
		inverse.r0.x = r0.x;
		inverse.r0.y = r1.x;
		inverse.r0.z = r2.x;

		inverse.r1.x = r0.y;
		inverse.r1.y = r1.y;
		inverse.r1.z = r2.y;

		inverse.r2.x = r0.z;
		inverse.r2.y = r1.z;
		inverse.r2.z = r2.z;
		return inverse;
	}
	PointMatrix3 Transpose()
	{
		PointMatrix3 transpose;
		transpose.r0 = r0;
		transpose.r1 = r1;
		transpose.r2 = r2;

		transpose.r0.y = r1.x;
		transpose.r1.x = r0.y;

		transpose.r0.z = r2.x;
		transpose.r2.x = r0.y;

		transpose.r1.z = r2.y;
		transpose.r2.y = r1.z;
		return transpose;
	}
	void FromArray(float data[3][3])
	{
		r0 = NiPoint3(data[0][0],data[0][1],data[0][2]);
		r1 = NiPoint3(data[1][0],data[1][1],data[1][2]);
		r2 = NiPoint3(data[2][0],data[2][1],data[2][2]);
	}
	void FromNiMatrix33(NiMatrix33 rot)
	{
		FromArray(rot.data);
	}
	NiMatrix33 ToNiMatrix33()
	{
		NiMatrix33 rot;
		rot.data[0][0] = r0.x;
		rot.data[0][1] = r0.y;
		rot.data[0][2] = r0.z;

		rot.data[1][0] = r1.x;
		rot.data[1][1] = r1.y;
		rot.data[1][2] = r1.z;

		rot.data[2][0] = r2.x;
		rot.data[2][1] = r2.y;
		rot.data[2][2] = r2.z;
		return rot;
	}
} PointMatrix3;
class Rotation
{
public:
PointMatrix3 RotateMatrix2D_X(float angle_x);
PointMatrix3 RotateMatrix2D_Y(float angle_y);
PointMatrix3 RotateMatrix2D_Z(float angle_z);

PointMatrix4 RotateMatrix3D_X(float angle_x);
PointMatrix4 RotateMatrix3D_Y(float angle_y);
PointMatrix4 RotateMatrix3D_Z(float angle_z);
private:
PointMatrix3 CreateMatrixRot2D_X(float angle_x);
PointMatrix3 CreateMatrixRot2D_Y(float angle_y);
PointMatrix3 CreateMatrixRot2D_Z(float angle_z);

PointMatrix4 CreateMatrixRot3D_X(float angle_x);
PointMatrix4 CreateMatrixRot3D_Y(float angle_y);
PointMatrix4 CreateMatrixRot3D_Z(float angle_z);
};
