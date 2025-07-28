#include "EditorApplication.h"
#include <Engine/Application/Window.h>
#include <Engine/Render/ImGuiRenderer.h>
#include <MaterialEditor/MaterialEditorWidget.h>
#include <imgui/imgui.h>
#include <Engine/FileSystem/FileSystem.h>
#include <EditorEntry/EditorWindow.h>
#include <Engine/Asset/AssetManager.h>
#include <MaterialEditor/MaterialTemplate.h>
#include <Engine/Game/MaterialInstance.h>

namespace volucris
{
	EditorWorld::EditorWorld()
		: World()
	{

	}

	EditorApplication::EditorApplication()
		: Application()
		, m_world(std::make_unique<EditorWorld>())
		, m_materialEditorWindow()
	{
	}

	bool EditorApplication::openEditor(const AssetData& assetData)
	{
		if (assetData.className == "Material")
		{
			std::shared_ptr<EditorWindow> window = nullptr;
			if (!m_materialEditorWindow.expired())
			{
				window = m_materialEditorWindow.lock();
			}

			if (!window)
			{
				window = std::make_shared<EditorWindow>();
				window->setTitle("Material Editor");
			}

			auto widget = std::make_shared<MaterialEditorWidget>();
			window->addChild(widget);
			m_materialEditorWindow = window;

			auto inst = std::make_shared<MaterialInstance>();
			inst->setMaterial(SoftObject<Material>(assetData.path));
			widget->setMaterial(inst);

			pushCommand([this, window]() {addWindow(window); });

			return true;
		}
		return false;
	}
}

using namespace volucris;

std::shared_ptr<volucris::Application> volucrisEntry(int argc, char* argv[])
{
	// 仅在测试时使用
	gFileSystem.mount("/Engine/Resource", fmt::format("{}/Resource", VOLUCRIS_ENGINE_ROOT));
	gFileSystem.mount("/Engine/Shader", fmt::format("{}/Source/Shader", VOLUCRIS_ENGINE_ROOT));
	
	auto app = std::make_shared<EditorApplication>();
	auto window = std::make_shared<EditorWindow>();
	window->setTitle("Volucris Editor");
	app->addWindow(window);

	auto widget = std::make_shared<MainWidget>();
	window->addChild(widget);

	return app;
}