#include <RHI/RHICommandList.h>
#include <Application/Window.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Core/Volucris.h>
#include <RHI/RHITexture.h>
#include <RHI/RHIResource.h>
#include <RHI/RHIRenderTarget.h>
#include <RHI/RHIBuffer.h>
#include <RHI/RHIVertexArray.h>
#include <RHI/RHIProgram.h>
#include <RHI/RHIVertexBuffer.h>
#include <Core/Assert.h>
#include "RHIOpenGL.h"
#include <thread>
#include <sstream>

namespace volucris
{
	struct RHICommandList::Impl
	{
		GLbitfield clearFlags;
	};

	GLbitfield getGLClearFlags(uint32 buffers)
	{
		GLbitfield flags = 0;
		if (buffers & RHIClearState::ColorBuffer)
		{
			flags |= GL_COLOR_BUFFER_BIT;
		}
		if (buffers & RHIClearState::DepthBuffer)
		{
			flags |= GL_DEPTH_BUFFER_BIT;
		}

		return flags;
	}

	RenderScope::RenderScope(const std::string& name)
	{
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, name.c_str());
	}

	RenderScope::~RenderScope()
	{
		glPopDebugGroup();
	}

	RHICommandList::RHICommandList()
		: m_window(nullptr)
		, m_impl(new Impl)
	{
		m_state.commandList = this;
		m_impl->clearFlags = getGLClearFlags(m_state.clearState.buffers);
	}

	RHICommandList::~RHICommandList()
	{
		delete m_impl;
	}

	bool RHICommandList::initialize(Window* window, bool sync)
	{
		m_window = window;
		makeCurrent();
		glfwSwapInterval(sync ? 1 : 0);
		gladLoadGLLoader(GLADloadproc(glfwGetProcAddress));

		{
			const auto vender = glGetString(GL_VENDOR);
			const auto renderer = glGetString(GL_RENDERER);
			const auto language = glGetString(GL_SHADING_LANGUAGE_VERSION);
			const auto version = glGetString(GL_VERSION);

			V_LOG_INFO(Engine, "context initialized");
			V_LOG_INFO(Engine, "	vender: {}", (char*)vender);
			V_LOG_INFO(Engine, "	renderer: {}", (char*)renderer);
			V_LOG_INFO(Engine, "	version: {}", (char*)version);
			V_LOG_INFO(Engine, "	language version: {}", (char*)language);
		}
		return true;
	}

	void RHICommandList::destroy()
	{
		m_window = nullptr;
	}

	void RHICommandList::makeCurrent()
	{
		glfwMakeContextCurrent(m_window->getHandle());
	}

	void RHICommandList::swapBuffers()
	{
		glfwSwapBuffers(m_window->getHandle());
	}

	void RHICommandList::clear(const RHIClearState& state)
	{
		if (state.color != m_state.clearState.color)
		{
			m_state.clearState.color = state.color;
			glClearColor(state.color.r, state.color.g, state.color.b, state.color.a);
		}

		if (state.buffers != m_state.clearState.buffers)
		{
			m_impl->clearFlags = getGLClearFlags(m_state.clearState.buffers);
			m_state.clearState.buffers = state.buffers;
		}
		glClear(m_impl->clearFlags);
	}

	void RHICommandList::executeCommand(const std::string& name)
	{
		//glDebug
	}

	void RHICommandList::bindTexture2D(uint32 index, RHITexture2D* texture)
	{
		glActiveTexture(GL_TEXTURE0 + index);
		glBindTexture(GL_TEXTURE_2D, texture ? texture->getId() : 0);
		m_state.texture2d = texture;
	}

	bool RHICommandList::setTexture2D(RHITexture2D* texture)
	{
		if (m_state.texture2d == texture)
		{
			return true;
		}
		
		if (!texture)
		{
			m_state.texture2d = texture;
			return true;
		}

		auto id = texture->getId();
		if (id > 0)
		{
			glBindTexture(GL_TEXTURE_2D, id);
			GL_CHECK();
			m_state.texture2d = texture;
			return true;
		}
		return false;
	}

	void RHICommandList::unsetTexture2D(RHITexture2D* texture)
	{
		if (m_state.texture2d == texture)
		{
			m_state.texture2d = nullptr;
		}
	}

	void RHICommandList::resetTexture2D()
	{
		m_state.texture2d = nullptr;
	}

	bool RHICommandList::setRenderTarget(RHIRenderTarget* renderTarget, Rect rect)
	{
		if (renderTarget == nullptr)
		{
			return false;
		}

		auto id = renderTarget->getId();
		if (id == 0)
		{
			return false;
		}
		
		switch (renderTarget->getUsage())
		{
		case volucris::RHIRenderTarget::ReadOnly:
			m_state.readTarget = renderTarget;
			glBindFramebuffer(GL_READ_FRAMEBUFFER, id);
			break;
		case volucris::RHIRenderTarget::WriteOnly:
			m_state.writeTarget = renderTarget;
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);
			break;
		case volucris::RHIRenderTarget::ReadWrite:
			m_state.readTarget = m_state.writeTarget = renderTarget;
			m_state.readTarget = renderTarget;
			glBindFramebuffer(GL_FRAMEBUFFER, id);
			break;
		default:
			return false;
		}

		if (rect.isValid())
		{
			setViewport(rect);
		}

		return true;
	}

	void RHICommandList::unsetRenderTarget(RHIRenderTarget* renderTarget)
	{
		if (!renderTarget)
		{
			return;
		}

		if (m_state.readTarget == renderTarget)
		{
			m_state.readTarget = nullptr;
		}
		if (m_state.writeTarget == renderTarget)
		{
			m_state.writeTarget = nullptr;
		}
		if (m_state.renderTarget == renderTarget)
		{
			m_state.readTarget = nullptr;
		}
	}

	void RHICommandList::setViewport(int x, int y, int w, int h)
	{
		Rect viewport = { x, y, w, h };
		setViewport(viewport);
	}

	void RHICommandList::setViewport(const Rect& rect)
	{
		if (m_state.viewport != rect)
		{
			glViewport(rect.x, rect.y, rect.width, rect.height);
			m_state.viewport = rect;
		}
	}

	bool RHICommandList::setBuffer(RHIBuffer* buffer)
	{
		auto type = buffer->getType();
		auto it = m_state.buffers.find(type);
		if (it != m_state.buffers.end() && (it->second == buffer))
		{
			return false;
		}
		m_state.buffers[type] = buffer;
		if (buffer && buffer->getId() == 0)
		{
			return false;
		}
		GL_CHECK();
		if (buffer)
		{
			glBindBuffer(getGLTarget(type), buffer->getId());
		}
		GL_CHECK();
		return true;
	}

	void RHICommandList::unsetBuffer(RHIBuffer* buffer)
	{
		auto it = m_state.buffers.find(buffer->getType());
		if (it != m_state.buffers.end() && (it->second == buffer))
		{
			m_state.buffers[buffer->getType()] = nullptr;
		}
	}

	void RHICommandList::makesureBufferUnset(RHIBuffer::Type type)
	{
		auto it = m_state.buffers.find(type);
		if (it != m_state.buffers.end() && m_state.buffers[type] != nullptr)
		{
			m_state.buffers[type] = nullptr;
			glBindBuffer(getGLTarget(type), 0);
		}
	}

	void RHICommandList::setVertexArray(RHIVertexArray* array)
	{
		if (m_state.vertexArray == array)
		{
			return;
		}
		m_state.vertexArray = array;
		if (array)
		{
			glBindVertexArray(array->getId());
		}
	}

	void RHICommandList::unsetVertexArray(RHIVertexArray* array)
	{
		if (array == m_state.vertexArray)
		{
			m_state.vertexArray = nullptr;
		}
	}

	void RHICommandList::setProgram(RHIProgram* program)
	{
		if (m_state.program == program)
		{
			return;
		}
		m_state.program = program;
		if (program)
		{
			glUseProgram(program->getId());
		}
	}

	void RHICommandList::unsetProgram(RHIProgram* program)
	{
		if (m_state.program == program)
		{
			m_state.program = nullptr;
		}
	}

	void RHICommandList::drawPrimitive(RHIVertexArray* vao, RHIElementBuffer* ebo, const PrimitiveSegment& mesh)
	{
		setVertexArray(info.vao);
		setBuffer(info.ebo);
		glDrawElements(getGLElementDrawMode(info.segment.mode), info.segment.count, getGLElementType(info.segment.type), (void*)info.segment.offset);
		GL_CHECK()
	}

	void RHICommandList::drawPrimitive(RHIProgram* program, const DrawInfo& info, Rect rect)
	{
		setProgram(program);
		setVertexArray(info.vao);
		setBuffer(info.ebo);
		glViewport(rect.x, rect.y, rect.width, rect.height);
		glDrawElements(getGLElementDrawMode(info.segment.mode), info.segment.count, getGLElementType(info.segment.type), (void*)info.segment.offset);
		GL_CHECK()
	}
}