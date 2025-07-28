#ifndef __volucris_material_editor_widget_h__
#define __volucris_material_editor_widget_h__

#include <Engine/Application/Widget.h>

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

		void setMaterial(const std::shared_ptr<MaterialInstance>& material);

	protected:
		void onBuild(bool init) override;

		void onWindowFocusChanged(FocusEvent* event) override;

	private:
		std::shared_ptr<ViewportWidget> m_viewport;
		std::shared_ptr<GameWorld> m_world;
		std::shared_ptr<MaterialInstance> m_material;
		std::shared_ptr<StaticMeshComponent> m_meshComponent;
		std::shared_ptr<MaterialParameterWidget> m_parameterWidget;
	};
}

#endif // !__volucris_material_editor_widget_h__
