#pragma once

#include "Vector3.h"
#include "Vector2.h"

namespace Milk3D
{
	struct Vec4
	{
		Vec4() : v(DirectX::XMVectorZero()) {}
		Vec4(float xyzw) : v(DirectX::XMVectorSet(xyzw, xyzw, xyzw, xyzw)) {}
		Vec4(float x, float yzw) : v(DirectX::XMVectorSet(x, yzw, yzw, yzw)) {}
		Vec4(float x, float y, float zw) : v(DirectX::XMVectorSet(x, y, zw, zw)) {}
		Vec4(float x, float y, float z, float w) : v(DirectX::XMVectorSet(x, y, z, 0)) {}
		Vec4(const std::initializer_list<float> & list)
		{
			float f[4]{ 0 };
			auto it = list.begin();
			for (int i = 0; i < 4, it != list.end(); ++i, ++it)
			{
				f[i] = *it;
			}
			v = DirectX::XMVectorSet(f[0], f[1], f[2], f[3]);
		}

		Vec4(Vec4 const & other) : v(other.v) {}
		Vec4 & operator=(Vec4 const & other) { v = other.v; return *this; }

		//*** Conversions ***//
		Vec4(DirectX::XMVECTOR const & other) : v(other) {}
		Vec4 & operator=(DirectX::XMVECTOR const & other) { v = other; return *this; }
		operator DirectX::XMVECTOR() const { return v; }

		Vec4(DirectX::XMFLOAT4 const & other) : v(DirectX::XMLoadFloat4(&other)) {}
		Vec4 & operator=(DirectX::XMFLOAT4 const & other) { v = (DirectX::XMLoadFloat4(&other)); return *this; }
		operator DirectX::XMFLOAT4() const { return GetFloat4(); }

		Vec4(Vec2 const & other) : v(other.v) {}
		Vec4 & operator=(Vec2 const & other) { v = other.v; return *this;}

		Vec4(Vec3 const & other) : v(other.v) {}
		Vec4 & operator=(Vec3 const & other) { v = other.v; return *this;}

		//*** Helper Functions ***//

		DirectX::XMVECTOR GetVector() const { return v; }
		DirectX::XMFLOAT4 GetFloat4() const { DirectX::XMFLOAT4 vec; DirectX::XMStoreFloat4(&vec, v); return vec; }
		float Dot(Vec4 const & rhs) const { return DirectX::XMVectorGetX(DirectX::XMVector4Dot(v, rhs.v)); }
		float Length() const { return DirectX::XMVectorGetX(DirectX::XMVector4Length(v)); }
		void Normalize() { DirectX::XMVector4Normalize(v); }
		float X() const { return DirectX::XMVectorGetX(v); }
		float Y() const { return DirectX::XMVectorGetY(v); }
		float Z() const { return DirectX::XMVectorGetZ(v); }
		float W() const { return DirectX::XMVectorGetW(v); }
		void SetX(float x) { v = DirectX::XMVectorSetX(v, x); }
		void SetY(float y) { v = DirectX::XMVectorSetY(v, y); }
		void SetZ(float z) { v = DirectX::XMVectorSetZ(v, z); }
		void SetW(float w) { v = DirectX::XMVectorSetW(v, w); }
		void Set(float x, float y, float z, float w) { v = DirectX::XMVectorSet(x, y, z, w); }

		/*** Operations ***/

		Vec4 operator*(float scalar) const { return Vec4(DirectX::XMVectorScale(v, scalar)); }
		float operator*(DirectX::XMFLOAT4 const & vector) const { return Dot(vector); }
		float operator*(DirectX::XMVECTOR vector) const { return Dot(vector); }
		float operator*(const Vec4 & vector) const { return Dot(vector); }
		Vec4 & operator*=(float scalar) { v = DirectX::XMVectorScale(v, scalar); return *this; }
		Vec4 & operator+=(const Vec4 & rhs) { v = DirectX::XMVectorAdd(v, rhs.v); return *this; }
		Vec4 & operator-=(const Vec4 & rhs) { v = DirectX::XMVectorSubtract(v, rhs.v); return *this; }
		bool operator==(const Vec4 & rhs) const { return DirectX::XMComparisonAllTrue(DirectX::XMVector4EqualR(v, rhs.v)); }
		float operator[](int i) const { return DirectX::XMVectorGetByIndex(v, i); }
		Vec4 operator-() const { return DirectX::XMVectorNegate(v); }
		Vec4 operator-(Vec4 const & other) const { return DirectX::XMVectorSubtract(v, other); }
		Vec4 operator+(Vec4 const & other) const { return DirectX::XMVectorAdd(v, other); }

		/*** Members ***/
		DirectX::XMVECTOR v;
	};
	
	inline float Dot(Vec4 const & lhs, Vec4 const & rhs) { return lhs.Dot(rhs); }
	inline Vec4 Cross(Vec4 const & lhs, Vec4 const & rhs) { return lhs.Dot(rhs); }
	inline float Length(Vec4 const & vec) { return vec.Length(); }
	inline Vec4 Normalize(Vec4 const & vec) { Vec4 newVec(vec); newVec.Normalize(); return newVec; }
	inline Vec4 operator*(float scalar, Vec4 const & vec) { return vec * scalar; }

	typedef Vec4 Plane;

}

