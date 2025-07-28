#ifndef __volucris_material_template_h__
#define __volucris_material_template_h__

#include <Engine/Game/GameObject.h>
#include <Engine/Game/Material.h>
#include "GLSLParser.h"

namespace volucris
{
	class MaterialTemplate : public Material
	{
	public:
		MaterialTemplate();

		MaterialTemplate(std::string vss, std::string fss);

		template <class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::base_object<Material>(*this);
			ar& m_uniforms;
			ar& m_uniformBlocks;
		}

		bool findProperty(const std::string& name, UniformProperty& property);

	private:
		bool parseTemplate(std::string vss, std::string fss);

		void updateUniforms();

		static float getDefaultFloatValue(const std::string& str);

	private:
		std::vector<UniformVariable> m_uniforms;
		std::vector<UniformBlock> m_uniformBlocks;
	};
}

BOOST_CLASS_EXPORT_KEY(volucris::MaterialTemplate)

#endif // !__volucris_material_template_h__
