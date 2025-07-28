#include <Game/MaterialInstance.h>
#include <Render/Renderer.h>
#include <Render/MaterialProxy.h>
#include <Render/MaterialInstanceProxy.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <Application/Application.h>

namespace volucris
{
	MaterialInstance::MaterialInstance()
		: Material()
		, m_floatParameters()
		, m_vec4Parameters()
		, m_proxy()
		, m_dirty(false)
		, m_material()
	{
	}

	MaterialInstance::MaterialInstance(const SoftObject<Material>& material)
		: MaterialInstance()
	{
		setMaterial(material);
	}

	void MaterialInstance::setMaterial(const SoftObject<Material>& material)
	{
		if (m_material == material || material == this)
		{
			return;
		}

		m_material = material;
		if (!m_material)
		{
			m_material.tryLoad();
		}

		m_floatParameters.clear();
		m_vec4Parameters.clear();

		if (!m_material)
		{
			return;
		}

		if (auto inst = dynamic_cast<MaterialInstance*>(m_material.get()))
		{
			m_floatParameters = inst->m_floatParameters;
			m_vec4Parameters = inst->m_vec4Parameters;
		}
		else
		{
			const auto& infos = m_material->getParameterInfos();
			for (auto idx = 0; idx < infos.size(); ++idx)
			{
				const auto& info = infos[idx];
				auto type = info.type;
				switch (type)
				{
				case volucris::MaterialParamterType::Float:
				{
					MaterialFloatParameter parameter = MaterialFloatParameter(info.name, std::get<float>(info.value));
					parameter.setId(idx);
					m_floatParameters.push_back(parameter);
				}
					break;
				case volucris::MaterialParamterType::Vector4:
				{
					MaterialVector4Parameter parameter = MaterialVector4Parameter(info.name, std::get<glm::vec4>(info.value));
					parameter.setId(idx);
					m_vec4Parameters.push_back(parameter);
				}
					break;
				case volucris::MaterialParamterType::Mat4:
					break;
				default:
					break;
				}
			}
		}
	}

	std::vector<MaterialParameterInfo> MaterialInstance::getParameters()
	{
		std::vector<MaterialParameterInfo> parameters;
		parameters.reserve(m_floatParameters.size() + m_vec4Parameters.size());
		for (const auto& parameter : m_floatParameters)
		{
			parameters.push_back(parameter.getParameterInfo());
		}

		for (const auto& parameter : m_vec4Parameters)
		{
			parameters.push_back(parameter.getParameterInfo());
		}
		return parameters;
	}

	MaterialUpdateData MaterialInstance::getUpdateData()
	{
		if (auto proxy = tryGetMaterialProxy())
		{
			return { proxy, getUpdateParameterInfos() };
		}
		return {};
	}

	std::vector<MaterialParameterUpdateInfo> MaterialInstance::getUpdateParameterInfos()
	{
		std::vector<MaterialParameterUpdateInfo> parameters;
		for (auto& parameter : m_floatParameters)
		{
			if (parameter.isDirty())
			{
				parameters.push_back(parameter.getUpdateInfo());
				parameter.markDirty(false);
			}
		}

		for (auto& parameter : m_vec4Parameters)
		{
			if (parameter.isDirty())
			{
				parameters.push_back(parameter.getUpdateInfo());
				parameter.markDirty(false);
			}
		}
		return parameters;
	}

	std::vector<MaterialParameterUpdateInfo> MaterialInstance::getAllUpdateParameterInfos()
	{
		std::vector<MaterialParameterUpdateInfo> parameters;
		for (auto& parameter : m_floatParameters)
		{
			parameters.push_back(parameter.getUpdateInfo());
			parameter.markDirty(false);
		}

		for (auto& parameter : m_vec4Parameters)
		{
			parameters.push_back(parameter.getUpdateInfo());
			parameter.markDirty(false);
		}
		return parameters;
	}

	std::shared_ptr<MaterialInstanceProxy> MaterialInstance::createMaterialProxy()
	{
		if (!m_material)
		{
			return nullptr;
		}

		auto base = m_material->getBaseProxy();
		auto proxy = std::make_shared<MaterialInstanceProxy>();
		auto updateParameters = getAllUpdateParameterInfos();

		gApp->addMaterialInstanceReference(getShared<MaterialInstance>());
		
		Renderer::getInstance().push([proxy, base, updateParameters]() {
			proxy->setMaterial(base);
			proxy->update(updateParameters);
			});
		return proxy;
	}

	bool MaterialInstance::setFloatParameter(const std::string& name, float value)
	{
		for (auto& param : m_floatParameters)
		{
			if (param.getName() == name)
			{
				param.setValue(value);
				m_dirty = true;
				return true;
			}
		}
		return false;
	}

	bool MaterialInstance::setVector4Parameter(const std::string& name, const glm::vec4& value)
	{
		for (auto& param : m_vec4Parameters)
		{
			if (param.getName() == name)
			{
				param.setValue(value);
				m_dirty = true;
				return true;
			}
		}
		return false;
	}
}

BOOST_CLASS_EXPORT_IMPLEMENT(volucris::MaterialInstance)