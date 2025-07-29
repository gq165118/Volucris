#include "AssetObjectHelper.h"
#include <MaterialEditor/MaterialTemplate.h>
#include <Engine/Game/MaterialInstance.h>
#include "MaterialObjectHelper.h"

namespace volucris
{
	std::shared_ptr<AssetObjectHelper> AssetObjectHelper::getAssetHelper(const std::shared_ptr<GameObject>& object)
	{
		if (!object) return nullptr;
		if (auto material = std::dynamic_pointer_cast<Material>(object))
		{
			return std::make_shared<MaterialObjectHelper>(material);
		}
		else if (auto materialInstance = std::dynamic_pointer_cast<MaterialInstance>(object))
		{
			return std::make_shared<MaterialInstanceObjectHelper>(materialInstance);
		}
		return nullptr;
	}
}