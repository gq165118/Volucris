#ifndef __volucris_rhi_state_h__
#define __volucris_rhi_state_h__

#include <glm/glm.hpp>
#include <Engine/Core/Rect.h>
#include <Engine/Core/TypesHelp.h>
#include <Engine/RHI/RHIBuffer.h>
#include <Engine/Core/DataType.h>
#include <Engine/Core/PrimitiveInfo.h>
#include "RHIUniform.h"

namespace volucris
{
	class RHITexture;
	class RHIRenderTarget;
	class RHIProgram;
	class RHIVertexArray;
	class RHIElementBuffer;
	class RHICommandList;
	class RHIUniformBuffer;

	struct RHIClearState
	{
		enum Buffer
		{
			ColorBuffer = 0x01,
			DepthBuffer = 0x02,
			ColorAndDepth = ColorBuffer | DepthBuffer,
			All = ColorBuffer | DepthBuffer,
		};

		uint32 buffers = Buffer::ColorAndDepth;
		glm::vec4 color = glm::vec4(1,1,0,1);
	};

	struct RHIDepthTest
	{
		bool enabled = false;
	};

	struct RHICullFace
	{
		bool enabled = false;
	};

	struct RHIState
	{
		RHICommandList* commandList = nullptr;
		Rect viewport = { 0,0,0,0 };
		RHIClearState clearState = RHIClearState();
		RHIDepthTest depthState = RHIDepthTest();
		RHICullFace cullFace = RHICullFace();
		RHITexture* texture2d = nullptr;
		RHIRenderTarget* readTarget = nullptr;	//当前绑定的读fbo
		RHIRenderTarget* writeTarget = nullptr;  //当前绑定的写fbo
		RHIRenderTarget* renderTarget = nullptr;
		RHIVertexArray* vertexArray = nullptr;	//当前绑定vao
		std::unordered_map<RHIBuffer::Type, RHIBuffer*> buffers = {};  //各类绑定的缓冲对象（VBO/PBO/Ubo等）
		std::unordered_map<uint32, RHIUniformBuffer*> uniformBuffers = {}; // Slot->Ubo 映射
		RHIProgram* program = nullptr;
	};

	struct MeshDrawInfo
	{
		RHIVertexArray* vao = nullptr;
		RHIElementBuffer* ebo = nullptr;
		PrimitiveSegment segment;
	};
}

#endif // !__volucris_rhi_state_h__
