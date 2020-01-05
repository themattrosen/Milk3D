// Morgen Hyde

#pragma once

#include <Audio/Core/SoundAsset.h>

namespace Milk3D {

	struct fmt_Chunk;

	class AudioData {

	public:
		AudioData(const fmt_Chunk& format, float* data, int samples);
		~AudioData();

		void GetSamples(float** data, unsigned int* frameCount, int targetChannels = 1, int targetSampleRate = 0);

		__inline short GetChannels() {

			return m_channels;
		}

		__inline int GetSampleRate() {

			return m_sampleRate;
		}

		__inline short GetBitsPerSample() {

			return m_bitsPerSample;
		}

	private:

		short m_format;		
		short m_channels;		
		int m_sampleRate;		
		int m_byteRate;		
		short m_blockAlign;   
		short m_bitsPerSample;

		float* m_data;
		int m_samples;
		int m_frames;
	};

} // namespace Milk3D