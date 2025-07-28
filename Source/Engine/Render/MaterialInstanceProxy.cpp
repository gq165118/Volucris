#include "Render/MaterialInstanceProxy.h"
#include <Core/Assert.h>
#include <RHI/RHICommandList.h>

namespace volucris
{
	MaterialInstanceProxy::MaterialInstanceProxy()
		: m_material(nullptr)
	{
		
	}

	void MaterialInstanceProxy::setMaterial(const std::shared_ptr<MaterialProxy>& material)
	{
		m_material = material;
		m_parameters = m_material->getParameters();
	}

	void MaterialInstanceProxy::update(const std::vector<MaterialParameterUpdateInfo>& parameters)
	{
		for (const auto& parameter : parameters)
		{
			auto idx = parameter.id;
			v_check(m_parameters[idx].type == parameter.type);
			m_parameters[idx].value = parameter.value;
		}
	}

	void MaterialInstanceProxy::use(RHICommandList* context)
	{
		context->setProgram(m_material->getProgram());
		for (auto id = 0; id < m_parameters.size(); ++id)
		{
			const auto& parameter = m_parameters[id];
			auto type = parameter.type;
			const auto& uniform = m_material->getUniform(id);
			switch (type)
			{
			case volucris::MaterialParamterType::Float:
				static_cast<RHIUniformFloat*>(uniform)->setValue(std::get<float>(parameter.value));
				break;
			case volucris::MaterialParamterType::Vector4:
				static_cast<RHIUniformVec4*>(uniform)->setValue(std::get<glm::vec4>(parameter.value));
				break;
			case volucris::MaterialParamterType::Mat4:
				static_cast<RHIUniformMat4*>(uniform)->setValue(std::get<glm::mat4>(parameter.value));
				break;
			default:
				break;
			}
			uniform->upload();
		}
	}
}
