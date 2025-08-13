#ifndef __volucris_material_instance_h__
#define __volucris_material_instance_h__

#include <Engine/Game/MaterialParameter.h>
#include <Engine/Render/MaterialParameterInfo.h>
#include <Engine/Game/Material.h>
#include <Engine/Game/SoftObject.h>

namespace volucris
{
	class MaterialProxy;

	struct MaterialUpdateData
	{
		std::shared_ptr<MaterialInstanceProxy> proxy = nullptr;
		std::vector<MaterialParameterUpdateInfo> infos;

		bool isValid() const
		{
			return proxy && !infos.empty();
		}
	};

	class MaterialInstance : public Material
	{
	public:
		MaterialInstance();

		MaterialInstance(const SoftObject<Material>& material);

		void setMaterial(const SoftObject<Material>& material);

		const SoftObject<Material>& getMaterial() const { return m_material;  }

		Material* getBaseMaterial() override;

		template <class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::base_object<Material>(*this);
			ar& m_material;
			ar& m_floatParameters;
			ar& m_vec4Parameters;
			ar& m_texture2dParameters;
			if (Archive::is_loading::value)
			{
				m_material.tryLoad();

				for (auto& textureParameter : m_texture2dParameters)
				{
					textureParameter.load();
				}
			}
		}

		bool setFloatParameter(const std::string& name, float value);

		bool setIntParameter(const std::string& name, int value);

		bool setVector4Parameter(const std::string& name, const glm::vec4& value);

		bool setMat4Parameter(const std::string& name, const glm::mat4& value);

		bool setTexture2DParameter(const std::string& name, const SoftObject<Texture2D>& value);

		std::string getClassName() const override { return "MaterialInstance"; }

		std::shared_ptr<MaterialProxy> getProxy();

		const std::vector<MaterialFloatParameter>& getFloatParamters() const { return m_floatParameters; }

		const std::vector<MaterialVector4Parameter>& getVec4Paramters() const { return m_vec4Parameters; }

		const std::vector<MaterialTexture2DParameter>& getTexture2DParamters() const { return m_texture2dParameters; }

		std::vector<MaterialParameter> getInstanceParameters() const;

		MaterialUpdateData getUpdateData();

	protected:
		std::vector<MaterialParameterUpdateInfo> getUpdateParameterInfos();

		std::vector<MaterialParameterUpdateInfo> getAllUpdateParameterInfos();

		std::shared_ptr<MaterialInstanceProxy> createMaterialProxy() override;

	private:
		SoftObject<Material> m_material;
		std::vector<MaterialFloatParameter> m_floatParameters;
		std::vector<MaterialIntParameter> m_intParameters;
		std::vector<MaterialVector4Parameter> m_vec4Parameters;
		std::vector<MaterialMat4Parameter> m_mat4Parameters;
		std::vector<MaterialTexture2DParameter> m_texture2dParameters;
		std::weak_ptr<MaterialProxy> m_proxy;
	};
}

BOOST_CLASS_EXPORT_KEY(volucris::MaterialInstance)

#endif // !__volucris_material_h__
