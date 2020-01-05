// Morgen Hyde

#include <Audio/Audio.h>
#include <Audio/Core/AudioConfig.h>

#include "Audio/Headers/portaudio.h"

#include <iostream>

namespace Milk3D {

	int PACallbackFunc(const void* p_input, void* p_output, unsigned long p_frameCount,
		const PaStreamCallbackTimeInfo* p_timeInfo,
		PaStreamCallbackFlags p_statusFlags, void* p_userData);

	AudioSystem::AudioSystem()
		: System(), m_audioAssetManager("Assets\\Audio\\") {

	}

	AudioSystem::~AudioSystem() {

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

		std::weak_ptr<AudioData> data = m_audioAssetManager.LoadFile("Screm.wav");
		if (data.lock() == nullptr) {
			std::cout << "Test" << std::endl;
		}
	}

	void AudioSystem::OnEvent(SystemUpdateEvent* e) {

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

		for (unsigned i = 0; i < samples; i += 2) {

			*pa_output++ = m_triangleOutput;
			*pa_output++ = m_triangleOutput;

			m_triangleOutput += m_triangleSlope;

			if (m_triangleOutput >= 1.0f) {
				m_triangleSlope *= -1.0f;
			}

			if (m_triangleOutput <= -1.0f) {
				m_triangleSlope *= -1.0f;
			}
		}

		pa_output -= samples;
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