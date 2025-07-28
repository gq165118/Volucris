#ifndef __volucris_material_parameter_info_h__
#define __volucris_material_parameter_info_h__

#include <variant>
#include <string>
#include <Engine/Core/TypesHelp.h>
#include <Engine/Core/GlmHelp.h>
#include <boost/serialization/variant.hpp>

namespace volucris
{
	enum class MaterialParamterType
	{
		Float,
		Vector4,
		Mat4
	};

	struct MaterialParameterUpdateInfo
	{
		size_t id = 0;
		MaterialParamterType type = MaterialParamterType::Float;
		std::variant<float, glm::vec4, glm::mat4> value;
	};

	struct MaterialParameterInfo
	{
		std::string name;
		MaterialParamterType type = MaterialParamterType::Float;
		std::variant<float, glm::vec4, glm::mat4> value;

		template <class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& name;
			ar& type;
			ar& value;
		}
	};
}

#endif // !__volucris_material_parameter_info_h__
