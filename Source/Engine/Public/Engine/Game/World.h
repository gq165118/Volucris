#ifndef __volucris_world_h__
#define __volucris_world_h__

#include <vector>
#include <memory>
#include <Engine/Game/GameObject.h>

namespace volucris
{
	class World : public GameObject
	{
	public:
		World();

		~World() override;

		void addObject(const std::shared_ptr<GameObject>& object)
		{
			m_objects.push_back(object);
		}


	private:
		std::vector<std::shared_ptr<GameObject>> m_objects;
	};
}

#endif // !__volucris_world_h__
