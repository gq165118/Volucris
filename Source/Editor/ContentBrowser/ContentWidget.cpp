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

namespace fs = std::filesystem;

namespace volucris
{
	static std::string getDefaultPackageName(const fs::path& dirpath, const std::string& name)
	{
		std::string packageName = (dirpath / name).generic_u8string();
		std::string assetName = fmt::format("{}.asset", packageName);
		if (gFileSystem.fileExists(assetName) || AssetManager::getInstance().isPackageRegistered(packageName))
		{
			for (size_t i = 1; i < std::numeric_limits<size_t>::max(); ++i)
			{
				packageName = (dirpath / fmt::format("{}_{}", name, i)).generic_u8string();
				assetName = fmt::format("{}.asset", packageName);
				if (!gFileSystem.fileExists(assetName) && !AssetManager::getInstance().isPackageRegistered(packageName))
				{
					break;
				}
			}
		}
		return packageName;
	}

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
		, m_controlItem(nullptr)
	{
		setCurrentFolder(u8"/Engine/Content/Editor");
	}

	void ContentWidget::setCurrentFolder(const std::string& folder)
	{
		m_folder = folder;
		m_controlItem = nullptr;

		const Icon folderIcon = { { 0, 0 }, { 128,128 } };
		m_items.clear();
		{
			auto parentNode = gFileSystem.parentNode(folder);
			if (!parentNode.path.empty())
			{
				auto item = createFolderItem(parentNode.path, "..");
				item->setSelectable(false);
				m_items.emplace_back(std::move(item));
			}
		}

		auto nodes = gFileSystem.getFileNodes(folder);
		for (const auto& node : nodes)
		{
			if (node.type == EFileType::Directory)
			{
				m_items.emplace_back(createFolderItem(node.path));
			}
		}

		const Icon textureIcon = { { 1, 0 }, { 128,128 } };
		for (const auto& node : nodes)
		{
			if (node.type != EFileType::Directory)
			{
				auto assetData = AssetManager::getInstance().loadAssetData(node.path);
				if (!assetData.path.empty())
				{
					std::unique_ptr<ContentItemWidget> item = nullptr;
					if (assetData.className == "Texture2D")
					{
						item = createTextureItem(node.path);
					}
					else if (assetData.className == "StaticMesh")
					{
						item = createStaticMeshItem(node.path);
					}
					else if (assetData.className == "Material")
					{
						item = createTextureItem(node.path);
					}

					if (item)
					{
						item->setAssetData(assetData);
						m_items.emplace_back(std::move(item));
					}
				}
			}
		}
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

		if (m_controlItem)
		{
			auto node = m_controlItem->getFileNode();
			if (node.type == EFileType::Directory)
			{
				setCurrentFolder(node.path);
			}
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
					deleteItem(item.get());
				}
				else
				{
					items.push_back(std::move(item));
				}
			}
			m_items = std::move(items);
		}
	}

	void ContentWidget::onRendererBuild(RHICommandList* cmdList)
	{
		auto window = (EditorWindow*)getTopWidget();
		auto iconTexture = window->getEditorIconTexture();
		for (const auto& item : m_items)
		{
			item->setTexture(iconTexture);
		}
	}

	void ContentWidget::onRendererDestroy(RHICommandList* cmdList)
	{
		for (const auto& item : m_items)
		{
			item->setTexture(nullptr);
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
					const auto packageName = getDefaultPackageName(cpath, name);
					auto package = std::make_shared<Package>(packageName);
					auto texture = std::make_shared<Texture2D>(loader.getTextureData());
					package->setObject(texture);
					if (AssetManager::getInstance().registry(package.get()))
					{
						AssetManager::getInstance().save(package.get());
						GEditorWorld->addObject(texture);
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
						const auto packageName = getDefaultPackageName(cpath, res.name);
						auto package = std::make_shared<Package>(packageName);
						package->setObject(res.mesh);
						if (AssetManager::getInstance().registry(package.get()))
						{
							AssetManager::getInstance().save(package.get());
							GEditorWorld->addObject(res.mesh);
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
				const auto packageName = getDefaultPackageName(cpath, loader.getAssetName());
				auto package = std::make_shared<Package>(packageName);
				package->setObject(mat);
				if (AssetManager::getInstance().registry(package.get()))
				{
					AssetManager::getInstance().save(package.get());
					GEditorWorld->addObject(mat);
				}
			}
		}
		return true;
	}

	void ContentWidget::onAssetRegistered(Package* package)
	{
		if (package->getAssetData().path.compare(0, m_folder.length(), m_folder) == 0)
		{
			
		}
	}

	std::unique_ptr<ContentItemWidget> ContentWidget::createItem(const FileNode& node, const Icon& icon, const std::string& name)
	{
		RHITexture2D* iconTexture = nullptr;
		if (auto window = dynamic_cast<EditorWindow*>(getTopWidget()))
		{
			iconTexture = window->getEditorIconTexture();
		}

		auto item = std::make_unique<ContentItemWidget>(node);
		item->setIcon(icon.pos, icon.size);
		item->setScale(m_scale);
		item->setTexture(iconTexture);
		if (!name.empty())
		{
			item->setDisplayName(name);
		}

		item->Clicked.bind([this](ContentItemWidget* clicked) {
			if (!m_multiSelect)
			{
				for (auto& item : m_items)
				{
					if (item.get() != clicked)
					{
						item->setSelected(false);
					}
				}
			}
			});

		item->DoubleClicked.bind([this](ContentItemWidget* clicked) {
			m_controlItem = clicked;
			((EditorApplication*)gApp)->openEditor(m_controlItem->getAssetData());
			});

		item->NodeNameChanged.bind([this](ContentItemWidget* item, const FileNode& node) {
			if (node.type == EFileType::Directory)
			{
				if (gFileSystem.directoryExists(item->getFileNode().path))
				{
					gFileSystem.renameDirectory(item->getFileNode().path, node.path);
				}
				else
				{
					gFileSystem.createDirectory(node.path);
				}
				item->setFileNode(node);
			}
			else
			{
				
			}
			});
		return item;
	}

	std::unique_ptr<ContentItemWidget> ContentWidget::createFolderItem(const std::string& path, const std::string& name)
	{
		FileNode node;
		node.path = path;
		node.type = EFileType::Directory;
		Icon icon = { {0,0}, {128,128} };
		return createItem(node, icon, name);
	}

	std::unique_ptr<ContentItemWidget> ContentWidget::createStaticMeshItem(const std::string& path)
	{
		FileNode node;
		node.path = path;
		node.type = EFileType::Asset;
		Icon icon = { {2,0}, {128,128} };
		return createItem(node, icon);
	}

	void ContentWidget::deleteItem(ContentItemWidget* item)
	{
		if (item->getFileNode().type == EFileType::Directory)
		{
			gFileSystem.deleteDirectory(item->getFileNode().path);
		}
		else
		{
			AssetManager::getInstance().unregister(item->getFileNode().path);
			gFileSystem.deleteAsset(item->getFileNode().path);
		}
	}

	std::unique_ptr<ContentItemWidget> ContentWidget::createTextureItem(const std::string& path)
	{
		FileNode node;
		node.path = path;
		node.type = EFileType::Asset;
		Icon icon = { {1,0}, {128,128} };
		return createItem(node, icon);
	}
}
