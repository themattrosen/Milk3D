#pragma once
#include "Component.h"

namespace Milk3D
{
	class AudioComponent : public ComponentType<AudioComponent>
	{
	public:
		void OnUpdate(float dt) override;
		void OnActivate() override;
		void OnDeactivate() override;

		void Serialize(Serializer& s) override;
	private:
		bool m_isListener;
		Vec3 m_offset;
	};
}
