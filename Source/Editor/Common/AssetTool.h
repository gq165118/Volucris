#ifndef __volucris_asset_tool_h__
#define __volucris_asset_tool_h__

#include <vector>
#include <Engine/Game/SoftObject.h>
#include <Engine/Asset/AssetManager.h>

namespace volucris
{
	class AssetTool
	{
	public:
		~AssetTool() = default;

		static AssetTool& getInstance()
		{
			static AssetTool instance;
			return instance;
		}

		bool save(const std::shared_ptr<Package>& package);

		void addDirtyAsset(const std::string& packageName, const std::shared_ptr<GameObject>& object);

		void renamePackage(const std::shared_ptr<Package>& package, const std::string& packageName);

	private:
		void removeDirtyAsset(const std::string& packageName);

	private:
		std::map<std::string, std::shared_ptr<GameObject>> m_dirtyAssets;

	private:
		AssetTool();
		// 禁止拷贝和赋值
		AssetTool(const AssetTool&) = delete;
		AssetTool& operator=(const AssetTool&) = delete;
		// 禁止移动
		AssetTool(AssetTool&&) = delete;
		AssetTool& operator=(AssetTool&&) = delete;
	};
}

#define gAssetTool volucris::AssetTool::getInstance()

#endif // !__volucris_asset_tool_h__
