#include <Asset/AssetManager.h>
#include <Game/Package.h>
#include <Core/Volucris.h>
#include <Asset/AssetWriter.h>
#include <Asset/AssetReader.h>
#include <Game/World.h>
#include <Game/Texture2D.h>
#include <Game/StaticMesh.h>
#include <FileSystem/FileSystem.h>
#include <Asset/AssetPath.h>

namespace volucris
{
	AssetManager::AssetManager()
	{
		// 初始化代码
	}

	void AssetManager::scan()
	{
		scanAssets("/Engine/Content");
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
		package->setAssetData(assetData);
		m_assets[packageName] = object;
		m_assetDatas[packageName] = assetData;
		AssetRegistered.invoke(assetData);

		return true;
	}

	void AssetManager::unregister(const std::string& packageName)
	{
		auto it = m_assetDatas.find(packageName);
		if (it == m_assetDatas.end())
		{
			return;
		}
		auto assetData = it->second;
		m_assetDatas.erase(it);
		m_assets.erase(packageName);
		AssetUnregistered.invoke(assetData);
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

	std::vector<AssetData> AssetManager::getAssets(const std::string& className) const
	{
		if (className.empty())
		{
			V_LOG_WARN(Engine, "getAssets called with empty className");
			return {};
		}

		std::vector<AssetData> assets;
		for (const auto& [path, assetData] : m_assetDatas)
		{
			if (assetData.className == className)
			{
				assets.push_back(assetData);
			}
		}
		return assets;
	}

	void AssetManager::scanAssets(const std::string& rootPath)
	{
		const auto& filenodes = gFileSystem.getFileNodes(rootPath);
		for (const auto& node : filenodes)
		{
			if (node.type == EFileType::Asset)
			{
				const auto assetData = loadAssetData(node.path);
				if (!assetData.className.empty())
				{
					m_assetDatas[node.path] = assetData;
				}
			}
			else if (node.type == EFileType::Directory)
			{
				scanAssets(node.path); // 递归扫描子目录
			}
		}
	}

	std::vector<std::string> AssetManager::getReferenceAssets(const std::string& packageName) const
	{
		std::vector<std::string> referenceAssets;
		for (const auto& [path, assetData] : m_assetDatas)
		{
			auto it = std::find(assetData.dependencies.begin(), assetData.dependencies.end(), packageName);
			if (it != assetData.dependencies.end())
			{
				referenceAssets.push_back(assetData.path);
			}
		}
		return referenceAssets;
	}

	std::vector<AssetData> AssetManager::getAssetsInDirectory(const std::string& directory, bool currentOnly) const
	{
		std::vector<AssetData> assets;
		for (const auto& [path, assetData] : m_assetDatas)
		{
			if (assetData.path.find(directory) == 0)
			{
				if (currentOnly)
				{
					AssetPath path(assetData.path);
					if (path.packagePath != directory)
					{
						continue; // 只获取当前目录下的资源
					}
				}
				assets.push_back(assetData);
			}
		}
		return assets;
	}
}