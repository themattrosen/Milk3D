// Morgen Hyde

#include <Audio/Core/ActiveSound.h>
#include <Audio/Core/AudioConfig.h>

#include <algorithm>

namespace Milk3D {

	ActiveSound::ActiveSound(std::weak_ptr<AudioData> audioData, std::shared_ptr<SoundState> assignedState)
		: m_stateInformation(assignedState), m_channels(CHANNELS), m_readIndex(0) {

		std::shared_ptr<AudioData> data = audioData.lock();
		data->GetSamples(&m_data, &m_samples, 2);
		m_samples *= 2;
		m_audioName = data->GetSoundName();

		m_3DAttrib.SetPanningAmount(0.5f);
	}

	ActiveSound::~ActiveSound() {

		delete[] m_data;
	}

	bool ActiveSound::GetProcessedBuffer(float* buffer, const int numFrames) {

		int outputSamples = m_channels * numFrames;

		// Temp until envelope implementation //
		if (m_stateInformation->IsStopping()) {
			m_stateInformation->SetStoppedState();
		}

		if (!m_stateInformation->IsActive()) {
			
			std::memset(buffer, 0, sizeof(float) * outputSamples);
			return false;
		}

		int samplesToCopy = std::min(outputSamples, m_samples - m_readIndex);

		std::memcpy(buffer, (m_data + m_readIndex), sizeof(float) * samplesToCopy);
		float* bufferStart = buffer;
		float leftGain = m_3DAttrib.GetLeftGain();
		float rightGain = m_3DAttrib.GetRightGain();

		for (int i = 0; i < samplesToCopy; i += 2) {

			*buffer++ *= leftGain;
			*buffer++ *= rightGain;
		}
		buffer = bufferStart;

		if (samplesToCopy < outputSamples) {
			std::memset(buffer + samplesToCopy, 0, sizeof(float) * (outputSamples - samplesToCopy));
		}

		m_readIndex += samplesToCopy;

		if (m_readIndex >= m_samples) {
			m_readIndex = m_samples;
			m_stateInformation->SetStoppingState();
		}

		return m_stateInformation->IsActive();
	}

	void ActiveSound::SetSound3DAttributes(const Sound3DAttributes& attributes) {

		m_3DAttrib = attributes;
	}

} // namespace Milk3D 