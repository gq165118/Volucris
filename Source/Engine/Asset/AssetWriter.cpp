#include <Asset/AssetWriter.h>
#include <Game/Package.h>
#include <FileSystem/FileSystem.h>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <fstream>
#include <memory> // 添加智能指针支持
#include <boost/serialization/shared_ptr.hpp>
#include <Core/Volucris.h>
#include "AssetFileHeader.h"

namespace volucris
{
	AssetWriter::AssetWriter(std::shared_ptr<Package> package)
		: m_package(package)
	{
		
	}
	AssetWriter::~AssetWriter()
	{
		// 资源清理逻辑（如果有）
	}

	bool AssetWriter::write()
	{
		if (!m_package) {
			return false; // 包不存在
		}
		
		const auto& assetData = m_package->getAssetData();
		// TODO: 检查是否和已经存在的文件guid相同
		const auto& filepath = fmt::format("{}.asset", assetData.path);
		if (!gFileSystem.fileExists(filepath) && !gFileSystem.createFile(filepath))
		{
			V_LOG_WARN(Engine, "create file {} failed.", filepath);
			return false;
		}
		
		auto physicalPath = gFileSystem.virtualToPhysical(filepath);

		std::ofstream fout(physicalPath, std::ios::binary | std::ios::trunc);
		boost::archive::binary_oarchive oa(fout);

		AssetFileHeader header = AssetFileHeader(true);
		// 写入magic key和版本信息
		oa << header;

		oa << m_package->getAssetData();

		oa << m_package;

		return true;
	}
} // namespace volucris