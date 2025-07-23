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

namespace volucris
{
	class RHITexture2D;
	class ContentItemWidget;

	DECLARE_EVENT_DELEGATE(ItemEvent, void, ContentItemWidget*)

	class ContentItemWidget
	{
	public:
		ItemEvent Clicked;
		ItemEvent DoubleClicked;
		ItemEvent ContextMenuTriggered;

	public:
		ContentItemWidget();

		ContentItemWidget(const FileNode& node);

		ContentItemWidget(RHITexture2D* texture, Point iconPos, Size iconSize);

		void setAssetData(const AssetData& asset)
		{
			m_assetData = asset;
		}

		const AssetData& getAssetData() const
		{
			return m_assetData;
		}

		void setTexture(RHITexture2D* texture);

		void setIcon(Point iconPos, Size iconSize);

		void setScale(float scale);

		void build();

		bool isClicked() const { return m_clicked; }

		bool isSelected() const { return m_selected; }

		static glm::vec2 getItemSize(float scale = 1.0);

		const FileNode& getFileNode() const { return m_node; }

		void setSelected(bool selected) { m_selected = selected; }

		void setClicked(bool clicked) { m_clicked = clicked; }

		void setDisplayName(const std::string& name);

		void enableNameEdit(bool editing) { m_editing = editing; }

	private:
		void update();

	private:
		RHITexture2D* m_texture;
		glm::vec2 m_minUV;
		glm::vec2 m_maxUV;
		glm::vec2 m_size;
		glm::vec2 m_iconSpace;
		glm::vec2 m_iconDrawSize;
		float m_fontSize;
		glm::vec2 m_fontRect;
		glm::vec4 m_hoverColor;
		glm::vec4 m_selectedColor;
		bool m_selected;
		FileNode m_node;
		Point m_iconPos;
		Size m_iconSize;
		char m_text[64];
		Timer m_timer;
		bool m_clicked;
		bool m_editing;
		AssetData m_assetData;
	};
}

#endif // !__volucris_content_item_widget_h__
