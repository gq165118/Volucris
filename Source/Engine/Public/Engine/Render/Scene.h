#ifndef __volucris_scene_h__
#define __volucris_scene_h__

#include <Engine/Core/Object.h>
#include <vector>
#include <Engine/Render/PrimitiveSceneProxy.h>

namespace volucris
{
	class GameWorld;

	class Scene
	{
	public:
		Scene(GameWorld* world);

		void addPrimitives(const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& primitives);

		void removePrimitives(const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& primitives);

		const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& getPrimitives() const { return m_primitives; }

	private:
		GameWorld* m_world;
		std::vector<std::shared_ptr<PrimitiveSceneProxy>> m_primitives;
	};
}

#endif // !__volucris_scene_h__
