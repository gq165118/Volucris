#include "Render/Texture2DProxy.h"
#include <RHI/RHITexture.h>
#include <Render/Renderer.h>

namespace volucris
{
	Texture2DProxy::Texture2DProxy()
		: Object()
		, m_texture(nullptr)
	{
	}

	Texture2DProxy::~Texture2DProxy()
	{
	}

	void Texture2DProxy::setTextureData(const Texture::TextureData& data)
	{
		Texture::EPixelFormat format;
		switch (data.format)
		{
		case Texture::ESourceFormat::RGB:
			format = Texture::EPixelFormat::R8G8B8;
			break;
		case Texture::ESourceFormat::RGBA:
			format = Texture::EPixelFormat::R8G8B8A8;
			break;
		default:
			break;
		}
		RHITextureDesc desc = RHITextureDesc::create2D(data.size.width, data.size.height, format);
		desc.sourceFormat = data.format;
		m_texture = std::make_shared<RHITexture2D>(desc);
		m_texture->setContext(RHICmdList);
		m_texture->createGpuResource();
		m_texture->init(data.data);
	}
}
