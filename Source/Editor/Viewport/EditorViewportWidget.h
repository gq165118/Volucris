#ifndef __volucris_editor_viewport_widget_h__
#define __volucris_editor_viewport_widget_h__

#include "ViewportWidget.h"

namespace volucris
{
	class EditorViewportWidget : public ViewportWidget
	{
	public:

	protected:
		std::unique_ptr<BaseView> onCreateView(const std::shared_ptr<Scene>& scene) override;
	};
}

#endif // !__volucris_editor_viewport_widget_h__
