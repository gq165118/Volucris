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

		void makesureBufferUnset(RHIBuffer::Type type);

		void setVertexArray(RHIVertexArray* array);

		void unsetVertexArray(RHIVertexArray* array);

		void setProgram(RHIProgram* program);

		void unsetProgram(RHIProgram* program);

		void drawPrimitive(RHIVertexArray* vao, RHIElementBuffer* ebo, const PrimitiveSegment& mesh);

		const RHIState& getState() const { return m_state; }

	private:
		Window* m_window;
		RHIState m_state;

		struct Impl;
		Impl* m_impl;
	};
}

#define RENDER_SCOPE(name) volucris::RenderScope V_UNIQUE_NAME(__renderScope_) = volucris::RenderScope(#name);

#endif // !__volucris_rhi_commmand_list_h__
