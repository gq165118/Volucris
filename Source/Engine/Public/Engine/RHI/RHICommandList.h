/**
 * @class RHICommandList
 * @brief 封装渲染命令的执行接口，作为 RHI 层的核心渲染指令管理类。
 *
 * RHICommandList 代表一个渲染上下文，封装了 OpenGL（或其他图形 API）的状态设置、
 * 渲染目标切换、资源绑定和绘制调用等功能。通过统一接口屏蔽底层 API 差异，
 * 实现平台无关的渲染命令封装。
 *
 * 该类内部持有一个 RHIState 结构体，用于记录当前上下文中的资源绑定状态，
 * 避免不必要的状态切换，提高渲染效率。
 */
#ifndef __volucris_rhi_commmand_list_h__
#define __volucris_rhi_commmand_list_h__

#include <memory>
#include <string>
#include <Engine/Core/Core.h>
#include <Engine/RHI/RHIState.h>
#include <Engine/RHI/RHIRenderTarget.h>
#include <Engine/Core/PrimitiveInfo.h>

namespace volucris
{
	class Window;
	class RHITexture2D;

	struct RenderScope
	{
		RenderScope(const std::string& name);

		~RenderScope();
	};

	class RHICommandList
	{
	public:
		RHICommandList();

		~RHICommandList() noexcept;

		bool initialize(Window* widnow, bool sync = false);

		void destroy();

		void makeCurrent();

		void swapBuffers();

		void clear(const RHIClearState& state);

		void setDepthTest(const RHIDepthTest& state);

		void setCullFace(const RHICullFace& cullface);

		void executeCommand(const std::string& name);

		void bindTexture2D(uint32 index, RHITexture2D* texture);

		bool setTexture2D(RHITexture2D* texture);

		void unsetTexture2D(RHITexture2D* texture);

		void resetTexture2D();

		bool setRenderTarget(RHIRenderTarget* renderTarget, Rect rect = Rect());

		void unsetRenderTarget(RHIRenderTarget* renderTarget);

		void setViewport(int x, int y, int w, int h);

		void setViewport(const Rect& rect);

		bool setBuffer(RHIBuffer* buffer);

		void unsetBuffer(RHIBuffer* buffer);

		void bindUniformBufferToSlot(RHIUniformBuffer* buffer, uint32 slot);

		void makesureBufferUnset(RHIBuffer::Type type);

		void setVertexArray(RHIVertexArray* array);

		void unsetVertexArray(RHIVertexArray* array);

		void setProgram(RHIProgram* program);

		void unsetProgram(RHIProgram* program);

		void drawPrimitive(RHIVertexArray* vao, RHIElementBuffer* ebo, const PrimitiveSegment& mesh);

		const RHIState& getState() const { return m_state; }

	private:
		Window* m_window;
		RHIState m_state;   //状态缓存

		struct Impl;
		Impl* m_impl;	//标记glclear
	};
}

#define RENDER_SCOPE(name) volucris::RenderScope V_UNIQUE_NAME(__renderScope_) = volucris::RenderScope(#name);

#endif // !__volucris_rhi_commmand_list_h__
