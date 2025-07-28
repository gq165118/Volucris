#ifndef __volucris_static_mesh_h__
#define __volucris_static_mesh_h__

#include <Engine/Game/GameObject.h>
#include <Engine/Core/PrimitiveInfo.h>
#include <Engine/Game/MeshResourceData.h>
#include <Engine/Game/MeshElements.h>
#include <Engine/Game/SoftObject.h>
#include <Engine/Game/Material.h>
#include <boost/serialization/unique_ptr.hpp>

namespace volucris
{
	class StaticMeshProxy;

	class StaticMesh : public GameObject
	{
	public:
		StaticMesh();

		void setMeshData(MeshData data);

		void setMaterial(uint32 index, const SoftObject<Material>& material)
		{
			if (index < m_materials.size())
			{
				m_materials[index] = material;
			}
		}

		const std::vector<SoftObject<Material>>& getMaterials() const { return m_materials; }

		template <class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar.template register_type<volucris::SmallMeshElements>();
			ar.template register_type<volucris::MediumMeshElements>();
			ar.template register_type<volucris::LargeMeshElements>();
			ar& boost::serialization::base_object<GameObject>(*this);
			ar& m_materials;
			ar& m_submeshes;
			ar& m_data;
		}

		std::shared_ptr<StaticMeshProxy> getProxy();

		std::string getClassName() const override { return "StaticMesh"; }

		void addSubMesh(std::unique_ptr<MeshElements> elements)
		{
			m_materials.push_back(SoftObject<Material>());
			m_submeshes.emplace_back(std::move(elements));
		}

	private:
		void buildSubMeshData(PrimitiveInfo& info);

	private:
		MeshData m_data;
		std::vector<SoftObject<Material>> m_materials;
		std::vector<std::unique_ptr<MeshElements>> m_submeshes;
		std::weak_ptr<StaticMeshProxy> m_proxy;
	};
}

BOOST_CLASS_EXPORT_KEY(volucris::StaticMesh)

#endif // !__volucris_static_mesh_h__
