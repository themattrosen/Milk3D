// Morgen Hyde

#include <Audio/Core/AudioAssetManager.h>
#include <Audio/Core/Int24.h>
#include <Audio/Core/FileDefinitions.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

namespace fs = std::experimental::filesystem;

namespace Milk3D {

	AudioAssetManager::AudioAssetManager(const std::string& rootDir)
		: m_rootDirectory(rootDir) {

	}

	AudioAssetManager::~AudioAssetManager() {

	}

	// Setup/Utility functions
	void AudioAssetManager::SetAssetRootDirectory(const std::string& rootDir) {

		if (rootDir.empty()) {
			return;
		}

		m_rootDirectory = rootDir;
	}

	// Loading and saving functions
	void AudioAssetManager::LoadDirectory(const std::string& directory) {
		for (const auto& entry : fs::recursive_directory_iterator(directory))
		{
			if (fs::is_regular_file(entry.path()))
			{
				auto ptr = LoadFile(entry.path().filename().string());
			}
		}

	}

	std::shared_ptr<AudioData> AudioAssetManager::LoadFile(const std::string& filename) {

		if (filename.find(".wav") != std::string::npos) {

			return LoadWave(filename);
		}

		return std::shared_ptr<AudioData>(nullptr);
	}

	// Get functions
	std::shared_ptr<AudioData> AudioAssetManager::GetFile(const std::string& filename) {

		std::unordered_map<std::string, std::shared_ptr<AudioData>>::iterator iter;
		iter = m_assetMap.find(filename);

		if (iter != m_assetMap.end()) {
			return iter->second;
		}

		return std::shared_ptr<AudioData>(nullptr);
	}

	// Filetype specific loading functions
	std::shared_ptr<AudioData> AudioAssetManager::LoadWave(const std::string& filename) {

		std::ifstream sourceFile(m_rootDirectory + filename, std::ios_base::binary);
		if (!sourceFile.is_open()) {

			return std::shared_ptr<AudioData>(nullptr);
		}

		RIFFChunk actualRiff;
		fmt_Chunk actualFmt;

		sourceFile.read(reinterpret_cast<char*>(&actualRiff), sizeof(RIFFChunk));
		if (memcmp(actualRiff.ID, expectedRiff.ID, 4) != 0) {

			std::cout << std::string("Failed to load " + filename + ", missing RIFF Tag").c_str() << std::endl;
		}

		if (memcmp(actualRiff.type, expectedRiff.type, 4) != 0) {

			std::cout << std::string("Failed to load " + filename + ", missing WAVE Tag").c_str() << std::endl;
		}

		char readHeader[8];
		sourceFile.read(readHeader, CHUNK_HEADER_SIZE);
		while (memcmp(readHeader, expectedFmt.ID, 4) != 0 && !sourceFile.eof()) {

			int ignoreSize = *reinterpret_cast<int*>(readHeader + 4);
			sourceFile.ignore(ignoreSize);
			sourceFile.read(readHeader, CHUNK_HEADER_SIZE);
		}

		if (sourceFile.eof()) {

			std::cout << std::string("Failed to read file " + filename + ", no format chunk").c_str() << std::endl;
		}

		int formatSize = *reinterpret_cast<int*>(readHeader + 4);
		sourceFile.read(reinterpret_cast<char*>(&(actualFmt.format)), formatSize);
		short bitDepth = actualFmt.bitsPerSample;

		sourceFile.read(readHeader, CHUNK_HEADER_SIZE);
		while (memcmp(readHeader, expectedData.ID, 4) != 0 && !sourceFile.eof()) {

			int fakeChunkSize = *reinterpret_cast<int*>(readHeader + 4);
			sourceFile.ignore(fakeChunkSize);
			sourceFile.read(readHeader, CHUNK_HEADER_SIZE);
		}

		if (sourceFile.eof()) {

			std::cout << std::string("Failed to read file " + filename + ", no data chunk").c_str() << std::endl;
		}
		int dataSize = *reinterpret_cast<int*>(readHeader + 4);
		int numSamples = dataSize / (bitDepth / 8);

		float* data = nullptr;

		if (bitDepth == 16) {

			short* shortData = new short[dataSize / sizeof(short)];
			sourceFile.read(reinterpret_cast<char*>(shortData), dataSize);
			data = new float[numSamples];

			for (int i = 0; i < numSamples; ++i) {

				data[i] = *shortData / MAX_16;
				shortData++;
			}

			shortData -= numSamples;
			delete[] shortData;
		}
		else if (bitDepth == 24) {

			Int24* integer24Data = new Int24[dataSize / sizeof(Int24)];
			sourceFile.read(reinterpret_cast<char*>(integer24Data), dataSize);
			data = new float[numSamples];

			for (int i = 0; i < numSamples; ++i) {

				data[i] = static_cast<int>(*integer24Data) / MAX_24;
				++integer24Data;
			}

			integer24Data -= numSamples;
			delete[] integer24Data;
		}
		else if (bitDepth == 32) {

			int* intData = new int[dataSize / sizeof(int)];
			sourceFile.read(reinterpret_cast<char*>(intData), dataSize);
			data = new float[numSamples];

			for (int i = 0; i < numSamples; ++i) {

				data[i] = *intData / MAX_32;
				++intData;
			}

			intData -= numSamples;
			delete[] intData;
		}
		else {

			std::stringstream stream;
			stream << bitDepth;

			// if this gets thrown ask Morgen to implement your favorite bit depth!
			std::cout << std::string("Failed to read file " + filename + ", unsupported bit depth: " + stream.str()).c_str() << std::endl;
		}

		size_t nameEnd = filename.find_last_of('\\');
		std::string localName = filename;
		if (nameEnd != std::string::npos) {
			localName = filename.substr(nameEnd);
		}

		// Pass data off to an audio data object
		std::shared_ptr<AudioData> audioData = std::make_shared<AudioData>(localName, actualFmt, data, numSamples);
		
		m_assetMap.insert( { localName, audioData} );

		return audioData;
	}


} // namespace Milk3D