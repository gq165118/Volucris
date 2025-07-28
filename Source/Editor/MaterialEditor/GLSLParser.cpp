#include "GLSLParser.h"
#include <algorithm>

namespace volucris
{
	std::string GLSLParser::preprocess(std::string source)
	{
        // 移除单行注释
        size_t pos = 0;
        while ((pos = source.find("//", pos)) != std::string::npos) {
            size_t end = source.find('\n', pos);
            std::string comment = source.substr(pos, end - pos);
            parseComment(comment);
            source.erase(pos, end - pos);
        }

        // 移除#version
        pos = 0;
        while ((pos = source.find("#", pos)) != std::string::npos) {
            size_t end = source.find('\n', pos);
            source.erase(pos, end - pos);
        }

        // 移除多行注释
        pos = 0;
        while ((pos = source.find("/*", pos)) != std::string::npos) {
            size_t end = source.find("*/", pos);
            if (end == std::string::npos) break;
            source.erase(pos, end - pos + 2);
        }

        // 标准化空格
        std::replace(source.begin(), source.end(), '\n', ' ');
        std::replace(source.begin(), source.end(), '\t', ' ');

        // 压缩连续空格
        std::string result;
        bool lastWasSpace = false;
        for (char c : source) 
        {
            if (std::isspace(c)) 
            {
                if (!lastWasSpace) 
                {
                    result += ' ';
                    lastWasSpace = true;
                }
            }
            else
            {
                result += c;
                lastWasSpace = false;
            }
        }

        return result;
    }

    void GLSLParser::parseComment(const std::string& comment)
    {
        size_t attrStart = 0;
        size_t attrEnd = 0;
        size_t equalsSignPos = 0;
        while ((equalsSignPos = comment.find("=", attrStart)) != std::string::npos)
        {
            attrEnd = equalsSignPos+1;
            while (attrEnd < comment.size() && std::isspace(comment[attrEnd])) attrEnd++;
            size_t vstart = attrEnd;
            while (attrEnd < comment.size() && !std::isspace(comment[attrEnd])) attrEnd++;
            std::string value = comment.substr(vstart, attrEnd-vstart);
            if (value.empty()) continue;

            vstart = attrStart;
            while (vstart < equalsSignPos && std::isspace(comment[vstart])) vstart++;
            size_t vend = equalsSignPos;
            while (vend > vstart && std::isspace(comment[vend])) vend--;

            attrStart = attrEnd + 1;

            std::string attr = comment.substr(vstart, vend - vstart);
            size_t dotPos = attr.find(".");
            if (dotPos == std::string::npos)
            {
                continue;
            }

            vend = dotPos;
            //while (vend > 0 && std::isspace(attr[vend])) vend--;
            vstart = vend;
            while (vstart > 0 && !std::isspace(attr[vstart])) vstart--;
            if (std::isspace(attr[vstart])) vstart++;
            std::string uname = attr.substr(vstart, vend-vstart);

            vstart = dotPos + 1;
            while (vstart < attr.size() && std::isspace(attr[vstart])) vstart++;
            vend = vstart;
            while (vend < attr.size() && !std::isspace(attr[vend])) vend++;
            std::string propertyName = attr.substr(vstart, vend);
            if (uname.empty() || propertyName.empty())
            {
                continue;
            }

            auto it = m_properties.find(uname);
            if (it == m_properties.end())
            {
                m_properties[uname] = {};
                it = m_properties.insert({ uname, {} }).first;
            }
            UniformProperty& property = it->second;

            if (propertyName == "group")
            {
                property.group = value;
            }
            else if (propertyName == "display_name")
            {
                property.displayName = value;
            }
            else if (propertyName == "value")
            {
                property.value = value;
            }
        }
    }

    void GLSLParser::parseUniforms(const std::string& source)
    {
        size_t start = 0;
        while ((start = source.find("uniform", start)) != std::string::npos) {
            // 跳过"uniform"单词
            size_t declStart = start + 7;
            while (declStart < source.size() && std::isspace(source[declStart])) declStart++;
            size_t declEnd = declStart;
            while (declEnd < source.size() && !std::isspace(source[declEnd])) declEnd++;

            std::string decl = source.substr(declStart, declEnd - declStart);
            if (declEnd == source.size())
            {
                start = declEnd;
                break;
            }

            // 查找下一个字符
            size_t memberStart = declEnd + 1;
            // 这个时候是ubo
            if (source[memberStart] == '{')
            {
                memberStart = memberStart + 1;
                size_t memberEnd = source.find('}', memberStart);
                if (memberEnd == std::string::npos) break;
                std::string members = source.substr(memberStart, memberEnd - memberStart);

                UniformBlock block;
                block.name = decl;
                size_t varStart = 0;
                size_t varEnd = members.find(";", varStart);
                while (varEnd != std::string::npos)
                {
                    std::string declaration = members.substr(varStart, varEnd - varStart);
                    UniformVariable uniform;
                    if (parseUniformVariable(declaration, uniform))
                    {
                        block.members.push_back(uniform);
                    }
                    varStart = varEnd + 1;
                    varEnd = members.find(";", varStart);
                }
                m_uniformBlocks.push_back(block);
                start = memberEnd + 1;
            }
            else
            {
                memberStart = memberStart + 1;
                size_t semicolon = source.find(';', declStart);
                if (semicolon == std::string::npos) break;
                std::string declaration = source.substr(declStart, semicolon - declStart);
                UniformVariable uniform;
                if (parseUniformVariable(declaration, uniform))
                {
                    m_uniforms.push_back(uniform);
                }
                start = semicolon + 1;
            }
        }
    }

    bool GLSLParser::parseUniformVariable(const std::string& declaration, UniformVariable& uniform)
    {
        size_t typeStart = 0;
        while (typeStart < declaration.size() && std::isspace(declaration[typeStart])) typeStart++;
        size_t typeEnd = typeStart;
        while (typeEnd < declaration.size() && !std::isspace(declaration[typeEnd])) typeEnd++;
        std::string type = declaration.substr(typeStart, typeEnd);
        size_t nameStart = typeEnd;
        while (nameStart < declaration.size() && std::isspace(declaration[nameStart])) nameStart++;
        size_t nameEnd = nameStart;
        while (nameEnd < declaration.size() && (std::isalnum(declaration[nameEnd]) || declaration[nameEnd] == '_')) nameEnd++;
        std::string name = declaration.substr(nameStart, nameEnd - nameStart);

        // 检查数组声明
        int arraySize = 0;
        if (nameEnd < declaration.size() && declaration[nameEnd] == '[') 
        {
            size_t arrayStart = nameEnd + 1;
            size_t arrayEnd = declaration.find(']', arrayStart);
            if (arrayEnd != std::string::npos) 
            {
                arraySize = std::stoi(declaration.substr(arrayStart, arrayEnd - arrayStart));
            }
        }

        auto it = m_properties.find(name);
        if (it != m_properties.end())
        {
            uniform = {it->second, type, name, arraySize };
        }
        else
        {
            uniform = { {}, type, name, arraySize };
        }

        
        return true;
    }
}
