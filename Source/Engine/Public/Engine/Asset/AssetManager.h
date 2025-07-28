#ifndef __volucris_asset_manager_h__
#define __volucris_asset_manager_h__

#include <Engine/Game/GameObject.h>
#include <map>
#include "AssetData.h"
#include <Engine/Core/Delegate.h>
#include <Engine/Game/Package.h>

namespace volucris
{
	DECLARE_EVENT_MUTI_DELEGATE(OnAssetRegistered, void, const Package*)

	class World;
	class Package;

	class AssetManager
	{
	public:
		OnAssetRegistered AssetRegistered;

	public:
		~AssetManager() = default;

		static AssetManager& getInstance()
		{
			static AssetManager inst;
			return inst;
		}

		void scan();

		bool registry(Package* package);

		void unregister(const std::string& packageName);

		void updateAssetData(const std::string& packageName, const AssetData& assetData);

		bool isPackageRegistered(const std::string& packageName) const
		{
			return m_assets.find(packageName) != m_assets.end();
		}

		void save(Package* package);

		std::shared_ptr<GameObject> load(const std::string& packageName, World* world = nullptr);

		template<typename T>
		std::shared_ptr<T> loadAsset(const std::string& packageName, World* world = nullptr)
		{
			if (auto asset = load(packageName, world))
			{
				return std::dynamic_pointer_cast<T>(asset);
			}
			return nullptr;
		}

		AssetData loadAssetData(const std::string& packageName) const;

		std::vector<AssetData> getAssets(const std::string& className) const;

		template<typename T, typename = std::enable_if_t<std::is_base_of_v<GameObject, T>>>
		std::vector<AssetData> getAssets() const
		{
			T object;
			return getAssets(object.getClassName());
		}

	private:
		void scanAssets(const std::string& rootPath);

	private:
		AssetManager();

		AssetManager(const AssetManager&) = delete;
		AssetManager(AssetManager&&) = delete;
		AssetManager& operator=(const AssetManager&) = delete;
		AssetManager& operator=(AssetManager&&) = delete;
		
	private:
		std::map<std::string, std::weak_ptr<GameObject>> m_assets;
		std::map<std::string, AssetData> m_assetDatas;
	};
}

#endif // !__volucris_asset_manager_h__
