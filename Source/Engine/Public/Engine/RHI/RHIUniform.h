#ifndef __volucris_rhi_uniform_h__
#define __volucris_rhi_uniform_h__

#include <string>
#include <Engine/Core/TypesHelp.h>
#include <glm/glm.hpp>

namespace volucris
{
	class RHIProgram;

	class RHIUniform
	{
	public:
		RHIUniform() : m_name(), m_location(-1) {}

		virtual ~RHIUniform() = default;

		bool init(RHIProgram* program, const std::string& name);

		const std::string& getName() const { return m_name; }

		virtual void upload() const = 0;

	protected:
		std::string m_name;
		int32 m_location;
	};

	class RHIUniformFloat : public RHIUniform
	{
	public:
		RHIUniformFloat(float value = 0.0)
			: RHIUniform(), m_value(value) 
		{}

		void setValue(float value)
		{
			m_value = value;
		}

		void upload() const  override;

	private:
		float m_value;
	};

	class RHIUniformVec4 : public RHIUniform
	{
	public:
		RHIUniformVec4(glm::vec4 value = {}) 
			: RHIUniform()
			, m_value(value) 
		{}

		void setValue(const glm::vec4& value)
		{
			m_value = value;
		}

		void upload() const override;

	private:
		glm::vec4 m_value;
	};

	class RHIUniformMat4 : public RHIUniform
	{
	public:
		RHIUniformMat4(glm::mat4 value = {}) 
			: RHIUniform()
			, m_value(value)
		{}

		void setValue(const glm::mat4& value)
		{
			m_value = value;
		}

		void upload() const  override;

	private:
		glm::mat4 m_value;
	};
}

#endif // !__volucris_rhi_uniform_h__
