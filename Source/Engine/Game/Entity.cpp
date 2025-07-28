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
		, m_components()
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

		std::vector<std::shared_ptr<Component>> components;
		if (auto sceneComp = std::dynamic_pointer_cast<SceneComponent>(component))
		{
			std::vector<std::shared_ptr<SceneComponent>> sceneComponents;
			getSceneComponents(sceneComp, sceneComponents);
			components.reserve(sceneComponents.size());
			for (const auto& component : sceneComponents)
			{
				components.push_back(component);
				static_cast<Component*>(component.get())->setEnity(this);
			}
		}
		else
		{
			component->setEnity(this);
			components = { component };
		}

		attachComponentsToScene(components);
		m_components.emplace_back(component);
	}

	void Entity::attach(SceneComponent* parent, const std::shared_ptr<SceneComponent>& sceneComponent)
	{
		if (parent->getEntity() != this || parent == sceneComponent->getParentSceneComponent())
		{
			return;
		}

		if (auto entity = sceneComponent->getEntity())
		{
			if (entity->getWorld() != getWorld())
			{
				entity->disattachComponentFromScene(sceneComponent);
			}

			if (auto oldParent = sceneComponent->getParentSceneComponent())
			{
				VectorHelp::quickRemove(oldParent->m_components, sceneComponent);
				sceneComponent->m_parentComp = nullptr;
			}
			else
			{
				VectorHelp::quickRemove(entity->m_components, static_cast<Component*>(sceneComponent.get()));
			}
		}
		
		parent->attach(sceneComponent);

		std::vector<std::shared_ptr<Component>> components;
		std::vector<std::shared_ptr<SceneComponent>> sceneComponents;
		getSceneComponents(sceneComponent, sceneComponents);
		components.reserve(sceneComponents.size());
		for (const auto& component : sceneComponents)
		{
			components.push_back(component);
			static_cast<Component*>(component.get())->setEnity(this);
		}
		attachComponentsToScene(components);
	}

	void Entity::disattach(const std::shared_ptr<Component>& component)
	{
		if (component->getEntity() != this)
		{
			return;
		}

		bool isSubChildRemoved = false;
		std::vector<std::shared_ptr<Component>> components;
		
		if (auto sceneComp = std::dynamic_pointer_cast<SceneComponent>(component))
		{
			std::vector<std::shared_ptr<SceneComponent>> sceneComponents;
			getSceneComponents(sceneComp, sceneComponents);
			components.reserve(sceneComponents.size());
			for (const auto& comp : sceneComponents)
			{
				components.push_back(comp);
				static_cast<Component*>(component.get())->setEnity(nullptr);
			}

			if (auto parent = sceneComp->getParentSceneComponent())
			{
				parent->disattach(sceneComp);
				isSubChildRemoved = true;
			}
			
		}
		else
		{
			components.push_back(component);
		}
		disattachComponentsFromScene(components);
		
		if (!isSubChildRemoved)
		{
			VectorHelp::quickRemove(m_components, component);
		}
	}

	void Entity::update()
	{
		for (const auto& comp : m_components)
		{
			comp->update();
		}
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
		proxies.reserve(m_components.size());
		for (const auto& comp : m_components)
		{
			if (auto sceneComp = std::dynamic_pointer_cast<SceneComponent>(comp))
			{
				std::vector<std::shared_ptr<SceneComponent>> components;
				getSceneComponents(sceneComp, components);
				for (const auto& component : components)
				{
					if (auto proxy = component->createProxy())
					{
						component->setPrimitiveSceneProxy(proxy);
						proxies.push_back(proxy);
					}
				}
			}
			else if (auto proxy = comp->createProxy())
			{
				proxies.push_back(proxy);
			}
		}

		return proxies;
	}

	std::vector<std::shared_ptr<PrimitiveSceneProxy>> Entity::getPrimitiveProxies()
	{
		std::vector<std::shared_ptr<PrimitiveSceneProxy>> proxies;
		proxies.reserve(m_components.size());
		for (const auto& comp : m_components)
		{
			if (auto sceneComp = std::dynamic_pointer_cast<SceneComponent>(comp))
			{
				std::vector<std::shared_ptr<SceneComponent>> components;
				getSceneComponents(sceneComp, components);
				for (const auto& component : components)
				{
					if (auto proxy = component->getPrimitiveSceneProxy())
					{
						proxies.push_back(proxy);
					}
				}
			}
			else if (auto proxy = comp->getPrimitiveSceneProxy())
			{
				proxies.push_back(proxy);
			}
		}

		return proxies;
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
		std::vector<std::shared_ptr<Component>> components;
		if (auto sceneComp = std::dynamic_pointer_cast<SceneComponent>(component))
		{
			std::vector<std::shared_ptr<SceneComponent>> sceneComponents;
			getSceneComponents(sceneComp, sceneComponents);
			components.reserve(sceneComponents.size());
			for (const auto& comp : sceneComponents)
			{
				components.push_back(comp);
			}
		}
		else
		{
			components = { component };
		}
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
		proxies.reserve(m_components.size());
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

	void Entity::getSceneComponents(const std::shared_ptr<SceneComponent>& root, std::vector<std::shared_ptr<SceneComponent>>& components)
	{
		components.push_back(root);
		for (const auto& sceneComponent : root->getAttachedComponents())
		{
			getSceneComponents(sceneComponent, components);
		}
	}
}