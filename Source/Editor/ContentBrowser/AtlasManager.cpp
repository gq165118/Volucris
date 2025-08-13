#include "AtlasManager.h"
#include <Engine/RHI/RHITexture.h>
#include <Engine/Game/Texture2D.h>
#include <Engine/RHI/RHIRenderTarget.h>
#include <EditorCore/editor.h>
#include <Engine/Application/Application.h>
#include <Engine/Render/PrimitiveSceneProxy.h>
#include <Engine/Game/StaticMesh.h>
#include <Engine/Game/Material.h>
#include <Engine/Render/EditorView.h>
#include <Engine/Render/Renderer.h>
#include <Engine/Core/Task.h>
#include <stb_image/stb_image_write.h>
#include <MaterialEditor/MaterialLoader.h>


namespace volucris
{

    struct CreateAtlasViewTask
    {
        mutable std::unique_ptr<EditorView> view;
        Size size;
        std::shared_ptr<Texture2DProxy> textureProxy;

        //PrimitiveSceneProxy* primitive = nullptr;
        //EditorWindow* client;

        CreateAtlasViewTask(std::unique_ptr<EditorView> v, Size s, std::shared_ptr<Texture2DProxy> tex/*, PrimitiveSceneProxy& prim*/)
            : view(std::move(v)), size(s), textureProxy(tex)/*, primitive(&prim)*//*, client(nullptr)*/ {}

        CreateAtlasViewTask(const CreateAtlasViewTask& task)
        {
            view = std::move(task.view);
            size = task.size;
            textureProxy = task.textureProxy;
            //primitive = task.primitive;
            //client = nullptr;
        }

        CreateAtlasViewTask(CreateAtlasViewTask&& task) noexcept
            : view(std::move(task.view)), size(task.size), textureProxy(task.textureProxy)/*, primitive(task.primitive)*//*, client(task.client)*/
        {
        }

        void execute()
        {
            //view->resize(size.width, size.height);
            view->init();
            view->setTextureProxy(textureProxy);
            //view->resize(128,128);
            view->buildData();
            auto v = view.get();
            Renderer::getInstance().addView(std::move(view));
            Renderer::getInstance().renderFrame();
            Renderer::getInstance().renderFrame();
            auto data = v->getViewData();

            //if (stbi_write_png("output.png", 144, 153, 4, data.data.data(), 144 * 4) == 0) 
            if (stbi_write_png("output.png", 2048, 2048, 3, data.data.data(), 2048 * 3) == 0) 
            {
                return ;
            }

            /*if (client)
            {
                auto data = v->getViewData();
                gApp->pushCommand([client = client, data = std::move(data)]() {
                    client->setViewData(std::move(data));
                    });
            }*/
        }
    };

    AtlasManager& AtlasManager::Get()
    {
        static AtlasManager Instance;
        return Instance;
    }

    AtlasManager::AtlasManager()
        : m_atlasSize(2048)
        , m_slotSize(128)
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

        // 初始化槽位
        const int32 slotsPerRow = m_atlasSize / m_slotSize;
        const  int32 totalSlots = slotsPerRow * slotsPerRow;
        m_slots.clear();
        m_slots.resize(totalSlots);


        for (int32 r = 0; r < slotsPerRow; ++r)
        {
            for (int32 c = 0; c < slotsPerRow; ++c)
            {
                int32 x = c* m_slotSize;
                int32 y = m_atlasSize - (r + 1) * m_slotSize;
                m_slots.push_back(TextureSlot(x, y, false));
            }
        }

