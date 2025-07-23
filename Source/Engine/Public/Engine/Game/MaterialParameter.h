#ifndef __volucris_material_parameter_h__
#define __volucris_material_parameter_h__

#include <string>
#include <glm/glm.hpp>
#include <Engine/Render/MaterialParameterInfo.h>
#include <glm/ext.hpp>
#include <Engine/Core/GlmHelp.h>

namespace volucris
{
	namespace MaterialParameterHelper
	{
		static MaterialParameterInfo getParameterInfo(const std::string& name, const float& value)
		{
			MaterialParameterInfo info;
			info.name = name;
			info.type = MaterialParamterType::Float;
			info.value = value;
			return info;
		}

		static MaterialParameterInfo getParameterInfo(const std::string& name, const glm::vec4& value)
		{
			MaterialParameterInfo info;
			info.name = name;
			info.type = MaterialParamterType::Vector4;
			info.value = value;
			return info;
		}

		static MaterialParameterUpdateInfo getParameterUpdateInfo(size_t id, const float& value)
		{
			MaterialParameterUpdateInfo info;
			info.id = id;
			info.type = MaterialParamterType::Float;
			info.value = value;
			return info;
		}

		static MaterialParameterUpdateInfo getParameterUpdateInfo(size_t id, const glm::vec4& value)
		{
			MaterialParameterUpdateInfo info;
			info.id = id;
			info.type = MaterialParamterType::Vector4;
			info.value = value;
			return info;
		}
	}

	template <typename T>
	class MaterialParameterTemplate
	{
		std::string m_name;
		T m_value;
		bool m_dirty;
		size_t m_id;

	public:
		MaterialParameterTemplate()
			: m_name()
			, m_value()
			, m_dirty(false)
			, m_id(0)
		{

		}

		MaterialParameterTemplate(const std::string& name, const T& value=T())
			: m_name(name)
			, m_value(value)
			, m_dirty(false)
			, m_id(0)
		{

		}

		void setId(size_t id) { m_id = id; }

		void setValue(const T& value)
		{
			m_value = value;
			m_dirty = true;
		}

		void markDirty(bool dirty) { m_dirty = dirty; }

		bool isDirty() const { return m_dirty; }

		const std::string getName() const { return m_name; }

		template <class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& m_name;
			ar& m_value;
		}

		MaterialParameterInfo getParameterInfo() const
		{
			return MaterialParameterHelper::getParameterInfo(m_name, m_value);
		}

		MaterialParameterUpdateInfo getUpdateInfo() const
		{
			return MaterialParameterHelper::getParameterUpdateInfo(m_id, m_value);
		}

	};

	using MaterialFloatParameter = MaterialParameterTemplate<float>;
	using MaterialVector4Parameter = MaterialParameterTemplate<glm::vec4>;
}

#endif // !__volucris_material_parameter_h__