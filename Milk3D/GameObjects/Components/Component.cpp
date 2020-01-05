#include "Component.h"

namespace Milk3D
{
	unsigned ComponentIDBase::s_componentIDBase = 0;

	unsigned ComponentIDBase::GetIDBase()
	{
		return s_componentIDBase;
	}
	Component::Component(GameObject * parent) :
		m_parent(parent)
	{
	}

	Component::~Component()
	{
	}
}