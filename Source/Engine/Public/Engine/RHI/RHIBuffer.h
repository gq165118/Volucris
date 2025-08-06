#ifndef __volucris_rhi_buffer_h__
#define __volucris_rhi_buffer_h__

#include <Engine/RHI/RHIResource.h>
#include <Engine/Core/TextureDefines.h>
#include <Engine/Core/Rect.h>
#include <Engine/Core/TypesHelp.h>

namespace volucris
{
	class RHIOpenGLBuffer;

	class RHIBuffer : public RHIResource
	{
	public:
		enum Type
		{
			VertexBuffer,
			ElementBuffer,
			PixelPackBuffer,
			PixelUnpackBuffer,
			UniformBuffer
		};

		enum Usage
		{
			StaticDraw,
			DynamicDraw,
			StreamRead,
			StreamWrite
		};

		RHIBuffer(Type type, Usage usage);

		~RHIBuffer() override;

		void createGpuResource();

		void init(const std::vector<uint8>& data);

		void init(const void* data, uint32 size);

		void update(const void* data, uint32 offset, uint32 size);

		Type getType() const { return m_type; }

		uint32 getId() const { return m_id; }

	protected:
		uint32 m_id;
		Type m_type;
		Usage m_usage;
		std::unique_ptr<RHIOpenGLBuffer> m_buffer;
	};

}

#endif // !__volucris_rhi_buffer_h__
