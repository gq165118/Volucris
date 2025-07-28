#include "AssetItemWidget.h"
#include <imgui.h>
#include <imgui_internal.h>

namespace volucris
{
    ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
    {
        return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
	}

    ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)
    {
        return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y);
    }

	AssetItemWidget::AssetItemWidget(const std::string& name)
		: Widget()
		, m_selected(false)
        , m_name(name)
		, m_assetData()
		, m_assetDatas()
	{

	}

	void AssetItemWidget::setAssetData(const AssetData& className)
	{
		m_assetData = className;
	}

	void AssetItemWidget::onBuild(bool)
	{
		ImGui::PushID(m_name.c_str());
		if (ImGui::BeginCombo("##options", m_assetData.path.c_str())) {
			m_assetDatas = AssetManager::getInstance().getAssets(m_assetData.className);
			for (auto i = 0; i < m_assetDatas.size(); ++i)
			{
				const auto& asset = m_assetDatas[i];
				bool isSelected = m_assetData.path == asset.path;
				if (ImGui::Selectable(asset.path.c_str(), isSelected))
				{
					m_assetData = asset;
					m_selected = true;
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::PopID();
	}


}

