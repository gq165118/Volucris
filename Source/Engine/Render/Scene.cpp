#include <Render/Scene.h>
#include <Game/GameWorld.h>

namespace volucris
{
    Scene::Scene(GameWorld* world)
        : m_world(world)
        , m_primitives()
    {
        size_t count = 0;

        for (const auto* region : world->getRegions())
        {
            for (const auto& entity : region->getEntities())
            {
                count += entity->getComponents().size();
            }
        }
        m_primitives.reserve(count);

        for (const auto* region : world->getRegions())
        {
            for (const auto& entity : region->getEntities())
            {
                const auto& primitives = entity->createPrimitiveProxies();
                m_primitives.insert(m_primitives.end(), primitives.begin(), primitives.end());
            }
        }
    }

    void Scene::addPrimitives(const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& primitives)
    {
        m_primitives.reserve(m_primitives.size() + primitives.size());
        m_primitives.insert(m_primitives.end(), primitives.begin(), primitives.end());
    }

    void Scene::removePrimitives(const std::vector<std::shared_ptr<PrimitiveSceneProxy>>& primitives)
    {

    }
}