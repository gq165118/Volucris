#ifndef __volucris_base_view_h__
#define __volucris_base_view_h__

#include <memory>
#include <Engine/RHI/RHICommandList.h>
#include <Engine/Core/TextureDefines.h>

namespace volucris
{
	class Scene;
	class RHIRenderTarget;
	class RHIReadPixelBuffer;
	class StaticMeshProxy;
	class MaterialInstanceProxy;

	class BaseView
	{
	public:
		BaseView();

		BaseView(const std::shared_ptr<Scene>& scene);

		~BaseView();

		virtual void init() {}

		void resize(int width, int height);

		virtual void render(RHICommandList* cmdList);

		void bindTarget(RHICommandList* cmdList);

		void swapViewData(RHICommandList* cmdList);

		Texture::TextureData getViewData() const
		{
			return m_targetData;
		}

		const std::shared_ptr<Scene>& getScene() const { return m_scene; }

	private:
		std::vector<std::unique_ptr<RHIRenderTarget>> m_targets;
		std::vector<std::unique_ptr<RHIReadPixelBuffer>> m_targetReaders;
		Texture::TextureData m_targetData;
		int m_current;
		std::shared_ptr<Scene> m_scene;
	};
}

#endif // !__volucris_view_h__
