#include "Application/Application.h"
#include <stdexcept>
#include <Application/window.h>
#include <GLFW/glfw3.h>
#include <Core/VectorHelp.h>
#include <Render/ImGuiRenderer.h>
#include <Render/Renderer.h>
#include <Application/FrameSynthesizer.h>
#include <Core/Volucris.h>
#include <Engine/Profile/Profile.h>
#include <Profile/ProfileManager.h>
#include <tracy/Tracy.hpp>
#include <iostream>
#include <Game/GameWorld.h>

namespace volucris
{
	Application* Application::s_instance = nullptr;

	Config Application::config()
	{
		return Config("/Engine/Config/VolucrisApplication");
	}

	Application::Application()
		: m_focusedWindow(nullptr)
		, m_mainWindow(nullptr)
		, m_windows()
		, m_queue(1024)
		, m_running(false)
	{
		if (s_instance != nullptr)
		{
			throw std::runtime_error("Application instance already exists.");
		}
		s_instance = this;
	}

	void Application::flushCommmands()
	{
		std::function<void()> command;
		while (m_queue.pop(command, false))
		{
			command();
		}
	}

	void Application::addWindow(const std::shared_ptr<Window>& window)
	{
		if (window->isValid())
		{
			return;
		}

		setFocusedWindow(nullptr);

		window->create();
		m_windows.push_back(window);
		if (m_mainWindow == nullptr)
		{
			m_mainWindow = window;
		}

		setFocusedWindow(window.get());
		// todo 添加渲染线程
	}

	void Application::removeWindow(const std::shared_ptr<Window>& window)
	{
		if (!window || !window->isValid())
		{
			return;
		}

		if (window == m_mainWindow)
		{
			for (const auto& window : m_windows)
			{
				setFocusedWindow(window.get());
				window->destroyImGuiRenderer();
			}

			for (const auto& window : m_windows)
			{
				if (window != m_mainWindow)
				{
					window->destroy();
				}
			}

			setFocusedWindow(nullptr);
			m_windows.clear();
		}
		else
		{
			window->destroyImGuiRenderer();
			// 上下文切换到主窗口
			setFocusedWindow(m_mainWindow.get());
			window->destroy();
			VectorHelp::quickRemove(m_windows, window);
		}
	}

	void Application::setMainWindow(const std::shared_ptr<Window>& window)
	{
		if (!window->isValid())
		{
			addWindow(window);
		}
		m_mainWindow = window;
	}

	void Application::setFocusedWindow(Window* window)
	{
		if (m_focusedWindow == window)
		{
			return;
		}

		if (m_focusedWindow)
		{
			m_focusedWindow->setFocused(false);
		}

		m_focusedWindow = window;
		if (m_focusedWindow && m_focusedWindow->getImGuiRenderer())
		{
			m_focusedWindow->setFocused(true);
			//// 渲染一帧
			//m_focusedWindow->build();

			//m_focusedWindow->getImGuiRenderer()->render();
		}
	}

	void Application::addGame(const std::shared_ptr<GameWorld>& newGame)
	{
		for (const auto& game : m_games)
		{
			if (game == newGame)
			{
				return;
			}
		}
		m_games.push_back(newGame);
	}

	void Application::removeGame(const std::shared_ptr<GameWorld>& universe)
	{
		VectorHelp::quickRemove(m_games, universe);
	}

	int Application::exec()
	{
		if (!m_mainWindow)
		{
			return 0;
		}

		Renderer::getInstance().run();
		m_running = true;

		while (m_mainWindow->isValid())
		{
			V_SCOPED_PROFILE;

			m_focusedWindow->build();

			for (const auto& game : m_games)
			{
				game->update();
			}

			Renderer::getInstance().push(nullptr);

			FrameSynthesier::getInstance().countGameFrame();

			m_focusedWindow->getImGuiRenderer()->render();

			glfwPollEvents();

			flushCommmands();
		}

		Renderer::getInstance().quit();
		return 0;
	}

	void Application::quit()
	{
		pushCommand([this]() {removeWindow(m_mainWindow); });
	}
}
