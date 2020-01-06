#pragma once
#include <Core\System\System.h>
#include <Core\Memory\PoolAllocator.h>
#include "GameObject.h"

#include <vector>
#include <unordered_map>

namespace Milk3D
{
	class GameSystem : public System
	{
	public:

		GameSystem();

		void OnEvent(SystemInitEvent* e) override;
		void OnEvent(SystemExitEvent* e) override;
		void OnEvent(SystemUpdateEvent* e) override;
		void OnEvent(SystemRenderEvent* e) override;

		void OnEvent(GetGameObjectEvent* e);
		void OnEvent(CreateGameObjectEvent* e);
		void OnEvent(DeleteGameObjectEvent* e);
		void OnEvent(ParentGameObjectEvent* e);
		void OnEvent(GameSaveEvent* e);
		void OnEvent(GameLoadEvent* e);
	private:
		GameObjectID m_lastID;
		std::vector<GameObject*> m_pool;
		std::vector<GameObjectID> m_openSlots;
	};
}
