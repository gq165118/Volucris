#include "Asset/AssetReader.h"
#include <boost/archive/binary_iarchive.hpp>
#include <fstream>
#include <memory>
#include <boost/serialization/shared_ptr.hpp>
#include <Core/Volucris.h>
#include "AssetFileHeader.h"
#include <FileSystem/FileSystem.h>
#include <Game/Package.h>

namespace volucris
{
	static bool isValidAssetFile(boost::archive::binary_iarchive& ia)
	{
		AssetFileHeader header;
		ia >> header;
		if (std::string(header.magic.data()) != AssetFileHeader::MAGIC_KEY || header.version != AssetFileHeader::VERSION)
		{
			V_LOG_ERROR(Engine, "Invalid asset file header: {}, version: {}", header.magic.data(), header.version);
			return false;
		}
		return true;
	}

	std::shared_ptr<Package> AssetReader::readPackage() const
	{
		if (m_path.empty())
		{
			V_LOG_ERROR(Engine, "AssetReader path is empty.");
			return nullptr;
		}
		const auto& filepath = fmt::format("{}.asset", m_path);
		auto physicalPath = gFileSystem.virtualToPhysical(filepath);
		std::ifstream fin(physicalPath, std::ios::binary);
		if (!fin.is_open())
		{
			V_LOG_ERROR(Engine, "Failed to open asset file: {}", physicalPath);
			return nullptr;
		}
		boost::archive::binary_iarchive ia(fin);
		if (!isValidAssetFile(ia))
		{
			V_LOG_ERROR(Engine, "Invalid asset file: {}", physicalPath);
			return nullptr;
		}

		AssetData assetData;
		ia >> assetData;

		std::shared_ptr<Package> package = nullptr;
		ia >> package;

		if (!package)
		{
			V_LOG_ERROR(Engine, "Failed to read package from asset file: {}", physicalPath);
			return nullptr;
		}

		package->setAssetData(assetData);

		return package;
	}

	AssetData AssetReader::readAssetData() const
	{
		if (m_path.empty())
		{
			V_LOG_ERROR(Engine, "AssetReader path is empty.");
			return {};
		}
		const auto& filepath = fmt::format("{}.asset", m_path);
		auto physicalPath = gFileSystem.virtualToPhysical(filepath);
		std::ifstream fin(physicalPath, std::ios::binary);
		if (!fin.is_open())
		{
			V_LOG_ERROR(Engine, "Failed to open asset file: {}", physicalPath);
			return {};
		}
		boost::archive::binary_iarchive ia(fin);
		if (!isValidAssetFile(ia))
		{
			V_LOG_ERROR(Engine, "Invalid asset file: {}", physicalPath);
			return AssetData();
		}

		AssetData assetData;
		ia >> assetData;

		if (assetData.path != m_path)
		{
			V_LOG_ERROR(Engine, "Invalid asset file, package name not valid.");
			V_LOG_ERROR(Engine, "package name: {}, current: {}", assetData.path, m_path);
			return AssetData();
		}

		return assetData;
	}
}