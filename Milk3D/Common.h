#pragma once

// OS defines
#ifdef _WIN32
#	define M_WINDOWS
#else
#	error Milk3D only supports Windows!
#endif

// assertions
#include <cassert>
#ifdef _DEBUG
#	define M_ASSERT(cond) \
		if((cond)) {} else { __debugbreak(); }
#else
#	define M_ASSERT(cond) 
#endif
#define M_STATIC_ASSERT(cond, ...) static_assert((cond), __VA_ARGS__) 

// alignment
#define M_ALIGN(num_bytes) __declspec(align( (num_bytes) ))

// inline
#define M_INLINE __inline
#define M_FORCE_INLINE __forceinline

namespace Milk3D
{
	using GameObjectID = unsigned;
	const constexpr GameObjectID INVALID_GAMEOBJECTID = (unsigned)(-1);

	const constexpr size_t MAX_GAMEOBJECTS = 200;

}
