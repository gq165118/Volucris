#ifndef __volucris_glsl_parser_h__
#define __volucris_glsl_parser_h__

#include <string>
#include <vector>
#include <unordered_map>

namespace volucris
{

    struct UniformProperty
    {
        std::string group = "Default";
        std::string displayName = "";
        std::string value = "";

        template <class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar& group;
            ar& displayName;
            ar& value;
        }
    };

    struct UniformVariable 
    {
        UniformProperty property;
        std::string type;
        std::string name;
        int arraySize = 0; // 0表示非数组

        template <class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar& type;
            ar& name;
            ar& arraySize;
            ar& property;
        }

        bool operator==(const UniformVariable& mem) const
        {
            return type == mem.type && name == mem.name && arraySize == mem.arraySize;
        }

        bool operator!=(const UniformVariable& mem) const
        {
            return !operator==(mem);
        }
    };

    struct UniformBlock
    {
        std::string name;
        std::vector<UniformVariable> members;

        template <class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar& name;
            ar& members;
        }

        bool operator==(const UniformBlock& other) const
        {
            return name == other.name &&
                members == other.members;
        }

        bool operator!=(const UniformBlock& mem) const
        {
            return !operator==(mem);
        }
    };

	class GLSLParser
	{
	public:
        void parse(const std::string& source) 
        {
            std::string processed = preprocess(source);
            parseUniforms(processed);
        }

        const std::vector<UniformVariable>& getUniforms() const { return m_uniforms; }
        
        const std::vector<UniformBlock>& getUniformBlocks() const { return m_uniformBlocks; }

        bool findUniformProperty(std::string& name, UniformProperty& property) const
        {
            auto it = m_properties.find(name);
            if (it != m_properties.end())
            {
                property = it->second;
                return true;
            }
            return false;
        }

    private:
        std::string preprocess(std::string source);

        void parseComment(const std::string& comment);

        void parseUniforms(const std::string& source);

        bool parseUniformVariable(const std::string& source, UniformVariable& var);

    private:
        std::unordered_map<std::string, UniformProperty> m_properties;
        std::vector<UniformVariable> m_uniforms;
        std::vector<UniformBlock> m_uniformBlocks;
	};
}

#endif // !__volucris_glsl_parser_h__
