#include "Render/View.h"
#include "Render/Scene.h"
#include "Render/MaterialInstanceProxy.h"
#include <Render/Renderer.h>
#include <RHI/RHIUniformBuffer.h>

namespace volucris
{
	View::View()
		: BaseView()
		, m_camera()
		, m_cameraUbo(nullptr)
	{

	}

	View::View(const std::shared_ptr<Scene>& scene)
		: BaseView(scene)
		, m_camera()
		, m_cameraUbo(nullptr)
	{

	}

	void View::init()
	{
		m_cameraUbo = std::make_unique<RHIUniformBuffer>(RHICmdList, RHIBuffer::DynamicDraw);
		m_cameraUbo->createGpuResource();
		m_cameraUbo->init(&m_camera, sizeof(m_camera));

		updateProjectionMatrix(glm::perspective(glm::radians(90.0f), 800 / 600.f, 0.1f, 100.f));
		updateViewMatrix(glm::lookAt(glm::vec3( 0.0f,0.0f,-3.0f ), { 0.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f }));
	}

	void View::updateProjectionMatrix(const glm::mat4& mat)
	{
		m_camera.projectionMat = mat;
	}

	void View::updateViewMatrix(const glm::mat4& mat)
	{
		m_camera.viewMat = mat;
	}

	void View::render(RHICommandList* cmdList)
	{
		RENDER_SCOPE(View);
		{
			RHIClearState state;
			state.color = { 0.0, 0.0, 0.0, 1.0 };
			cmdList->clear(state);
		}
		{
			RHIDepthTest state = { true };
			cmdList->setDepthTest(state);
		}
		{
			RHICullFace cullface = { true };
			cmdList->setCullFace(cullface);
		}



		m_cameraUbo->update(&m_camera, 0, sizeof(m_camera));
		m_cameraUbo->bindToSlot(0);

		for (const auto& primitive : getScene()->getPrimitives())
		{
			for (const auto& info : primitive->getPrimitiveDrawInfos())
			{
				info.material->use(cmdList);
				cmdList->drawPrimitive(info.vao, info.ebo, info.mesh);
			}
		}
	}
}
