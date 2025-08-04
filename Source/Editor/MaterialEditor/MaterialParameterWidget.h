#ifndef __volucris_material_parameter_widget_h__
#define __volucris_material_parameter_widget_h__

#include <Engine/Application/Widget.h>
#include <unordered_map>
#include <Engine/Game/MaterialInstance.h>
#include <Engine/Game/MaterialParameter.h>
#include "GLSLParser.h"

namespace volucris
{
	class MaterialTemplate;
	class MaterialInstance;

	class MaterialParameterWidget : public Widget
	{
	public:
		MaterialParameterWidget();

		void setMaterial(const SoftObject<MaterialInstance>& material);

		void onBuild(bool) override;

	private:
		struct Parameter
		{
			std::string name;
			MaterialParameter desc;
			UniformProperty property;
		};

		SoftObject<MaterialInstance> m_material;
		std::unordered_map<std::string, std::vector<Parameter>> m_parameters;
	};
}

#endif // !__volucris_material_parameter_widget_h__
