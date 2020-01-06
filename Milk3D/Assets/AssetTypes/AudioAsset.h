#pragma once

#include "Asset.h"

namespace Milk3D
{
	class AudioAsset : public Asset
	{
	public:
		AudioAsset(const std::string& dir, const std::string& filename) :
			Asset(dir, filename, at_AudioAsset)
		{
			auto pind = filename.find_last_of('.');

			m_eventName = filename.substr(0, pind);
		}

		const std::string& GetEventName() const { return m_eventName; }

		void LoadInternal() override {}
		void UnloadInternal() override {}

	private:
		std::string m_eventName;
	};
}