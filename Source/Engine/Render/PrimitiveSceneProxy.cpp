#include "Render/PrimitiveSceneProxy.h"

namespace volucris
{
	PrimitiveSceneProxy::PrimitiveSceneProxy()
	{
	}

	PrimitiveSceneProxy::~PrimitiveSceneProxy()
	{
	}

	std::vector<PrimitiveDrawInfo> PrimitiveSceneProxy::getPrimitiveDrawInfos() const
	{
		const auto segments = m_meshProxy->getSegments();
		std::vector<PrimitiveDrawInfo> infos;
		infos.reserve(segments.size());
		auto vao = m_meshProxy->getVertexArray();
		auto ebo = m_meshProxy->getElementBuffer();
		for (auto idx = 0; idx < segments.size(); ++idx)
		{
			if (!m_materials[idx]) continue;
			PrimitiveDrawInfo info;
			info.vao = vao;
			info.ebo = ebo;
			info.mesh = segments[idx];
			info.material = m_materials[idx].get();
			infos.push_back(info);
		}
		return infos;
	}
}
