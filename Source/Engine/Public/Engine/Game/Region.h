#ifndef __volucris_region_h__
#define __volucris_region_h__

#include <Engine/Game/GameObject.h>
#include <Engine/Game/Entity.h>

namespace volucris
{
	class GameWorld;

	class Region : public GameObject
	{
	public:
		Region();

		void addEntity(const std::shared_ptr<Entity>& entity);

		void removeEntity(const std::shared_ptr<Entity>& entity);

		void update();

		GameWorld* getWorld() const { return m_world; }

		const std::vector<std::shared_ptr<Entity>>& getEntities() const { return m_entites; }

	protected:
		friend class GameWorld;
		void setWorld(GameWorld* world);

	private:
		GameWorld* m_world;
		std::vector<std::shared_ptr<Entity>> m_entites;
	};
}

#endif // !__volucris_region_h__
