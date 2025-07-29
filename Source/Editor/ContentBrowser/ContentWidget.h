#ifndef __volucris_content_widget_h__
#define __volucris_content_widget_h__

#include <Engine/Application/Widget.h>
#include <glm/glm.hpp>
#include <Engine/FileSystem/FileSystem.h>
#include <Engine/Asset/AssetData.h>

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

	protected:
		void onBuild(bool init) override;

		void onRendererBuild(RHICommandList* cmdList) override;

		void onRendererDestroy(RHICommandList* cmdList) override;

		bool onDrop(DropEvent* event) override;

		void onAssetRegistered(const AssetData& assetData);

		void onAssetUnregistered(const AssetData& assetData);

	private:
		struct Icon
		{
			Point pos;
			Size size;
		};

		std::unique_ptr<ContentItemWidget> createItem(const FileNode& node, const Icon& icon, const std::string& name = "");

		std::unique_ptr<ContentItemWidget> createFolderItem(const std::string& path, const std::string& name = "");

		std::unique_ptr<ContentItemWidget> createTextureItem(const std::string& path);

		std::unique_ptr<ContentItemWidget> createStaticMeshItem(const std::string& path);

		void addAssetItem(const AssetData& assetData);

		void deleteItem(ContentItemWidget* item);

	private:
		float m_scale;
		glm::vec2 m_itemSize;
		std::vector<std::unique_ptr<ContentItemWidget>> m_items;
		bool m_multiSelect;
		std::string m_folder;
		ContentItemWidget* m_controlItem;
		RHITexture2D*  m_iconTexture;
	};
}

#endif // !__volucris_folder_navigation_widget_h__
