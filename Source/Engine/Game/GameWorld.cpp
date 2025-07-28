#include "Game/GameWorld.h"
#include <Render/Scene.h>
#include <Game/MaterialInstance.h>

namespace volucris
{
	GameWorld::GameWorld()
		: World()
		, m_persistentRegion(std::make_unique<Region>())
		, m_regions()
		, m_scene()
	{
	}

	std::shared_ptr<Scene> GameWorld::createScene()
	{
		std::shared_ptr<Scene> scene = nullptr;
		if (!m_scene.expired())
		{
			scene = m_scene.lock();
		}

		if (scene == nullptr)
		{
			scene = std::make_shared<Scene>(this);
			m_scene = scene;
		}

		return scene;
	}

	std::shared_ptr<Scene> GameWorld::getScene()
	{
		if (!m_scene.expired())
		{
			return m_scene.lock();
		}
		return nullptr;
	}

	void GameWorld::update()
	{
		m_persistentRegion->update();
		for (const auto& region : m_regions)
		{
			region->update();
		}
	}

	Region* GameWorld::addRegion(std::unique_ptr<Region> region)
	{
		auto proxy = region.get();
		m_regions.push_back(std::move(region));
		return proxy;
	}

	std::vector<Region*> GameWorld::getRegions() const
	{
		std::vector<Region*> regions;
		regions.reserve(m_regions.size() + 1);
		regions.push_back(m_persistentRegion.get());
		for (const auto& region : m_regions)
		{
			regions.push_back(region.get());
		}
		return regions;
	}
}
