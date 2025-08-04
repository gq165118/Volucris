#include "ContentWidget.h"
#include <Engine/FileSystem/FileSystem.h>
#include <ContentBrowser/ContentItemWidget.h>
#include <EditorEntry/EditorApplication.h>
#include <Engine/Asset/AssetManager.h>
#include <Engine/Game/Texture2D.h>
#include <imgui.h>
#include <EditorEntry/EditorWindow.h>
#include <Engine/Application/Event.h>
#include <filesystem>
#include <EditorCore/Editor.h>
#include <EditorCore/ImageLoader.h>
#include <Engine/Game/Package.h>
#include "MeshLoader.h"
#include <Engine/Game/StaticMesh.h>
#include "MaterialEditor/MaterialLoader.h"
#include <MaterialEditor/MaterialEditorWidget.h>
#include <Engine/Game/MaterialInstance.h>
#include <Common/AssetTool.h>
#include <Engine/Asset/AssetPath.h>
#include "AssetMenuContext.h"

namespace fs = std::filesystem;

namespace volucris
{
	static std::string getDefaultFolderName(const fs::path& dirpath, const std::string& name)
	{
		std::string folderName = (dirpath / name).generic_u8string();
		if (gFileSystem.directoryExists(folderName) )
		{
			for (size_t i = 1; i < std::numeric_limits<size_t>::max(); ++i)
			{
				folderName = (dirpath / fmt::format("{}_{}", name, i)).generic_u8string();
				if (!gFileSystem.directoryExists(folderName))
				{
					break;
				}
			}
		}
		return folderName;
	}

	ContentWidget::ContentWidget()
		: Widget()
		, m_scale(1.0)
		, m_itemSize(ContentItemWidget::getItemSize())
		, m_multiSelect(false)
		, m_folderDirty(false)
		, m_folder()
		, m_nameChangedPackages()
	{
		setCurrentFolder(u8"/Engine/Content/Editor");

		gAssetTool.AssetCreated.bindObject(this, &ContentWidget::onAssetCreated);
		gAssetTool.AssetDirtyStateChanged.bindObject(this, &ContentWidget::onAssetDirty);
		AssetManager::getInstance().AssetLoaded.bindObject(this, &ContentWidget::onAssetLoaded);
	}

	ContentWidget::~ContentWidget()
	{
		gAssetTool.AssetCreated.unbind(this);
		gAssetTool.AssetDirtyStateChanged.unbind(this);
		AssetManager::getInstance().AssetLoaded.unbind(this);
	}

	void ContentWidget::setCurrentFolder(const std::string& folder)
	{
		m_folder = folder;

		m_items.clear();
		{
			auto parentNode = gFileSystem.parentNode(folder);
			if (!parentNode.path.empty())
			{
				auto item = createFolderItem(parentNode.path);
				item->setDisplayName("..");
				item->setSelectable(false);
				m_items.emplace_back(std::move(item));
			}
		}

		auto nodes = gFileSystem.getFileNodes(folder, (int)EFileType::Directory);
		for (const auto& node : nodes)
		{
			m_items.emplace_back(createFolderItem(node.path));
		}

		const auto& assetInfos = gAssetTool.getAssetsInfoInFolder(m_folder);

		for (const auto& assetInfo : assetInfos)
		{
			if (auto item = createAssetItem(assetInfo))
			{
				m_items.push_back(std::move(item));
			}
		}
		m_folderDirty = false;
	}

	void ContentWidget::setSelectedItem(ContentItemWidget* item)
	{
		if (!m_multiSelect)
		{
			for (auto& it : m_items)
			{
				if (it.get() != item)
				{
					it->setSelected(false);
				}
			}
		}
	}

	void ContentWidget::addNameChangedPackageName(const std::shared_ptr<Package>& package, const std::string& newPackageName)
	{
		m_nameChangedPackages.push_back({ package, newPackageName });
	}

