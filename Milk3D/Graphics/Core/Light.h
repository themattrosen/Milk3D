#pragma once

#include "Core/Math/Vector3.h"

namespace Milk3D
{
	enum LightType : int32_t
	{
		point = 0,
		spot,
		directional
	};
	struct Light
	{
		Light() = default;
		Light(int32_t enabled_, Vec3 const & position_, int32_t lightType_, Vec3 const & direction_, Vec3 const & color_)
			: enabled(enabled_), position(position_), type(lightType_), direction(direction_), color(color_) {}

		Light(Vec3 const & position_, int32_t lightType_, Vec3 const & direction_, Vec3 const & color_) 
			: position(position_), type(lightType_), direction(direction_), color(color_) {}

		int32_t enabled = 1;
		Vec3 position = Vec3(0, 0, 0);
		int32_t type = LightType::point;
		Vec3 direction = Vec3(0, 0, 0);
		Vec3 color = Vec3(0, 0, 0);
		float padding = 0.0f;
	};
}