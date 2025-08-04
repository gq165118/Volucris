#include "Thumbnail.h"
#include <Engine/RHI/RHITexture.h>

namespace volucris
{
	void Thumbnail::update()
	{
		if (!texture) return;

		auto texSize = texture->getSize();
		auto ustep = size.width * 1.0f / texSize.width;
		auto vstep = size.height * 1.0f / texSize.height;

		minUV = { pos.x * ustep, 1.0f - pos.y * vstep };
		maxUV = { minUV.x + ustep, minUV.y - vstep };
	}

	bool Thumbnail::isValid() const
	{
		return texture && texture->getSize().isValid();
	}
} // namespace volucris