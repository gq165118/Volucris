/**
 * @class RHIRenderTarget
 * @brief RHI 渲染目标（Framebuffer Object，FBO）抽象类
 *
 * 本类封装了图形渲染管线中的“渲染目标”概念，对应 OpenGL 中的 Framebuffer 对象（FBO），
 * 用于管理颜色附件与深度附件的绑定、使用模式以及渲染目标的生命周期。
 *
 * 设计参考了虚幻引擎的 RHI（Render Hardware Interface）架构，旨在屏蔽底层图形 API 的差异，
 * 提供跨平台、可扩展的渲染目标管理接口，使上层渲染逻辑无需直接调用 OpenGL/Vulkan/D3D 等 API。
 *
 * ## 主要功能：
 * - 创建并管理 GPU 端的帧缓冲资源
 * - 按需绑定多路颜色附件（Multiple Render Targets, MRT）
 * - 绑定深度或深度/模板附件
 * - 切换渲染目标的使用模式（只读、只写、读写）
 * - 检查渲染目标完整性（Framebuffer completeness）
 *
 * ## 成员说明：
 * - m_size：渲染目标的分辨率（所有附件应保持相同尺寸）
 * - m_usage：当前使用模式（ReadOnly、WriteOnly、ReadWrite），可映射到 OpenGL 的 GL_READ_FRAMEBUFFER / GL_DRAW_FRAMEBUFFER
 * - m_colorAttachments：颜色附件映射表，支持通过索引绑定多路颜色缓冲
 * - m_depthAttachment：深度（或深度+模板）附件
 * - m_valid：当前 FBO 是否完整可用
 * - m_id：底层图形 API 分配的帧缓冲对象 ID（OpenGL 中为 GLuint）
 *
 * ## 使用示例（OpenGL 后端）：
 * @code
 * auto rt = std::make_shared<RHIRenderTarget>(Size(1920, 1080));
 * rt->createGpuResource();
 * rt->attachColor(colorTex, 0);
 * rt->attachDepth(depthTex);
 * rt->setUsage(cmdList, RHIRenderTarget::WriteOnly);
 * if (rt->update()) {
 *     // 渲染到该目标
 * }
 * @endcode
 *
 * @note 使用 RHIRenderTarget 时需确保所有绑定附件尺寸与格式匹配，否则会导致 FBO 不完整。
 *       在 OpenGL 中，FBO 本身不保存尺寸信息，m_size 为 RHI 层额外维护，便于一致性检查与跨平台兼容。
 */
#ifndef __volucris_rhi_frame_buffer_h__
#define __volucris_rhi_frame_buffer_h__

#include <Engine/RHI/RHIResource.h>
#include <Engine/RHI/RHIState.h>
#include "RHITexture.h"

namespace volucris
{
	class RHIRenderTarget : public RHIResource
	{
	public:
		enum Usage
		{
			ReadOnly,
			WriteOnly,
			ReadWrite
		};

		RHIRenderTarget(const Size& size, Usage usage = ReadWrite);

		~RHIRenderTarget();

		void createGpuResource();

		void setUsage( RHICommandList* cmdList, Usage usage);

		void attachColor(const std::shared_ptr<RHITexture>& texture, int32 index);

		void attachDepth(const std::shared_ptr<RHITexture2D>& texture);
		
		RHIResource* getAttachedColor(int32 index) const
		{
			auto it = m_colorAttachments.find(index);
			if (it != m_colorAttachments.end())
			{
				return it->second.get();
			}
			return nullptr;
		}

		uint32 getId();

		bool update();

		Size getSize() const { return m_size; }

		Usage getUsage() const { return m_usage; }

	private:
		Size m_size;
		Usage m_usage;
		std::unordered_map<int32, std::shared_ptr<RHIResource>> m_colorAttachments;
		std::shared_ptr<RHIResource> m_depthAttachment;
		bool m_valid;
		uint32 m_id;
	};
}

#endif // !__volucris_rhi_frame_buffer_h__
