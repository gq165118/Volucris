#ifndef __volucris_primitive_info_h__
#define __volucris_primitive_info_h__

#include <Engine/Core/DataType.h>
#include <vector>
#include <string>
#include <Engine/Core/TypesHelp.h>

namespace volucris
{
	enum PrimitiveType
	{
		Vertex,
		Normal,
		Color,
		SecondColor,
		TextureCoordinate
	};

	struct PrimitiveBlock
	{
		PrimitiveType type;
		DataType dataType;
		size_t offset;
		size_t count;

		template <class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& type;
			ar& dataType;
			ar& offset;
			ar& count;
		}
	};

	struct PrimitiveSegment
	{
		ElementDataType type;
		ElementDrawMode mode;
		size_t offset;
		size_t count;

		template <class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& type;
			ar& mode;
			ar& offset;
			ar& count;
		}
	};

	struct PrimitiveInfo
	{
		std::vector<uint8> data;   //存储顶点数据的原始二进制内容（通常包括位置、颜色、法线、纹理坐标等）
		std::vector<uint8> segmentData;  //存储索引数据（Index Buffer）的原始二进制内容。
		std::vector<PrimitiveBlock> blocks;  //描述 data 中每种属性的布局（例如位置、颜色、法线等）
		std::vector<PrimitiveSegment> segments;  //描述如何将索引数组（segmentData）解释为图元（Primitive，如三角形、线段等）。
		std::vector<std::string> materials;

		template <class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& data;
			ar& segmentData;
			ar& blocks;
			ar& segments;
			ar& materials;
		}
	};
}

#endif // !__volucris_primitive_info_h__
