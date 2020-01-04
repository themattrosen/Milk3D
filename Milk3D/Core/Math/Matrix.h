#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Milk3D
{
	struct Matrix
	{
		Matrix() : m(DirectX::XMMatrixIdentity()) {}
		Matrix(float diagVal) : m({ diagVal, 0, 0, 0 }, { 0,diagVal, 0, 0 }, { 0,0,diagVal,0 }, { 0,0,0,1 }) {}
		Matrix(const std::initializer_list<std::initializer_list<float>>& list) 
		{ 
			DirectX::XMFLOAT4X4 mat;
			int c = 0, r = 0;
			for (auto it = list.begin(); it != list.end(); ++it)
			{
				c = 0;
				for (auto e : *it)
				{
					mat.m[r][c++] = e;
				}
				r++;
			}
			*this = mat;
		}
		Matrix(const std::initializer_list<float>& list) 
		{
			DirectX::XMFLOAT4X4 mat;
			int c = 0, r = 0;
			for (auto e : list)
			{
				mat.m[r][c++] = e;
				if (c >= 4)
				{
					r++;
					c = 0;
				}
			}
		}

		Matrix(Matrix const & other) : m(other.m) { }
		Matrix & operator=(Matrix const & other) { m = other.m; return *this; }

		//*** Conversions ***//
		Matrix(DirectX::XMMATRIX const & other) { m = other; }
		Matrix & operator=(DirectX::XMMATRIX const & other) { m = other; return *this; }
		operator DirectX::XMMATRIX() const { return m; }

		Matrix(DirectX::XMFLOAT4X4 const & other) : m(DirectX::XMLoadFloat4x4(&other)) { }
		Matrix & operator=(DirectX::XMFLOAT4X4 const & other) { m = DirectX::XMLoadFloat4x4(&other); }
		operator DirectX::XMFLOAT4X4() const { return GetFloat4x4(); }

		//*** Helper Functions ***//

		DirectX::XMMATRIX GetMatrix() const { return m; }
		DirectX::XMFLOAT4X4 GetFloat4x4() const { DirectX::XMFLOAT4X4 mat; DirectX::XMStoreFloat4x4(&mat, m); return mat; }

		Matrix & Transpose() { m = DirectX::XMMatrixTranspose(m); return *this; }
		Matrix & Identity() { m = DirectX::XMMatrixIdentity(); return *this; }
		Matrix & Inverse() { m = DirectX::XMMatrixInverse(nullptr, m); return *this; }

		/*** Operations ***/

		Matrix operator*(float scalar) const { return Matrix(m * scalar); }
		Vec4 operator*(Vec4 const & vector) const { return DirectX::XMVector4Transform(vector.v, m); }
		Vec3 operator*(Vec3 const & vector) const { return DirectX::XMVector3Transform(vector.v, m); }
		Vec2 operator*(Vec2 const & vector) const { return DirectX::XMVector2Transform(vector.v, m); }
		Matrix operator*(const Matrix & rhs) const { return DirectX::XMMatrixMultiply(m, rhs); }
		Matrix & operator*=(float scalar) { m *= scalar; }
		Matrix & operator*=(const Matrix & rhs) { m *= rhs.m; return *this; }
		Matrix & operator+=(const Matrix & rhs) { m += rhs.m; return *this; }
		Matrix & operator-=(const Matrix & rhs) { m -= rhs.m; return *this; }
		bool operator==(const Matrix & rhs) const 
		{
			for (int i = 0; i < 4; i++)
			{
				uint32_t comparisonValue = DirectX::XMVector4EqualR(m.r[i], rhs.m.r[i]);
				if (!DirectX::XMComparisonAllTrue(comparisonValue))
					return false;
			}
			return true;  
		}
		Vec4 operator[](int i) const { return m.r[i]; }
		DirectX::XMVECTOR & operator[](int i) { return m.r[i]; }

		/*** Members ***/
		DirectX::XMMATRIX m;
	};
	
	inline Matrix Identity() { return Matrix(); }
	inline Matrix Rotation(Vec3 const & axis, float radians) { return Matrix(DirectX::XMMatrixRotationAxis(axis.v, radians)); }
	inline Matrix Scale(float x, float y, float z) { return Matrix(DirectX::XMMatrixScaling(x, y, z)); }
	inline Matrix Scale(Vec3 const & scale) { return Matrix(DirectX::XMMatrixScalingFromVector(scale.v)); }
	inline Matrix Scale(float scale) { return Scale(scale, scale, scale); }
	inline Matrix Translate(float x, float y, float z) { return DirectX::XMMatrixTranslation(x, y, z); }
	inline Matrix Translate(Vec3 const & translation) { return DirectX::XMMatrixTranslationFromVector(translation.v); }

}

