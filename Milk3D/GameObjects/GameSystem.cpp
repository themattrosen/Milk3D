#include "GameSystem.h"
#include "Common.h"
#include "Reflection\GameObjectFactory.h"

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
		for (auto* g : m_pool)
		{
			if (g)
			{
				delete g;
				g = nullptr;
			}
		}
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
		m_pool[id]->OnActivate();
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
		obj->OnDeactivate();
		obj->OnDelete();
		delete obj;
		m_pool[e->id] = nullptr;
	}

	void GameSystem::OnEvent(ParentGameObjectEvent * e)
	{
		GetGameObjectEvent get;
		get.id = e->id;
		Milk3D::SendEvent(&get);

		GetGameObjectEvent parent;
		parent.id = get.setObj->GetParent();
		Milk3D::SendEvent(&parent);

		if (e->newParent == INVALID_GAMEOBJECTID)
		{
			parent.setObj->RemoveChild(e->id);
		}
		else
		{
			parent.setObj->RemoveChild(e->id);
			GetGameObjectEvent newParent;
			newParent.id = e->newParent;
			Milk3D::SendEvent(&newParent);

			newParent.setObj->AddChild(e->id);
		}
	}

	void GameSystem::OnEvent(GameSaveEvent * e)
	{
		int size = (int)m_pool.size();
		Serializer s(e->path.c_str(), sm_Save);
		if (!s.IsValid()) return;

		s % m_lastID;
		int numObjs = 0;
		for (int i = 0; i < size; ++i)
			if (m_pool[i]) ++numObjs;
		s % numObjs;

		for (int i = 0; i < size; ++i)
		{
			GameObject* obj = m_pool[i];
			if (obj)
			{
				s % i;
				unsigned typeID =
					GameObjectFactory::GetTypeID(obj);
				s % typeID;
				obj->Serialize(s);
			}
		}

		s % Serializer::Endline();

		size = (int)m_openSlots.size();
		s % size;
		for (int i = 0; i < size; ++i)
		{
			s % m_openSlots[i];
		}
		s % Serializer::Endline();

	}

	void GameSystem::OnEvent(GameLoadEvent * e)
	{
		Serializer s(e->path.c_str(), sm_Load);
		if (!s.IsValid()) return;

		s % m_lastID;
		m_pool.resize(m_lastID, nullptr);
		int numObjs;
		s % numObjs;

		for (int i = 0; i < numObjs; ++i)
		{
			int ind;
			s % ind;
			unsigned typeID;
			s % typeID;

			m_pool[ind] = GameObjectFactory::Create(typeID);
			m_pool[ind]->m_id = ind;
			m_pool[ind]->Serialize(s);
		}

		int numSlots;
		s % numSlots;
		m_openSlots.resize(numSlots);
		for (int i = 0; i < numSlots; ++i)
		{
			s % m_openSlots[i];
		}
		s % Serializer::Endline();
	}

}