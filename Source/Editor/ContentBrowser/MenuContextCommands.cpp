#include "MenuContextCommands.h"
#include "ContentBrowser/ContentItemWidget.h"
#include "ContentWidget.h"
#include "AssetMenuContext.h"
#include <Engine/Game/MaterialInstance.h>
#include <EditorCore/Editor.h>
#include <EditorEntry/EditorWindow.h>
#include <MaterialEditor/MaterialEditorWidget.h>
#include <Engine/Application/Application.h>
#include <Engine/Game/Package.h>

namespace volucris
{
	MenuContextCommand::MenuContextCommand()
	{
	}

	RenameCommand::RenameCommand(ItemContext* context)
		: MenuContextCommand()
		, m_context(context)
	{
	}

	void RenameCommand::execute()
	{
		m_context->getItemWidget()->setEditing(true);
	}

	DeleteFolderCommand::DeleteFolderCommand(FolderContext* context)
		: MenuContextCommand()
		, m_context(context)  
	{

	}

	void DeleteFolderCommand::execute()
	{
		const auto& path = m_context->getFullPath();
		const auto& assets = gAssetTool.getAssetsInfoInFolder(path, true);
		for (const auto& asset : assets)
		{
			AssetManager::getInstance().unregister(asset.data.path);
		}

		gFileSystem.deleteDirectory(path);
	}

	DeleteAssetCommand::DeleteAssetCommand(AssetContext* context)
		: MenuContextCommand()
		, m_context(context)
	{
	}

	void DeleteAssetCommand::execute()
	{
		AssetManager::getInstance().unregister(m_context->getFullPath());
	}

	CreateMaterialInstanceCommand::CreateMaterialInstanceCommand(MaterialContext* context)
		: MenuContextCommand()
		, m_context(context)
	{
	}

	CreateMaterialInstanceCommand::CreateMaterialInstanceCommand(MaterialInstanceContext* context)
		: MenuContextCommand()
		, m_context(context)
	{
	}

	void CreateMaterialInstanceCommand::execute()
	{
		auto packageName = m_context->getAssetInfo().data.path;
		auto parent = SoftObject<Material>(packageName);
		if (parent.tryLoad())
		{
			auto materialInstance = std::make_shared<MaterialInstance>();
			materialInstance->setMaterial(parent);
			const auto folder = m_context->getContentWidget()->getCurrentFolder();
			const auto instPackageName = AssetTool::getDefaultPackageName(folder, fmt::format("{}_Inst", m_context->getAssetName()));
			auto package = std::make_shared<Package>(instPackageName);
			package->setObject(materialInstance);
			AssetManager::getInstance().registry(package.get());
		}
		else
		{
			V_LOG_ERROR(Editor, "Failed to create material instance: Parent material not found.");
		}
	}

	SaveAssetCommand::SaveAssetCommand(AssetContext* context)
		: MenuContextCommand()
		, m_context(context)
	{

	}

	void SaveAssetCommand::execute()
	{
		if (!m_context->isDirty())
		{
			return;
		}
		auto object = m_context->getAssetInfo().object;
		auto package = std::make_shared<Package>(m_context->getAssetInfo().data.path);
		package->setAssetData(m_context->getAssetInfo().data);
		package->setObject(object);
		gAssetTool.save(package);
	}

	OpenMaterialInstanceEditorCommand::OpenMaterialInstanceEditorCommand(MaterialInstanceContext* context)
		: MenuContextCommand()
		, m_context(context)
	{
	}

	void OpenMaterialInstanceEditorCommand::execute()
	{
		auto materialInstance = SoftObject<MaterialInstance>(m_context->getAssetInfo().data.path);

		auto window = std::make_shared<EditorWindow>();
		window->setTitle("Material Editor");

		auto widget = std::make_shared<MaterialEditorWidget>();
		window->addChild(widget);

		widget->setMaterial(materialInstance);

		gApp->pushCommand([this, window]() {gApp->addWindow(window); });
	}
}
