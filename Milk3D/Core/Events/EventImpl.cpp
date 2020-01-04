//Matthew Rosen

#include <string>

namespace Milk3D
{
#undef REGISTER_EVENT
#define REGISTER_EVENT(type, ...) #type ,

	const char* g_eventNames[] =
	{
	#	include <Core\Events\EventRegistry.h>
		"EventMax"
	};

#undef REGISTER_EVENT
#define REGISTER_EVENT(type, ...) 
} // namespace Milk3D
