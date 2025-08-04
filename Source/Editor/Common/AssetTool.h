#ifndef __volucris_asset_tool_h__
#define __volucris_asset_tool_h__

#include <vector>
#include <Engine/Game/SoftObject.h>
#include <Engine/Asset/AssetManager.h>

namespace volucris
{
	struct AssetInfo
	{
		AssetData data;
		std::shared_ptr<GameObject> object = nullptr;
		bool dirty = false;
	};

	DECLARE_EVENT_MUTI_DELEGATE(AssetInfoEvent, void, const AssetInfo&)
	DECLARE_EVENT_MUTI_DELEGATE(AssetDeletedEvent, void, const std::string&)

	class AssetTool
	{
	public:
		AssetInfoEvent AssetCreated;
		AssetInfoEvent AssetDirtyStateChanged;
		AssetDeletedEvent AssetDeleted;

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

		std::vector<AssetInfo> getAssetsInfoInFolder(const std::string& folder, bool recursion=false) const;

		static std::string getDefaultPackageName(const std::string& folderPath, const std::string& name);

	private:
		void removeDirtyAsset(const std::string& packageName);

		void onAssetLoaded(Package* package);

		void onAssetRegistered(Package* package);

		void onAssetUnregistered(const std::string& packageName);

		void onAssetDirtyStateChanged(GameObject* object);

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
