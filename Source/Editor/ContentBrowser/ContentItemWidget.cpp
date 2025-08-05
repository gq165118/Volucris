#include <ContentBrowser/ContentItemWidget.h>
#include <Engine/RHI/RHITexture.h>
#include <imgui_internal.h>
#include <filesystem>
#include "AssetMenuContext.h"

namespace fs = std::filesystem;

namespace volucris
{
	void DrawTextCenteredInRect(const ImVec2& rect_min, const ImVec2& rect_max, float fontSize, const char* text, ImVec4 color = ImVec4(1, 1, 1, 1))
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImFont* font = ImGui::GetFont();

		// 1. 计算文字尺寸
		ImVec2 text_size = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, text);

		// 2. 计算矩形尺寸
		float rect_width = rect_max.x - rect_min.x;
		float rect_height = rect_max.y - rect_min.y;

		bool text_fits = (text_size.x <= rect_width);

		// 设置裁剪区域（防止文字溢出）
		draw_list->PushClipRect(rect_min, rect_max, true);

		// 计算垂直居中位置
		float y_pos = rect_min.y + (rect_height - text_size.y) * 0.5f;

		if (text_fits) {
			// 文字未超出 - 水平居中
			float x_pos = rect_min.x + (rect_width - text_size.x) * 0.5f;
			draw_list->AddText(font, fontSize, ImVec2(x_pos, y_pos), ImGui::ColorConvertFloat4ToU32(color), text);
		}
		else {
			// 文字超出 - 左对齐并截断
			draw_list->AddText(font, fontSize, ImVec2(rect_min.x, y_pos), ImGui::ColorConvertFloat4ToU32(color), text);
		}

		// 恢复裁剪区域
		draw_list->PopClipRect();
	}

	static glm::vec2 ItemSize = { 96,128 };

	ContentItemWidget::ContentItemWidget()
		: m_size(ItemSize)
		, m_iconSpace()
		, m_iconDrawSize()
		, m_hoverColor({ 0.9411, 0.850, 0.559 ,1.0 })
		, m_selectedColor({ 0.882, 0.725, 0.219 ,1.0 })
		, m_selected(false)
		, m_text()
		, m_clicked(false)
		, m_editing(false)
		, m_deleteSelected(false)
		, m_selectable(true)
		, m_asset(nullptr)
		, m_textColor(1.0,1.0,1.0,1.0)
	{
		setScale(1.0);
	}

	void ContentItemWidget::setContext(std::unique_ptr<ItemContext> asset)
	{
		m_asset = std::move(asset);
		m_asset->buildMenuCountextGroup();
		setDisplayName(m_asset->getDisplayName());
	}

	void ContentItemWidget::setScale(float scale)
	{
		m_size = ItemSize * scale;
		float iconSize = 88 * scale;
		m_iconDrawSize = { iconSize , iconSize };
		m_iconSpace.x = 4 * scale;
		m_iconSpace.y = 4 * scale;

		m_fontSize = 16 * scale;
	}

	void ContentItemWidget::build()
	{
		if (!m_asset) 			return;
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return;

		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		ImVec2 iconRectMin = { cursorPos.x + m_iconSpace.x, cursorPos.y + m_iconSpace.y };
		ImVec2 iconRectMax = ImVec2(iconRectMin.x + m_iconDrawSize.x, iconRectMin.y + m_iconDrawSize.y);
		ImVec2 fontRectMin = { cursorPos.x + m_iconSpace.x, iconRectMax.y + m_iconSpace.y };
		ImVec2 fontRectMax = { cursorPos.x + m_size.x - m_iconSpace.x, cursorPos.y + m_size.y };

		const ImVec2 size = ImVec2(m_size.x, m_size.y);

		ImGui::InvisibleButton("##xx", size);
		bool click = ImGui::IsItemClicked();
		bool hovered = ImGui::IsItemHovered();
		
		if (click)
		{
			m_selected = true;
			if (m_timer.isRunning())
			{
				m_timer.stop();
				auto duration = m_timer.getDuration();
				if (duration < 0.5)
				{
					m_asset->doubleClicked();
				}
				else
				{
					m_timer.start();
					m_asset->clicked();
				}
			}
			else
			{
				m_timer.start();
				m_asset->clicked();
			}
		}
		else if (m_selectable && m_asset->buildMenuContext())
		{
			m_selected = true;
			m_asset->clicked();
		}

		if (hovered)
		{
			window->DrawList->AddRectFilled(
				cursorPos, { cursorPos.x + m_size.x, cursorPos.y + m_size.y },
				IM_COL32(82, 82, 82, 255)
			);
		}
		else if (isSelected())
		{
			window->DrawList->AddRectFilled(
				cursorPos, { cursorPos.x + m_size.x, cursorPos.y + m_size.y },
				IM_COL32(49,49,49, 255)
			);
		}

		const auto& thumbnail = m_asset->getThumbnail();
		if (thumbnail.isValid())
		{
			auto id = thumbnail.texture->getId();
			ImTextureID texID = (ImTextureID)(intptr_t)id;

			window->DrawList->AddImage(texID,
				iconRectMin,
				iconRectMax,
				{ thumbnail.minUV.x, thumbnail.minUV.y },
				{ thumbnail.maxUV.x, thumbnail.maxUV.y }
			);
		}

		if (m_editing)
		{
			ImGui::SetNextWindowPos(fontRectMin);
			ImGui::SetNextWindowSize({fontRectMax.x, fontRectMax.y - fontRectMin.y});
			ImGui::BeginChild("input_container", ImVec2(200, 30),
				false,
				ImGuiWindowFlags_NoBackground |
				ImGuiWindowFlags_NoDecoration);
			ImGui::SetNextItemWidth(fontRectMax.x);
			ImGui::SetKeyboardFocusHere();
			if (ImGui::InputText("##NameEdit", m_text, 64, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (!ImGui::IsItemActive()) {
					ImGui::ActivateItemByID(ImGui::GetItemID());
				}
				std::string name = m_text;
				if (!name.empty())
				{
					m_asset->rename(name);
				}
				setDisplayName(m_asset->getDisplayName());
				m_editing = false;
			}
			ImGui::EndChild();
		}
		else
		{
			DrawTextCenteredInRect(fontRectMin, fontRectMax, m_fontSize, m_text, { m_textColor.r, m_textColor.g, m_textColor.b, m_textColor.a});
		}
	}


	glm::vec2 ContentItemWidget::getItemSize(float scale)
	{
		return ItemSize * scale;
	}

	ItemContext* ContentItemWidget::getItemContext() const
	{
		return m_asset.get();
	}

	void ContentItemWidget::setEditing(bool editing)
	{
		m_editing = editing;
		setDisplayName(m_editing ? m_asset->getAssetName() : m_asset->getDisplayName());
	}

	void ContentItemWidget::setDisplayName(const std::string& name)
	{
		const auto size = name.length() < 64 ? name.length() : 63;
		memcpy(m_text, name.c_str(), size);
		m_text[size] = '\0';
	}
}