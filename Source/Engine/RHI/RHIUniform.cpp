#include "RHI/RHIUniform.h"
#include <RHI/RHIOpenGL.h>
#include <RHI/RHIProgram.h>
#include <glm/ext.hpp>

namespace volucris
{
	bool RHIUniform::init(RHIProgram* program, const std::string& name)
	{
		m_name = name;
		m_location = glGetUniformLocation(program->getId(), name.c_str());
		return m_location >= 0;
	}

	void RHIUniformFloat::upload() const
	{
		glUniform1f(m_location, m_value);
	}

	void RHIUniformVec4::upload() const
	{
		glUniform4fv(m_location, 1, glm::value_ptr(m_value));
	}

	void RHIUniformMat4::upload() const
	{
		glUniformMatrix4fv(m_location, 1, false, glm::value_ptr(m_value));
	}

	void RHIUniformInt::upload() const
	{
		glUniform1i(m_location, m_value);
	}
}
