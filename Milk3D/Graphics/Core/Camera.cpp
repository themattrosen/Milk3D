#include "Camera.h"

namespace Milk3D
{

	Camera::Camera() : m_position(0, 0, 0), m_baseLookAt(0, 0, 1), m_baseUp(0, 1, 0)
	{

	}
	Camera::Camera(Vec3 const & lookAtVector, Vec3 const & upVector) : 
		m_position(0, 0, 0), m_baseLookAt(Normalize(lookAtVector)), m_baseUp(Normalize(upVector))
	{

	}
	Camera::Camera(Vec3 const & position) : m_position(position), m_baseLookAt(0, 0, 1), m_baseUp(0, 1, 0)
	{

	}

	void Camera::SetLookAt(Vec3 const & newLookAt)
	{
		m_baseLookAt = Normalize(newLookAt);
		m_isDirty = true;
	}
	void Camera::SetUp(Vec3 const & newUp)
	{
		m_baseUp = Normalize(newUp);
		m_isDirty = true;
	}

	void Camera::SetPosition(Vec3 const & position)
	{
		m_position = position;
		m_isDirty = true;
	}
	void Camera::SetRotation(Quaternion const & rotation)
	{
		m_rotation = rotation;
		m_isDirty = true;
	}

	Matrix const & Camera::GetViewingMatrix() const
	{
		if (m_isDirty)
		{
			m_isDirty = false;
			
			m_lookAt = m_rotation * m_baseLookAt;
			m_up = m_rotation * m_baseUp;
			m_right = Cross(m_lookAt, m_up);

			m_matrix = DirectX::XMMatrixLookToLH(m_position.v, m_lookAt.v, m_up.v);
		}

		return m_matrix;
	}
}
