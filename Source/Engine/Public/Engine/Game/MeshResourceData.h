#ifndef __volucris_mesh_resource_data_h__
#define __volucris_mesh_resource_data_h__

#include <glm/glm.hpp>
#include <vector>
#include <Engine/Core/TypesHelp.h>
#include <Engine/Core/PrimitiveInfo.h>
#include <Engine/Core/GlmHelp.h>

namespace volucris
{

	class MeshData
	{
		std::vector<glm::vec3> m_vertices;
		std::vector<glm::vec3> m_normals;
		std::vector<glm::vec3> m_uv0;
		std::vector<glm::vec3> m_uv1;
		std::vector<glm::vec4> m_color;
		std::vector<glm::vec4> m_secondColor;

	public:
		void setVertices(std::vector<glm::vec3> vertices)
		{
			m_vertices = std::move(vertices);
		}

		void setNormals(std::vector<glm::vec3> normals)
		{
			m_normals = std::move(normals);
		}

		void setColor(std::vector<glm::vec4> colors)
		{
			m_color = std::move(colors);
		}

		void setUV0(std::vector<glm::vec3> uv0)
		{
			m_uv0 = std::move(uv0);
		}

		void setUV1(std::vector<glm::vec3> uv1)
		{
			m_uv1 = std::move(uv1);
		}

		PrimitiveInfo build();

		template <class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& m_vertices;
			ar& m_normals;
			ar& m_uv0;
			ar& m_uv1;
			ar& m_color;
			ar& m_secondColor;
		}
	};
}

#endif // !__volucris_mesh_resource_data_h__
