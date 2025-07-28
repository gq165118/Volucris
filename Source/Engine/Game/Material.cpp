#include "Game/Material.h"
#include <Render/MaterialProxy.h>
#include <Render/Renderer.h>
#include <Render/MaterialInstanceProxy.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <Application/Application.h>

namespace volucris
{
	Material::Material()
		: GameObject()
		, m_vss()
		, m_fss()
		, m_parameters()
		, m_proxy()
		, m_matProxy()
	{
	}

	Material::Material(std::string vss, std::string fss)
		: Material()
	{
		setSource(std::move(vss), std::move(fss));
	}

	std::shared_ptr<MaterialProxy> Material::getBaseProxy()
	{
		std::shared_ptr<MaterialProxy> proxy = nullptr;
		if (!m_proxy.expired())
		{
			proxy = m_proxy.lock();
		}

		if (!proxy)
		{
			proxy = std::make_shared<MaterialProxy>();

			Renderer::getInstance().push([proxy, parameters = getParameterInfos(), vss = m_vss, fss = m_fss]() {
				proxy->setSource(vss, fss);
				proxy->setParameters(parameters);
				});
			m_proxy = proxy;
		}
		return proxy;
	}

	std::shared_ptr<MaterialInstanceProxy> Material::getMaterialProxy()
	{
		std::shared_ptr<MaterialInstanceProxy> proxy = nullptr;
		if (!m_matProxy.expired())
		{
			proxy = m_matProxy.lock();
		}

		if (!proxy)
		{
			proxy = createMaterialProxy();
			m_matProxy = proxy;
		}
		return proxy;
	}

	std::shared_ptr<MaterialInstanceProxy> Material::tryGetMaterialProxy() const
	{
		std::shared_ptr<MaterialInstanceProxy> proxy = nullptr;
		if (!m_matProxy.expired())
		{
			proxy = m_matProxy.lock();
		}
		return proxy;
	}

	std::shared_ptr<MaterialInstanceProxy> Material::createMaterialProxy()
	{
		auto proxy = std::make_shared<MaterialInstanceProxy>();
		auto baseProxy = getBaseProxy();
		Renderer::getInstance().push([proxy, baseProxy]() {
			proxy->setMaterial(baseProxy);
			});
		return proxy;
	}

	std::vector<MaterialParameterInfo> Material::getParameterInfos() const
	{
		std::vector<MaterialParameterInfo> infos;
		for (const auto& param : m_parameters)
		{
			infos.push_back(param.getInfo());
		}
		return infos;
	}
}

BOOST_CLASS_EXPORT_IMPLEMENT(volucris::Material)