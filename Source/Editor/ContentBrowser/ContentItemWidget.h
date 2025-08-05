#ifndef __volucris_content_item_widget_h__
#define __volucris_content_item_widget_h__

#include <Engine/Application/Widget.h>
#include <Engine/Core/Size.h>
#include <Engine/Core/Point.h>
#include <glm/glm.hpp>
#include <Engine/FileSystem/FileSystem.h>
#include <Engine/Core/Timer.h>
#include <Engine/Core/Delegate.h>
#include <Engine/Asset/AssetData.h>
#include <Engine/Game/SoftObject.h>
#include <MaterialEditor/MaterialTemplate.h>
#include <ContentBrowser/AssetMenuContext.h>

namespace volucris
{
	class ContentItemWidget;

	class ContentItemWidget
	{
	public:
		ContentItemWidget();

		void setContext(std::unique_ptr<ItemContext> asset);

		void setScale(float scale);

		void build();

		bool isClicked() const { return m_clicked; }

		bool isSelected() const { return m_selectable && m_selected; }

		static glm::vec2 getItemSize(float scale = 1.0);

		void setSelected(bool selected) { m_selected = selected; }

		void setSelectable(bool selectable) { m_selectable = selectable; }

		void setClicked(bool clicked) { m_clicked = clicked; }

		void setEditing(bool editing);

		void setDisplayName(const std::string& name);

		void enableNameEdit(bool editing) { m_editing = editing; }

		bool isDeleteSelected() const { return m_deleteSelected; }

		void setTextColor(const glm::vec4& color) { m_textColor = color; }

		ItemContext* getItemContext() const;

		bool shouldExecuteCommmand() const { return m_asset->shouldExecuteCommmand(); }

		void executeMenuCommand() { m_asset->execute(); }

	private:
		glm::vec2 m_size;
		glm::vec2 m_iconSpace;
		glm::vec2 m_iconDrawSize;
		float m_fontSize;
		glm::vec2 m_fontRect;
		glm::vec4 m_hoverColor;
		glm::vec4 m_selectedColor;
		bool m_selected;
		char m_text[64];
		Timer m_timer;
		bool m_clicked;
		bool m_editing;
		bool m_deleteSelected;
		bool m_selectable;
		std::unique_ptr<ItemContext> m_asset;
		glm::vec4 m_textColor;
	};
}

#endif // !__volucris_content_item_widget_h__
