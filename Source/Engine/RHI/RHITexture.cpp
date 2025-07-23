#include "RHI/RHITexture.h"
#include <glad/glad.h>
#include <RHI/RHICommandList.h>
#include <RHI/RHIOpenGL.h>

namespace volucris
{
	GLenum getTextureInternalFormat(Texture::EPixelFormat format)
	{
		switch (format)
		{
		case volucris::Texture::EPixelFormat::R8G8B8:
			return GL_RGB8;
		case volucris::Texture::EPixelFormat::R8G8B8A8:
			return GL_RGBA8;
		default:
			break;
		}
		return GL_NONE;
	}

	GLenum getTextureFormat(Texture::ESourceFormat format)
	{
		switch (format)
		{
		case volucris::Texture::ESourceFormat::Invalid:
			break;
		case volucris::Texture::ESourceFormat::RGB:
			return GL_RGB;
		case volucris::Texture::ESourceFormat::RGBA:
			return GL_RGBA;
		default:
			break;
		}
		return GL_NONE;
	}

	RHITexture::RHITexture()
		: RHIResource()
		, m_type(TextureType::Texture2D)
		, m_id(0)
	{
	}

	RHITexture::~RHITexture()
	{
		if (m_id > 0)
		{
			glDeleteTextures(1, &m_id);
		}
	}

	void RHITexture::createGpuResource()
	{
		glGenTextures(1, &m_id);
	}

	RHITexture2D::RHITexture2D()
		: RHITexture()
		, m_size(0, 0)
		, m_pixelFormat(Texture::EPixelFormat::Invalid)
		, m_sourceFormat(Texture::ESourceFormat::Invalid)
	{
	}

	RHITexture2D::RHITexture2D(const RHITextureDesc& desc)
		: RHITexture()
		, m_size(desc.size)
		, m_pixelFormat(desc.pixelFormat)
		, m_sourceFormat(desc.sourceFormat)
	{
	}

	RHITexture2D::~RHITexture2D()
	{
		getContext()->unsetTexture2D(this);
	}

	bool RHITexture2D::init()
	{
		if (m_pixelFormat == Texture::EPixelFormat::Invalid || !m_size.isValid())
		{
			return false;
		}
		getContext()->setTexture2D(this);
		glTexStorage2D(GL_TEXTURE_2D, 1, getTextureInternalFormat(m_pixelFormat), m_size.width, m_size.height);
		GL_CHECK()
		return true;
	}

	bool RHITexture2D::init(const std::vector<uint8>& data)
	{
		if (m_pixelFormat == Texture::EPixelFormat::Invalid || !m_size.isValid())
		{
			return false;
		}
		auto ctx = getContext();
		ctx->makesureBufferUnset(RHIBuffer::PixelPackBuffer);
		ctx->makesureBufferUnset(RHIBuffer::PixelUnpackBuffer);
		ctx->setTexture2D(this);
		getContext()->makesureBufferUnset(RHIBuffer::PixelPackBuffer);
		getContext()->makesureBufferUnset(RHIBuffer::PixelUnpackBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, getTextureInternalFormat(m_pixelFormat), m_size.width, m_size.height, 0, getTextureFormat(m_sourceFormat), GL_UNSIGNED_BYTE, data.data());
		GL_CHECK()
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		GL_CHECK()
		return true;
	}

	void RHITexture2D::update(const Rect& rect, const std::vector<uint8>& data)
	{
		auto ctx = getContext();
		ctx->makesureBufferUnset(RHIBuffer::PixelPackBuffer);
		ctx->makesureBufferUnset(RHIBuffer::PixelUnpackBuffer);
		ctx->setTexture2D(this);
		glTexSubImage2D(GL_TEXTURE_2D, 0, rect.x, rect.y, rect.width, rect.height, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
		GL_CHECK();
	}

	std::shared_ptr<RHITexture> RHICreateTexture(const RHITextureDesc& desc)
	{
		switch (desc.texClass)
		{
		case TextureType::Texture2D:
			return std::make_shared<RHITexture2D>(desc);
			break;
		default:
			break;
		}
		return nullptr;
	}
}
