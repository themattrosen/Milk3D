#include "AudioComponent.h"

namespace Milk3D
{

	void AudioComponent::OnUpdate(float dt)
	{
	}

	void AudioComponent::OnActivate()
	{
	}

	void AudioComponent::OnDeactivate()
	{
	}

	void AudioComponent::Serialize(Serializer & s)
	{
		s % m_isListener % m_offset;
	}

}
