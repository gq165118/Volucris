#ifndef __volucris_material_proxy_h__
#define __volucris_material_proxy_h__

#include <Engine/Core/Object.h>
#include <Engine/Render/MaterialParameterInfo.h>
#include <string>
#include <vector>

namespace volucris
{
	class RHIUniform;
	class RHIUniformBlock;
	class RHIProgram;
	class RHICommandList;

	class MaterialProxy : public Object
	{
	public:
		MaterialProxy();

		void setSource(const std::string& vss, const std::string& fss);

		void setParameters(const std::vector<MaterialParameterInfo>& parameters);

		const std::vector<MaterialParameterInfo>& getParameters() const { return m_parameters; }

		RHIProgram* getProgram() const;

		RHIUniform* getUniform(size_t index)
		{
			return m_uniforms[index].get();
		}

	private:
		std::unique_ptr<RHIProgram> m_program;
		std::vector<MaterialParameterInfo> m_parameters;
		std::vector<std::unique_ptr<RHIUniform>> m_uniforms;
		std::vector<std::unique_ptr<RHIUniformBlock>> m_uniformBlocks;
	};
}

#endif // !__volucris_material_proxy_h__
