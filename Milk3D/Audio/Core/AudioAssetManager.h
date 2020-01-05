// Morgen Hyde

#pragma once

#include <Audio/Core/AudioData.h>

#include <unordered_map>
#include <memory>

namespace Milk3D {

	class AudioAssetManager {

	public:

		AudioAssetManager(const std::string& rootDir = "");
		~AudioAssetManager();

		// Setup/Utility functions
		void SetAssetRootDirectory(const std::string& rootDir);

		// Loading and saving functions
		void LoadDirectory(const std::string& = "");
		std::shared_ptr<AudioData> LoadFile(const std::string& filename);

		// Get functions
		std::shared_ptr<AudioData> GetFile(const std::string& filename);

	private:

		std::shared_ptr<AudioData> LoadWave(const std::string& filename);
		std::weak_ptr<AudioData> LoadADPCM(const std::string& filename);

		std::string m_rootDirectory;
		std::unordered_map<std::string, std::shared_ptr<AudioData>> m_assetMap;

	};

} // namespace Milk3D