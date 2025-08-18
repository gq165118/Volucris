#ifndef __volucris_atlas_manager_h__
#define __volucris_atlas_manager_h__

#include <Engine/Core/TypesHelp.h>
#include <unordered_map>
#include <Engine/RHI/RHIRenderTarget.h>
#include <Engine/Core/GUID.h>

namespace volucris
{
    class RHITexture2D;
    class Texture2D;

    // 贴图槽信息
    struct TextureSlot
    {
        TextureSlot()
            :X(0)
            ,Y(0)
            ,bOccupied(false)
        {}

        TextureSlot(int32 x, int32 y, bool occupied)
            :X(x), Y(y), bOccupied(occupied)
        {
        }

        int32 X;
        int32 Y;
        bool bOccupied;
    };

    // 贴图资源信息
    struct TextureResourceinfo
    {
        std::string path;         // 原始贴图路径
        std::string className;    // 贴图所属类名
        GUID guid;         // 唯一标识符
        TextureSlot slot;  // 在Atlas中的位置信息
    };

    class AtlasManager
    {
    public:
        // 单例访问
        static AtlasManager& Get();

        bool ensureTextureUV(const GUID& inGuid,
                             const std::string& inPath,
                             const std::string& inClassName,
                             std::shared_ptr<Texture2D> inTex,
                             glm::vec4& outUV);

        // 初始化Atlas
        void initializeAtlas(int32 inAtlasSize = 2048, int32 inSlotSize = 128);

        // 注册贴图资源
        bool registerTexture(const std::string& inPath, const std::string& inClassName, const GUID& inGuid, std::shared_ptr<Texture2D> inTex);

        // 获取贴图在Atlas中的UV范围
        bool getTextureUV(const GUID& inGuid, glm::vec4& outUV) const;

        // 获取整个Atlas贴图
       std::shared_ptr<RHITexture2D> getAtlasTexture() const { return m_atlasTexture; }

       void setCurTexture(std::shared_ptr<Texture2D> tex) { m_currentTexture = tex; }


    private:
        // 私有构造函数
        AtlasManager();

        // 查找空闲槽位
        bool findFreeSlot(TextureSlot& outSlot, int32& index);

        bool loadAtlasFromPNG(const std::string& path);

        // 绘制贴图到Atlas
        void drawTextureToAtlas(const TextureResourceinfo& ininfo, int32 index);

        // Atlas尺寸
        int32 m_atlasSize;
        int32 m_slotSize;

        // Atlas贴图
       std::shared_ptr<RHITexture2D> m_atlasTexture;
       std::unique_ptr<RHIRenderTarget> m_atlasRenderTarget;

        // 贴图资源映射
        //std::unordered_map<GUID, TextureResourceinfo> m_textureResources;
        std::unordered_map<std::string, TextureResourceinfo> m_textureResources;  //key：guid转为string

        // 槽位管理
        std::vector<TextureSlot> m_slots;

        // 当前使用的槽位索引
        int32 m_currentSlotindex;

        //当前需要绘制到atlas上的贴图
        std::shared_ptr<Texture2D> m_currentTexture;

        //存储加载atlas的数据
        std::vector<uint8_t> m_data;
        int32 m_width = 0;
        int32 m_height = 0;
    };
}

#endif // !__volucris_atlas_manager_h__
