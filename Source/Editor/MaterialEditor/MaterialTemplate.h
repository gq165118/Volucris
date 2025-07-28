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

		void setMaterialSourcePath(const std::string& vsp, const std::string fsp)
		{
			m_vertexSourcePath = vsp;
			m_fragmentSourcePath = fsp;
		}

		const std::string& getVertexSourcePath() const { return m_vertexSourcePath; }

		const std::string& getFragmentSourcePath() const { return m_fragmentSourcePath; }

		void setMaterialSource(const std::string& vss, const std::string& fss);

		template <class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::base_object<Material>(*this);
			ar& m_vertexSourcePath;
			ar& m_fragmentSourcePath;
			ar& m_uniforms;
			ar& m_uniformBlocks;
		}

		bool findProperty(const std::string& name, UniformProperty& property);

	private:
		bool parseTemplate(std::string vss, std::string fss);

		void updateUniforms();

		static float getDefaultFloatValue(const std::string& str);

	private:
		std::string m_vertexSourcePath;
		std::string m_fragmentSourcePath;
		std::vector<UniformVariable> m_uniforms;
		std::vector<UniformBlock> m_uniformBlocks;
	};
}

BOOST_CLASS_EXPORT_KEY(volucris::MaterialTemplate)

#endif // !__volucris_material_template_h__
