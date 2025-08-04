#ifndef __volucris_asset_command_h__
#define __volucris_asset_command_h__

namespace volucris
{
	class ItemContext;
	class AssetContext;
	class FolderContext;
	class MaterialContext;
	class MaterialInstanceContext;

	class MenuContextCommand
	{
	public:
		MenuContextCommand();

		virtual ~MenuContextCommand() = default;

		virtual void execute() = 0;

	};

	class RenameCommand : public MenuContextCommand
	{
	public:
		RenameCommand(ItemContext* context);

		void execute() override;

	private:
		ItemContext* m_context;
	};

	class DeleteFolderCommand : public MenuContextCommand
	{
	public:
		DeleteFolderCommand(FolderContext* context);

		void execute() override;

	private:
		FolderContext* m_context;
	};

	class SaveAssetCommand : public MenuContextCommand
	{
	public:
		SaveAssetCommand(AssetContext* context);

		void execute() override;

	private:
		AssetContext* m_context;
	};

	class DeleteAssetCommand : public MenuContextCommand
	{
	public:
		DeleteAssetCommand(AssetContext* context);

		void execute() override;

	private:
		AssetContext* m_context;
	};

	class CreateMaterialInstanceCommand : public MenuContextCommand
	{
	public:
		CreateMaterialInstanceCommand(MaterialContext* context);

		CreateMaterialInstanceCommand(MaterialInstanceContext* context);
		
		void execute() override;

	private:
		AssetContext* m_context;
	};

	class OpenMaterialInstanceEditorCommand : public MenuContextCommand
	{
	public:
		OpenMaterialInstanceEditorCommand(MaterialInstanceContext* context);

		void execute() override;

	private:
		MaterialInstanceContext* m_context;
	};
}

#endif // !__volucris_asset_command_h__
