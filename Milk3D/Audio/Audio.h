// Morgen Hyde

#pragma once

#include <Core\System\System.h>

#include <Audio\Core\AudioAssetManager.h>
#include <Audio\Core\ActiveSound.h>

typedef void PaStream;

namespace Milk3D {

	class AudioSystem : public System {

	public:
		AudioSystem();
		~AudioSystem();

		// System Functions
		void OnEvent(SystemInitEvent* e) override;
		void OnEvent(SystemExitEvent* e) override;
		void OnEvent(SystemUpdateEvent* e) override;

		// Audio Callback function for signal processing
		void AudioCallback(float* pa_output, unsigned long& pa_frameCount);

		// Audio playback control functions (event callbacks and direct functions)
		void StopAll();

		//void OnEvent(PlayEvent* e);
		//void OnEvent(StopEvent* e);
	
	private:

		// Audio Playback Objects //

		PaStream* m_outStream;
		//MPSCQueue m_soundQueue;
		//std::vector<SoundState*> m_soundStates;
		unsigned int m_nextAvailableState;
		ActiveSound* m_activeSound;

		float m_triangleOutput;
		float m_triangleSlope;

		// Singletons //

		AudioAssetManager m_audioAssetManager;
	};
}