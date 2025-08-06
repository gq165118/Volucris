#include "RHI/RHIUniformBuffer.h"
#include "RHIOpenGL.h"
#include "RHI/RHICommandList.h"

namespace volucris
{
	RHIUniformBuffer::RHIUniformBuffer(RHICommandList* context, Usage usage)
		: RHIBuffer(RHIBuffer::UniformBuffer, usage)
	{
		setContext(context);
	}

	void RHIUniformBuffer::bindToSlot(int32 slot)
	{
		m_context->bindUniformBufferToSlot(this, slot);
	}
}
