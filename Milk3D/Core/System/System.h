#pragma once

namespace Milk3D
{
	class System
	{
	public:
		virtual ~System() {}
		virtual void Init() = 0;
		virtual void Update(float dt) = 0;
		virtual void LateUpdate() {}
		virtual void Exit() = 0;

	};
}
