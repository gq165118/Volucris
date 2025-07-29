#ifndef __volucris_asset_object_helper_h__
#define __volucris_asset_object_helper_h__

#include <vector>
#include <string>
#include <map>
#include <memory>

namespace volucris
{
	class GameObject;

	class AssetObjectHelper
	{
	public:
		virtual ~AssetObjectHelper() = default;

		virtual std::vector<std::string> getDependences() const = 0;

		virtual bool updateDependences(const std::map<std::string, std::string>& dependence) = 0;

		static std::shared_ptr<AssetObjectHelper> getAssetHelper(const std::shared_ptr<GameObject>& object);
	};
}

#endif // !__volucris_asset_object_helper_h__