	void ContentWidget::onBuild(bool init)
	{
		ImGui::Begin("Content");
		m_multiSelect = ImGui::GetIO().KeyCtrl;
		int width = m_itemSize.x;
		int space = 10 * m_scale;
		auto size = ImGui::GetContentRegionAvail();
		int num_columns = (size.x + space) / (width + space);  // 列数
		num_columns = num_columns < 1 ? 1 : num_columns;
		ImGui::Columns(num_columns, nullptr, false); // 创建列

		ContentItemWidget* clickedItem = nullptr;
		ContentItemWidget* selectItem = nullptr;
		bool deleteOperation = false;
		for (int i = 0; i < m_items.size(); ++i) {
			ImGui::PushID(i);
			m_items[i]->build();

			if (m_items[i]->isClicked())
			{
				clickedItem = m_items[i].get();
			}
			else if (m_items[i]->isSelected())
			{
				selectItem = m_items[i].get();
			}

			if (m_items[i]->isDeleteSelected())
			{
				deleteOperation = true;
			}

			ImGui::NextColumn();
			ImGui::PopID();
		}
		ImGui::Columns(1); // 结束列

		bool createFolder = false;
		bool refreshFolder = false;
		if (ImGui::BeginPopupContextWindow("ContentContext", 
			ImGuiPopupFlags_NoOpenOverItems |
			ImGuiPopupFlags_MouseButtonRight))
		{
			for (auto& item : m_items)
			{
				item->setSelected(false);
			}
			ImGui::SeparatorText("Folder");
			if (ImGui::MenuItem("Refresh"))
			{
				refreshFolder = true;
			}

			if (ImGui::MenuItem("Create")) 
			{
				createFolder = true;
			}
			ImGui::EndPopup();
		}

		ImGui::End();

		if (m_folderDirty)
		{
			setCurrentFolder(m_folder);
		}

		if (refreshFolder)
		{
			setCurrentFolder(m_folder);
		}

		if (createFolder)
		{
			auto crtPath = fs::path(m_folder);
			auto folderName = getDefaultFolderName(crtPath, "New Folder");
			const auto path = crtPath / folderName;
			auto item = createFolderItem(path.generic_u8string());
			item->setSelected(true);
			item->setEditing(true);
			m_items.push_back(std::move(item));
		}

		if (deleteOperation)
		{
			std::vector<std::unique_ptr<ContentItemWidget>> items;
			for (auto& item : m_items)
			{
				if (item->isSelected())
				{
					//deleteItem(item.get());
				}
				else
				{
					items.push_back(std::move(item));
				}
			}
			m_items = std::move(items);
		}

		if (!m_nameChangedPackages.empty())
		{
			for (const auto& [package, newPackageName] : m_nameChangedPackages)
			{
				gAssetTool.renamePackage(package, newPackageName);
			}
			m_nameChangedPackages.clear();
			setCurrentFolder(m_folder);
		}
	}

	void ContentWidget::onRendererBuild(RHICommandList* cmdList)
	{
		auto window = (EditorWindow*)getTopWidget();
		auto iconTexture = window->getEditorIconTexture();
		for (const auto& item : m_items)
		{
			item->getItemContext()->getThumbnail().texture = iconTexture;
			item->getItemContext()->getThumbnail().update();
		}
	}

	void ContentWidget::onRendererDestroy(RHICommandList* cmdList)
	{
		for (const auto& item : m_items)
		{
			item->getItemContext()->getThumbnail().texture = nullptr;
		}
	}

