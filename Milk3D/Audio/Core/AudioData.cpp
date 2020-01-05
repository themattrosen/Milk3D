// Morgen Hyde

#include <Audio/Core/AudioData.h>
#include <Audio/Core/FileDefinitions.h>

#include <cstring>

namespace Milk3D {

	AudioData::AudioData(const fmt_Chunk& format, float* data, int samples)
		: m_format(format.format), m_channels(format.channels), m_sampleRate(format.sampleRate),
		  m_byteRate(format.byteRate), m_blockAlign(format.blockAlign), m_bitsPerSample(format.bitsPerSample),
	      m_data(data), m_samples(samples), m_frames(samples / m_channels) 
	{
	}

	AudioData::~AudioData() {

		delete[] m_data;
	}

	void AudioData::GetSamples(float** data, int* frameCount, int targetChannels, int targetSampleRate) {

		if (!targetSampleRate) {

			*frameCount = m_frames;
			float* output = new float[m_samples];

			std::memcpy(output, m_data, sizeof(float) * m_samples);

			*data = output;
			
			return;
		}
		
		float fractionalIndex = 0.0f;
		float fractionalInc = static_cast<float>(targetSampleRate) / static_cast<float>(m_sampleRate);

		*frameCount = m_frames * fractionalInc;

		float* output = new float[*frameCount * m_channels];

		// TODO: resample audio file to meet target output via linear interpolation
		for (; fractionalIndex < *frameCount; fractionalIndex += fractionalInc) {

			  
		}
	}

} // namespace Milk3D