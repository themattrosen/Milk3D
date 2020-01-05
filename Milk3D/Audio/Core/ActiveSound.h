// Morgen Hyde

#pragma once

#include <Audio/Core/AudioData.h>

#include <memory>

namespace Milk3D {

	class ActiveSound {

	public:

		ActiveSound(std::weak_ptr<AudioData> audioData);
		~ActiveSound();

		void GetProcessedBuffer(float* buffer, const int numFrames);

		__inline int GetNumSamples() const {
			 
			return m_samples;
		}

		__inline short GetChannels() const {

			return m_channels;
		}

	private:

		float* m_data;
		short m_channels;
		int m_samples;
		int m_readIndex;
	};

} // namespace Milk3D