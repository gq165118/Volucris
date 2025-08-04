#ifndef __volucris_asset_path_h__
#define __volucris_asset_path_h__

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

namespace volucris
{
	struct AssetPath
	{
		std::string fullpath;
		std::string path;
		std::string name;

		AssetPath()
			: fullpath(), path(), name()
		{ }

		AssetPath(const std::string& packageName)
			: fullpath(packageName)
		{
			auto packagePath = fs::path(packageName);
			path = packagePath.parent_path().generic_u8string();
			name = packagePath.stem().generic_u8string();
		}

		AssetPath(const std::string& packagePath, const std::string& assetName)
			: path(packagePath), name(assetName)
		{
			fullpath = (fs::path(path) / name).generic_u8string();
		}

		void setName(const std::string& newName)
		{
			name = newName;
			fullpath = (fs::path(path) / name).generic_u8string();
		}

		template <class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& fullpath;
			ar& path;
			ar& name;
		}
	};
}

#endif // !__volucris_asset_path_h__
