#pragma once

#include <DirectXMath.h>
#include <initializer_list>

namespace Milk3D
{
	struct Vec2
	{
		Vec2() : v(DirectX::XMVectorZero()) {}
		Vec2(float xy) : v(DirectX::XMVectorSet(xy, xy, 0, 0)) {}
		Vec2(float x, float y) : v(DirectX::XMVectorSet(x, y, 0, 0)) {}
		Vec2(const std::initializer_list<float> & list)
		{
			float f[2]{ 0 };
			auto it = list.begin();
			for (int i = 0; i < 2, it != list.end(); ++i, ++it)
			{
				f[i] = *it;
			}
			v = DirectX::XMVectorSet(f[0], f[1], 1, 1);
		}

		Vec2(Vec2 const & other) : v(other.v) {}
		Vec2 & operator=(Vec2 const & other) { v = other.v; return *this;}

		//*** Conversions ***//
		Vec2(DirectX::XMVECTOR const & other) : v(other) {}
		Vec2 & operator=(DirectX::XMVECTOR const & other) { v = other; return *this; }
		operator DirectX::XMVECTOR() const { return v; }

		Vec2(DirectX::XMFLOAT2 const & other) : v(DirectX::XMLoadFloat2(&other)) {}
		Vec2 & operator=(DirectX::XMFLOAT2 const & other) { v = (DirectX::XMLoadFloat2(&other)); return *this; }
		operator DirectX::XMFLOAT2() const { return GetFloat2(); }

		//*** Helper Functions ***//

		DirectX::XMVECTOR GetVector() const { return v; }
		DirectX::XMFLOAT2 GetFloat2() const { DirectX::XMFLOAT2 vec; DirectX::XMStoreFloat2(&vec, v); return vec; }
		float Dot(Vec2 const & rhs) const { return DirectX::XMVectorGetX(DirectX::XMVector2Dot(v, rhs.v)); }
		float Length() const { return DirectX::XMVectorGetX(DirectX::XMVector2Length(v)); }
		void Normalize() { DirectX::XMVector2Normalize(v); }
		float X() const { return DirectX::XMVectorGetX(v); }
		float Y() const { return DirectX::XMVectorGetY(v); }
		float Z() const { return DirectX::XMVectorGetZ(v); }
		float W() const { return DirectX::XMVectorGetW(v); }
		void SetX(float x) { v = DirectX::XMVectorSetX(v, x); }
		void SetY(float y) { v = DirectX::XMVectorSetY(v, y); }
		void Set(float x, float y) { v = DirectX::XMVectorSet(x, y, Z(), W()); }

		//*** Operations ***//

		Vec2 operator*(float scalar) const { return Vec2(DirectX::XMVectorScale(v, scalar)); }
		float operator*(DirectX::XMFLOAT2 const & vector) const { return Dot(vector); }
		float operator*(DirectX::XMVECTOR vector) const { return Dot(vector); }
		float operator*(const Vec2 & vector) const { return Dot(vector); }
		Vec2 & operator*=(float scalar) { v = DirectX::XMVectorScale(v, scalar); return *this; }
		Vec2 & operator+=(const Vec2 & rhs) { v = DirectX::XMVectorAdd(v, rhs.v); return *this; }
		Vec2 & operator-=(const Vec2 & rhs) { v = DirectX::XMVectorSubtract(v, rhs.v); return *this; }
		bool operator==(const Vec2 & rhs) const { return DirectX::XMComparisonAllTrue(DirectX::XMVector2EqualR(v, rhs.v)); }
		float operator[](int i) const { return DirectX::XMVectorGetByIndex(v, i); }
		Vec2 operator-() const { return DirectX::XMVectorNegate(v); }
		Vec2 operator-(Vec2 const & other) const { return DirectX::XMVectorSubtract(v, other); }
		Vec2 operator+(Vec2 const & other) const { return DirectX::XMVectorAdd(v, other); }

		/*** Members ***/
		DirectX::XMVECTOR v;
	};

	inline float Dot(Vec2 const & lhs, Vec2 const & rhs) { return lhs.Dot(rhs); }
	inline Vec2 Cross(Vec2 const & lhs, Vec2 const & rhs) { return lhs.Dot(rhs); }
	inline float Length(Vec2 const & vec) { return vec.Length(); }
	inline Vec2 Normalize(Vec2 const & vec) { Vec2 newVec(vec); newVec.Normalize(); return newVec; }
	inline Vec2 operator*(float scalar, Vec2 const & vec) { return vec * scalar; }

}

