#ifndef __volucris_material_parameter_h__
#define __volucris_material_parameter_h__

#include <string>
#include <glm/glm.hpp>
#include <Engine/Render/MaterialParameterInfo.h>
#include <glm/ext.hpp>
#include <Engine/Core/GlmHelp.h>
#include <Engine/Game/SoftObject.h>
#include <Engine/Game/Texture2D.h>

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

		static MaterialParameterInfo getParameterInfo(const std::string& name, const SoftObject<Texture2D>& value)
		{
			MaterialParameterInfo info;
			info.name = name;
			info.type = MaterialParamterType::Texture2D;
			if (value.object())
			{
				info.value = value->getProxy();
			}
			else
			{
				info.value = std::shared_ptr<Texture2DProxy>(nullptr);
			}
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

		static MaterialParameterUpdateInfo getParameterUpdateInfo(size_t id, const SoftObject<Texture2D>& value)
		{
			MaterialParameterUpdateInfo info;
			info.id = id;
			info.type = MaterialParamterType::Texture2D;
			if (value.object())
			{
				info.value = value->getProxy();
			}
			else
			{
				info.value = std::shared_ptr<Texture2DProxy>(nullptr);
			}
			return info;
		}
	}

	struct MaterialParameter
	{
		std::string name;
		MaterialParamterType type = MaterialParamterType::Float;
		std::variant<float, glm::vec4, glm::mat4, SoftObject<Texture2D>> value;

		template <class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& name;
			ar& type;
			ar& value;
		}

		MaterialParameterInfo getInfo() const
		{
			MaterialParameterInfo info;
			info.name = name;
			info.type = type;
			switch (type)
			{
			case volucris::MaterialParamterType::Float:
				info.value = std::get<float>(value);
				break;
			case volucris::MaterialParamterType::Vector4:
				info.value = std::get<glm::vec4>(value);
				break;
			case volucris::MaterialParamterType::Mat4:
				info.value = std::get<glm::mat4>(value);
				break;
			case volucris::MaterialParamterType::Texture2D:
			{
				auto texture = std::get<SoftObject<Texture2D>>(value);
				if (auto object = texture.tryLoad())
				{
					info.value = object->getProxy();
				}
				else
				{
					info.value = std::shared_ptr<Texture2DProxy>(nullptr);
				}
			}
				break;
			default:
				break;
			}
			return info;
		}
	};

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
			, m_dirty(true)
			, m_id(0)
		{

		}

		MaterialParameterTemplate(const std::string& name, const T& value=T())
			: m_name(name)
			, m_value(value)
			, m_dirty(true)
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

		const T& getValue() const { return m_value; }

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
	using MaterialTexture2DParameter = MaterialParameterTemplate<SoftObject<Texture2D>>;
}

#endif // !__volucris_material_parameter_h__