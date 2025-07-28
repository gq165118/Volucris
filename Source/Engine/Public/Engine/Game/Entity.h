#ifndef __volucris_entity_h__
#define __volucris_entity_h__

#include <Engine/Game/GameObject.h>

namespace volucris
{
	class Region;
	class Component;
	class GameWorld;
	class SceneComponent;
	class PrimitiveSceneProxy;

	class Entity : public GameObject
	{
	public:
		Entity();

		~Entity() override;

		void attach(const std::shared_ptr<Component>& component);

		void attach(SceneComponent* parent, const std::shared_ptr<SceneComponent>& component);

		void disattach(const std::shared_ptr<Component>& component);

		void update();

		const std::vector<std::shared_ptr<Component>>& getComponents() const { return m_components; }

		GameWorld* getWorld() const;

		void setRegion(Region* region);

		Region* getRegion() const { return m_region; }

	private:
		void disattachFromScene();

		void attachToScene();

		void attachComponentsToScene(const std::vector<std::shared_ptr<Component>>& components);

		void disattachComponentFromScene(const std::shared_ptr<Component>& component);

		void disattachComponentsFromScene(const std::vector<std::shared_ptr<Component>>& components);

		void getSceneComponents(const std::shared_ptr<SceneComponent>& root, std::vector<std::shared_ptr<SceneComponent>>& components);

		friend class Scene;
		std::vector<std::shared_ptr<PrimitiveSceneProxy>> createPrimitiveProxies();

		std::vector<std::shared_ptr<PrimitiveSceneProxy>> getPrimitiveProxies();

	private:
		std::vector<std::shared_ptr<Component>> m_components;
		Region* m_region;
	};
}

#endif // !__volucris_entity_h__
