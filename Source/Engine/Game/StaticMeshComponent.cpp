#include "Game/StaticMeshComponent.h"
#include <Core/Volucris.h>
#include <Render/PrimitiveSceneProxy.h>

namespace volucris
{
	StaticMeshComponent::StaticMeshComponent()
		: SceneComponent()
		, m_mesh()
		, m_proxy()
	{
	}

	StaticMeshComponent::StaticMeshComponent(const SoftObject<StaticMesh>& mesh)
		: StaticMeshComponent()
	{
		setStaticMesh(mesh);
	}

	void StaticMeshComponent::setStaticMesh(const SoftObject<StaticMesh>& mesh)
	{
		m_mesh = mesh;
		if (!m_mesh.object() && !m_mesh.tryLoad())
		{
			V_LOG_WARN(Engine, "StaticMeshComponent accept invalid mesh");
		}
		else
		{
			m_materials = m_mesh->getMaterials();
			for (auto& material : m_materials)
			{
				material.tryLoad();
			}
		}
		markRenderStateDirty();
	}

	void StaticMeshComponent::setMaterial(uint32 index, const SoftObject<Material>& material)
	{
		if (index < m_materials.size())
		{
			m_materials[index] = material;
			m_materials[index].tryLoad();
			markRenderStateDirty();
		}
	}

	std::shared_ptr<PrimitiveSceneProxy> StaticMeshComponent::createProxy()
	{
		if (!m_mesh)
		{
			return nullptr;
		}
		auto proxy = std::make_shared<PrimitiveSceneProxy>();
		std::vector<std::shared_ptr<MaterialInstanceProxy>> materials;
		materials.reserve(m_materials.size());
		for (const auto& material : m_materials)
		{
			if (material)
			{
				materials.push_back(material->getMaterialProxy());
			}
			else
			{
				materials.push_back(nullptr);
			}
		}
		proxy->setMesh(m_mesh->getProxy());
		proxy->setMaterials(std::move(materials));
		m_proxy = proxy;
		return proxy;
	}

	void StaticMeshComponent::onRenderStateChanged()
	{
		std::shared_ptr<PrimitiveSceneProxy> proxy = nullptr;
		if (!m_proxy.expired())
		{
			proxy = m_proxy.lock();
		}

		auto mesh = m_mesh.object();
		if (m_mesh.object() && !proxy)
		{

		}
	}

}
