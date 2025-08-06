#include <Render/BaseView.h>
#include <RHI/RHICommandList.h>
#include <RHI/RHIBuffer.h>
#include <RHI/RHIRenderTarget.h>
#include <Core/Volucris.h>
#include <Render/Renderer.h>
#include <RHI/RHIPixelBuffer.h>
#include <RHI/RHIVertexBuffer.h>
#include <RHI/RHIVertexArray.h>
#include <RHI/RHIProgram.h>
#include <RHI/RHIShader.h>
#include <Render/StaticMeshProxy.h>
#include <Render/MaterialInstanceProxy.h>
#include <Render/Scene.h>

constexpr int FrameCount = 2;

namespace volucris
{
	BaseView::BaseView()
		: m_targets()
		, m_targetReaders()
		, m_targetData()
		, m_current(0)
		, m_scene(nullptr)
	{
	}

	BaseView::BaseView(const std::shared_ptr<Scene>& scene)
		: BaseView()
	{
		m_scene = scene;
	}


	BaseView::~BaseView()
	{
		for (auto& target : m_targets)
		{
			RHICmdList->unsetRenderTarget(target.get());
		}
	}

	void BaseView::resize(int width, int height)
	{
		if (width <= 8 || height <= 8)
		{
			width = height = 8;
		}

		V_LOG_INFO(Engine, "resize view to {}x{}", width, height);

		for (auto& target : m_targets)
		{
			RHICmdList->unsetRenderTarget(target.get());
		}
		
		m_targets.clear();
		m_targetReaders.clear();

		RHITextureDesc desc;
		desc.size = { width, height };
		desc.pixelFormat = Texture::EPixelFormat::R8G8B8;

		auto size = 3 * width * height;
		m_targetData.format = getSourceFormat(desc.pixelFormat);
		m_targetData.size = { width, height };
		m_targetData.data.resize(size);

		for (int i = 0; i < FrameCount; ++i)
		{
			// 初始化贴图
			auto texture = std::make_shared<RHITexture2D>(desc);
			texture->setContext(RHICmdList);
			texture->createGpuResource();
			RHICmdList->setTexture2D(texture.get());
			texture->init();

			// 
			auto target = std::make_unique<RHIRenderTarget>(Size(width, height));
			target->setContext(RHICmdList);
			target->createGpuResource();
			RHICmdList->setRenderTarget(target.get());
			target->attachColor(texture, 0);
			v_check(target->update())
			m_targets.emplace_back(std::move(target));

			auto reader = std::make_unique<RHIReadPixelBuffer>(RHIBuffer::StreamRead);
			reader->setContext(RHICmdList);
			reader->createGpuResource();
			reader->bindTexture(texture);
			v_check(RHICmdList->setBuffer(reader.get()))
			reader->init(nullptr, (uint32)size);
			m_targetReaders.emplace_back(std::move(reader));
		}

		m_current = 0;
	}

	void BaseView::render(RHICommandList* cmdList)
	{
		RENDER_SCOPE(BaseView);
		RHIClearState state;
		state.color = { 0.0, 0.8, 1.0, 1.0 };
		cmdList->clear(state);

		for (const auto& primitive : m_scene->getPrimitives())
		{
			for (const auto& info : primitive->getPrimitiveDrawInfos())
			{
				info.material->use(RHICmdList);
				cmdList->drawPrimitive(info.vao, info.ebo, info.mesh);
			}
		}
	}

	void BaseView::bindTarget(RHICommandList* cmdList)
	{
		Rect rect = { 0,0,0,0 };
		rect.setSize(m_targets[m_current]->getSize());
		cmdList->setRenderTarget(m_targets[m_current].get(), rect);
	}

	void BaseView::swapViewData(RHICommandList* cmdList)
	{
		Rect rect = { 0,0,0,0 };
		rect.setSize(m_targets[m_current]->getSize());

		int next = (m_current + 1) % FrameCount;
		m_targetReaders[next]->startRead(rect);

		m_targetReaders[m_current]->readColorTo(m_targetData.data);
		m_current = next;
	}
}