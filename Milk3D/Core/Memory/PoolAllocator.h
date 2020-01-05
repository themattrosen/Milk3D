//Matthew Rosen
#pragma once

#include <Common.h>
#include <cstdlib>

namespace Milk3D
{
	// internal pool allocator used by PoolAllocator<T>
	// DON'T USE THIS UNLESS YOU REALLY NEED IT
	template<unsigned B>
	class PoolAllocatorImpl
	{
		struct PoolObject
		{
			PoolObject* next;
		};
	public:
		PoolAllocatorImpl(unsigned maxObjects, bool allowExtraAllocations = true)
		{
			M_STATIC_ASSERT(B >= sizeof(void*), "PoolAllocatorImpl is created with size B too small!");
			m_sizePerObject = B;
			m_maxObjects = maxObjects;
			m_poolSize = m_maxObjects * m_sizePerObject;
			m_numObjects = 0; 
			m_allowExtraAllocations = allowExtraAllocations;
			m_numExtraAllocations = 0; 
			m_freeList = nullptr;

			CreatePool();
		}

		~PoolAllocatorImpl()
		{
			M_ASSERT(m_numExtraAllocations == 0);
			if (m_pool)
			{
				free(m_pool);
				m_pool = nullptr;
			}

			m_freeList = nullptr;
		}

		void* Alloc()
		{
			M_ASSERT(m_pool);

			// case no more available objects
			if (m_freeList == nullptr)
			{
				M_ASSERT(m_allowExtraAllocations);
				++m_numExtraAllocations;
				++m_numObjects;
				void* object = malloc(m_sizePerObject);
				return object;
			}
			// case objects available
			else
			{
				PoolObject* object = m_freeList;
				m_freeList = m_freeList->next;
				++m_numObjects;
				return object;
			}
		}

		void Free(void* object)
		{
			M_ASSERT(object && m_pool);
			s8* obj = (s8*)object;
			s64 objDistance, poolDistance;
			objDistance = obj - m_pool;
			poolDistance = (m_pool + m_poolSize) - m_pool;

			// case object is not within the pool
			if (poolDistance < objDistance && objDistance >= 0)
			{
				M_ASSERT(m_allowExtraAllocations);
				--m_numExtraAllocations;
				free(object);
			}
			// case object is within the pool
			else
			{
				PoolObject* memObj = (PoolObject*)(object);
				memObj->next = m_freeList;
				m_freeList = memObj;
			}

			--m_numObjects;
		}

		void Clear()
		{
			M_ASSERT(m_numExtraAllocations == 0);
			free(m_pool);
			m_freeList = nullptr;

			CreatePool();
		}

	private:

		void CreatePool()
		{
			m_pool = (s8*)malloc(m_poolSize);
			M_ASSERT(m_pool && "allocating space for memory pool failed!");

			// init the free list
			for (u32 i = 0; i < m_maxObjects; ++i)
			{
				PoolObject* obj = (PoolObject*)(m_pool + i * m_sizePerObject);
				obj->next = m_freeList;
				m_freeList = obj;
			}
		}

		unsigned m_sizePerObject;
		unsigned m_maxObjects;
		unsigned m_numObjects;
		unsigned m_poolSize;
		char* m_pool;
		bool m_allowExtraAllocations;
		unsigned m_numExtraAllocations;
		PoolObject* m_freeList;
	};

	// templated pool allocator
	// it can handle extra allocations beyond the max objects, but won't clean up
	// that memory for you.
	// sizeof(T) must be >= sizeof(void*) or won't compile
	// Alloc doesn't call T's constructor,
	// Construct does
	// Free doesn't call T's destructor
	// Destruct does
	// Clear doesn't call destructors
	template<typename T>
	class PoolAllocator
	{
	public:
		PoolAllocator(unsigned maxObjects, bool allowExtraAllocations = true)
			: m_allocator(maxObjects, allowExtraAllocations) {}

		T* Alloc()
		{
			return reinterpret_cast<T *>(m_allocator.Alloc());
		}

		template<typename... Args>
		T* Construct(Args... args)
		{
			T* obj = Alloc();
			T* mem = new (obj) T(args...);
			return obj;
		}

		void Free(T* address)
		{
			m_allocator.Free(address);
		}

		void Destruct(T* address)
		{
			address->~T();
			Free(address);
		}

		void Clear()
		{
			m_allocator.Clear();
		}

	private:
		PoolAllocatorImpl<sizeof(T)> m_allocator;
	};

}
