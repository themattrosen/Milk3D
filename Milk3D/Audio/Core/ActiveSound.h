// Morgen Hyde

#pragma once

#include <Audio/Core/AudioData.h>
#include <Audio/Core/SoundState.h>
#include <Audio/Core/3DSoundAttributes.h>

#include <memory>
#include <string>

namespace Milk3D {

	class ActiveSound {

	public:

		ActiveSound(std::weak_ptr<AudioData> audioData, std::shared_ptr<SoundState> assignedState);
		~ActiveSound();

		ActiveSound(const ActiveSound&) = delete;
		ActiveSound& operator=(const ActiveSound&) = delete;

		bool GetProcessedBuffer(float* buffer, const int numFrames);

		void SetSound3DAttributes(const Sound3DAttributes& attributes);

		__inline bool IsActive() const {

			return m_stateInformation->IsActive();
		}

		__inline int GetNumSamples() const {
			 
			return m_samples;
		}

		__inline short GetChannels() const {

			return m_channels;
		}

		_inline const std::string& GetAudioName() const {

			return m_audioName;
		}

	private:

		std::shared_ptr<SoundState> m_stateInformation;
		std::string m_audioName;

		float* m_data;
		short m_channels;
		int m_samples;
		int m_readIndex;

		Sound3DAttributes m_3DAttrib;
	};

} // namespace Milk3D