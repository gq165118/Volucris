#ifndef __volucris_asset_path_h__
#define __volucris_asset_path_h__

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

namespace volucris
{
	struct AssetPath
	{
		std::string packagePath;
		std::string assetName;

		AssetPath(const std::string& packageName)
		{
			auto path = fs::path(packageName);
			packagePath = path.parent_path().generic_u8string();
			assetName = path.stem().generic_u8string();
		}
	};
}

#endif // !__volucris_asset_path_h__
