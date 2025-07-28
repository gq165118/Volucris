#ifndef __volucris_texture2d_resource_h__
#define __volucris_texture2d_resource_h__

#include <vector>
#include <Engine/Core/TypesHelp.h>
#include <Engine/Render/Resource.h>
#include <Engine/Core/TextureDefines.h>

namespace volucris
{
	class RHITexture2D;

	class Texture2DProxy : public Object
	{
	public:
		Texture2DProxy();

		~Texture2DProxy();

		void setTextureData(const Texture::TextureData& data);

		std::shared_ptr<RHITexture2D> getTexture() const
		{
			return m_texture;
		}

	private:
		std::shared_ptr<RHITexture2D> m_texture;
	};
}

#endif // !__volucris_texture2d_resource_h__
