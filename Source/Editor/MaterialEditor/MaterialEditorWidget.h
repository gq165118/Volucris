#ifndef __volucris_material_editor_widget_h__
#define __volucris_material_editor_widget_h__

#include <Engine/Application/Widget.h>

namespace volucris
{
	class MaterialTemplate;
	class GameWorld;
	class ViewportWidget;
	class MaterialParameterWidget;

	class MaterialEditorWidget : public Widget
	{
	public:
		MaterialEditorWidget();

		void setMaterial(const std::shared_ptr<MaterialTemplate>& material);

	protected:
		void onBuild(bool init) override;

		void onWindowFocusChanged(FocusEvent* event) override;

	private:
		std::shared_ptr<ViewportWidget> m_viewport;
		std::shared_ptr<GameWorld> m_world;
		std::shared_ptr<MaterialTemplate> m_material;
		std::shared_ptr<MaterialParameterWidget> m_parameterWidget;
	};
}

#endif // !__volucris_material_editor_widget_h__
