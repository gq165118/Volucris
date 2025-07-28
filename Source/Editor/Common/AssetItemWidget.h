#ifndef __volucris_asset_item_widget_h__
#define __volucris_asset_item_widget_h__

#include <Engine/Application/Widget.h>
#include <Engine/Game/SoftObject.h>
#include <Engine/Application/Event.h>

namespace volucris
{
	DECLARE_EVENT_DELEGATE(PackageEvent, void, AssetData)

	class AssetItemWidget : public Widget
	{
	public:
		PackageEvent SelectedChanged;

	public:
		AssetItemWidget(const std::string& name);

		void setAssetData(const AssetData& className);

		void onBuild(bool) override;
	
		const AssetData& getAssetData() const { return m_assetData; }

		bool isSelected() const { return m_selected; }

	private:
		bool m_selected;
		std::string m_name;
		AssetData m_assetData;
		std::vector<AssetData> m_assetDatas;
	};
}

#endif // !__volucris_asset_item_widget_h__
