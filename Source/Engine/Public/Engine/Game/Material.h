#ifndef __volucris_material_h__
#define __volucris_material_h__

#include <Engine/Game/GameObject.h>
#include <Engine/Game/MaterialParameter.h>
#include <Engine/Render/MaterialParameterInfo.h>

namespace volucris
{
	class MaterialProxy;
	class MaterialInstanceProxy;

	class Material : public GameObject
	{
		RTTR_ENABLE(GameObject)

	public:
		Material();

		Material(std::string vss, std::string fss);

		void setSource(std::string vss, std::string fss)
		{
			m_vss = std::move(vss);
			m_fss = std::move(fss);
		}

		template <class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::base_object<GameObject>(*this);
			ar& m_vss;
			ar& m_fss;
			ar& m_parameters;

			if (Archive::is_loading::value)
			{
				for (auto& param : m_parameters)
				{
					if (param.type == MaterialParamterType::Texture2D)
					{
						auto& texture = std::get<SoftObject<Texture2D>>(param.value);
						texture.tryLoad();
					}
				}
			}
		}

		std::string getClassName() const { return "Material"; }

		void setParameters(const std::vector<MaterialParameter>& parameters)
		{
			m_parameters = parameters;
			markDirty(true);
		}

		const std::vector<MaterialParameter>& getParameters() const { return m_parameters; }

		std::shared_ptr<MaterialProxy> getBaseProxy();

		std::shared_ptr<MaterialInstanceProxy> getMaterialProxy();

		std::shared_ptr<MaterialInstanceProxy> tryGetMaterialProxy() const;

		virtual Material* getBaseMaterial()
		{
			return this;
		}

	protected:
		virtual std::shared_ptr<MaterialInstanceProxy> createMaterialProxy();

	private:
		std::vector<MaterialParameterInfo> getParameterInfos() const;

	private:
		std::string m_vss;
		std::string m_fss;
		std::vector<MaterialParameter> m_parameters;
		std::weak_ptr<MaterialProxy> m_proxy;
		std::weak_ptr<MaterialInstanceProxy> m_matProxy;
	};
}  

BOOST_CLASS_EXPORT_KEY(volucris::Material)

#endif // !__volucris_material_h__
