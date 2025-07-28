#ifndef __volucris_material_parameter_info_h__
#define __volucris_material_parameter_info_h__

#include <variant>
#include <string>
#include <Engine/Core/TypesHelp.h>
#include <Engine/Core/GlmHelp.h>
#include <boost/serialization/variant.hpp>
#include <Engine/Render/Texture2DProxy.h>

namespace volucris
{
	enum class MaterialParamterType
	{
		Float,
		Vector4,
		Mat4,
		Texture2D,
	};

	struct MaterialParameterUpdateInfo
	{
		size_t id = 0;
		MaterialParamterType type = MaterialParamterType::Float;
		std::variant<float, glm::vec4, glm::mat4, std::shared_ptr<Texture2DProxy>> value;
	};

	struct MaterialParameterInfo
	{
		std::string name;
		MaterialParamterType type = MaterialParamterType::Float;
		std::variant<float, glm::vec4, glm::mat4, std::shared_ptr<Texture2DProxy>> value;
	};
}

#endif // !__volucris_material_parameter_info_h__
