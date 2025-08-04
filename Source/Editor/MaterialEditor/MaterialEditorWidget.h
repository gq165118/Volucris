#ifndef __volucris_material_editor_widget_h__
#define __volucris_material_editor_widget_h__

#include <Engine/Application/Widget.h>
#include <Engine/Game/SoftObject.h>
#include <Engine/Game/MaterialInstance.h>

namespace volucris
{
	class MaterialInstance;
	class GameWorld;
	class ViewportWidget;
	class MaterialParameterWidget;
	class StaticMeshComponent;

	class MaterialEditorWidget : public Widget
	{
	public:
		MaterialEditorWidget();

		~MaterialEditorWidget() override;

		void setMaterial(const SoftObject<MaterialInstance>& material);

	protected:
		void onBuild(bool init) override;

		void onWindowFocusChanged(FocusEvent* event) override;

	private:
		std::shared_ptr<ViewportWidget> m_viewport;
		std::shared_ptr<GameWorld> m_world;
		SoftObject<MaterialInstance> m_material;
		std::shared_ptr<StaticMeshComponent> m_meshComponent;
		std::shared_ptr<MaterialParameterWidget> m_parameterWidget;
	};
}

#endif // !__volucris_material_editor_widget_h__
