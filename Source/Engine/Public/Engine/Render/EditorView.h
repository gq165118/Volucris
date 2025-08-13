#ifndef __editor_volucris_view_h__
#define __editor_volucris_view_h__

#include <Engine/Render/View.h>

namespace volucris
{
#if 1
	class Scene;
	class RHIRenderTarget;
	class RHIReadPixelBuffer;
	class PrimitiveSceneProxy;
	class Texture2DProxy;

	class EditorView : public BaseView
	{
	public:
		EditorView();

		//EditorView(const std::shared_ptr<Scene>& scene);
		EditorView(const std::shared_ptr<PrimitiveSceneProxy>& proxy);

		//~EditorView();

		void init() override;

		void setTextureProxy(std::shared_ptr<Texture2DProxy> tex);

		void buildData();

		void render(RHICommandList* cmdList) override;

		//Texture::TextureData getViewData() const
		//{
		//	return m_targetData;
		//}

		//void bindTarget(RHICommandList* cmdList);
		
		void swapViewData(RHICommandList* cmdList) override;

	private:
		/*std::vector<std::unique_ptr<RHIRenderTarget>> m_targets;
		std::vector<std::unique_ptr<RHIReadPixelBuffer>> m_targetReaders;
		Texture::TextureData m_targetData;
		int m_current;*/
		//std::shared_ptr<Scene> m_scene;
		std::shared_ptr<PrimitiveSceneProxy> m_proxy;
		std::shared_ptr<Texture2DProxy> m_texture;

		bool  m_isSaved;   //测试用
	};
#else
 
#endif
}

#endif // !__volucris_view_h__
