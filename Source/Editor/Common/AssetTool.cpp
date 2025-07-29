#include <Common/AssetTool.h>
#include <EditorCore/Editor.h>
#include <Engine/Asset/AssetWriter.h>
#include <Engine/Game/Material.h>
#include <Engine/Game/MaterialParameter.h>
#include <Engine/Render/MaterialParameterInfo.h>
#include <Engine/Game/MaterialInstance.h>
#include "AssetObjectHelper.h"

namespace volucris
{
	static std::vector<std::string> dependenceCheck(GameObject* object)
	{
		
	}

	AssetTool::AssetTool()
		: m_dirtyAssets()
	{
	}

	bool AssetTool::save(const std::shared_ptr<Package>& package)
	{
		if (!package)
		{
			return false;
		}

		if (package->getAssetObject() == nullptr)
		{
			V_LOG_ERROR(Editor, "AssetTool::save: Package has no asset object set.");
			return false;
		}

		if (auto helper = AssetObjectHelper::getAssetHelper(package->getAssetObject()))
		{
			auto assetData = package->getAssetData();
			assetData.dependencies = helper->getDependences();
			package->setAssetData(assetData);
		}

		AssetWriter writer = AssetWriter(std::shared_ptr<Package>(package));
		
		if (writer.write())
		{
			V_LOG_INFO(Editor, "AssetTool::save: Package saved successfully: {}", package->getAssetData().path);
			removeDirtyAsset(package->getAssetData().path);
			return true;
		}
		else
		{
			V_LOG_ERROR(Editor, "AssetTool::save: Failed to save package: {}", package->getAssetData().path);
		}
		return false;
	}

	void AssetTool::addDirtyAsset(const std::string& packageName, const std::shared_ptr<GameObject>& object)
	{
		if (!object || packageName.empty())
		{
			V_LOG_WARN(Editor, "AssetTool: Invalid dirty object.");
			return;
		}

		if (!AssetManager::getInstance().isPackageRegistered(packageName))
		{
			V_LOG_WARN(Editor, "AssetTool: Package not registered: {}", packageName);
			return;
		}

		m_dirtyAssets[packageName] = object;
	}

	void AssetTool::renamePackage(const std::shared_ptr<Package>& package, const std::string& newPackageName)
	{
		auto& inst = AssetManager::getInstance();
		auto assetData = package->getAssetData();

		if (!package || newPackageName.empty() || !inst.isPackageRegistered(assetData.path))
		{
			V_LOG_WARN(Editor, "AssetTool: Invalid package or new package name.");
			return;
		}

		m_dirtyAssets.erase(package->getAssetData().path);

		inst.unregister(package->getAssetData().path);
		
		assetData.path = newPackageName;
		package->setAssetData(assetData);
		inst.registry(package.get());

		m_dirtyAssets[newPackageName] = package->getAssetObject();

		const auto packageNames = inst.getReferenceAssets(package->getAssetData().path);
		for (const auto& packageName : packageNames)
		{
			if (auto object = inst.load(packageName))
			{
				auto helper = AssetObjectHelper::getAssetHelper(object);
				if (helper->updateDependences({ { package->getAssetData().path, newPackageName } }))
				{
					m_dirtyAssets[packageName] = object;
				}
			}
		}
	}

	void AssetTool::removeDirtyAsset(const std::string& packageName)
	{
		m_dirtyAssets.erase(packageName);
	}
}