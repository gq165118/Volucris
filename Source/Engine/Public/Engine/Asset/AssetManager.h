#ifndef __volucris_asset_manager_h__
#define __volucris_asset_manager_h__

#include <map>
#include "AssetData.h"
#include <Engine/Core/Delegate.h>
#include <Engine/Game/Package.h>

namespace volucris
{
	DECLARE_EVENT_MUTI_DELEGATE(PackageEvent, void, Package*)
	DECLARE_EVENT_MUTI_DELEGATE(AssetUnRegisterEvent, void, const std::string&)
	DECLARE_EVENT_MUTI_DELEGATE(AssetLoaded, void, Package*)

	class World;
	class Package;
	class GameObject;

	class AssetManager
	{
	public:
		PackageEvent AssetRegistered;
		PackageEvent AssetLoaded;
		AssetUnRegisterEvent AssetUnregistered;

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

		bool isPackageRegistered(const std::string& packageName) const
		{
			return m_assets.find(packageName) != m_assets.end();
		}

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

		void updateAssetDependence(GameObject* object);

		bool save(const std::shared_ptr<Package>& package);

		std::shared_ptr<GameObject> tryLoad(const std::string& packageName, World* world = nullptr)
		{
			auto it = m_assets.find(packageName);
			if (it != m_assets.end() && !it->second.expired())
			{
				return it->second.lock();
			}
			return nullptr;
		}

		AssetData getAssetData(const std::string& packageName) const;

		std::vector<AssetData> getAssets(const std::string& className) const;

		template<typename T, typename = std::enable_if_t<std::is_base_of_v<GameObject, T>>>
		std::vector<AssetData> getAssets() const
		{
			T object;
			return getAssets(object.getClassName());
		}

		std::vector<std::string> getReferenceAssets(const std::string& packageName) const;

		std::vector<AssetData> getAssetsInDirectory(const std::string& directory, bool recursion =false) const;

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
