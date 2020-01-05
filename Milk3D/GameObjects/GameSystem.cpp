#include "GameSystem.h"
#include "Common.h"

namespace Milk3D
{
	GameSystem::GameSystem() :
		System(), m_lastID(0), m_pool(), m_openSlots()
	{
		m_pool.reserve(MAX_GAMEOBJECTS);
	}

	void GameSystem::OnEvent(SystemInitEvent * e)
	{
		Subscribe(et_GetGameObjectEvent);
		Subscribe(et_CreateGameObjectEvent);
		Subscribe(et_DeleteGameObjectEvent);
		Subscribe(et_ParentGameObjectEvent);
		Subscribe(et_GameSaveEvent);
		Subscribe(et_GameLoadEvent);
	}

	void GameSystem::OnEvent(SystemExitEvent * e)
	{
		UnsubscribeAll();
	}

	void GameSystem::OnEvent(SystemUpdateEvent * e)
	{
		GameObjectUpdateEvent update;
		update.dt = e->dt;

		Milk3D::SendEvent(&update);
	}

	void GameSystem::OnEvent(SystemRenderEvent * e)
	{
		GameObjectRenderEvent render;
		Milk3D::SendEvent(&render);
	}

	void GameSystem::OnEvent(GetGameObjectEvent * e)
	{
		if (e->id == INVALID_GAMEOBJECTID ||
			e->id >= m_pool.size()) return;

		auto* obj = m_pool[e->id];
		e->setObj = obj;
	}

	void GameSystem::OnEvent(CreateGameObjectEvent * e)
	{
		GameObjectID id;
		// case no more open slots
		if (m_openSlots.empty())
		{
			id = m_pool.size();
			m_pool.push_back(e->obj);
			m_pool.back()->m_id = id;

		}
		// case open slot available
		else
		{
			id = m_openSlots.back();
			m_openSlots.pop_back();
			m_pool[id] = e->obj;
			m_pool[id]->m_id = id;
		}

		m_pool[id]->OnCreate();
		*(e->setID) = id;
		
		if (e->parentID == INVALID_GAMEOBJECTID) return;

		ParentGameObjectEvent parent;
		parent.id = id;
		parent.newParent = e->parentID;
		Milk3D::SendEvent(&parent);
	}

	void GameSystem::OnEvent(DeleteGameObjectEvent * e)
	{
		if (e->id == INVALID_GAMEOBJECTID) return;
		if (e->id >= m_pool.size() || m_pool[e->id] == nullptr) return;

		GameObject* obj = m_pool[e->id];

		// delete children
		auto& children = obj->GetChildren();
		for (auto& id : children)
		{
			DeleteGameObjectEvent d;
			d.id = id;
			Milk3D::SendEvent(&d);
		}

		// remove from parent
		ParentGameObjectEvent d;
		d.id = obj->GetID();
		d.newParent = INVALID_GAMEOBJECTID;
		Milk3D::SendEvent(&d);

		// delete
		obj->OnDelete();
		delete obj;
		m_pool[e->id] = nullptr;
	}

	void GameSystem::OnEvent(ParentGameObjectEvent * e)
	{
	}

	void GameSystem::OnEvent(GameSaveEvent * e)
	{
	}

	void GameSystem::OnEvent(GameLoadEvent * e)
	{
	}

}