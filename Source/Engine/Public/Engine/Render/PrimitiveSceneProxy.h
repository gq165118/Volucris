#ifndef __volucris_primitive_scene_proxy_h__
#define __volucris_primitive_scene_proxy_h__

#include <Engine/Render/StaticMeshProxy.h>

namespace volucris
{
	class MaterialInstanceProxy;

	struct PrimitiveDrawInfo
	{
		RHIVertexArray* vao;
		RHIElementBuffer* ebo;
		PrimitiveSegment mesh;
		MaterialInstanceProxy* material;
	};

	class PrimitiveSceneProxy
	{
	public:
		PrimitiveSceneProxy();

		~PrimitiveSceneProxy();

		void setMesh(const std::shared_ptr<StaticMeshProxy>& mesh)
		{
			m_meshProxy = mesh;
		}

		void setMaterials(const std::vector<std::shared_ptr<MaterialInstanceProxy>>& materials)
		{
			m_materials = materials;
		}

		std::vector<PrimitiveDrawInfo> getPrimitiveDrawInfos() const;

	private:
		std::shared_ptr<StaticMeshProxy> m_meshProxy;
		std::vector<std::shared_ptr<MaterialInstanceProxy>> m_materials;
	};
}

#endif // !__volucris_primitive_scene_proxy_h__
