#ifndef __volucris_material_object_helper_h__
#define __volucris_material_object_helper_h__

#include "AssetObjectHelper.h"
#include <memory>
#include <Engine/Game/Material.h>
#include <Engine/Game/MaterialInstance.h>

namespace volucris
{
	class MaterialObjectHelper : public AssetObjectHelper
	{
	public:
		MaterialObjectHelper(const std::shared_ptr<Material>& material)
			: m_material(material)
		{
		}

		virtual ~MaterialObjectHelper() = default;

		virtual std::vector<std::string> getDependences() const override;

		virtual bool updateDependences(const std::map<std::string, std::string>& dependence) override;

	private:
		std::shared_ptr<Material> m_material;
	};

	class MaterialInstanceObjectHelper : public AssetObjectHelper
	{
		public:
		MaterialInstanceObjectHelper(const std::shared_ptr<MaterialInstance>& materialInstance)
			: m_materialInstance(materialInstance)
		{
		}
		virtual ~MaterialInstanceObjectHelper() = default;

		virtual std::vector<std::string> getDependences() const override;
		
		virtual bool updateDependences(const std::map<std::string, std::string>& dependence) override;

	private:
		std::shared_ptr<MaterialInstance> m_materialInstance;
	};
}

#endif // !__volucris_material_object_helper_h__
