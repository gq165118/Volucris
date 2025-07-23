#ifndef __volucris_material_proxy_h__
#define __volucris_material_proxy_h__

#include <Engine/Core/Object.h>
#include <Engine/Render/MaterialParameterInfo.h>
#include <string>
#include <vector>

namespace volucris
{
	class RHIUniform;
	class RHIProgram;
	class RHICommandList;

	class MaterialProxy : public Object
	{
	public:
		MaterialProxy();

		void setSource(const std::string& vss, const std::string& fss);

		void setParameters(const std::vector<MaterialParameterInfo>& parameters);

		void update(const std::vector<MaterialParameterUpdateInfo>& parameters);

		RHIProgram* getProgram() const;

		void use(RHICommandList* context);

	private:
		std::unique_ptr<RHIProgram> m_program;
		std::vector<std::unique_ptr<RHIUniform>> m_uniforms;
	};
}

#endif // !__volucris_material_proxy_h__
