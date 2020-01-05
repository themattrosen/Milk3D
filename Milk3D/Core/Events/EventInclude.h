//Matthew Rosen
#pragma once
#include <Core\Util.h>
#include "EventStructTypes.h"

#define REGISTER_EVENT(type, ...) et_##type##Event ,

namespace Milk3D
{
	enum EventType
	{
#		include "EventRegistry.h"

		et_EventCount
	};

	struct Event
	{
		EventType type;
		explicit Event(EventType _type) : type(_type) {}
		virtual const char* ToString() const = 0;
	};

#define SEPARATE_ARGS(arg) arg ;

#undef REGISTER_EVENT
#define REGISTER_EVENT(type, ...) \
struct type##Event : public Event	\
{	\
	type##Event() : Event(EventType::et_##type##Event) {}	\
	type##Event(const type##Event &) = default;	\
	~type##Event() = default;	\
	virtual const char* ToString() const override { return #type ; }	\
	APPLY_ON_EACH(SEPARATE_ARGS, __VA_ARGS__)	\
};

#pragma warning( push )  
#pragma warning( disable : 4003 ) 

#include "EventRegistry.h"

#pragma warning( pop )
#undef REGISTER_EVENT
#define REGISTER_EVENT(type, ...) 

} // namespace Milk3D
