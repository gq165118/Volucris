#ifndef __volucris_material_parameter_widget_h__
#define __volucris_material_parameter_widget_h__

#include <Engine/Application/Widget.h>
#include <unordered_map>
#include <Engine/Render/MaterialParameterInfo.h>
#include "GLSLParser.h"

namespace volucris
{
	class MaterialTemplate;

	class MaterialParameterWidget : public Widget
	{
	public:
		MaterialParameterWidget();

		void setMaterial(const std::shared_ptr<MaterialTemplate>& material);

		void onBuild(bool) override;

	private:
		struct Parameter
		{
			std::string name;
			MaterialParameterInfo info;
			UniformProperty property;
		};

		std::shared_ptr<MaterialTemplate> m_material;
		std::unordered_map<std::string, std::vector<Parameter>> m_parameters;
	};
}

#endif // !__volucris_material_parameter_widget_h__
