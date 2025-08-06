#include "AtlasManager.h"
#include <Engine/RHI/RHITexture.h>
#include <Engine/Game/Texture2D.h>
#include <Engine/RHI/RHIRenderTarget.h>
#include <EditorCore/editor.h>
#include <Engine/Application/Application.h>
#include <Engine/Render/PrimitiveSceneProxy.h>


namespace volucris
{

    AtlasManager& AtlasManager::Get()
    {
        static AtlasManager Instance;
        return Instance;
    }

    AtlasManager::AtlasManager()
        : m_atlasSize(0)
        , m_slotSize(0)
        , m_currentSlotindex(0)
    {
    }

    void AtlasManager::initializeAtlas(int32 inAtlasSize, int32 inSlotSize)
    {
        m_atlasSize = inAtlasSize;
        m_slotSize = inSlotSize;

        // 创建Atlas贴图
        RHITextureDesc desc;
        desc.size = { m_atlasSize , m_atlasSize };
        desc.pixelFormat = Texture::EPixelFormat::R8G8B8;
        m_atlasTexture = std::make_shared<RHITexture2D>(desc);

        // 创建渲染目标
        m_atlasRenderTarget = std::make_unique<RHIRenderTarget>(Size(m_atlasSize, m_atlasSize));
       // AtlasRenderTarget->Initialize(AtlasSize, AtlasSize);

        // 初始化槽位
        int32 SlotsPerRow = m_atlasSize / m_slotSize;
        int32 TotalSlots = SlotsPerRow * SlotsPerRow;
        m_slots.resize(TotalSlots);

        for (int32 Y = 0; Y < SlotsPerRow; ++Y)
        {
            for (int32 X = 0; X < SlotsPerRow; ++X)
            {
                int32 Index = Y * SlotsPerRow + X;
                m_slots[Index].X = X * m_slotSize;
                m_slots[Index].Y = Y * m_slotSize;
                m_slots[Index].bOccupied = false;
            }
        }
    }

    bool AtlasManager::registerTexture(const std::string& inPath, const std::string& inClassName, const GUID& inGuid)
    {
        // 检查是否已存在
        if (m_textureResources.find(inGuid.toString()) != m_textureResources.end())
        {
            return true;
        }

        // 查找空闲槽位
        TextureSlot reeSlot;
        if (!findFreeSlot(reeSlot))
        {
            //V_LOG_WARN(LogEngine, Error, TEXT("No free slots available in texture atlas!"));
            V_LOG_WARN(Editor, "No free slots available in texture atlas!");
            return false;
        }

        // 创建资源信息
        TextureResourceinfo newInfo;
        newInfo.path = inPath;
        newInfo.className = inClassName;
        newInfo.guid = inGuid;
        newInfo.slot = reeSlot;

        // 添加到映射
        m_textureResources[inGuid.toString()] = newInfo;

        // 绘制到Atlas
        //drawTextureToAtlas(NewInfo);

        return true;
    }

    bool AtlasManager::getTextureUV(const GUID& inGuid, glm::vec4& outUV) const
    {
        auto it = m_textureResources.find(inGuid.toString());
        if (it == m_textureResources.end())
        {
            return false;
        }

        const TextureSlot& slot = it->second.slot;
        float U0 = static_cast<float>(slot.X) / m_atlasSize;
        float V0 = static_cast<float>(slot.Y) / m_atlasSize;
        float U1 = static_cast<float>(slot.X + m_slotSize) / m_atlasSize;
        float V1 = static_cast<float>(slot.Y + m_slotSize) / m_atlasSize;

        outUV = glm::vec4(U0, V0, U1, V1);
        return true;
    }

    bool AtlasManager::findFreeSlot(TextureSlot& outSlot)
    {
        // 线性查找 (可优化为维护空闲列表)
        for (; m_currentSlotindex < m_slots.size(); ++m_currentSlotindex)
        {
            if (!m_slots[m_currentSlotindex].bOccupied)
            {
                outSlot = m_slots[m_currentSlotindex];
                m_slots[m_currentSlotindex].bOccupied = true;
                return true;
            }
        }

        return false;
    }

    void AtlasManager::drawTextureToAtlas(const TextureResourceinfo& inInfo)
    {
        // 加载原始贴图

        // 确保尺寸正确
        if (m_currentTexture->getTextureData().size.width != m_slotSize || (m_currentTexture->getTextureData().size.height != m_slotSize))
        {
            V_LOG_WARN(Editor, "Texture {} is not{}, will be resized", inInfo.path, m_slotSize);
            //TODO：大小不合适需要resize
            //m_currentTexture->resize(m_slotSize, m_slotSize);
        }


        {
            PrimitiveSceneProxy primitive;
        }
       /* auto func = [manager = this]() {
            primitive;
            naterial;
            render();
            auto texture = readback();
            gApp->pushCommand([texture]() {
                manager->renderFinished(texture);
                })
            };
        Renderer::getInstance().pushCommmand(func);*/

        //// 设置渲染目标
        //FRHICommandListImmediate& RHICmdList = GetImmediateCommandList_ForRenderCommand();
        //RHICmdList.TransitionResource(EResourceTransitionAccess::EWritable, AtlasRenderTarget->GetRenderTargetTexture());

        //// 设置视口
        //RHICmdList.SetViewport(
        //    InInfo.Slot.X, InInfo.Slot.Y,
        //    0.0f,
        //    InInfo.Slot.X + SlotSize, InInfo.Slot.Y + SlotSize,
        //    1.0f
        //);

        //// 绘制贴图
        //FTextureDrawingPolicy DrawingPolicy;
        //DrawingPolicy.DrawTexture(
        //    RHICmdList,
        //    SourceTexture->GetResource(),
        //    FVector2D(SlotSize, SlotSize),
        //    FLinearColor::White,
        //    EBlendMode::BLEND_Opaque
        //);

        //// 生成mipmap
        //AtlasTexture->GenerateMips(RHICmdList);
    }
}