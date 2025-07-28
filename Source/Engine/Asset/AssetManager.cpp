#include <Asset/AssetManager.h>
#include <Game/Package.h>
#include <Core/Volucris.h>
#include <Asset/AssetWriter.h>
#include <Asset/AssetReader.h>
#include <Game/World.h>
#include <Game/Texture2D.h>
#include <Game/StaticMesh.h>

namespace volucris
{
	AssetManager::AssetManager()
	{
		// 初始化代码
	}

	bool AssetManager::registry(Package* package)
	{
		if (m_assetDatas.find(package->getAssetData().path) != m_assetDatas.end()) 
		{
			V_LOG_WARN(Engine, "Package {} is already registered.", package->getAssetData().path);
			return false; // 如果包已经注册，则返回false
		}

		const auto object = package->getAssetObject();
		if (!object)
		{
			V_LOG_WARN(Engine, "Package {} registry failed. no valid asset object", package->getAssetData().path);
			return false;
		}
		auto className = object->getClassName();
		if (className.empty())
		{
			V_LOG_WARN(Engine, "Package {} registry failed. no valid asset object: className", package->getAssetData().path);
			return false;
		}

		auto assetData = package->getAssetData();
		assetData.className = std::move(className);
		assetData.guid = GUID::generate();
		auto packageName = assetData.path;
		package->m_assetData = assetData;
		m_assets[packageName] = object;
		m_assetDatas[packageName] = assetData;
		AssetRegistered.invoke(package);

		return true;
	}

	void AssetManager::unregister(const std::string& packageName)
	{
		
	}

	void AssetManager::updateAssetData(const std::string& packageName, const AssetData& assetData)
	{
		auto it = m_assetDatas.find(packageName);
		if (it != m_assetDatas.end()) {
			it->second = assetData; // 更新现有数据
		} else {
			V_LOG_WARN(Engine, "Asset data for package {} not found.", packageName);
		}
		
		// todo: 触发事件通知
		//m_packageRegisteredEvent.invoke(packageName);
	}

	void AssetManager::save(Package* package)
	{
		if (!package->getAssetObject())
		{
			V_LOG_ERROR(Engine, "save package failed. not only 1 child");
			return;
		}

		AssetWriter writer = AssetWriter(package->getShared<Package>());
		writer.write();
	}

	std::shared_ptr<GameObject> AssetManager::load(const std::string& packageName, World* world)
	{
		std::shared_ptr<GameObject> object = nullptr;
		
		auto it = m_assets.find(packageName);
		if (it != m_assets.end() && !it->second.expired()) 
		{
			object = it->second.lock(); // 如果包已注册且未过期，则返回
		}
		
		if (!object)
		{
			AssetReader reader = AssetReader(packageName);
			auto package = reader.readPackage();
			if (package)
			{
				object = package->getAssetObject();
				m_assets[packageName] = object;
			}
			else
			{
				V_LOG_WARN(Engine, "load asset failed. {}", packageName);
			}
		}

		if (object && world)
		{
			world->addObject(object);
		}

		return object;
	}

	AssetData AssetManager::loadAssetData(const std::string& packageName) const
	{
		AssetReader reader = AssetReader(packageName);
		return reader.readAssetData();
	}
}