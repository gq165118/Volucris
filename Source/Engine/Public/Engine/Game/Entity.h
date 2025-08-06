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

		void setRootComponent(const std::shared_ptr<Component>& component)
		{
			m_rootComponent = component;
		}

		void attach(const std::shared_ptr<Component>& component);

		void attach(SceneComponent* parent, const std::shared_ptr<Component>& component);

		void disattach(const std::shared_ptr<Component>& component);

		void update();

		GameWorld* getWorld() const;

		void setRegion(Region* region);

		Region* getRegion() const { return m_region; }

		template <class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::base_object<GameObject>(*this);
			ar& m_rootComponent;
		}

		const std::shared_ptr<Component>& getRootComponent() const { return m_rootComponent; }

		std::vector<std::shared_ptr<Component>> getComponents() const;

	private:
		void disattachFromScene();

		void attachToScene();

		void attachComponentsToScene(const std::vector<std::shared_ptr<Component>>& components);

		void disattachComponentFromScene(const std::shared_ptr<Component>& component);

		void disattachComponentsFromScene(const std::vector<std::shared_ptr<Component>>& components);

		void getComponents(const std::shared_ptr<SceneComponent>& root, std::vector<std::shared_ptr<Component>>& components) const;

		friend class Scene;
		std::vector<std::shared_ptr<PrimitiveSceneProxy>> createPrimitiveProxies();

		std::vector<std::shared_ptr<PrimitiveSceneProxy>> getPrimitiveProxies();

	private:
		std::shared_ptr<Component> m_rootComponent;
		Region* m_region;
	};
}

BOOST_CLASS_EXPORT_KEY(volucris::Entity)

#endif // !__volucris_entity_h__
