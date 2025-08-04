#ifndef __volucris_editor_window_h__
#define __volucris_editor_window_h__

#include <Engine/Application/Window.h>
#include <Engine/Core/TextureDefines.h>

namespace volucris
{
	class ViewportWidget;
	class LogWidget;
	class ContentBrowserWidget;
	class RHITexture2D;
	class StaticMesh;

	class MainWidget : public Widget
	{
	public:
		MainWidget();

	protected:
		void onBuild(bool init) override;

	private:
		void buildMenubar();

	private:
		std::shared_ptr<ViewportWidget> m_viewport;
		std::shared_ptr<LogWidget> m_logWidget;
		std::shared_ptr<ContentBrowserWidget> m_contentBrowserWidget;
	};

	class EditorWindow : public Window
	{
	public:
		EditorWindow();

		std::shared_ptr<RHITexture2D> getEditorIconTexture() const;

		StaticMesh* getQuadMesh() const;

		void onRendererBuild(RHICommandList* cmdList) override;

		void onRendererDestroy(RHICommandList* cmdList) override;

		//������
		void setViewData(Texture::TextureData data);

	private:
		std::shared_ptr<RHITexture2D> m_iconTexture;
		std::unique_ptr<StaticMesh> m_quadMesh;

		RHICommandList* m_cmdList;
		std::string m_iniFileName;
	};
}

#endif // !__volucris_editor_window_h__
