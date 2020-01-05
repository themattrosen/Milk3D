
#pragma once

#include "Core/Math/Math.h"

namespace Milk3D
{

	class Camera
	{
	public:
		Camera();
		Camera(Vec3 const & lookAt, Vec3 const & up);
		Camera(Vec3 const & position);

		//*** Setters ***//
		void SetLookAt(Vec3 const & newLookAt);
		void SetUp(Vec3 const & newUp);
		void SetPosition(Vec3 const & position);
		void SetRotation(Quaternion const & other);
		
		//*** Getters ***//
		Matrix const & GetViewingMatrix() const;
		Vec3 GetLookAt() const { GetViewingMatrix(); return m_lookAt; }
		Vec3 GetUp() const { GetViewingMatrix(); return m_up; }
		Vec3 GetRight() const { GetViewingMatrix(); return m_right; }
		bool IsDirty() const { return m_isDirty; }
		Vec3 GetPosition() const { return m_position; }
		Quaternion const & GetRotation() const { return m_rotation; }

	private:
		Vec3 m_position = Vec3(0, 0, 0);
		Quaternion m_rotation;

		Vec3 m_baseLookAt;
		Vec3 m_baseUp;

		mutable Matrix m_matrix;
		mutable Vec3 m_lookAt;
		mutable Vec3 m_up;
		mutable Vec3 m_right;
		mutable bool m_isDirty = true;
	};
	
}

