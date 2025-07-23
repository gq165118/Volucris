#ifndef __volucris_viewport_widget_h__
#define __volucris_viewport_widget_h__

#include <Engine/Application/Widget.h>
#include <Engine/Core/TextureDefines.h>

namespace volucris
{
	class View;
	class GameWorld;
	class Window;
	class RHITexture2D;
	class RHICommandList;
	class RHIWritePixelBuffer;
	class Material;

	class ViewportWidget : public Widget
	{
	public:
		ViewportWidget();

		void setWorld(const std::shared_ptr<GameWorld>& world);

		void setViewData(Texture::TextureData data);

		void setTestMaterial(const std::shared_ptr<Material>& material);

	protected:
		void onBuild(bool init) override;

		void viewSizeChanged(Size size);

		void onRendererBuild(RHICommandList* cmdList) override;

		void onRendererDestroy(RHICommandList* cmdList) override;

		void onWindowFocusChanged(FocusEvent* event) override;

	private:
		void recreateUploaders(RHICommandList* cmdList);

		void clearUploaders(RHICommandList* cmdList);

		void createView();

		void releaseView();

	private:
		View* m_view;
		Size m_size;
		int m_current;
		std::shared_ptr<RHITexture2D> m_viewTexture;
		std::vector<std::shared_ptr<RHIWritePixelBuffer>> m_uploaders;
		bool m_ready;

	private:
		std::shared_ptr<GameWorld> m_world;
		std::shared_ptr<Material> m_material;
	};
}

#endif // !__volucris_viewport_widget_h__
