// Morgen Hyde

#pragma once
#include <string>

namespace Milk3D {

	class ActiveSound;

	enum PlayingState {
		ps_Virtual = -1,
		ps_ToPlay,
		ps_Playing,
		ps_Stopping,
		ps_Stopped
	};

	class SoundState
	{
	public:
		SoundState();
		~SoundState();

		void AssignActiveSound(ActiveSound* newSound);
		ActiveSound* RemoveActiveSound();

		__inline bool IsAssigned() const { return m_assignedSound != nullptr; }
		__inline bool IsActive() const { return m_currentState != PlayingState::ps_Stopped; }
		__inline bool IsStopping() const { return m_currentState == PlayingState::ps_Stopping; }

		// Functions for getting and setting certain state 
		// information from and to the sound
		void SetStoppingState();
		bool SetStoppingIfNamed(const std::string& soundName);

		__inline void SetStoppedState() { m_currentState = PlayingState::ps_Stopped; }

	private:
		friend bool operator<(const SoundState&, const SoundState&);

		void ResetSoundState();

		ActiveSound* m_assignedSound;
		PlayingState m_currentState;
		unsigned short m_attachedPriority;
	};

	bool operator<(const SoundState& lhs, const SoundState& rhs);

} // namespace Milk3D