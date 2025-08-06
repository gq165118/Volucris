#include <Game/Entity.h>
#include <Game/Component.h>
#include <Core/VectorHelp.h>
#include <Game/SceneComponent.h>
#include <Game/GameWorld.h>
#include <Game/Region.h>
#include <Render/Renderer.h>
#include <Render/Scene.h>

namespace volucris
{
	Entity::Entity()
		: GameObject()
		, m_rootComponent(nullptr)
		, m_region(nullptr)
	{

	}

	Entity::~Entity()
	{

	}

	void Entity::attach(const std::shared_ptr<Component>& component)
	{
		if (component->getEntity() == this)
		{
			return;
		}

		if (auto entity = component->getEntity())
		{
			if (entity->getWorld() != getWorld())
			{
				entity->disattachComponentFromScene(component);
			}
			entity->disattach(component);
		}

		component->setEnity(this);

		if (!m_rootComponent)
		{
			m_rootComponent = component;
		} 
		else if (auto sceneComponent = std::dynamic_pointer_cast<SceneComponent>(m_rootComponent))
		{
			sceneComponent->attach(component);
		}
	}

	void Entity::attach(SceneComponent* parent, const std::shared_ptr<Component>& component)
	{
		if (component->getEntity() == this)
		{
			return;
		}

		if (auto entity = component->getEntity())
		{
			if (entity->getWorld() != getWorld())
			{
				entity->disattachComponentFromScene(component);
			}
			entity->disattach(component);
		}
		
		component->setEnity(this);
		parent->attach(component);

		if (auto sceneComponent = std::dynamic_pointer_cast<SceneComponent>(component))
		{
			std::vector<std::shared_ptr<Component>> components;
			getComponents(sceneComponent, components);
			for (const auto& component : components)
			{
				component->setEnity(this);
			}
			attachComponentsToScene(components);
		}
	}

	void Entity::disattach(const std::shared_ptr<Component>& component)
	{
		if (component->getEntity() != this)
		{
			return;
		}

		component->setEnity(nullptr);
		
		if (const auto parent = component->getParentComponent())
		{
			parent->disattach(component);
		}

		std::vector<std::shared_ptr<Component>> components;
		if (auto sceneComp = std::dynamic_pointer_cast<SceneComponent>(component))
		{
			getComponents(sceneComp, components);
			for (const auto& comp : components)
			{
				components.push_back(comp);
				component->setEnity(nullptr);
			}	
		}
		else
		{
			components.push_back(component);
		}
		disattachComponentsFromScene(components);
		
	}

	void Entity::update()
	{
		m_rootComponent->update();
	}

	GameWorld* Entity::getWorld() const
	{
		if (m_region)
		{
			return m_region->getWorld();
		}
		return nullptr;
	}

	void Entity::setRegion(Region* region)
	{
		if (region == m_region)
		{
			return;
		}

		auto shared = getShared<Entity>();

		if (m_region)
		{
			if (region && m_region->getWorld() != region->getWorld())
			{
				disattachFromScene();
			}

			auto oldRegion = m_region;
			m_region = nullptr;
			oldRegion->removeEntity(shared);
		}

		m_region = region;
		if (m_region)
		{
			m_region->addEntity(shared);
			attachToScene();
		}
	}

	std::vector<std::shared_ptr<PrimitiveSceneProxy>> Entity::createPrimitiveProxies()
	{
		std::vector<std::shared_ptr<PrimitiveSceneProxy>> proxies;

		const auto& components = getComponents();
		for (const auto& comp : components)
		{
			if (auto proxy = comp->createProxy())
			{
				comp->setPrimitiveSceneProxy(proxy);
				proxies.push_back(proxy);
			}
		}

		return proxies;
	}

	std::vector<std::shared_ptr<PrimitiveSceneProxy>> Entity::getPrimitiveProxies()
	{
		std::vector<std::shared_ptr<PrimitiveSceneProxy>> proxies;
		const auto& components = getComponents();
		for (const auto& comp : components)
		{
			if (auto proxy = comp->getPrimitiveSceneProxy())
			{
				proxies.push_back(proxy);
			}
		}

		return proxies;
	}

	std::vector<std::shared_ptr<Component>> Entity::getComponents() const
	{
		if (!m_rootComponent)
		{
			return {};
		}

		if (auto sceneComp = std::dynamic_pointer_cast<SceneComponent>(m_rootComponent))
		{
			std::vector<std::shared_ptr<Component>> components;
			getComponents(sceneComp, components);
			return components;
		}
		return { m_rootComponent };
	}

	void Entity::disattachFromScene()
	{
		if (!m_region) return;
		auto world = m_region->getWorld();
		if (!world) return;

		if (auto scene = world->getScene())
		{
			Renderer::getInstance().push([scene, primitives = getPrimitiveProxies()]() {
				scene->removePrimitives(primitives);
				});
		}
	}

	void Entity::attachToScene()
	{
		if (!m_region) return;
		auto world = m_region->getWorld();
		if (!world) return;

		if (auto scene = world->getScene())
		{
			const auto primitives = createPrimitiveProxies();
			if (primitives.empty())
			{
				return;
			}

			Renderer::getInstance().push([scene, primitives = primitives]() {
				scene->addPrimitives(primitives);
				});
		}
	}

	void Entity::attachComponentsToScene(const std::vector<std::shared_ptr<Component>>& components)
	{
		if (!m_region) return;
		auto world = m_region->getWorld();
		if (!world) return;
		auto scene = world->getScene();
		if (!scene) return;

		std::vector<std::shared_ptr<PrimitiveSceneProxy>> primitives;
		for (const auto& component : components)
		{
			auto proxy = component->createProxy();
			component->setPrimitiveSceneProxy(proxy);
			primitives.push_back(proxy);
		}

		Renderer::getInstance().push([scene, primitives]() {
			scene->addPrimitives(primitives);
			});
	}

	void Entity::disattachComponentFromScene(const std::shared_ptr<Component>& component)
	{
		std::vector<std::shared_ptr<Component>> components = getComponents();
		disattachComponentsFromScene(components);
	}

	void Entity::disattachComponentsFromScene(const std::vector<std::shared_ptr<Component>>& components)
	{
		if (!m_region) return;
		auto world = m_region->getWorld();
		if (!world) return;
		auto scene = world->getScene();
		if (!scene) return;

		std::vector<std::shared_ptr<PrimitiveSceneProxy>> proxies;
		proxies.reserve(components.size());
		for (const auto& component : components)
		{
			if (auto proxy = component->getPrimitiveSceneProxy())
			{
				component->setPrimitiveSceneProxy(proxy);
				proxies.push_back(proxy);
			}
		}
		
		if (proxies.empty()) return;

		Renderer::getInstance().push([scene, primitives = proxies]() {
			scene->removePrimitives(primitives);
			});
	}

	void Entity::getComponents(const std::shared_ptr<SceneComponent>& root, std::vector<std::shared_ptr<Component>>& components) const
	{
		components.push_back(root);

		for (const auto& component : root->getAttachedComponents())
		{
			if (auto sceneComponent = std::dynamic_pointer_cast<SceneComponent>(component))
			{
				getComponents(sceneComponent, components);
			}
			else
			{
				components.push_back(component);
			}
		}
	}
}

BOOST_CLASS_EXPORT_IMPLEMENT(volucris::Entity)