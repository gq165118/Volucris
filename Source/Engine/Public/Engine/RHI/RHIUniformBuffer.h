#ifndef __volucris_rhi_uniform_buffer_h__
#define __volucris_rhi_uniform_buffer_h__

#include <Engine/RHI/RHIBuffer.h>

namespace volucris
{
	class RHIUniformBuffer : public RHIBuffer
	{
	public:
		RHIUniformBuffer(RHICommandList* context, Usage usage = Usage::StaticDraw);

		void bindToSlot(int32 slot);

	private:

	};
}

#endif // !__volucris_rhi_uniform_buffer_h__
