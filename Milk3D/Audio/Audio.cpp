// Morgen Hyde

#include <Audio/Audio.h>
#include <Audio/Core/AudioConfig.h>

#include "Audio/Headers/portaudio.h"

#include <iostream>

namespace Milk3D {

	int PACallbackFunc(const void* p_input, void* p_output, unsigned long p_frameCount,
		const PaStreamCallbackTimeInfo* p_timeInfo,
		PaStreamCallbackFlags p_statusFlags, void* p_userData);

	struct SoundStateSorter {

		bool operator()(std::shared_ptr<SoundState> lhs, std::shared_ptr<SoundState> rhs) {

			if (*lhs < *rhs) {
				return true;
			}
			else {
				return false;
			}
		}
	} SoundStateSorting;

	AudioSystem::AudioSystem()
		: System(), m_audioAssetManager("Assets\\Audio\\"), 
		  m_helperBuffer(new float[CHANNELS * AUDIOFRAMES]) {

		for (int i = 0; i < MAXSOUNDS; ++i) {

			m_soundStates.push_back(std::make_shared<SoundState>());
		}

	}

	AudioSystem::~AudioSystem() {

		delete[] m_helperBuffer;
	}

	void AudioSystem::OnEvent(SystemInitEvent* e) {

		PaError error = Pa_Initialize();
		if (error != paNoError) {
			std::cout << Pa_GetErrorText(error) << std::endl;
		}

		// setup the audio output stream
		PaStreamParameters streamParam;
		streamParam.device = Pa_GetDefaultOutputDevice();
		streamParam.channelCount = CHANNELS;
		streamParam.sampleFormat = paFloat32;
		streamParam.suggestedLatency = Pa_GetDeviceInfo(streamParam.device)->defaultHighOutputLatency;
		//std::cout << streamParam.suggestedLatency << std::endl;
		streamParam.hostApiSpecificStreamInfo = 0;

		Pa_OpenStream(&m_outStream, 0, &streamParam, SAMPLERATE, AUDIOFRAMES, 0, PACallbackFunc, this);
		error = Pa_StartStream(m_outStream);
		if (error) {
			std::cout << "Error on stream start: " << Pa_GetErrorText(error) << std::endl;
		}

		m_triangleOutput = 0.0f;

		float samplesPerCycle = (float)SAMPLERATE / WAVEFREQUENCY;
		m_triangleSlope = WAVEGAIN / (samplesPerCycle / 4.0f);
	}

	void AudioSystem::OnEvent(SystemUpdateEvent* e) {

		for (int i = MAXSOUNDS - 1; i >= 0; --i) {

			std::shared_ptr<SoundState> curr = m_soundStates[i];

			if (curr->IsAssigned()) {

				if (!(curr->IsActive())) {

					// If assigned and not active we can reclaim the sound state
					ActiveSound* deadSound = curr->RemoveActiveSound();
					if (deadSound) {

						delete deadSound;
					}
				}
			}
			// If unassigned, we can exit early, since its sorted by readiness
			else {

				break;
			}
		}

		// Sort the sound states to preserve state ordering
		std::sort(m_soundStates.begin(), m_soundStates.end(), SoundStateSorting);
	}

	void AudioSystem::OnEvent(SystemExitEvent* e) {

		PaError l_error = Pa_StopStream(m_outStream);
		if (l_error != paNoError) {
			std::cout << Pa_GetErrorText(l_error) << std::endl;
		}

		l_error = Pa_Terminate();
		if (l_error != paNoError) {
			std::cout << Pa_GetErrorText(l_error) << std::endl;
		}
	}

	void AudioSystem::AudioCallback(float* pa_output, unsigned long& pa_frameCount) {

		unsigned int samples = pa_frameCount * CHANNELS;
		std::memset(pa_output, 0, samples * sizeof(float));

		if (m_activeSound) {

			m_activeSound->GetProcessedBuffer(pa_output, pa_frameCount);
		}

		unsigned queueSize = m_soundQueue.GetSize();
		while (queueSize)
		{
			ActiveSound* nextSound = m_soundQueue.Pop();
			bool active = nextSound->GetProcessedBuffer(m_helperBuffer, static_cast<int>(pa_frameCount));

			if (active) {

				m_soundQueue.Push(nextSound);
			}

			for (int i = 0; i < samples; ++i) {

				*pa_output++ += *m_helperBuffer++;
			}

			pa_output -= samples;
			m_helperBuffer -= samples;
			--queueSize;
		}
	}

	void AudioSystem::PlaySound(const std::string& filename) {

		std::shared_ptr<AudioData> audio = m_audioAssetManager.GetFile(filename);
		if (audio == nullptr) {

			audio = m_audioAssetManager.LoadFile(filename);
			if (audio == nullptr) {

				// Give up because somethings very wrong
				return;
			}
		}

		// Find an available sound state (should be front of vector???)
		std::shared_ptr<SoundState> availableState = m_soundStates.front();

		ActiveSound* newSound = new ActiveSound(audio, availableState);
		availableState->AssignActiveSound(newSound);
		m_soundQueue.Push(newSound);

		// Resort the vector for the next play call
		std::sort(m_soundStates.begin(), m_soundStates.end(), SoundStateSorting);
	}

	void AudioSystem::PlaySound(const std::string& filename, float panningAmount) {

		std::shared_ptr<AudioData> audio = m_audioAssetManager.GetFile(filename);
		if (audio == nullptr) {

			audio = m_audioAssetManager.LoadFile(filename);
			if (audio == nullptr) {

				// Give up because somethings very wrong
				return;
			}
		}

		// Find an available sound state (should be front of vector???)
		std::shared_ptr<SoundState> availableState = m_soundStates.front();

		ActiveSound* newSound = new ActiveSound(audio, availableState);

		Sound3DAttributes attrib;
		attrib.SetPanningAmount(panningAmount);
		newSound->SetSound3DAttributes(attrib);

		availableState->AssignActiveSound(newSound);
		m_soundQueue.Push(newSound);

		// Resort the vector for the next play call
		std::sort(m_soundStates.begin(), m_soundStates.end(), SoundStateSorting);

	}

	void AudioSystem::StopSound(const std::string& filename) {

		for (int i = MAXSOUNDS - 1; i >= 0; --i) {

			std::shared_ptr<SoundState> curr = m_soundStates[i];
			if (curr->IsAssigned()) {

				curr->SetStoppingIfNamed(filename);
			}
			// Early exit due to sorting method
			else {

				return;
			}
		}
	}

	void AudioSystem::StopAll() {

	}

	int PACallbackFunc(const void* input, void* output, unsigned long frameCount,
		const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
	{
		AudioSystem* core = reinterpret_cast<AudioSystem*>(userData);
		core->AudioCallback(reinterpret_cast<float*>(output), frameCount);
		return paContinue;
	}

} // namespace Milk3D