	bool ContentWidget::onDrop(DropEvent* event)
	{
		const auto cpath = fs::path(m_folder);

		std::vector<MaterialLoader> matLoaders;
		for (const auto& filepath : event->files)
		{
			V_LOG_INFO(Editor, "drop file: {}", filepath);
			auto path = fs::path(filepath);
			const auto ext = path.extension();
			if (ext == ".png" || ext == ".jpg" || ext == ".jpeg")
			{
				V_LOG_INFO(Editor, "convert image file");
				ImageLoader loader = ImageLoader(filepath);
				if (loader.load())
				{
					const auto name = path.stem().generic_u8string();
					const auto packageName = AssetTool::getDefaultPackageName(m_folder, name);
					auto package = std::make_shared<Package>(packageName);
					auto texture = std::make_shared<Texture2D>(loader.getTextureData());
					package->setObject(texture);
					if (AssetManager::getInstance().registry(package.get()))
					{
						gAssetTool.addDirtyAsset(packageName, texture);
					}
					V_LOG_INFO(Editor, "convert image success, {}", packageName);
				}
			}
			else if (ext == ".obj" || ext == ".fbx")
			{
				V_LOG_INFO(Editor, "convert mesh file");
				MeshLoader loader = MeshLoader(filepath);
				if (loader.load())
				{
					const auto& resources = loader.getMeshes();
					for (const auto& res : resources)
					{
						const auto packageName = AssetTool::getDefaultPackageName(m_folder, res.name);
						auto package = std::make_shared<Package>(packageName);
						package->setObject(res.mesh);
						if (AssetManager::getInstance().registry(package.get()))
						{
							gAssetTool.addDirtyAsset(packageName, res.mesh);
						}

						V_LOG_INFO(Editor, "convert mesh success, {}", packageName);
					}
				}
			}
			else if (ext == ".vert")
			{
				bool added = false;
				for (auto& loader : matLoaders)
				{
					added = loader.setVertexSource(filepath);
					if (added)
					{
						break;
					}
				}
				if (!added)
				{
					MaterialLoader loader;
					loader.setVertexSource(filepath);
					matLoaders.push_back(std::move(loader));
				}
			}
			else if (ext == ".frag")
			{
				bool added = false;
				for (auto& loader : matLoaders)
				{
					added = loader.setFragmentSource(filepath);
					if (added)
					{
						break;
					}
				}
				if (!added)
				{
					MaterialLoader loader;
					loader.setFragmentSource(filepath);
					matLoaders.push_back(std::move(loader));
				}
			}
		}

		for (auto& loader : matLoaders)
		{
			if (loader.load())
			{
				auto mat = loader.getMaterial();
				const auto packageName = AssetTool::getDefaultPackageName(m_folder, loader.getAssetName());
				auto package = std::make_shared<Package>(packageName);
				package->setObject(mat);
				if (AssetManager::getInstance().registry(package.get()))
				{
					gAssetTool.addDirtyAsset(packageName, mat);
				}
			}
		}
		return true;
	}

	void ContentWidget::onAssetCreated(const AssetInfo& assetInfo)
	{
		AssetPath path = AssetPath(assetInfo.data.path);
		if (path.path != m_folder)
		{
			return;
		}
		if (auto item = createAssetItem(assetInfo))
		{
			item->setDisplayName(path.name);
			item->setEditing(true);
			auto proxy = item.get();
			m_items.push_back(std::move(item));
			m_multiSelect = false;
			setSelectedItem(proxy);
		}
	}

	void ContentWidget::onAssetDirty(const AssetInfo& assetInfo)
	{
		AssetPath path = AssetPath(assetInfo.data.path);
		if (path.path != m_folder)
		{
			return;
		}

		for (auto& item : m_items)
		{
			if (item->getItemContext()->getAssetName() == path.name)
			{
				item->getItemContext()->setDirty(assetInfo.dirty);
				item->setDisplayName(item->getItemContext()->getDisplayName());
				return;
			}
		}
	}

	void ContentWidget::onAssetLoaded(Package* package)
	{
		AssetPath path = AssetPath(package->getAssetData().path);
		if (path.path != m_folder)
		{
			return;
		}

		for (auto& item : m_items)
		{
			if (item->getItemContext()->getAssetName() == path.name)
			{
				if (auto assetContext = dynamic_cast<AssetContext*>(item->getItemContext()))
				{
					auto assetInfo = assetContext->getAssetInfo();
					assetInfo.object = package->getAssetObject();
					assetContext->setAssetInfo(assetInfo);
				}
				item->setTextColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				return;
			}
		}
	}

