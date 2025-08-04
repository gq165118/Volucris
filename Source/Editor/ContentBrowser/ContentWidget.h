#ifndef __volucris_content_widget_h__
#define __volucris_content_widget_h__

#include <Engine/Application/Widget.h>
#include <glm/glm.hpp>
#include <Engine/FileSystem/FileSystem.h>
#include <Engine/Asset/AssetData.h>
#include <Common/Thumbnail.h>
#include <Common/AssetTool.h>

namespace volucris
{
	class ContentItemWidget;
	class RHITexture2D;
	class Package;

	class ContentWidget : public Widget
	{
	public:
		ContentWidget();

		~ContentWidget() override;

		void setCurrentFolder(const std::string& folder);

		void setSelectedItem(ContentItemWidget* item);

		void dirtyCurrentFolder(const std::string& folder)
		{
			m_folder = folder;
			m_folderDirty = true;
		}

		const std::string& getCurrentFolder() const { return m_folder; }

		void addNameChangedPackageName(const std::shared_ptr<Package>& package, const std::string& newPackageName);

	protected:
		void onBuild(bool init) override;

		void onRendererBuild(RHICommandList* cmdList) override;

		void onRendererDestroy(RHICommandList* cmdList) override;

		bool onDrop(DropEvent* event) override;

		void onAssetCreated(const AssetInfo& assetInfo);

		void onAssetDirty(const AssetInfo& assetInfo);

		void onAssetLoaded(Package* package);

		void onAssetUnregistered(const std::string& packageName);

	private:
		std::unique_ptr<ContentItemWidget> createFolderItem(const std::string& fullpath);

		std::unique_ptr<ContentItemWidget> createAssetItem(const AssetInfo& assetInfo);

		std::unique_ptr<ContentItemWidget> createMaterialItem(const AssetInfo& assetInfo);

		std::unique_ptr<ContentItemWidget> createMaterialInstanceItem(const AssetInfo& assetInfo);

		std::unique_ptr<ContentItemWidget> createTexture2DItem(const AssetInfo& assetInfo);

		std::unique_ptr<ContentItemWidget> createStaticMeshItem(const AssetInfo& assetInfo);

	private:
		float m_scale;
		glm::vec2 m_itemSize;
		std::vector<std::unique_ptr<ContentItemWidget>> m_items;
		bool m_multiSelect;
		bool m_folderDirty;
		std::string m_folder;
		std::vector<std::pair<std::shared_ptr<Package>, std::string>> m_nameChangedPackages;
	};
}

#endif // !__volucris_folder_navigation_widget_h__
