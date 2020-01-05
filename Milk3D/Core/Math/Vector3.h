#pragma once

#include <initializer_list>
#include <DirectXMath.h>
#include "Vector2.h"

namespace Milk3D
{
	struct Vec3
	{
		Vec3() : v(DirectX::XMVectorZero()) {}
		Vec3(float xyz) : v(DirectX::XMVectorSet(xyz, xyz, xyz, 0)) {}
		Vec3(float x, float yz) : v(DirectX::XMVectorSet(x, yz, yz, 0)) {}
		Vec3(float x, float y, float z) : v(DirectX::XMVectorSet(x, y, z, 0)) {}
		Vec3(const std::initializer_list<float> & list)
		{
			float f[3]{ 0 };
			auto it = list.begin();
			for (int i = 0; i < 3, it != list.end(); ++i, ++it)
			{
				f[i] = *it;
			}
			v = DirectX::XMVectorSet(f[0], f[1], f[2], 1);
		}

		Vec3(Vec3 const & other) : v(other.v) {}
		Vec3 & operator=(Vec3 const & other) { v = other.v; return *this; }

		//*** Conversions ***//
		Vec3(DirectX::XMVECTOR const & other) : v(other) {}
		Vec3 & operator=(DirectX::XMVECTOR const & other) { v = other; return *this; }
		operator DirectX::XMVECTOR() const { return v; }

		Vec3(DirectX::XMFLOAT3 const & other) : v(DirectX::XMLoadFloat3(&other)) {}
		Vec3 & operator=(DirectX::XMFLOAT3 const & other) { v = (DirectX::XMLoadFloat3(&other)); return *this; }
		operator DirectX::XMFLOAT3() const { return GetFloat3(); }

		Vec3(Vec2 const & other) : v(other.v) {}
		Vec3 & operator=(Vec2 const & other) { v = other.v; return *this; }

		//*** Helper Functions ***//

		DirectX::XMVECTOR GetVector() const { return v; }
		DirectX::XMFLOAT3 GetFloat3() const { DirectX::XMFLOAT3 vec; DirectX::XMStoreFloat3(&vec, v); return vec; }
		float Dot(Vec3 const & rhs) const { return DirectX::XMVectorGetX(DirectX::XMVector3Dot(v, rhs.v)); }
		Vec3 Cross(Vec3 const & rhs) const { return DirectX::XMVector3Cross(v, rhs.v); }
		float Length() const { return DirectX::XMVectorGetX(DirectX::XMVector3Length(v)); }
		void Normalize() { v = DirectX::XMVector3Normalize(v); }
		float X() const { return DirectX::XMVectorGetX(v); }
		float Y() const { return DirectX::XMVectorGetY(v); }
		float Z() const { return DirectX::XMVectorGetZ(v); }
		float W() const { return DirectX::XMVectorGetW(v); }
		void SetX(float x) { v = DirectX::XMVectorSetX(v, x); }
		void SetY(float y) { v = DirectX::XMVectorSetY(v, y); }
		void SetZ(float z) { v = DirectX::XMVectorSetZ(v, z); }
		void Set(float x, float y, float z) { v = DirectX::XMVectorSet(x, y, z, W()); }

		/*** Operations ***/

		Vec3 operator*(float scalar) const { return Vec3(DirectX::XMVectorScale(v, scalar)); }
		float operator*(const Vec3 & vector) const { return Dot(vector); }
		Vec3 & operator*=(float scalar) { v = DirectX::XMVectorScale(v, scalar); return *this; }
		Vec3 & operator+=(const Vec3 & rhs) { v = DirectX::XMVectorAdd(v, rhs.v); return *this; }
		Vec3 & operator-=(const Vec3 & rhs) { v = DirectX::XMVectorSubtract(v, rhs.v); return *this; }
		bool operator==(const Vec3 & rhs) const { return DirectX::XMComparisonAllTrue(DirectX::XMVector3EqualR(v, rhs.v)); }
		float operator[](int i) const { return DirectX::XMVectorGetByIndex(v, i); }
		Vec3 operator-() const { return DirectX::XMVectorNegate(v); }
		Vec3 operator-(Vec3 const & other) const { return DirectX::XMVectorSubtract(v, other); }
		Vec3 operator+(Vec3 const & other) const { return DirectX::XMVectorAdd(v, other); }

		/*** Members ***/
		DirectX::XMVECTOR v;
	};

	inline float Dot(Vec3 const & lhs, Vec3 const & rhs) { return lhs.Dot(rhs); }
	inline Vec3 Cross(Vec3 const & lhs, Vec3 const & rhs) { return lhs.Cross(rhs); }
	inline float Length(Vec3 const & vec) { return vec.Length(); }
	inline Vec3 Normalize(Vec3 const & vec) { Vec3 newVec(vec); newVec.Normalize(); return newVec; }
	inline Vec3 operator*(float scalar, Vec3 const & vec) { return vec * scalar; }

}
