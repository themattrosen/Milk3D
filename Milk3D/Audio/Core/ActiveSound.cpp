// Morgen Hyde

#include <Audio/Core/ActiveSound.h>
#include <Audio/Core/AudioConfig.h>

#include <algorithm>

namespace Milk3D {

	ActiveSound::ActiveSound(std::weak_ptr<AudioData> audioData)
		: m_channels(CHANNELS), m_readIndex(0) {

		audioData.lock()->GetSamples(&m_data, &m_samples, 2);
		m_samples *= 2;
	}

	ActiveSound::~ActiveSound() {

		delete[] m_data;
	}

	void ActiveSound::GetProcessedBuffer(float* buffer, const int numFrames) {

		int outputSamples = m_channels * numFrames;

		int samplesToCopy = std::min(outputSamples, m_samples - m_readIndex);

		std::memcpy(buffer, (m_data + m_readIndex), sizeof(float) * samplesToCopy);

		if (samplesToCopy < outputSamples) {
			std::memset(buffer + samplesToCopy, 0, sizeof(float) * (outputSamples - samplesToCopy));
		}

		m_readIndex += samplesToCopy;

		if (m_readIndex >= m_samples) {
			m_readIndex = m_samples;
		}
	}


} // namespace Milk3D 