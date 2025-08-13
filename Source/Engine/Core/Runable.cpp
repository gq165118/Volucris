#include "Core/Runable.h"
#include <Core/Volucris.h>
#include <Application/FrameSynthesizer.h>

namespace volucris
{
	Runable::Runable(size_t queueSize)
		: m_queue(queueSize)
		, m_thread()
		, m_running(false)
		, m_quitFence(nullptr)
	{
	}

	Runable::~Runable()
	{
	}

	void Runable::push(std::function<void()> cmd, bool block)
	{
		m_queue.push(std::move(cmd), block);
	}

	void Runable::flushCommands()
	{
		Fence fence;
		push([this, &fence]() {
			fence.signal();
			});
		fence.wait();
	}

	void Runable::quit()
	{
		Fence fence;
		push([this, &fence]() {
			setRunning(false);
			m_quitFence = &fence;
			});
		fence.wait();
	}

	bool Runable::start(const std::function<void()>& main)
	{
		m_thread = std::thread([this, main]() {
			// 初始化
			V_LOG_INFO(Engine, "start thread")
			setRunning(initialize());
			if (!isRunning())
			{
				return;
			}

			bool render = false;
			std::function<void()> msg;
			while (m_running)
			{
				main();
				
				while (m_queue.pop(msg, true))
				{
					if (!msg)
					{
						break;
					}
					msg();

					if (!isRunning())
					{
						break;
					}
				}
			}

			V_LOG_INFO(Engine, "destroy thread")
			destroy();	//清理renderer 的view cmdList等对象
			V_LOG_INFO(Engine, "quit thread")
			m_quitFence->signal();
			});
		m_thread.detach();
		return true;
	}
}

