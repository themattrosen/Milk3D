#pragma once

#include "Matrix.h"
#include "Vector4.h"

namespace Milk3D
{
	class Quaternion
	{
		public:
			Quaternion();
			Quaternion(Vec4 const & rotation);
			Quaternion(Vec3 const & axis, float angle);
			Quaternion(float x, float y, float z, float w);
			Quaternion(float roll, float pitch, float yaw);

			// Default conversion and operator= work
			
			void SetRotation(float x, float y, float z, float w);
			void SetRotation(Quaternion const & other);
			void SetRotation(Vec3 const & axis, float angle);

			void AddRotation(float x, float y, float z, float w);
			void AddRotation(Quaternion const & other);
			void AddRotation(Vec3 const & axis, float angle);

			//*** Operations ***//
			Quaternion & operator+=(Quaternion const & other);
			Quaternion & operator-=(Quaternion const & other);
			Quaternion & operator*=(Quaternion const & other);

			Quaternion operator+(Quaternion const & other) const;
			Quaternion operator-(Quaternion const & other) const;
			Quaternion operator*(Quaternion const & other) const;
			Vec3 operator*(Vec3 const & other) const;

			Quaternion operator-() const;

			Quaternion Inverse() const;

			DirectX::XMMATRIX const & GetMatrix() const;
			 operator DirectX::XMMATRIX const &() const { return GetMatrix(); }

			void GetAxisAngle(Vec3 & axis, float & angle);
			float X() const;
			float Y() const;
			float Z() const;
			float W() const;

		private:
			Vec4 rotation = { 0,0,0,0 };
			mutable Matrix matrix;
			mutable bool isDirty = true;
	};
}

