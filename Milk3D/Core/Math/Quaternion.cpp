#include "Quaternion.h"

namespace Milk3D
{
	Quaternion::Quaternion() : isDirty(false) {}

	Quaternion::Quaternion(Vec4 const & newRotation) : rotation(newRotation), isDirty(true)
	{
		
	}
	Quaternion::Quaternion(Vec3 const & axis, float angle) : rotation(DirectX::XMQuaternionRotationAxis(axis, angle)), isDirty(true)
	{
		
	}
	Quaternion::Quaternion(float x, float y, float z, float w) : rotation({ x,y,z,w }), isDirty(true)
	{

	}

	Quaternion::Quaternion(float roll, float pitch, float yaw) : rotation(DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll)), isDirty(true)
	{
		
	}

	void Quaternion::SetRotation(float x, float y, float z, float w)
	{
		*this = Quaternion(x, y, z, w);
		isDirty = true;
	}
	void Quaternion::SetRotation(Quaternion const & other)
	{
		*this = other;
		isDirty = true;
	}
	void Quaternion::SetRotation(Vec3 const & axis, float angle)
	{
		*this = Quaternion(axis, angle);
		isDirty = true;
	}

	void Quaternion::AddRotation(float x, float y, float z, float w)
	{
		*this += Quaternion(x, y, z, w);
	}
	void Quaternion::AddRotation(Quaternion const & other)
	{
		*this += other;
		isDirty = true;
	}
	void Quaternion::AddRotation(Vec3 const & axis, float angle)
	{
		*this += Quaternion(axis, angle);
		isDirty = true;
	}

	// TODO: 
	Quaternion & Quaternion::operator += (Quaternion const & other)
	{
		rotation += other.rotation;
		isDirty = true;
		return *this;
	}

	// TODO:
	Quaternion & Quaternion::operator -= (Quaternion const & other)
	{
		rotation -= other.rotation;
		isDirty = true;
		return *this;
	}
	Quaternion & Quaternion::operator *= (Quaternion const & other)
	{
		rotation = DirectX::XMQuaternionMultiply(rotation.v, other.rotation.v);
		isDirty = true;
		return *this;
	}

	Quaternion Quaternion::operator+(Quaternion const & other) const
	{
		Quaternion rot(*this);
		rot += other;
		return rot;
	}
	Quaternion Quaternion::operator-(Quaternion const & other) const
	{
		Quaternion rot(*this);
		rot -= other;
		return rot;
	}
	Quaternion Quaternion::operator*(Quaternion const & other) const
	{
		Quaternion rot(*this);
		rot *= other;
		return rot;
	}

	Vec3 Quaternion::operator*(Vec3 const & other) const
	{
		return DirectX::XMVector3Transform(other.v, GetMatrix());
	}

	Quaternion Quaternion::operator-() const
	{
		return Inverse();
	}

	Quaternion Quaternion::Inverse() const
	{
		Quaternion inverse;
		inverse.rotation = DirectX::XMQuaternionInverse(rotation);
		return inverse;
	}

	DirectX::XMMATRIX const & Quaternion::GetMatrix() const
	{
		if (isDirty)
		{
			isDirty = false;
			matrix.m = DirectX::XMMatrixRotationQuaternion(rotation.v);
		}
		return matrix.m;
	}

	void Quaternion::GetAxisAngle(Vec3 & axis, float & angle)
	{
		DirectX::XMQuaternionToAxisAngle(&axis.v, &angle, rotation);
	}

	float Quaternion::X() const
	{
		return rotation.X();
	}
	float Quaternion::Y() const
	{
		return rotation.Y();
	}
	float Quaternion::Z() const
	{
		return rotation.Z();
	}
	float Quaternion::W() const
	{
		return rotation.W();
	}
}
