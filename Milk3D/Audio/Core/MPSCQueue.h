// Morgen Hyde

#pragma once

#include <atomic>
#include <vector>

namespace Milk3D {

	class ActiveSound;

	class MPSCQueue
	{
	public:
		// This performs best when maxElements is a power of 2
		MPSCQueue(unsigned maxElements = 64);
		~MPSCQueue();

		bool Push(ActiveSound* newActiveSound);
		ActiveSound* Pop();

		unsigned GetSize() const;
		unsigned GetCapacity() const;

		bool Empty() const;
		bool Full() const;

	private:

		std::atomic<unsigned> m_head;
		std::atomic<unsigned> m_tail;
		std::atomic<unsigned> m_reserve;

		std::atomic<unsigned> m_size;

		const unsigned m_maxElements;
		ActiveSound** m_soundArray;
		std::vector<ActiveSound*> m_debugArray;
	};

} // namespace Milk3D