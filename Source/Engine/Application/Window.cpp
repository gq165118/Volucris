#include "Application/Window.h"
#include <Application/Application.h>
#include <stdexcept>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <Render/ImGuiRenderer.h>
#include <Core/Volucris.h>
#include <Core/Assert.h>
#include <Application/Event.h>

namespace volucris
{
	Window::Window()
		: Widget()
		, m_handle(nullptr)
		, m_imguiRenderer(nullptr)
		, m_title("Window")
		, m_focused(false)
	{
	}

	Window::~Window()
	{
		
	}

	void Window::setTitle(const std::string& title)
	{
		m_title = title;
		if (m_handle)
		{
			glfwSetWindowTitle(m_handle, title.c_str());
		}
	}

	void Window::setFocused(bool focused)
	{
		if (m_focused == focused)
		{
			return;
		}
		m_focused = focused;
		if (m_focused)
		{
			m_imguiRenderer->makeCurrent();
			glfwFocusWindow(m_handle);
		}

		FocusEvent e;
		e.focused = m_focused;
		setFocusEvent(&e);
	}

	void Window::create(bool offscreen)
	{
		V_LOG_DEBUG(Engine, "create window: {}", m_title);
		m_handle = glfwCreateWindow(800, 600, m_title.c_str(), nullptr, nullptr);
		if (!offscreen)
		{
			m_imguiRenderer = std::make_unique<ImGuiRenderer>(this);

			glfwSetWindowUserPointer(m_handle, this);
			glfwSetWindowCloseCallback(m_handle, [](GLFWwindow* handle) {
				auto window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
				gApp->removeWindow(window->getShared<Window>());
				});

			glfwSetWindowSizeCallback(m_handle, [](GLFWwindow* handle, int width, int height) {
				auto window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
				//window->setSize({ width, height });
				});

			glfwSetWindowFocusCallback(m_handle, [](GLFWwindow* handle, int focused) {
				auto window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
				if (focused)
				{
					gApp->setFocusedWindow(window);
				}
				});

			glfwSetDropCallback(m_handle, [](GLFWwindow* handle, int count, const char** paths) {
				auto window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
				if (count <= 0)
				{
					return;
				}
				double x, y;
				glfwGetCursorPos(handle, &x, &y);
				DropEvent e;
				for (auto i = 0; i < count; ++i)
				{
					e.files.push_back(paths[i]);
				}

				e.pos = Point(static_cast<float>(x), static_cast<float>(y));
				{
					window->setDropEvent(&e);
				}
				});
			rendererCreated(m_imguiRenderer->getCommandList());
			setFocused(true);
			glfwShowWindow(m_handle);
		}
	}

	void Window::destroy()
	{
		V_LOG_DEBUG(Engine, "destroy window: {}", m_title);
		if (m_handle)
		{
			glfwDestroyWindow(m_handle);
			m_handle = nullptr;
		}
	}

	void Window::destroyImGuiRenderer()
	{
		beforeRedererDestroy(m_imguiRenderer->getCommandList());
		setFocused(false);
		m_imguiRenderer = nullptr;
	}

	void Window::onBuild(bool init)
	{
		v_check(glfwGetCurrentContext() == m_handle)
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		buildChildren();
	}

	void Window::onWindowFocusChanged(FocusEvent* event)
	{
		if (!m_imguiRenderer)
		{
			return;
		}

		m_imguiRenderer->installedCallbacks(event->focused);
	}
}
