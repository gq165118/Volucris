/**
 * @class StaticMeshProxy
 * @brief 提供 StaticMesh 的渲染代理对象，封装与 RHI 资源（VAO/EBO）及绘制段相关的数据。
 *
 * `StaticMeshProxy` 是 StaticMesh 数据与底层渲染管线（OpenGL/Vulkan/DX等）的桥梁，
 * 负责根据上传的 `PrimitiveInfo` 创建 GPU 顶点数组对象（VAO）和元素缓冲对象（EBO），
 * 并记录各个绘制段（PrimitiveSegment）以供绘制调用使用。
 *
 * 主要职责：
 * - 从 CPU 端的 `PrimitiveInfo` 数据构建 VAO、EBO 等 GPU 资源
 * - 持有多个绘制段（`PrimitiveSegment`）以支持多材质或子网格渲染
 * 
 * 理解使用PrimitiveSegment： 
 * 在渲染一个完整的mesh时，并不总是一次性渲染完成，mesh可能包含多个字子mesh
 * 每一个子mesh可以使用不同的绘制模式，也可能绑定不同的材质，
 * 顶点数据可能都存在同一个vao/ebo当中，但是需要不同的偏移和count来绘制，PrimitiveSegment就是来描述每段数据的
 *
 * @note 本类不包含逻辑层的数据，如网格名称或物理信息，仅用于渲染。
 *
 * @see PrimitiveInfo, PrimitiveSegment, RHIVertexArray, RHIElementBuffer, StaticMeshComponent
 * @author
 * @date 2025-08-07
 */

#ifndef __volucris_static_mesh_proxy_h__
#define __volucris_static_mesh_proxy_h__

#include <Engine/Core/Object.h>
#include <Engine/Core/PrimitiveInfo.h>
#include <Engine/RHI/RHIState.h>

namespace volucris
{
	class RHIVertexArray;
	class RHIElementBuffer;

	class StaticMeshProxy : public Object
	{
	public:
		StaticMeshProxy();

		bool init(const PrimitiveInfo& info);

		RHIVertexArray* getVertexArray() const { return m_vao.get(); }

		RHIElementBuffer* getElementBuffer() const { return m_ebo.get(); }

		const std::vector<PrimitiveSegment>& getSegments() const { return m_segments; }

	private:
		std::shared_ptr<RHIVertexArray> m_vao;
		std::shared_ptr<RHIElementBuffer> m_ebo;
		std::vector<PrimitiveSegment> m_segments;
	};
}

#endif // !__volucris_static_mesh_proxy_h__
