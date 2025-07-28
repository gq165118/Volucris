#include "MaterialParameterWidget.h"
#include <imgui/imgui.h>
#include "MaterialTemplate.h"
#include <Engine/Game/MaterialInstance.h>
#include <Common/AssetItemWidget.h>
  
namespace volucris
{
	MaterialParameterWidget::MaterialParameterWidget()
		: Widget()
		, m_material(nullptr)
	{
	}

	void MaterialParameterWidget::setMaterial(const std::shared_ptr<MaterialInstance>& material)
	{
		m_material = material;
		m_parameters.clear();
		for (const auto& parameter : material->getInstanceParameters())
		{
			UniformProperty property;
			std::dynamic_pointer_cast<MaterialTemplate>(material->getMaterial().object())->findProperty(parameter.name, property);
			auto it = m_parameters.find(property.group);
			Parameter param;
			param.desc = parameter;
			param.property = std::move(property);
			if (param.property.displayName.empty())
			{
				param.name = fmt::format("{}: ", parameter.name);
			}
			else
			{
				param.name = fmt::format("{}: ", param.property.displayName);
			}
			if (it == m_parameters.end())
			{
				std::string groupName = param.property.group;
				m_parameters.insert({ std::move(groupName), {std::move(param)}});
			}
			else
			{
				it->second.push_back(std::move(param));
			}
		}
	}

	void MaterialParameterWidget::onBuild(bool)
	{
		ImGui::Begin("ParameterWidget");

		size_t idx = 0;
		for (auto& [group, parameters] : m_parameters)
		{
			if (ImGui::CollapsingHeader(group.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				for (auto& paramter : parameters)
				{
					auto& info = paramter.desc;
					auto& property = paramter.property;

					ImGui::Text(paramter.name.c_str());
					ImGui::SameLine();

					ImGui::PushID(idx);
					auto type = paramter.desc.type;
					switch (type)
					{
					case volucris::MaterialParamterType::Float:
						if (ImGui::DragFloat("##", std::get_if<float>(&info.value), 0.1f))
						{
							m_material->setFloatParameter(info.name, std::get<float>(info.value));
						}
						break;
					case volucris::MaterialParamterType::Vector4:
						if (ImGui::DragFloat4("##", glm::value_ptr(std::get<glm::vec4>(info.value)), 0.1f))
						{
							m_material->setVector4Parameter(info.name, std::get<glm::vec4>(info.value));
						}
						break;
					case volucris::MaterialParamterType::Mat4:
						break;
					case volucris::MaterialParamterType::Texture2D:
					{
						AssetData asset;
						asset.className = "Texture2D";
						asset.path = std::get<SoftObject<Texture2D>>(info.value).getPath();
						AssetItemWidget assetWidget(info.name);
						assetWidget.setAssetData(asset);
						assetWidget.build();
						if (assetWidget.isSelected())
						{
							paramter.desc.value = assetWidget.getAssetData().path;
							m_material->setTexture2DParameter(info.name, assetWidget.getAssetData().path);
						}
					}
						break;
					default:
						break;
					}
					ImGui::PopID();
					idx++;
				}
			}
		}

		if (ImGui::CollapsingHeader("Property", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Parent:");
			ImGui::SameLine();
			AssetData asset;
			asset.className = "Material";
			asset.path = m_material->getMaterial().getPath();
			AssetItemWidget assetWidget("");
			assetWidget.setAssetData(asset);
			assetWidget.build();
			if (assetWidget.isSelected())
			{
				m_material->setMaterial(assetWidget.getAssetData().path);
				setMaterial(m_material);
			}
		}

		ImGui::End();
	}
}
