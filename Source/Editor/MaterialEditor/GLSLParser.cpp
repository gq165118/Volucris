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
            }


            //// 找到分号结束
            //size_t semicolon = source.find(';', declStart);
            //if (semicolon == std::string::npos) break;

            //// 获取声明内容
            //std::string declaration = source.substr(declStart, semicolon - declStart);

            //// 检查是否是结构体类型
            //bool isStruct = false;
            //for (const auto& [structName, _] : m_structDefs) {
            //    if (declaration.find(fmt::format("struct {}", str)) != 0) {
            //        isStruct = true;
            //        break;
            //    }
            //}

            //if (isStruct) {
            //    // 解析结构体uniform
            //    size_t nameStart = 0;
            //    while (nameStart < declaration.size() && std::isspace(declaration[nameStart])) nameStart++;

            //    size_t nameEnd = nameStart;
            //    while (nameEnd < declaration.size() && !std::isspace(declaration[nameEnd])) nameEnd++;
            //    std::string structType = declaration.substr(nameStart, nameEnd - nameStart);

            //    size_t instanceStart = nameEnd;
            //    while (instanceStart < declaration.size() && std::isspace(declaration[instanceStart])) instanceStart++;

            //    size_t instanceEnd = instanceStart;
            //    while (instanceEnd < declaration.size() && (std::isalnum(declaration[instanceEnd]) || declaration[instanceEnd] == '_')) instanceEnd++;
            //    std::string instanceName = declaration.substr(instanceStart, instanceEnd - instanceStart);

            //    // 检查数组声明
            //    int arraySize = 0;
            //    if (instanceEnd < declaration.size() && declaration[instanceEnd] == '[') {
            //        size_t arrayStart = instanceEnd + 1;
            //        size_t arrayEnd = declaration.find(']', arrayStart);
            //        if (arrayEnd != std::string::npos) {
            //            arraySize = std::stoi(declaration.substr(arrayStart, arrayEnd - arrayStart));
            //        }
            //    }

            //    if (m_structDefs.find(structType) != m_structDefs.end()) {
            //        m_uniformStructs.push_back({
            //            structType,
            //            instanceName,
            //            m_structDefs[structType]
            //            });
            //    }
            //}
            //else {
            //    // 解析基本uniform
            //    size_t typeEnd = 0;
            //    while (typeEnd < declaration.size() && !std::isspace(declaration[typeEnd])) typeEnd++;
            //    std::string type = declaration.substr(0, typeEnd);

            //    size_t nameStart = typeEnd;
            //    while (nameStart < declaration.size() && std::isspace(declaration[nameStart])) nameStart++;

            //    size_t nameEnd = nameStart;
            //    while (nameEnd < declaration.size() && (std::isalnum(declaration[nameEnd]) || declaration[nameEnd] == '_')) nameEnd++;
            //    std::string name = declaration.substr(nameStart, nameEnd - nameStart);

            //    // 检查数组声明
            //    int arraySize = 0;
            //    if (nameEnd < declaration.size() && declaration[nameEnd] == '[') {
            //        size_t arrayStart = nameEnd + 1;
            //        size_t arrayEnd = declaration.find(']', arrayStart);
            //        if (arrayEnd != std::string::npos) {
            //            arraySize = std::stoi(declaration.substr(arrayStart, arrayEnd - arrayStart));
            //        }
            //    }

            //    m_uniforms.push_back({ type, name, arraySize });
            //}

            start =  start + 1;
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

        uniform = { type, name, arraySize };
        return true;
    }
}
