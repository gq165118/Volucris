#ifndef __volucris_game_world_h__
#define __volucris_game_world_h__

#include <Engine/Game/World.h>
#include <Engine/Game/Region.h>
#include <Engine/Game/SoftObject.h>

namespace volucris
{
	class Scene;

	class GameWorld : public World
	{
	public:
		GameWorld();

		Region* getPersistentRegion() const { return m_persistentRegion.get(); }

		std::shared_ptr<Scene> getScene();

		std::shared_ptr<Scene> createScene();

		void update();

		Region* addRegion(std::unique_ptr<Region> region);

		std::vector<Region*> getRegions() const;

	private:
		std::unique_ptr<Region> m_persistentRegion;
		std::vector<std::unique_ptr<Region>> m_regions;
		std::weak_ptr<Scene> m_scene;
	};
}

#endif // !__volucris_game_world_h__
