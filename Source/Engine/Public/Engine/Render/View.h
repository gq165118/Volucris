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
	class MaterialProxy;

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

		void setTestStaticMesh(const std::shared_ptr<StaticMeshProxy>& staticmesh)
		{
			m_mesh = staticmesh;
		}

		void setTestMaterial(const std::shared_ptr<MaterialProxy>& mat)
		{
			m_material = mat;
		}

	private:
		std::vector<std::unique_ptr<RHIRenderTarget>> m_targets;
		std::vector<std::unique_ptr<RHIReadPixelBuffer>> m_targetReaders;
		Texture::TextureData m_targetData;
		int m_current;
		std::shared_ptr<Scene> m_scene;

		// 测试数据
		std::shared_ptr<StaticMeshProxy> m_mesh;
		std::shared_ptr<MaterialProxy> m_material;
	};
}

#endif // !__volucris_view_h__
