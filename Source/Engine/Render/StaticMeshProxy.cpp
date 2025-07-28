#include "Render/StaticMeshProxy.h"
#include <RHI/RHIVertexArray.h>
#include <RHI/RHIVertexBuffer.h>
#include <Render/Renderer.h>
#include <glm/glm.hpp>

namespace volucris
{
	static size_t getDataSizeByType(DataType type)
	{
		switch (type)
		{
		case volucris::DataType::Float:
			return sizeof(float);
		case volucris::DataType::UInt:
			return sizeof(uint32);
		default:
			break;
		}
		return 0;
	}

	StaticMeshProxy::StaticMeshProxy()
		: Object()
		, m_vao(nullptr)
		, m_ebo(nullptr)
		, m_segments()
	{
	}

	bool StaticMeshProxy::init(const PrimitiveInfo& info)
	{
		auto vbo = std::make_shared<RHIVertexBuffer>(RHICmdList);
		vbo->createGpuResource();
		vbo->init(info.data);
		std::vector<RHIVertexBuffer::Description> descriptions;
		for (const auto& block : info.blocks)
		{
			RHIVertexBuffer::Description desc;
			auto type = block.type;
			int stride = 0;
			switch (type)
			{
			case volucris::Vertex:
				desc.location = 0;
				stride = sizeof(glm::vec3);
				break;
			case volucris::Normal:
				desc.location = 1;
				break;
			case volucris::Color:
				desc.location = 2;
				break;
			case volucris::SecondColor:
				desc.location = 3;
				break;
			case volucris::TextureCoordinate:
				desc.location = 4;
				break;
			default:
				break;
			}
			desc.normalized = false;
			desc.offset = block.offset;
			desc.type = block.dataType;
			desc.size = block.count;
			desc.stride = block.count * getDataSizeByType(block.dataType);
			descriptions.emplace_back(desc);
		}
		vbo->setDescriptions(descriptions);
		m_vao = std::make_shared<RHIVertexArray>(RHICmdList);
		m_vao->init(vbo);
		m_ebo = std::make_shared<RHIElementBuffer>(RHICmdList);
		m_ebo->createGpuResource();
		m_ebo->init(info.segmentData);

		m_segments = info.segments;

		return true;
	}
}
