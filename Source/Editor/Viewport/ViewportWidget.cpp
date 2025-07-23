#include <Viewport/ViewportWidget.h>
#include <imgui.h>
#include <Engine/Render/View.h>
#include <Engine/Application/Window.h>
#include <Engine/Render/Renderer.h>
#include <Engine/Core/Task.h>
#include <Engine/Core/Assert.h>
#include <Engine/Render/ImGuiRenderer.h>
#include <Engine/RHI/RHIBuffer.h>
#include <Engine/RHI/RHITexture.h>
#include <EditorCore/editor.h>
#include <Engine/Application/Application.h>
#include <Engine/Game/GameWorld.h>
#include <Engine/Application/Event.h>
#include <Engine/RHI/RHIPixelBuffer.h>
#include <Engine/Asset/AssetManager.h>
#include <Engine/Game/StaticMesh.h>
#include <EditorEntry/EditorApplication.h>
#include <EditorEntry/EditorWindow.h>

namespace volucris
{
	struct CreateViewTask
	{
		mutable std::unique_ptr<View> view;
		Size size;
		ViewportWidget* client;

		CreateViewTask(std::unique_ptr<View> v, Size s)
			: view(std::move(v)), size(s), client(nullptr) {}

		CreateViewTask(const CreateViewTask& task)
		{
			view = std::move(task.view);
			size = task.size;
			client = nullptr;
		}

		CreateViewTask(CreateViewTask&& task) noexcept
			: view(std::move(task.view)), size(task.size), client(task.client)
		{
		}

		void execute()
		{
			view->resize(size.width, size.height);
			auto v = view.get();
			Renderer::getInstance().addView(std::move(view));
			Renderer::getInstance().renderFrame();
			Renderer::getInstance().renderFrame();
			if (client)
			{
				auto data = v->getViewData();
				gApp->pushCommand([client=client, data = std::move(data)]() {
					client->setViewData(std::move(data));
					});
			}
		}
	};;

	ViewportWidget::ViewportWidget()
		: Widget()
		, m_view(nullptr)
		, m_size(8, 8)
		, m_current(0)
		, m_viewTexture(nullptr)
		, m_uploaders()
		, m_world(nullptr)
		, m_ready(false)
	{
	}

	void ViewportWidget::setWorld(const std::shared_ptr<GameWorld>& world)
	{
		releaseView();
		m_world = world;
		createView();
	}

	void ViewportWidget::onBuild(bool init)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::Begin("Preview");

		auto pos = ImGui::GetWindowPos();
		auto winSize = ImGui::GetWindowSize();
		ImVec2 size = ImGui::GetContentRegionAvail();
		Size viewSize = Size((int)size.x, (int)size.y);
		if (viewSize != m_size)
		{
			V_LOG_DEBUG(Editor, "ViewportWidget size changed: {}, {}", viewSize.width, viewSize.height);
			viewSizeChanged(viewSize);
		}

		if (m_view)
		{
			Renderer::getInstance().push([client = this, view = m_view]() {
				auto data = view->getViewData();
				gApp->pushCommand([client, data = std::move(data)]() {
					client->setViewData(std::move(data));
					});
				});

			if (m_ready)
			{
				auto id = m_viewTexture->getId();
				if (id > 0)
				{
					ImTextureID texID = (ImTextureID)(intptr_t)id;
					ImGui::Image(texID, size, ImVec2(0, 1), ImVec2(1, 0));
				}
			}
		}

