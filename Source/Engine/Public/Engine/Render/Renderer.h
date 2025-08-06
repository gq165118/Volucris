#ifndef __volucris_renderer_h__
#define __volucris_renderer_h__

#include <Engine/Core/Runable.h>
#include <Engine/Game/MaterialInstance.h>

namespace volucris
{
	class Window;
	class BaseView;
	class RHICommandList;

	class Renderer : public Runable
	{
	public:
		~Renderer();

		static Renderer& getInstance()
		{
			static Renderer inst;
			return inst;
		}

		void pushRenderCommand(std::function<void(RHICommandList*)> command, bool block = true);

		RHICommandList* getRenderCommand() { return m_cmdList.get(); }

		void addView(std::unique_ptr<BaseView> view);

		void updateMaterialParameters(std::vector<MaterialUpdateData> datas);

		void removeView(BaseView* view);

		void renderFrame() { main(); }

	protected:
		Renderer();
		void main();

	private:
		friend class Application;
		void run() override;

		bool initialize() override;
		   
		void destroy() override;

	protected:
		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) = delete;

	private:
		std::unique_ptr<Window> m_window;
		std::vector<std::unique_ptr<BaseView>> m_views;
		std::unique_ptr<RHICommandList> m_cmdList;
	};
}

#define RHICmdList volucris::Renderer::getInstance().getRenderCommand()

#define ENQUEUE_RENDER_COMMAND(cmd) \
	volucris::Renderer::getInstance().push(cmd, true);

#endif // !__volucris_renderer_h__
