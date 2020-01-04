//Matthew Rosen
#pragma once

#include <Common.h>
#include <cstdlib>
#include <cstring>


namespace Milk3D
{

	// simple stack allocator
	// Alloc<T> allocates an object without constructing it
	//		it can also allocate an array of objects without construction
	// Construct<T, Args...> allocates an object and constructs it
	// Free<T> frees an object without calling its destructor
	// Destruct<T> frees an object after calling its destructor
	//		Free and Destruct can also free an array of objects (Destruct will call dtor on each)
	//		Must pass in number of elements if its an array
	// Clear resets internal ptrs, doesn't destruct any elements
	//
	// Only basic safety checking is done, a lot of trust is placed in the user
	// Usage: 
	//	* when allocating and freeing, you must free objects in reverse order that 
	//    they were allocated in
	//	* you can allocate objects of any type, and those objects are guaranteed
	//    to be in contiguous memory
	//	* if your ptr becomes misaligned, freeing it will be catastrophic, and 
	//    there's no way for the stack allocator to know until it crashes
	class StackAllocator
	{
	public:
		StackAllocator(size_t sizeInBytes)
			: m_stackSize(sizeInBytes)
		{
			m_memStack = (signed char*)malloc(sizeInBytes);
			M_ASSERT(m_memStack);
			std::memset(m_memStack, 0, sizeInBytes);
			m_nextPtr = m_memStack;
		}

		~StackAllocator()
		{
			M_ASSERT(m_nextPtr >= m_memStack &&
				"Some pointer became misaligned before freeing. This is really bad");

			if (m_memStack)
			{
				free(m_memStack);
				m_memStack = nullptr;
			}

			m_nextPtr = nullptr;
			m_stackSize = 0;
		}

		template<typename T>
		T* Alloc(unsigned numElements = 1)
		{
			M_ASSERT(m_nextPtr + sizeof(T) * numElements <= m_memStack + m_stackSize &&
				"Allocation too big, would go outside stack!")

			T* mem = reinterpret_cast<T*>(m_nextPtr);
			m_nextPtr += sizeof(T) * numElements;

			return mem;
		}

		template<typename T, typename... Args>
		T* Construct(Args... args)
		{
			T* obj = Alloc<T>();
			if(obj)
				T* mem = new (obj) T(args...);
			return obj;
		}

		template<typename T>
		void Free(T* address, unsigned numElements = 1)
		{
			M_ASSERT(address && (signed char*)address == m_nextPtr - sizeof(T) * numElements);
			m_nextPtr -= sizeof(T) * numElements;
			std::memset(m_nextPtr, 0, sizeof(T) * numElements);
		}

		template<typename T>
		void Destruct(T* address, unsigned numElements = 1)
		{
			M_ASSERT(address);
			for(unsigned i = 0; i < numElements; ++i)
				address[i].~T();
			Free(address);
		}

		void Clear()
		{
			m_nextPtr = m_memStack;
		}

	private:
		__int64 m_stackSize;
		signed char* m_memStack;
		signed char* m_nextPtr;
	};
}
