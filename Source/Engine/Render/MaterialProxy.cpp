#include "Render/MaterialProxy.h"
#include <RHI/RHIProgram.h>
#include <RHI/RHIShader.h>
#include <RHI/RHICommandList.h>
#include <Render/Renderer.h>
#include <RHI/RHIUniform.h>

namespace volucris
{
	MaterialProxy::MaterialProxy()
		: Object()
		, m_program(nullptr)
	{
	}

	void MaterialProxy::setSource(const std::string& vss, const std::string& fss)
	{
		auto vs = std::make_shared<RHIShader>(RHIShader::VertexShader);
		auto fs = std::make_shared<RHIShader>(RHIShader::FragmentShader);
		if (vs->init(vss) && fs->init(fss))
		{
			m_program = std::make_unique<RHIProgram>();
			m_program->init({ vs, fs });
		}
	}

	void MaterialProxy::setParameters(const std::vector<MaterialParameterInfo>& parameters)
	{
		m_parameters = parameters;
		int texIdx = 0;
		for (const auto& parameter : parameters)
		{
			auto type = parameter.type;
			std::unique_ptr<RHIUniform> uniform;
			switch (type)
			{
			case volucris::MaterialParamterType::Float:
			{
				auto uni = std::make_unique<RHIUniformFloat>();
				uni->setValue(std::get<float>(parameter.value));
				uniform = std::move(uni);
			}
				break;
			case volucris::MaterialParamterType::Vector4:
			{
				auto uni = std::make_unique<RHIUniformVec4>();
				uni->setValue(std::get<glm::vec4>(parameter.value));
				uniform = std::move(uni);
			}
				break;
			case volucris::MaterialParamterType::Mat4:
			{
				auto uni = std::make_unique<RHIUniformMat4>();
				uni->setValue(std::get<glm::mat4>(parameter.value));
				uniform = std::move(uni);
			}
				break;
			case volucris::MaterialParamterType::Texture2D:
			{
				auto uni = std::make_unique<RHIUniformInt>();
				uni->setValue(texIdx++);
				uniform = std::move(uni);
			}
				break;
			default:
				v_check(false)
				break;
			}
			uniform->init(m_program.get(), parameter.name);
			m_uniforms.push_back(std::move(uniform));
		}
	}

	RHIProgram* MaterialProxy::getProgram() const
	{
		return m_program.get();
	}
}
