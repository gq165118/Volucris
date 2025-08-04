#ifndef __volucris_thumbnail_h__
#define __volucris_thumbnail_h__

#include <memory>
#include <Engine/Core/Point.h>
#include <Engine/Core/Size.h>
#include <glm/glm.hpp>

namespace volucris
{
	class RHITexture2D;

	struct Thumbnail
	{
		std::shared_ptr<RHITexture2D> texture;
		Point pos;
		Size size;

		glm::vec2 minUV;
		glm::vec2 maxUV;

		void update();

		bool isValid() const;
	};
}

#endif // !__volucris_thumbnail_h__
