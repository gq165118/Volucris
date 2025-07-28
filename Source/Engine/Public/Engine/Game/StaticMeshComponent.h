#ifndef __volucris_static_mesh_component_h__
#define __volucris_static_mesh_component_h__

#include <Engine/Game/SceneComponent.h>
#include <Engine/Game/SoftObject.h>
#include <Engine/Game/StaticMesh.h>

namespace volucris
{
	class StaticMeshComponent : public SceneComponent
	{
	public:
		StaticMeshComponent();

		StaticMeshComponent(const SoftObject<StaticMesh>& mesh);

		void setStaticMesh(const SoftObject<StaticMesh>& mesh);

		void setMaterial(uint32 index, const SoftObject<Material>& material);

		std::shared_ptr<PrimitiveSceneProxy> createProxy() override;

	protected:
		void onRenderStateChanged() override;

	private:
		SoftObject<StaticMesh> m_mesh;
		std::vector<SoftObject<Material>> m_materials;
		std::weak_ptr<PrimitiveSceneProxy> m_proxy;
	};
}

#endif // !__volucris_static_mesh_component_h__
