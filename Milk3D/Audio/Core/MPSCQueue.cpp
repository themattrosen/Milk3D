// Morgen Hyde

#include <Audio/Core/MPSCQueue.h>

namespace Milk3D {

	MPSCQueue::MPSCQueue(unsigned maxElements)
		: m_maxElements(maxElements)
	{
		m_soundArray = new ActiveSound*[m_maxElements];
		std::memset(m_soundArray, 0, sizeof(ActiveSound*) * m_maxElements);

		m_debugArray.resize(m_maxElements);
	}

	MPSCQueue::~MPSCQueue()
	{
		delete[] m_soundArray;
	}

	bool MPSCQueue::Push(ActiveSound* newActive)
	{
		// Reserve a spot in the circular buffer
		unsigned reservedIndex = m_reserve.fetch_add(1);
		if (Full()) {
			m_reserve.fetch_sub(1);
			return false;
		}

		// Correct the reserved index back into the range of the array
		unsigned correctedIndex = reservedIndex & 0x3F;

		// Write the sound into the circular buffer at the correct index
		m_soundArray[correctedIndex] = newActive;
		m_debugArray[correctedIndex] = newActive;

		// Now the tough part..
		// Attempt to update the tail of the queue to where we just wrote, we can do this
		// if and only if the tail is one less than the the reserve. Otherwise it means some
		// other thread is writing to somewhere earlier in the queue and we gotta wait
		unsigned expectedTail = reservedIndex++;
		while (!m_tail.compare_exchange_strong(expectedTail, reservedIndex)) {}

		m_size.fetch_add(1);
		return true;
	}

	ActiveSound* MPSCQueue::Pop()
	{
		unsigned headLocal = m_head.load();
		if (headLocal == m_tail.load()) {
			return nullptr;
		}

		ActiveSound* popSound = m_soundArray[headLocal & 0x3F];
		popSound = m_debugArray[headLocal & 0x3F];
		m_head.fetch_add(1);

		m_size.fetch_sub(1);
		return popSound;
	}

	unsigned MPSCQueue::GetSize() const {
		return m_size.load();
	}

	unsigned MPSCQueue::GetCapacity() const {
		return m_maxElements;
	}

	bool MPSCQueue::Empty() const
	{
		//  Define head == tail as an empty queue
		return (m_head.load() == m_tail.load());
	}

	bool MPSCQueue::Full() const
	{
		unsigned tailTemp = m_tail.load();
		return (tailTemp + 1 == m_head.load());
	}

} // namespace Milk3D