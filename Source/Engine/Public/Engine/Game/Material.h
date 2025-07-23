#ifndef __volucris_material_h__
#define __volucris_material_h__

#include <Engine/Game/GameObject.h>
#include "MaterialParameter.h"

namespace volucris
{
	class MaterialProxy;

	class Material : public GameObject
	{
	public:
		Material();

		Material(std::string vss, std::string fss);

		template <class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::base_object<GameObject>(*this);
			ar& m_vss;
			ar& m_fss;
			ar& m_floatParameters;
			ar& m_vec4Parameters;
		}

		bool setFloatParameter(const std::string& name, float value);

		bool setVector4Parameter(const std::string& name, const glm::vec4& value);

		std::string getClassName() const override { return "Material"; }

		std::shared_ptr<MaterialProxy> getProxy();

	protected:
		MaterialFloatParameter& addParameter(const std::string& name, float value);

		MaterialVector4Parameter& addParameter(const std::string& name, glm::vec4 value);

		void clearParameters()
		{
			m_floatParameters.clear();
			m_vec4Parameters.clear();
		}

	private:
		std::string m_vss;
		std::string m_fss;
		std::vector<MaterialFloatParameter> m_floatParameters;
		std::vector<MaterialVector4Parameter> m_vec4Parameters;
		std::weak_ptr<MaterialProxy> m_proxy;
	};
}

BOOST_CLASS_EXPORT_KEY(volucris::Material)

#endif // !__volucris_material_h__
