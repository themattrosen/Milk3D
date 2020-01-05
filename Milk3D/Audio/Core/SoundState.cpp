// Morgen Hyde

#include <Audio/Core/SoundState.h>
#include <Audio/Core/ActiveSound.h>

namespace Milk3D {

	SoundState::SoundState() {

		ResetSoundState();
	}

	SoundState::~SoundState() {

	}

	void SoundState::AssignActiveSound(ActiveSound* newSound) {

		m_assignedSound = newSound;
		m_currentState = ps_Playing;
	}

	ActiveSound* SoundState::RemoveActiveSound() {

		ActiveSound* temp = m_assignedSound;
	
		ResetSoundState();

		return temp;
	}

	void SoundState::SetStoppingState() {

		m_currentState = ps_Stopping;
	}

	bool SoundState::SetStoppingIfNamed(const std::string& soundName) {

		if (soundName == m_assignedSound->GetAudioName()) {

			m_currentState = ps_Stopping;
			return true;
		}

		return false;
	}

	void SoundState::ResetSoundState() {

		m_assignedSound = nullptr;
		m_currentState = ps_ToPlay;
		m_attachedPriority = -1;
	}

	bool operator<(const SoundState& lhs, const SoundState& rhs) {

		if (lhs.m_currentState < rhs.m_currentState) {
			return true;
		}

		return false;
	}

} // namespace Milk3D