#include "AssetMenuContext.h"
#include "ContentWidget.h"
#include <imgui_internal.h>
#include <Engine/Game/Package.h>

namespace volucris
{
	void ItemContext::clicked()
	{
		m_contentWidget->setSelectedItem(m_itemWidget);
	}

	bool ItemContext::buildMenuContext()
	{
		if (ImGui::BeginPopupContextItem())
		{
			for (const auto& group : m_menuGroups)
			{
				if (!group.name.empty())
				{
					ImGui::TextUnformatted(group.name.c_str());
					ImGui::Separator();
				}
				for (const auto& item : group.items)
				{
					if (ImGui::MenuItem(item.name.c_str()))
					{
						m_executionCommand = item.command.get();
					}
				}
			}
			ImGui::EndPopup();
			return true;
		}
		return false;
	}

	void ItemContext::execute()
	{
		m_executionCommand->execute();
		m_executionCommand = nullptr;
	}

	FolderContext::FolderContext(ContentWidget* contentWidget, ContentItemWidget* itemWidget)
		: ItemContext(contentWidget, itemWidget)
	{
		
	}

	void FolderContext::rename(const std::string& newName)
	{
		
	}

	void FolderContext::doubleClicked()
	{
		m_contentWidget->dirtyCurrentFolder(getFullPath());
	}

	void FolderContext::buildMenuCountextGroup()
	{
		{
			MenuContextGroup group;
			group.name = "";
			{
				MenuContextItem item;
				item.name = "Rename";
				item.command = std::make_unique<RenameCommand>(this);
				group.items.push_back(std::move(item));
			}
			{
				MenuContextItem item;
				item.name = "Delete";
				item.command = std::make_unique<DeleteFolderCommand>(this);
				group.items.push_back(std::move(item));
			}
			addMenuContextGroup(std::move(group));
		}
	}

	void AssetContext::rename(const std::string& newName)
	{
		if (!newName.empty() && newName != getAssetName())
		{
			auto package = std::make_shared<Package>();
			package->setObject(SoftObject<GameObject>(m_assetInfo.data.path).tryLoad());
			package->setAssetData(m_assetInfo.data);
			m_contentWidget->addNameChangedPackageName(package, AssetPath(getAssetPath(), newName).fullpath);
		}
	}

	void AssetContext::buildMenuCountextGroup()
	{
		MenuContextGroup group;
		{
			MenuContextItem item;
			item.name = "Save";
			item.command = std::make_unique<SaveAssetCommand>(this);
			group.items.push_back(std::move(item));
		}
		{
			MenuContextItem item;
			item.name = "Rename";
			item.command = std::make_unique<RenameCommand>(this);
			group.items.push_back(std::move(item));
		}
		{
			MenuContextItem item;
			item.name = "Delete";
			item.command = std::make_unique<DeleteAssetCommand>(this);
			group.items.push_back(std::move(item));
		}
		addMenuContextGroup(std::move(group));
	}

	void MaterialContext::buildMenuCountextGroup()
	{
		AssetContext::buildMenuCountextGroup();
		{
			{
				auto group = getMenuContextGroup(0);
				MenuContextItem item;
				item.name = "Create Instance";
				item.command = std::make_unique<CreateMaterialInstanceCommand>(this);
				group->items.insert(group->items.begin(), std::move(item));
			}
			MenuContextGroup group;
			group.name = "Material";
			{
				MenuContextItem item;
				item.name = "Reload From Source";
				item.command = std::make_unique<DeleteAssetCommand>(this);
				group.items.push_back(std::move(item));
			}
			addMenuContextGroup(std::move(group));
		}
	}
	void MaterialInstanceContext::buildMenuCountextGroup()
	{
		AssetContext::buildMenuCountextGroup();
		{
			auto group = getMenuContextGroup(0);
			{
				MenuContextItem item;
				item.name = "Open Editor";
				item.command = std::make_unique<OpenMaterialInstanceEditorCommand>(this);
				group->items.insert(group->items.begin(), std::move(item));
			}

			{
				MenuContextItem item;
				item.name = "Create Instance";
				item.command = std::make_unique<CreateMaterialInstanceCommand>(this);
				group->items.insert(group->items.begin(), std::move(item));
			}
		}
	}

	void Texture2DContext::buildMenuCountextGroup()
	{
		AssetContext::buildMenuCountextGroup();
	}
}