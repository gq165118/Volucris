#include "Render/EditorView.h"
#include <Engine/RHI/RHICommandList.h>
#include <Engine/RHI/RHIBuffer.h>
#include <Engine/RHI/RHIRenderTarget.h>
//#include <Engine/Core/Volucris.h>
#include <Engine/Render/Renderer.h>
#include <Engine/RHI/RHIPixelBuffer.h>
#include <Engine/RHI/RHIVertexBuffer.h>
#include <Engine/RHI/RHIVertexArray.h>
#include <Engine/RHI/RHIProgram.h>
#include <Engine/RHI/RHIShader.h>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <Engine/Render/StaticMeshProxy.h>
#include <RHI/RHIOpenGL.h>

constexpr int FrameCount = 1;
#define  ATLAS_SIZE  256

namespace volucris
{
#if 1
	static std::shared_ptr<StaticMeshProxy> mesh = nullptr;
	static std::shared_ptr<RHIProgram> shader = nullptr;

	static glm::vec3 vertices[] = {
		{-0.5f, -0.5f, 1.0f},
		{ 0.5f, -0.5f, 1.0f},
		{ 0.5f,  0.5f, 1.0f},
		{-0.5f,  0.5f, 1.0f},
		{ 1.0f,  0.0f, 0.0f},
		{ 0.0f,  1.0f, 0.0f},
		{ 0.0f,  0.0f, 1.0f},
		{ 1.0f,  0.0f, 1.0f},
	};

	static uint32 indices[] = {
		0, 1, 2, 0, 2, 3
	};

	static char* vss = R"(
		#version 430 core
		layout(location=0) in vec3 v_pos;
		layout(location=2) in vec3 v_color;

		layout(location=0) out vec3 vertexColor;
		void main() {
			gl_Position=vec4(v_pos, 1.0);
			vertexColor = v_color;
		}
	)";

	static char* fss = R"(
		#version 430 core
		layout (location=0) in vec3 vertexColor;
		layout (location=0) out vec4 color;
		void main() {  color = vec4(vertexColor, 1.0); }
	)";

	EditorView::EditorView()
		: m_targets()
		, m_targetReaders()
		, m_targetData()
		, m_current(0)
		, m_scene(nullptr)
	{
	}

	EditorView::EditorView(const std::shared_ptr<Scene>& scene)
		: EditorView()
	{
		m_scene = scene;
	}


	EditorView::~EditorView() 
	{
		for (auto& target : m_targets)
		{
			RHICmdList->unsetRenderTarget(target.get());
		}
		mesh = nullptr;
		shader = nullptr;
	}

	void EditorView::buildData()
	{
		if (!mesh)
		{
			PrimitiveInfo info;
			info.data.resize(sizeof(vertices));
			memcpy(info.data.data(), (uint8*)vertices, info.data.size());
			{
				PrimitiveBlock block;
				block.dataType = DataType::Float;
				block.type = PrimitiveType::Vertex;
				block.count = 3;
				block.offset = 0;
				info.blocks.push_back(block);
			}
			{
				PrimitiveBlock block;
				block.dataType = DataType::Float;
				block.type = PrimitiveType::Color;
				block.count = 3;
				block.offset = 4 * sizeof(glm::vec3);
				info.blocks.push_back(block);
			}
			info.segmentData.resize(sizeof(indices));
			memcpy(info.segmentData.data(), (uint8*)indices, sizeof(indices));
			{
				PrimitiveSegment segment;
				segment.type = ElementDataType::UInt;
				segment.mode = ElementDrawMode::Traingles;
				segment.count = 6;
				segment.offset = 0;
				info.segments.push_back(segment);
			}
			mesh = std::make_shared<StaticMeshProxy>();  //GPU 渲染代理（Proxy）
			mesh->init(info);

			auto vs = std::make_shared<RHIShader>(RHIShader::VertexShader);
			vs->init(vss);

			auto fs = std::make_shared<RHIShader>(RHIShader::FragmentShader);
			fs->init(fss);

			shader = std::make_shared<RHIProgram>();
			shader->init({ vs, fs });
		}
		for (auto& target : m_targets)
		{
			RHICmdList->unsetRenderTarget(target.get());
		}

		m_targets.clear();
		m_targetReaders.clear();

		//配置颜色纹理格式和数据缓存
		RHITextureDesc desc;
		int width, height;
		width = height = ATLAS_SIZE;
		desc.size = { width, height };
		desc.pixelFormat = Texture::EPixelFormat::R8G8B8;

		auto size = 3 * width * height;
		m_targetData.format = getSourceFormat(desc.pixelFormat);
		m_targetData.size = { width, height };
		m_targetData.data.resize(size);

		//为每帧构建渲染目标、颜色纹理和读取缓冲
		for (int i = 0; i < FrameCount; ++i)
		{
			// 初始化贴图
			auto texture = std::make_shared<RHITexture2D>(desc);
			texture->setContext(RHICmdList);
			texture->createGpuResource();
			RHICmdList->setTexture2D(texture.get());
			texture->init();

			//创建 Render Target 并绑定颜色纹理
			auto target = std::make_unique<RHIRenderTarget>(Size(width, height));
			target->setContext(RHICmdList);
			target->createGpuResource();
			RHICmdList->setRenderTarget(target.get());
			target->attachColor(texture, 0);
			v_check(target->update())
			m_targets.emplace_back(std::move(target));

			// 创建读取缓冲
			auto reader = std::make_unique<RHIReadPixelBuffer>(RHIBuffer::StreamRead);
			reader->setContext(RHICmdList);
			reader->createGpuResource();
			reader->bindTexture(texture);
			v_check(RHICmdList->setBuffer(reader.get()))
				reader->init(nullptr, (uint32)size);
			m_targetReaders.emplace_back(std::move(reader));
		}

		m_current = 0;
		GL_CHECK();
	}

	void EditorView::render(RHICommandList* cmdList)
	{
		RENDER_SCOPE(EditorView);
		Rect vp;
		vp.setPoint({ 0,0 });
		vp.setSize(m_targets[m_current]->getSize());
		cmdList->setRenderTarget(m_targets[m_current].get(), vp);
		RHIClearState state;
		state.color = { 0.0, 0.8, 1.0, 1.0 };
		cmdList->clear(state);

		RHICmdList->drawPrimitive(shader.get(), *(mesh->get(0)), Rect(0, ATLAS_SIZE- ATLAS_SIZE/4,ATLAS_SIZE/4, ATLAS_SIZE/4));

		swapViewData(cmdList);
		GL_CHECK();
	}

	void EditorView::swapViewData(RHICommandList* cmdList)
	{
		Rect rect = { 0,0,0,0 };
		rect.setSize(m_targets[m_current]->getSize());

		int next = (m_current + 1) % FrameCount;
		//把 FBO 的颜色附件指定区域读到绑定的 PBO（像素缓冲）里
		m_targetReaders[next]->startRead(rect);

		//把上一步中存入 GPU PBO 的像素数据 异步读回到 CPU 侧
		m_targetReaders[m_current]->readColorTo(m_targetData.data);

		m_current = next;
		GL_CHECK();
	}

#else

#endif
}