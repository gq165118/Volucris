#include <ContentBrowser/ContentItemWidget.h>
#include <Engine/RHI/RHITexture.h>
#include <imgui_internal.h>
#include <filesystem>

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
		: m_texture(nullptr)
		, m_minUV()
		, m_maxUV()
		, m_size(ItemSize)
		, m_iconSpace()
		, m_iconDrawSize()
		, m_hoverColor({ 0.9411, 0.850, 0.559 ,1.0})
		, m_selectedColor({ 0.882, 0.725, 0.219 ,1.0 })
		, m_selected(false)
		, m_node()
		, m_iconPos()
		, m_iconSize()
		, m_text()
		, m_clicked(false)
		, m_editing(false)
		, m_deleteSelected(false)
		, m_selectable(true)
	{
		setScale(1.0);
	}

	ContentItemWidget::ContentItemWidget(const FileNode& node)
		: ContentItemWidget()
	{
		setFileNode(node);
	}

	ContentItemWidget::ContentItemWidget(RHITexture2D* texture, Point iconPos, Size iconSize)
		: ContentItemWidget()
	{
		setIcon(iconPos, iconSize);
		setTexture(texture);
	}

	void ContentItemWidget::setFileNode(const FileNode& node)
	{
		m_node = node;
		setDisplayName(fs::path(m_node.path).stem().generic_string());
	}

	void ContentItemWidget::setTexture(RHITexture2D* texture)
	{
		m_texture = texture;
		update();
	}

	void ContentItemWidget::setIcon(Point iconPos, Size iconSize)
	{
		m_iconPos = iconPos;
		m_iconSize = iconSize;

		update();
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

	void ContentItemWidget::update()
	{
		if (!m_texture)
		{
			return;
		}

		auto texSize = m_texture->getSize();
		auto ustep = m_iconSize.width * 1.0f / texSize.width;
		auto vstep = m_iconSize.height * 1.0f / texSize.height;

		m_minUV = { m_iconPos.x * ustep, 1.0f - m_iconPos.y * vstep };
		m_maxUV = { m_minUV.x + ustep, m_minUV.y - vstep };
	}

	void ContentItemWidget::build()
	{
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
					DoubleClicked.invoke(this);
				}
				else
				{
					m_timer.start();
					Clicked.invoke(this);
				}
			}
			else
			{
				m_timer.start();
				Clicked.invoke(this);
			}
		}
		else if (m_selectable && ImGui::BeginPopupContextItem())
		{
			m_selected = true;
			Clicked.invoke(this);
			if (ImGui::MenuItem("Rename"))
			{
				m_editing = true;
			}
			if (ImGui::MenuItem("Delete"))
			{
				m_deleteSelected = true;
			}

			if (m_assetData.className == "Material")
			{
				ImGui::SeparatorText("Material");
				if (ImGui::MenuItem("Reload"))
				{
					ReloadMaterial.invoke(SoftObject<MaterialTemplate>(m_assetData.path));
				}
				if (ImGui::MenuItem("Create Instance"))
				{
					CreateInstance.invoke(SoftObject<MaterialTemplate>(m_assetData.path));
				}
			}

			ImGui::EndPopup();
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

		if (m_texture)
		{
			auto id = m_texture->getId();
			ImTextureID texID = (ImTextureID)(intptr_t)id;

			window->DrawList->AddImage(texID,
				iconRectMin,
				iconRectMax,
				{ m_minUV.x, m_minUV.y },
				{ m_maxUV.x, m_maxUV.y }
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
				if (name.empty())
				{
					setDisplayName(fs::path(m_node.path).stem().generic_string());
				}
				else
				{
					FileNode node = m_node;
					node.path = (fs::path(m_node.path).parent_path() / name).generic_u8string();
					NodeNameChanged.invoke(this, node);
				}
				m_editing = false;
			}
			ImGui::EndChild();
		}
		else
		{
			DrawTextCenteredInRect(fontRectMin, fontRectMax, m_fontSize, m_text);
		}
	}
	glm::vec2 ContentItemWidget::getItemSize(float scale)
	{
		return ItemSize * scale;
	}

	void ContentItemWidget::setDisplayName(const std::string& name)
	{
		const auto size = name.length() < 64 ? name.length() : 63;
		memcpy(m_text, name.c_str(), size);
		m_text[size] = '\0';
	}
}