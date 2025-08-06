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
		RHIUniform() : m_name(), m_location(-1), m_program(nullptr) {}

		virtual ~RHIUniform() = default;

		virtual bool init(RHIProgram* program, const std::string& name);

		const std::string& getName() const { return m_name; }

		virtual void upload() const = 0;

	protected:
		std::string m_name;
		int32 m_location;
		RHIProgram* m_program;
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

		void upload() const override;

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

	class RHIUniformInt : public RHIUniform
	{
	public:
		RHIUniformInt(int value = 0)
			: RHIUniform()
			, m_value(value)
		{
		}

		void setValue(int value)
		{
			m_value = value;
		}

		int getValue() const
		{
			return m_value;
		}

		void upload() const  override;

	private:
		int m_value;
	};

	class RHIUniformBlock : public RHIUniform
	{
	public:
		RHIUniformBlock(uint32 value = 0)
			: RHIUniform()
			, m_value(value)
		{
		}

		bool init(RHIProgram* program, const std::string& name);

		void upload() const  override;

		void setValue(uint32 value)
		{
			m_value = value;
		}

		uint32 getValue() const
		{
			return m_value;
		}

	private:
		uint32 m_value;
	};
}

#endif // !__volucris_rhi_uniform_h__
