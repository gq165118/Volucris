#include <Game/Texture2D.h>
// 必须包含的存档类型
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <Render/Texture2DProxy.h>
#include <Render/Renderer.h>

namespace volucris
{
	Texture2D::Texture2D()
		: GameObject()
		, m_data()
		, m_proxy()
	{
	}

	Texture2D::Texture2D(Texture::TextureData data)
		: GameObject()
		, m_data(std::move(data))
		, m_proxy()
	{
	}

	Texture2D::~Texture2D()
	{
	}


	std::shared_ptr<Texture2DProxy> Texture2D::getProxy()
	{
		std::shared_ptr<Texture2DProxy> proxy = nullptr;
		if (!m_proxy.expired())
		{
			proxy = m_proxy.lock();
		}
		if (!proxy)
		{
			proxy = std::make_shared<Texture2DProxy>();
			Renderer::getInstance().push([proxy, data = m_data]() {
				proxy->setTextureData(data);
				});
			m_proxy = proxy;
		}
		return proxy;
	}

} // namespace volucris

BOOST_CLASS_EXPORT_IMPLEMENT(volucris::Texture2D)
