#include "Game/Component.h"
#include <Game/Entity.h>
#include <Game/SceneComponent.h>

namespace volucris
{
	Component::Component()
		: GameObject()
		, m_dirtyFlags(0)
		, m_entity(nullptr)
		, m_primitiveSceneProxy()
	{
	}

	GameWorld* Component::getWorld() const
	{
		if (m_entity)
		{
			return m_entity->getWorld();
		}
		return nullptr;
	}

	void Component::update()
	{
		if (m_dirtyFlags & DirtyFlag::RenderState)
		{
			onRenderStateChanged();
		}
		if (m_dirtyFlags & DirtyFlag::TransformState)
		{
			onTransformStateChanged();
		}
		m_dirtyFlags = 0;
		markDirty(false);
	}

	std::shared_ptr<PrimitiveSceneProxy> Component::getPrimitiveSceneProxy() const
	{
		if (m_primitiveSceneProxy.expired())
		{
			return nullptr;
		}
		return m_primitiveSceneProxy.lock();
	}

	void Component::setEnity(Entity* entity)
	{
		m_entity = entity;
	}
} // namespace volucris

BOOST_CLASS_EXPORT(volucris::Component)