#ifndef __volucris_view_h__
#define __volucris_view_h__

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

	class View
	{
	public:
		View();

		View(const std::shared_ptr<Scene>& scene);

		virtual ~View();

		void resize(int width, int height);

		virtual void render(RHICommandList* cmdList);

		virtual void swapViewData(RHICommandList* cmdList);

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
}

#endif // !__volucris_view_h__
