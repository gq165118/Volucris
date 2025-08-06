#include "EditorViewportWidget.h"
#include <Engine/Render/View.h>
#include <Engine/RHI/RHIUniformBuffer.h>

namespace volucris
{
	std::unique_ptr<BaseView> EditorViewportWidget::onCreateView(const std::shared_ptr<Scene>& scene)
	{
		return std::make_unique<View>(scene);
	}
}
