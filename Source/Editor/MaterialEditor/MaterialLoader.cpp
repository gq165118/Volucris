#include "MaterialLoader.h"
#include <filesystem>
#include <EditorCore/Editor.h>
#include "GLSLParser.h"
#include <fstream>

namespace fs = std::filesystem;

namespace volucris
{
	MaterialLoader::MaterialLoader()
		: m_name()
		, m_vsp()
		, m_fsp()
		, m_key()
	{
	}

	bool MaterialLoader::setVertexSource(const std::string& filepath)
	{
		if (m_name.empty())
		{
			init(filepath);
		}
		else if (m_key != getKey(filepath))
		{
			return false;
		}
		m_vsp = filepath;
		return true;
	}

	bool MaterialLoader::setFragmentSource(const std::string& filepath)
	{
		if (m_name.empty())
		{
			init(filepath);
		}
		else if (m_key != getKey(filepath))
		{
			return false;
		}
		m_fsp = filepath;
		return true;
	}

	bool MaterialLoader::load()
	{
		auto vss = getSource(m_vsp);
		auto fss = getSource(m_fsp);
		if (!vss.empty() && !fss.empty())
		{
			m_material = std::make_shared<MaterialTemplate>(std::move(vss), std::move(fss));
			return true;
		}
		return false;
	}

	void MaterialLoader::init(const std::string& filepath)
	{
		m_key = getKey(filepath);

		m_name = fs::path(filepath).stem().generic_u8string();
	}

	std::string MaterialLoader::getKey(const std::string& filepath)
	{
		auto p = fs::path(filepath);
		if (p.has_extension()) {
			return (p.parent_path() / p.stem()).generic_u8string();
		}
		return p.generic_u8string();
	}

	std::string MaterialLoader::getSource(const std::string& filepath)
	{
		std::string source;
		std::ifstream fin(filepath);
		if (!fin.is_open())
		{
			return "";
		}
		return std::string(std::istreambuf_iterator<char>(fin), std::istreambuf_iterator<char>());
	}
}
