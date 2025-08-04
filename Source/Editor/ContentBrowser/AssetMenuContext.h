#ifndef __volucris_asset_menu_context_h__
#define __volucris_asset_menu_context_h__

#include <Common/AssetTool.h>
#include <Engine/Asset/AssetPath.h>
#include "MenuContextCommands.h"
#include <Common/Thumbnail.h>

namespace volucris
{
	class ContentWidget;
	class ContentItemWidget;

	struct MenuContextItem
	{
		std::string name;
		std::unique_ptr<MenuContextCommand> command;
	};

	struct MenuContextGroup
	{
		std::string name;
		std::vector<MenuContextItem> items;
	};

	class ItemContext
	{
	public:
		ItemContext(ContentWidget* contentWidget, ContentItemWidget* itemWidget)
			: m_dirty(false)
			, m_assetPath("")
			, m_displayName()
			, m_thumbnail()
			, m_menuGroups()
			, m_contentWidget(contentWidget)
			, m_itemWidget(itemWidget)
			, m_executionCommand(nullptr)
		{
		}

		virtual ~ItemContext() = default;

		const std::string& getAssetPath() const
		{
			return m_assetPath.path;
		}

		const std::string& getAssetName() const
		{
			return m_assetPath.name;
		}

		const std::string& getDisplayName() const
		{
			return m_displayName;
		}

		const std::string& getFullPath() const { return m_assetPath.fullpath; }

		void setDirty(bool dirty)
		{
			m_dirty = dirty;
			updateDisplayName();
			clearMenuContextGroups();
			buildMenuCountextGroup();
		}

		virtual void clicked();

		virtual void doubleClicked() {}

		void setAssetPath(AssetPath assetPath)
		{
			m_assetPath = std::move(assetPath);
			updateDisplayName();
		}

		ContentWidget* getContentWidget() const
		{
			return m_contentWidget;
		}

		ContentItemWidget* getItemWidget() const
		{
			return m_itemWidget;
		}

		void setThumbnail(const Thumbnail& thumbnail)
		{
			m_thumbnail = thumbnail;
		}

		Thumbnail& getThumbnail()
		{
			return m_thumbnail;
		}

		void setDisplayName(const std::string& name)
		{
			m_displayName = name;
			m_dirty = false;
		}

		virtual void rename(const std::string& newName) = 0;

		bool buildMenuContext();

		void addMenuContextGroup(MenuContextGroup group)
		{
			m_menuGroups.push_back(std::move(group));
		}

		bool isDirty() const
		{
			return m_dirty;
		}

		bool shouldExecuteCommmand() const
		{
			return m_executionCommand != nullptr;
		}

		void execute();

	protected:
		MenuContextGroup* getMenuContextGroup(size_t index)
		{
			if (index < m_menuGroups.size())
			{
				return &m_menuGroups[index];
			}
			return nullptr;
		}

		void clearMenuContextGroups()
		{
			m_menuGroups.clear();
		}

	private:
		void updateDisplayName()
		{
			m_displayName = m_dirty ? fmt::format("*{}", m_assetPath.name) : m_assetPath.name;
		}

		virtual void buildMenuCountextGroup() {}

	private:
		bool m_dirty;
		AssetPath m_assetPath;
		std::string m_displayName;
		Thumbnail m_thumbnail;
		std::vector<MenuContextGroup> m_menuGroups;
		MenuContextCommand* m_executionCommand;

	protected:
		ContentWidget* m_contentWidget;
		ContentItemWidget* m_itemWidget;
	};

	class FolderContext : public ItemContext
	{
	public:
		FolderContext(ContentWidget* contentWidget, ContentItemWidget* itemWidget);

		void rename(const std::string& newName) override;

		void doubleClicked() override;

	private:
		void buildMenuCountextGroup() override;
	};

	

	class AssetContext : public ItemContext
	{
	public:
		AssetContext(ContentWidget* contentWidget, ContentItemWidget* itemWidget)
			: ItemContext(contentWidget, itemWidget)
			, m_assetInfo()
		{
		}

		void setAssetInfo(const AssetInfo& info)
		{
			m_assetInfo = info;
			setAssetPath(m_assetInfo.data.path);
			setDirty(m_assetInfo.dirty);
			clearMenuContextGroups();
			buildMenuCountextGroup();
		}

		const AssetInfo& getAssetInfo() const
		{
			return m_assetInfo;
		}

		void rename(const std::string& newName) override;

	protected:
		void buildMenuCountextGroup() override;

	private:
		AssetInfo m_assetInfo;
	};

	class MaterialContext : public AssetContext
	{
	public:
		MaterialContext(ContentWidget* contentWidget, ContentItemWidget* itemWidget)
			: AssetContext(contentWidget, itemWidget)
		{
			
		}

	protected:
		void buildMenuCountextGroup() override;
	};

	class MaterialInstanceContext : public AssetContext
	{
	public:
		MaterialInstanceContext(ContentWidget* contentWidget, ContentItemWidget* itemWidget)
			: AssetContext(contentWidget, itemWidget)
		{
			
		}

	protected:
		void buildMenuCountextGroup() override;
	};

	class Texture2DContext : public AssetContext
	{
	public:
		Texture2DContext(ContentWidget* contentWidget, ContentItemWidget* itemWidget)
			: AssetContext(contentWidget, itemWidget)
		{
			
		}

	protected:
		void buildMenuCountextGroup() override;
	};
}

#endif // !__volucris_asset_menu_context_h__
