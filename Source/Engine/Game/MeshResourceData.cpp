#include "Game/MeshResourceData.h"

namespace volucris
{
	PrimitiveInfo MeshData::build()
	{
		if (m_vertices.empty())
		{
			return {};
		}
		PrimitiveInfo info;
		size_t size = 0;
		size += m_vertices.size() * sizeof(glm::vec3);
		size += m_normals.size() * sizeof(glm::vec3);
		size += m_uv0.size() * sizeof(glm::vec3);
		size += m_uv1.size() * sizeof(glm::vec3);
		size += m_color.size() * sizeof(glm::vec4);
		size += m_secondColor.size() * sizeof(glm::vec4);

		info.data.resize(size);
		size_t offset = 0;

		auto addBlock = [&](PrimitiveType type, const std::vector<glm::vec3>& data)
			{
				if (data.empty())
				{
					return;
				}
				PrimitiveBlock block;
				block.type = type;
				block.dataType = DataType::Float;
				block.offset = offset;
				block.count = 3;
				const auto size = data.size() * sizeof(glm::vec3);
				memcpy(info.data.data() + offset, data.data(), size);
				offset += size;
				info.blocks.push_back(block);
			};

		auto addVec4Block = [&](PrimitiveType type, const std::vector<glm::vec4>& data)
			{
				if (data.empty())
				{
					return;
				}
				PrimitiveBlock block;
				block.type = type;
				block.dataType = DataType::Float;
				block.offset = offset;
				block.count = 4;
				const auto size = data.size() * sizeof(glm::vec4);
				memcpy(info.data.data() + offset, data.data(), size);
				offset += size;
				info.blocks.push_back(block);
			};

		addBlock(PrimitiveType::Vertex, m_vertices);
		addBlock(PrimitiveType::Normal, m_normals);
		addBlock(PrimitiveType::TextureCoordinate, m_uv0);
		addVec4Block(PrimitiveType::Color, m_color);
		addVec4Block(PrimitiveType::SecondColor, m_secondColor);
		return info;
	}
}
