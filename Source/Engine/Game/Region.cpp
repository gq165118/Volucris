#include "Game/Region.h"
#include <Core/VectorHelp.h>

namespace volucris
{
	Region::Region()
		: GameObject()
		, m_world(nullptr)
		, m_entites()
	{

	}

	void Region::addEntity(const std::shared_ptr<Entity>& entity)
	{
		if (entity->getRegion() != nullptr)
		{
			return;
		}
		entity->setRegion(this);
		m_entites.push_back(entity);
	}

	void Region::setWorld(GameWorld* world)
	{
		m_world = world;
	}

	void Region::removeEntity(const std::shared_ptr<Entity>& entity)
	{
		if (entity->getRegion() != this)
		{
			return;
		}
		VectorHelp::quickRemove(m_entites, entity);
		entity->setRegion(nullptr);
	}

	void Region::update()
	{
		for (const auto& entity : m_entites)
		{
			entity->update();
		}
	}
}
