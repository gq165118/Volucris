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

		auto texSlot = 0;
		for (auto id = 0; id < m_parameters.size(); ++id)
		{
			if (m_parameters[id].type == MaterialParamterType::Texture2D)
			{
				auto texture = std::get<std::shared_ptr<Texture2DProxy>>(m_parameters[id].value);
				auto uniform = static_cast<RHIUniformInt*>(m_material->getUniform(id));
				uniform->setValue(texSlot++);
			}
		}
	}

	void MaterialInstanceProxy::update(const std::vector<MaterialParameterUpdateInfo>& parameters)
	{
		for (const auto& parameter : parameters)
		{
			auto idx = parameter.id;
			v_check(m_parameters[idx].type == parameter.type);
			m_parameters[idx].value = parameter.value;
			const auto& uniform = m_material->getUniform(idx);
			switch (parameter.type)
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
		}
	}

	void MaterialInstanceProxy::use(RHICommandList* context)
	{
		context->setProgram(m_material->getProgram());
		auto texId = 0;
		for (auto id = 0; id < m_parameters.size(); ++id)
		{
			const auto& parameter = m_parameters[id];
			if (parameter.type == MaterialParamterType::Texture2D)
			{
				if (auto texture = std::get<std::shared_ptr<Texture2DProxy>>(parameter.value))
				{
					context->bindTexture2D(texId, texture->getTexture().get());
				}
				++texId;
			}
			m_material->getUniform(id)->upload();
		}
	}
}