        m_currentSlotindex = 0;
    }

    bool AtlasManager::ensureTextureUV(const GUID& inGuid,
        const std::string& inPath,
        const std::string& inClassName,
        std::shared_ptr<Texture2D> inTex,
        glm::vec4& outUV)
    {
        const std::string key = inGuid.toString();

        // 如果已经存在则直接返回UV
        const float inv = 1.0f / float(m_atlasSize);
        if (auto it = m_textureResources.find(key); it != m_textureResources.end())
        {
            const TextureSlot& s = it->second.slot;
            outUV = glm::vec4(s.X * inv, s.Y * inv, (s.X + m_slotSize) * inv, (s.Y + m_slotSize) * inv);

            return true;
        }

        //找空槽
        TextureSlot freeSlot{};
        int32 slotIndex = -1;
        if (!findFreeSlot(freeSlot, slotIndex))
        {
            V_LOG_WARN(Editor, "atlas is full");
            return false;
        }

        // 记录资源信息
        TextureResourceinfo info;
        info.className = inClassName;
        info.guid = inGuid;
        info.path = inPath;
        info.slot = freeSlot;

        //画进atlas
        drawTextureToAtlas(info, slotIndex);

        //标记占用
        m_slots[slotIndex].bOccupied = true;
        m_textureResources.emplace(key, info);

        outUV = glm::vec4(freeSlot.X * inv, freeSlot.Y * inv, (freeSlot.X + m_slotSize) * inv, (freeSlot.Y + m_slotSize) * inv);

        return true;
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
        drawTextureToAtlas(newInfo, 1);

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

    bool AtlasManager::findFreeSlot(TextureSlot& outSlot, int32 index)
    {
        const int32 N = static_cast<int32>(m_slots.size());
        for (int32 i = m_currentSlotindex; i < N; ++i)
        {
            if (!m_slots[i].bOccupied)
            {
                outSlot = m_slots[i];
                m_currentSlotindex = i;
                return true;
            }
        }

        //环回再照一次
        for (int32 i = 0; i < N; ++i)
        {
            if (!m_slots[i].bOccupied)
            {
                outSlot = m_slots[i];
                m_currentSlotindex = i;
                return true;
            }
        }

        return false;

    }

    void AtlasManager::drawTextureToAtlas(const TextureResourceinfo& inInfo, int32 index)
    {
        // 加载原始贴图

        // 确保尺寸正确
        if (m_currentTexture->getTextureData().size.width != m_slotSize || (m_currentTexture->getTextureData().size.height != m_slotSize))
        {
            V_LOG_WARN(Editor, "Texture {} is not{}, will be resized", inInfo.path, m_slotSize);
            //TODO：大小不合适需要resize
            //m_currentTexture->resize(m_slotSize, m_slotSize);
        } 

        auto mesh = std::make_shared<StaticMesh>();
        {
            MeshData data;
            data.setVertices({
                {-1, -1, 0.0},
                {1, -1, 0.0},
                {1, 1, 0.0},
                {-1, 1, 0.0},
                });

            /*data.setVertices({
               {0.0, 0.0, 0.0},
               {1, 0.0, 0.0},
               {1, 1, 0.0},
               {0.0, 1, 0.0},
                });*/
            
            data.setUV0({
                {0.0,0.0,0.0},
                {1.0,0.0,0.0},
                {1.0,1.0,0.0},
                {0.0,1.0,0.0}
                });

            mesh->setMeshData(std::move(data));
            auto ele = std::make_unique<SmallMeshElements>();
            ele->create() = 0;
            ele->create() = 1;
            ele->create() = 2;
            ele->create() = 0;
            ele->create() = 2;
            ele->create() = 3;
            mesh->addSubMesh(std::move(ele));
        }

        //glm::mat4 proj = glm::ortho(0.0f, (float)ATLAS_SIZE, 0.0f, (float)ATLAS_SIZE);

        //PrimitiveSceneProxy 包含mesh 和 material 信息
        auto  primitive = std::make_shared<PrimitiveSceneProxy>();
        //auto mat = SoftObject<Material>("/Engine/Content/Editor/atlas_ori");
        //auto mat = SoftObject<Material>("/Engine/Content/Editor/atlas_origin");
        //auto mat = SoftObject<Material>("/Engine/Content/Editor/atlas");
        //auto mat = SoftObject<Material>("/Engine/Content/Editor/atlas_uv");
        //auto mat = SoftObject<Material>("/Engine/Content/Editor/atlas_uv_l");
        auto mat = SoftObject<Material>("/Engine/Content/Editor/atlas_proj");
        auto materialIns = std::make_shared<MaterialInstance>(mat);
        //materialIns->setTexture2DParameter("u_texture", SoftObject<Texture2D>("/Engine/Content/Editor/T_awesomeface"));
        materialIns->setTexture2DParameter("u_texture", SoftObject<Texture2D>("/Engine/Content/Editor/texture"));
        materialIns->setVector4Parameter("uRect", glm::vec4(128, 0, 128, 128));
        materialIns->setVector4Parameter("uFboSize", glm::vec4(2048, 2048, 0, 0));
        //materialIns->setMat4Parameter("u_projection", proj);
        auto materialInsProxy = materialIns->getMaterialProxy();

        primitive->setMesh(mesh->getProxy());
        primitive->setMaterials({ materialInsProxy });

        auto data = m_currentTexture->getTextureData();
        auto texProxy = m_currentTexture->getProxy();


        //Render
        {
            auto view = std::make_unique<EditorView>(primitive);
            CreateAtlasViewTask task = CreateAtlasViewTask(std::move(view), Size(128, 128), texProxy);
            Renderer::getInstance().push(createTask(std::move(task)));

            if (gApp->isRunning())
            {
                Renderer::getInstance().flushCommands();
                gApp->flushCommmands();
            }

        }
       
        //TODO：考虑只渲染一次

    }
}