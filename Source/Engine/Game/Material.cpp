#include <Game/Material.h>
#include <Render/Renderer.h>
#include <Render/MaterialProxy.h>

namespace volucris
{
	Material::Material()
		: GameObject()
		, m_vss()
		, m_fss()
		, m_floatParameters()
		, m_vec4Parameters()
		, m_proxy()
		, m_dirty(false)
	{
	}

	Material::Material(std::string vss, std::string fss)
		: GameObject()
		, m_vss(std::move(vss))
		, m_fss(std::move(fss))
		, m_floatParameters()
		, m_vec4Parameters()
		, m_proxy()
		, m_dirty(false)
	{
	}

	std::vector<MaterialParameterInfo> Material::getParameters()
	{
		std::vector<MaterialParameterInfo> parameters;
		parameters.reserve(m_floatParameters.size() + m_vec4Parameters.size());
		for (auto& parameter : m_floatParameters)
		{
			parameter.setId(parameters.size());
			parameters.push_back(parameter.getParameterInfo());
		}

		for (auto& parameter : m_vec4Parameters)
		{
			parameter.setId(parameters.size());
			parameters.push_back(parameter.getParameterInfo());
		}
		return parameters;
	}

	MaterialFloatParameter& Material::addParameter(const std::string& name, float value)
	{
		m_floatParameters.push_back({ name, value });
		return *m_floatParameters.rbegin();
	}

	MaterialVector4Parameter& Material::addParameter(const std::string& name, glm::vec4 value)
	{
		m_vec4Parameters.push_back({ name, value });
		return *m_vec4Parameters.rbegin();
	}

	std::vector<MaterialParameterUpdateInfo> Material::getUpdateParameterInfos()
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

	bool Material::setFloatParameter(const std::string& name, float value)
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

	bool Material::setVector4Parameter(const std::string& name, const glm::vec4& value)
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

	std::shared_ptr<MaterialProxy> Material::getProxy()
	{
		std::shared_ptr<MaterialProxy> proxy = nullptr;
		if (!m_proxy.expired())
		{
			proxy = m_proxy.lock();
		}

		if (!proxy)
		{
			proxy = std::make_shared<MaterialProxy>();
			
			Renderer::getInstance().push([proxy, parameters = getParameters(), vss=m_vss, fss=m_fss]() {
				proxy->setSource(vss, fss);
				proxy->setParameters(parameters);
				});
			m_proxy = proxy;
			m_dirty = false;
		}
		return proxy;
	}

	void Material::update()
	{
		if (!m_dirty)
		{
			return;
		}

		auto proxy = getProxy();
		Renderer::getInstance().push([proxy, parameters = getUpdateParameterInfos()]() {
			proxy->update(parameters);
			});
		m_dirty = false;
	}
}

BOOST_CLASS_EXPORT_IMPLEMENT(volucris::Material)