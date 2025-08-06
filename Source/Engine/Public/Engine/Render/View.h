#ifndef __volucris_view_h__
#define __volucris_view_h__

#include <Engine/Render/BaseView.h>

namespace volucris
{
	struct Camera
	{
		glm::mat4 projectionMat;
		glm::mat4 viewMat;
	};

	class RHIUniformBuffer;

	class View : public BaseView
	{
	public:
		View();

		View(const std::shared_ptr<Scene>& scene);

		void init() override;

		void updateProjectionMatrix(const glm::mat4& mat);

		void updateViewMatrix(const glm::mat4& mat);

		void render(RHICommandList* cmdList) override;

	private: 
		Camera m_camera;
		std::unique_ptr<RHIUniformBuffer> m_cameraUbo;
	};
}

#endif // !__volucris_view_h__
