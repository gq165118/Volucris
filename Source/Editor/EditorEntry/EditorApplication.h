#ifndef __volucris_editor_application_h__
#define __volucris_editor_application_h__

#include <Engine/Application/Application.h>
#include <Engine/Game/World.h>
#include <Engine/Asset/AssetData.h>

namespace volucris
{
	class EditorWindow;
	class ContentBrowserWidget;

	class EditorWorld : public World
	{
	public:
		EditorWorld();
	};

	class EditorApplication : public Application
	{
	public:
		EditorApplication();

		EditorWorld* getEditorWorld() { return m_world.get(); }

		bool openEditor(const AssetData& assetData);

	private:
		std::unique_ptr<EditorWorld> m_world;
		std::weak_ptr<EditorWindow> m_materialEditorWindow;
	};
}

#define GEditorWorld ((EditorApplication*)gApp)->getEditorWorld()

#endif // !__volucris_editor_application_h__
