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
	{
	}

	Material::Material(std::string vss, std::string fss)
		: GameObject()
		, m_vss(std::move(vss))
		, m_fss(std::move(fss))
		, m_floatParameters()
		, m_vec4Parameters()
		, m_proxy()
	{
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

	bool Material::setFloatParameter(const std::string& name, float value)
	{
		for (auto& param : m_floatParameters)
		{
			if (param.getName() == name)
			{
				param.setValue(value);
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
			std::vector<MaterialParameterInfo> parameters;
			for (const auto& parameter : m_floatParameters)
			{
				parameters.push_back(parameter.getParameterInfo());
			}

			for (const auto& parameter : m_vec4Parameters)
			{
				parameters.push_back(parameter.getParameterInfo());
			}
			Renderer::getInstance().push([proxy, parameters, vss=m_vss, fss=m_fss]() {
				proxy->setSource(vss, fss);
				proxy->setParameters(parameters);
				});
			m_proxy = proxy;
		}
		return proxy;
	}
}

BOOST_CLASS_EXPORT_IMPLEMENT(volucris::Material)