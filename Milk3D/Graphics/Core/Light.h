#pragma once

#include "Core/Math/Vector3.h"

namespace Milk3D
{
	enum LightType : int
	{
		point = 0,
		spot,
		directional
	};
	struct Light
	{
		Light() = default;
		Light(int enabled_, Vec3 const & position_, int lightType_, Vec3 const & direction_, Vec3 const & color_)
			: enabled(enabled_), position(position_), type(lightType_), direction(direction_), color(color_) {}

		Light(Vec3 const & position_, int lightType_, Vec3 const & direction_, Vec3 const & color_)
			: position(position_), type(lightType_), direction(direction_), color(color_) {}

		int enabled = 1;
		DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0, 0, 0);
		int type = LightType::point;
		DirectX::XMFLOAT3 direction = DirectX::XMFLOAT3(0, 0, 0);
		DirectX::XMFLOAT3 color = DirectX::XMFLOAT3(0, 0, 0);
		float padding = 0.0f;
	};
}