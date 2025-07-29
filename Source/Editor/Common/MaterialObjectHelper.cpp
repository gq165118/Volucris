#include "MaterialObjectHelper.h"
#include <MaterialEditor/MaterialTemplate.h>
#include <EditorCore/Editor.h>

namespace volucris
{
	std::vector<std::string> MaterialObjectHelper::getDependences() const
	{
		if (!m_material)
		{
			return {};
		}
		std::vector<std::string> dependences;
		for (const auto& param : m_material->getParameters())
		{
			if (param.type == MaterialParamterType::Texture2D)
			{
				dependences.push_back(std::get<SoftObject<Texture2D>>(param.value).getPath());
			}
		}
		return dependences;
	}

	bool MaterialObjectHelper::updateDependences(const std::map<std::string, std::string>& dependence)
	{
		if (!m_material)
		{
			return false;
		}

		auto material = std::dynamic_pointer_cast<MaterialTemplate>(m_material);
		if (!material)
		{
			return false;
		}

		return false;
	}

	std::vector<std::string> MaterialInstanceObjectHelper::getDependences() const
	{
		if (!m_materialInstance)
		{
			return {};
		}

		std::vector<std::string> dependences;
		if (const auto& material = m_materialInstance->getMaterial())
		{
			dependences.push_back(material.getPath());
		}

		for (const auto& param : m_materialInstance->getInstanceParameters())
		{
			if (param.type == MaterialParamterType::Texture2D)
			{
				dependences.push_back(std::get<SoftObject<Texture2D>>(param.value).getPath());
			}
		}
		return dependences;
	}

	bool MaterialInstanceObjectHelper::updateDependences(const std::map<std::string, std::string>& dependence)
	{
		if (!m_materialInstance)
		{
			return false;
		}
		auto material = m_materialInstance->getMaterial();
		bool dirty = false;
		{
			auto it = dependence.find(material.getPath());
			if (it != dependence.end())
			{
				m_materialInstance->setMaterial(SoftObject<Material>(it->second));
			}
			dirty = true;
		}

		std::vector<std::pair<std::string, std::string>> updateTextures;
		for (auto& param : m_materialInstance->getInstanceParameters())
		{
			if (param.type == MaterialParamterType::Texture2D)
			{
				const auto& texture = std::get<SoftObject<Texture2D>>(param.value);
				auto it = dependence.find(texture.getPath());
				if (it != dependence.end())
				{
					updateTextures.push_back({ param.name, it->second });
				}
			}
		}

		dirty = dirty || !updateTextures.empty();

		for (const auto& [name, path] : updateTextures)
		{
			if (!m_materialInstance->setTexture2DParameter(name, SoftObject<Texture2D>(path)))
			{
				V_LOG_WARN(Editor, "Failed to update texture parameter: {}", name);
			}
		}

		if (dirty)
		{
			m_materialInstance->markDirty(true);
		}
		return dirty;
	}
}
