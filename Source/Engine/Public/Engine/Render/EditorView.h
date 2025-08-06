#ifndef __editor_volucris_view_h__
#define __editor_volucris_view_h__

#include <Engine/Render/View.h>

namespace volucris
{
#if 1
	class Scene;
	class RHIRenderTarget;
	class RHIReadPixelBuffer;

	class EditorView : public BaseView
	{
	public:
		EditorView();

		EditorView(const std::shared_ptr<Scene>& scene);

		~EditorView();

		void buildData();

		void render(RHICommandList* cmdList) override;

		//void swapViewData(RHICommandList* cmdList) override;

		Texture::TextureData getViewData() const
		{
			return m_targetData;
		}

	private:
		std::vector<std::unique_ptr<RHIRenderTarget>> m_targets;
		std::vector<std::unique_ptr<RHIReadPixelBuffer>> m_targetReaders;
		Texture::TextureData m_targetData;
		int m_current;
		std::shared_ptr<Scene> m_scene;
	};
#else
 
#endif
}

#endif // !__volucris_view_h__