	std::unique_ptr<ContentItemWidget> ContentWidget::createFolderItem(const std::string& fullpath)
	{
		std::shared_ptr<RHITexture2D> iconTexture = nullptr;
		if (auto window = dynamic_cast<EditorWindow*>(getTopWidget()))
		{
			iconTexture = window->getEditorIconTexture();
		}

		Thumbnail folderThumbnail;
		folderThumbnail.texture = iconTexture;
		folderThumbnail.pos = { 0, 0 };
		folderThumbnail.size = { 128, 128 };
		folderThumbnail.update();
		auto item = std::make_unique<ContentItemWidget>();
		auto context = std::make_unique<FolderContext>(this, item.get());
		context->setAssetPath(fullpath);
		context->setThumbnail(folderThumbnail);
		item->setContext(std::move(context));
		return item;
	}

	std::unique_ptr<ContentItemWidget> ContentWidget::createAssetItem(const AssetInfo& assetInfo)
	{
		Thumbnail thumbnail;
		thumbnail.size = { 128, 128 };

		std::unique_ptr<ContentItemWidget>  item = nullptr;

		if (assetInfo.data.className == "Material")
		{
			thumbnail.pos = { 1, 0 };
			item = createMaterialItem(assetInfo);
		}
		else if (assetInfo.data.className == "MaterialInstance")
		{
			thumbnail.pos = { 1, 0 };
			item = createMaterialInstanceItem(assetInfo);
		}
		else if (assetInfo.data.className == "Texture2D")
		{
			thumbnail.pos = { 1, 0 };
			item = createTexture2DItem(assetInfo);
		}
		else if (assetInfo.data.className == "StaticMesh")
		{
			thumbnail.pos = { 2, 0 };
			item = createStaticMeshItem(assetInfo);
		}
		else
		{
			return nullptr;
		}

		std::shared_ptr<RHITexture2D> iconTexture = nullptr;
		if (auto window = dynamic_cast<EditorWindow*>(getTopWidget()))
		{
			iconTexture = window->getEditorIconTexture();
		}
		thumbnail.texture = iconTexture;
		thumbnail.update();
		item->getItemContext()->getThumbnail() = thumbnail;

		if (assetInfo.object)
		{
			item->setTextColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		}
		else
		{
			item->setTextColor(glm::vec4(.6f, .6f, .6f, 1.0f));
		}

		return item;
	}

	std::unique_ptr<ContentItemWidget> ContentWidget::createMaterialItem(const AssetInfo& info)
	{
		auto item = std::make_unique<ContentItemWidget>();
		auto context = std::make_unique<MaterialContext>(this, item.get());
		context->setAssetInfo(info);
		item->setContext(std::move(context));
		return item;
	}

	std::unique_ptr<ContentItemWidget> ContentWidget::createMaterialInstanceItem(const AssetInfo& info)
	{
		auto item = std::make_unique<ContentItemWidget>();
		auto context = std::make_unique<MaterialInstanceContext>(this, item.get());
		context->setAssetInfo(info);
		item->setContext(std::move(context));
		return item;
	}

	std::unique_ptr<ContentItemWidget> ContentWidget::createTexture2DItem(const AssetInfo& assetInfo)
	{
		auto item = std::make_unique<ContentItemWidget>();
		auto context = std::make_unique<Texture2DContext>(this, item.get());
		context->setAssetInfo(assetInfo);
		item->setContext(std::move(context));
		return item;
	}

	std::unique_ptr<ContentItemWidget> ContentWidget::createStaticMeshItem(const AssetInfo& assetInfo)
	{
		auto item = std::make_unique<ContentItemWidget>();
		auto context = std::make_unique<AssetContext>(this, item.get());
		context->setAssetInfo(assetInfo);
		item->setContext(std::move(context));
		return item;
	}
}
