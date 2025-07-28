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
