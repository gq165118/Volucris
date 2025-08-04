#ifndef __volucris_component_h__
#define __volucris_component_h__

#include <Engine/Game/GameObject.h>
#include <Engine/Core/TypesHelp.h>

namespace volucris
{
	class Entity;
	class GameWorld;
	class PrimitiveSceneProxy;

	class Component : public GameObject
	{
		RTTR_ENABLE(GameObject)

	public:
		Component();

		void markRenderStateDirty()
		{
			m_dirtyFlags |= DirtyFlag::RenderState;
			markDirty(true);
		}

		void markTransformDirty()
		{
			m_dirtyFlags |= DirtyFlag::TransformState;
			markDirty(true);
		}

		Entity* getEntity() const { return m_entity; }

		GameWorld* getWorld() const;

		virtual void update();

		virtual std::shared_ptr<PrimitiveSceneProxy> createProxy() { return nullptr; }

		void setPrimitiveSceneProxy(const std::shared_ptr<PrimitiveSceneProxy>& proxy) { m_primitiveSceneProxy = proxy; }

		std::shared_ptr<PrimitiveSceneProxy> getPrimitiveSceneProxy() const;

	protected:
		void setEnity(Entity* entity);

		virtual void onRenderStateChanged() {}

		virtual void onTransformStateChanged() {}

	private:
		friend class Entity;
		enum DirtyFlag
		{
			RenderState = 0x01,
			TransformState = 0x02,
		};
		uint8 m_dirtyFlags;

		Entity* m_entity;
		std::weak_ptr<PrimitiveSceneProxy> m_primitiveSceneProxy;
	};
}

#endif // !__volucris_icomponent_h__