		ImGui::End();
		ImGui::PopStyleVar(2);
	}

	void ViewportWidget::viewSizeChanged(Size size)
	{
		if (size.width < 8 || size.height < 8)
		{
			size = { 8, 8 };
		}

		auto curSize = m_size;
		m_size = size;
		if (m_view && m_size != curSize)
		{
			m_size = size;
			recreateUploaders(getContext());
			Renderer::getInstance().push([client=this, view=m_view, size= m_size]() {
				view->resize(size.width, size.height);
				Renderer::getInstance().renderFrame();
				Renderer::getInstance().renderFrame();

				auto data = view->getViewData();
				gApp->pushCommand([client, data = std::move(data)]() {
					client->setViewData(std::move(data));
					});

				});
			// 确保执行完成
			Renderer::getInstance().flushCommands();
		}
	}

	void ViewportWidget::onRendererBuild(RHICommandList* cmdList)
	{
		createView();
	}

	void ViewportWidget::onRendererDestroy(RHICommandList* cmdList)
	{
		releaseView();
	}

	void ViewportWidget::onWindowFocusChanged(FocusEvent* event)
	{
		if (event->focused)
		{
			createView();
		}
		else
		{
			releaseView();
			if (!gApp->isRunning())
			{
				m_ready = false;
			}
		}
	}

	void ViewportWidget::recreateUploaders(RHICommandList* cmdList)
	{
		clearUploaders(cmdList);

		auto size = m_size.width * m_size.height * 3;
		RHITextureDesc desc;
		desc.pixelFormat = Texture::EPixelFormat::R8G8B8;
		desc.size = m_size;
		desc.texClass = TextureType::Texture2D;
		for (int i = 0; i < 2; ++i)
		{
			auto texture = std::make_shared<RHITexture2D>(desc);
			texture->setContext(cmdList);
			texture->createGpuResource();
			cmdList->setTexture2D(texture.get());
			texture->init();

			auto uploader = std::make_shared<RHIWritePixelBuffer>(RHIBuffer::StreamWrite);
			uploader->setContext(cmdList);
			uploader->createGpuResource();
			cmdList->setBuffer(uploader.get());
			uploader->init(nullptr, (uint32)size);
			uploader->bindTexture(texture);
			m_uploaders.push_back(std::move(uploader));
		}

		m_current = 0;
	}

	void ViewportWidget::clearUploaders(RHICommandList* cmdList)
	{
		for (auto& uploader : m_uploaders)
		{
			cmdList->unsetBuffer(uploader.get());
		}

		m_uploaders.clear();
	}

	void ViewportWidget::setViewData(Texture::TextureData data)
	{
		auto context = getContext();
		if (!m_view || !context)
		{
			return;
		}
		if (data.size.width != m_size.width || data.size.height != m_size.height)
		{
			return;
		}

		if (m_uploaders.empty())
		{
			return;
		}

		auto cmdList = getContext();

		auto& currentUploader = m_uploaders[m_current];
		currentUploader->startWrite(std::move(data.data));

		if (!m_ready && m_current == 1)
		{
			m_ready = true;
		}

		m_current = (m_current + 1) % m_uploaders.size();
		currentUploader = m_uploaders[m_current];
		currentUploader->writeToTexture();
		m_viewTexture = currentUploader->getTexture();
	}

	void ViewportWidget::createView()
	{
		auto context = getContext();
		if (!m_view && m_world && context)
		{
			auto view = std::make_unique<View>(m_world->getScene());
			m_view = view.get();
			if (auto mesh = AssetManager::getInstance().loadAsset<StaticMesh>("/Engine/Content/Editor/Cube", GEditorWorld))
			{
				m_view->setTestStaticMesh(mesh->getProxy());
			}

			if (auto mat = AssetManager::getInstance().loadAsset<Material>("/Engine/Content/Editor/test", GEditorWorld))
			{
				m_view->setTestMaterial(mat->getProxy());
			}

			CreateViewTask task = CreateViewTask(std::move(view), m_size);
			if (gApp->isRunning())
			{
				task.client = this;
			}
			Renderer::getInstance().push(createTask(std::move(task)));
			recreateUploaders(context);

			if (gApp->isRunning())
			{
				Renderer::getInstance().flushCommands();
				gApp->flushCommmands();
			}
		}
	}

	void ViewportWidget::releaseView()
	{
		if (m_view)
		{
			auto cmdList = getContext();
			Renderer::getInstance().push([view = m_view]() {
				Renderer::getInstance().removeView(view);
				});
			clearUploaders(cmdList);
			if (m_viewTexture)
			{
				cmdList->unsetTexture2D(m_viewTexture.get());
				m_viewTexture = nullptr;
			}
			Renderer::getInstance().flushCommands();
			gApp->flushCommmands();
			m_view = nullptr;
		}
	}
}