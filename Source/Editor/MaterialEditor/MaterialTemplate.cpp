#include "MaterialTemplate.h"
#include "GLSLParser.h"
#include "EditorCore/Editor.h"
#include <Engine/Core/GlmHelp.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <charconv>
#include <Engine/Game/MaterialParameter.h>

namespace volucris
{
	static float stringToFloat(std::string_view str)
	{
		if (str.empty()) { return 0.0f; }
		float value = 0.0f;
		auto result = std::from_chars(str.data(), str.data() + str.size(), value);
		return value;
	}

	static glm::vec4 stringToVec4(std::string_view str)
	{
		glm::vec4 value = { 0,0,0,1 };
		if (str.length() <= 9) {
			return value;
		}

		auto startPos = str.find("(");
		if (startPos == std::string::npos) return value;
		auto endPos = str.find(")");
		if (endPos == std::string::npos) return value;
		++startPos;
		const auto& content = str.substr(startPos, endPos - startPos);

		startPos = 0;
		float* values = glm::value_ptr(value);
		auto idx = 0;
		while ((endPos = content.find(",", startPos)) != std::string::npos && idx < 4)
		{
			values[idx] = stringToFloat(content.substr(startPos, endPos - startPos));
			startPos = endPos + 1;
			++idx;
		}

		return value;
	}

	MaterialTemplate::MaterialTemplate()
		: Material()
		, m_uniforms()
		, m_uniformBlocks()
	{
	}

	MaterialTemplate::MaterialTemplate(std::string vss, std::string fss)
		: Material(vss, fss)
		, m_uniforms()
		, m_uniformBlocks()
	{
		setMaterialSource(vss, fss);
	}

	void MaterialTemplate::setMaterialSource(const std::string& vss, const std::string& fss)
	{
		setSource(vss, fss);
		parseTemplate(vss, fss);
		updateUniforms();
	}

	bool MaterialTemplate::findProperty(const std::string& name, UniformProperty& property)
	{
		for (const auto& uniform : m_uniforms)
		{
			if (uniform.name == name)
			{
				property = uniform.property;
				return true;
			}
		}
		return false;
	}

	bool MaterialTemplate::parseTemplate(std::string vss, std::string fss)
	{
		m_uniforms.clear();
		m_uniformBlocks.clear();

		GLSLParser vsParser;
		GLSLParser fsParser;

		vsParser.parse(vss);

		fsParser.parse(fss);

		std::unordered_map<std::string, UniformVariable> uniforms;
		std::unordered_map<std::string, UniformBlock> uniformBlocks;

		for (const auto& var : vsParser.getUniforms())
		{
			uniforms[var.name] = var;
		}

		for (const auto& var : fsParser.getUniforms())
		{
			auto it = uniforms.find(var.name);
			if (it == uniforms.end())
			{
				uniforms[var.name] = var;
			}
			else if (it->second != var)
			{
				V_LOG_WARN(Editor, "parse material source failed. find different uniform with same name {}", var.name);
			}
		}

		for (const auto& var : vsParser.getUniformBlocks())
		{
			uniformBlocks[var.name] = var;
		}

		for (const auto& var : fsParser.getUniformBlocks())
		{
			auto it = uniformBlocks.find(var.name);
			if (it == uniformBlocks.end())
			{
				uniformBlocks[var.name] = var;
			}
			else if (it->second != var)
			{
				V_LOG_WARN(Editor, "parse material source failed. find different uniform with same name {}", var.name);
			}
		}

		m_uniforms.reserve(uniforms.size());
		m_uniformBlocks.reserve(uniformBlocks.size());

		for (const auto& [_, uniform] : uniforms)
		{
			m_uniforms.push_back(uniform);
		}

		for (const auto& [_, uniform] : uniformBlocks)
		{
			m_uniformBlocks.push_back(uniform);
		}
		return true;
	}

	void MaterialTemplate::updateUniforms()
	{
		std::vector<MaterialParameter> parameters;
		for (const auto& uniform : m_uniforms)
		{
			MaterialParameter info;
			if (uniform.type == "float")
			{
				info.name = uniform.name;
				info.type = MaterialParamterType::Float;
				info.value = stringToFloat(uniform.property.value);
				parameters.push_back(info);
			}
			else if (uniform.type == "vec4")
			{
				info.name = uniform.name;
				info.type = MaterialParamterType::Vector4;
				info.value = stringToVec4(uniform.property.value);
				parameters.push_back(info);
			}
			else if (uniform.type == "sampler2D")
			{
				info.name = uniform.name;
				info.type = MaterialParamterType::Texture2D;
				info.value = SoftObject<Texture2D>();
				parameters.push_back(info);
			}
			else
			{
				V_LOG_WARN(Editor, "MaterialTemplate: Unsupported uniform type: {}", uniform.type);
			}
		}
		setParameters(std::move(parameters));
	}

}

BOOST_CLASS_EXPORT_IMPLEMENT(volucris::MaterialTemplate)