/**
 * @class PrimitiveSceneProxy
 * @brief 封装图元的网格和材质信息，管理渲染所需的图元数据。
 *
 * `PrimitiveSceneProxy` 类用于表示场景中的一个图元，并封装了与该图元相关的网格数据（`StaticMeshProxy`）
 * 和材质数据（`MaterialInstanceProxy`）。它提供了设置网格和材质的方法，并能够返回渲染该图元所需的绘制信息。
 *
 * 常见用法流程包括：
 * - setMesh() 设置图元的网格数据
 * - setMaterials() 设置图元的材质数据
 * - getPrimitiveDrawInfos() 获取图元的绘制信息
 *
 * 该类允许通过设置不同的网格和材质实例，方便地管理场景中的多个图元，并提供渲染相关的必要数据。
 *
 * 主要功能：
 * - 管理图元的网格和材质信息。
 * - 提供获取图元绘制所需数据的方法。
 * - 支持多材质的图元渲染。
 */

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
