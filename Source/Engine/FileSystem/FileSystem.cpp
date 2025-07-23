#include "FileSystem/FileSystem.h"
#include <map>
#include <vector>
#include <Core/Volucris.h>
#include <Core/VectorHelp.h>
#include <filesystem>
#include <fstream>
#include <boost/locale.hpp>

namespace fs = std::filesystem;

namespace volucris
{
	struct FileSystem::Impl
	{
		std::vector<MountPoint> mountPoints;
	};

	FileSystem::FileSystem()
		: m_impl(new Impl)
	{
	}

	FileSystem::~FileSystem()
	{
		delete m_impl;
	}

	bool FileSystem::mount(MountPoint point)
	{
		for (auto& mp : m_impl->mountPoints)
		{
			if (mp.path == point.path && mp.physicalPath == mp.physicalPath)
			{
				V_LOG_WARN(Engine, "file system mount path [{}] failed. it already mounted.", mp.path);
				return false;
			}
		}
		point.path = fs::path(point.path).lexically_normal().generic_u8string();
		point.physicalPath = fs::path(point.physicalPath).lexically_normal().generic_u8string();
		m_impl->mountPoints.push_back(std::move(point));
		return true;
	}

	bool FileSystem::unmount(const std::string& virtualPath)
	{
		return VectorHelp::quickRemoveAllIf<MountPoint>(m_impl->mountPoints, [&virtualPath](const MountPoint& point)->bool {
			return point.path == virtualPath;
			});
	}

	bool FileSystem::isWritable(const std::string& virtualPath)
	{
		if (auto point = findMountPoint(virtualPath))
		{
			return point->writable;
		}
		return false;
	}

	bool FileSystem::fileExists(const std::string& virtualPath)
	{
		auto physicalPath = virtualToPhysical(virtualPath);
		if (!physicalPath.empty())
		{
			return fs::exists(physicalPath) && !fs::is_directory(physicalPath);
		}
		return false;
	}

	bool FileSystem::directoryExists(const std::string& virtualPath)
	{
		auto physicalPath = virtualToPhysical(virtualPath);
		if (!physicalPath.empty())
		{
			return fs::exists(physicalPath) && fs::is_directory(physicalPath);
		}
		return false;
	}

	void FileSystem::renameDirectory(const std::string& oldPath, const std::string& newPath)
	{
		auto oldPhysicalPath = virtualToPhysical(oldPath);
		auto newPhysicalPath = virtualToPhysical(newPath);
		fs::rename(oldPath, newPath);
	}

	std::string FileSystem::virtualToPhysical(const std::string& virtualPath)
	{
		if (auto point = findMountPoint(virtualPath))
		{
			auto relpath = fs::relative(virtualPath, point->path).lexically_normal();
			std::string relativePart = virtualPath.substr(point->path.length());

			// 构建完整物理路径
			fs::path fullPath = fs::path(point->physicalPath) / relpath;
			return fullPath.generic_u8string();
		}
		return "";
	}

	std::string FileSystem::physicalToVirtual(const std::string& physicalPath)
	{
		auto path = fs::path(physicalPath).generic_u8string();
		for (const auto& mp : m_impl->mountPoints)
		{
			if (mp.physicalPath.length() <= path.length() &&
				path.compare(0, mp.physicalPath.length(), mp.physicalPath) == 0)
			{
				std::string relativePart = path.substr(mp.physicalPath.length());
				fs::path fullPath = fs::path(mp.path) / relativePart;
				return fullPath.generic_u8string();
			}
		}
		return "";
	}

	std::vector<FileNode> FileSystem::getFileNodes(const std::string& virtualPath, int filters)
	{
		std::vector<FileNode> nodes;
		for (auto& mp : m_impl->mountPoints)
		{
			if (mp.path.length() <= virtualPath.length() && virtualPath.compare(0, mp.path.length(), mp.path) == 0)
			{
				auto relativePart = fs::relative(virtualPath, mp.path);
				fs::path physicalPath = (fs::path(mp.physicalPath) / relativePart);
				// [CP] unix 不需要转为utf16
				const auto path = boost::locale::conv::utf_to_utf<char16_t>(physicalPath.c_str());
				for (const auto& entry : fs::directory_iterator(fs::path(path)))
				{
					if (entry.is_directory())
					{
						if (filters & (int)EFileType::Directory)
						{
							nodes.push_back({ EFileType::Directory, (fs::path(virtualPath) / entry.path().filename()).generic_u8string() });
						}
					}
					else
					{
						if (filters & (int)EFileType::Asset && entry.path().extension() == ".asset")
						{
							nodes.push_back({ EFileType::Asset, (fs::path(virtualPath) / entry.path().stem()).generic_u8string() });
						}
						else if (filters & (int)EFileType::NotAsset)
						{
							nodes.push_back({ EFileType::NotAsset, (fs::path(virtualPath) / entry.path().stem()).generic_u8string() });
						}
					}
				}
			}
		}
		return nodes;
	}

	bool FileSystem::createFile(const std::string& virtualPath, const void* initialData, size_t size)
	{
		auto physicalPath = virtualToPhysical(virtualPath);
		if (physicalPath.empty())
		{
			return false;
		}
		
		fs::path path = physicalPath;

		if (!fs::is_directory(path.parent_path()))
		{
			V_LOG_WARN(Engine, "can not create file because of parent path not is existed folder, {}", physicalPath);
			return false;
		}

		if (fs::exists(path))
		{
			V_LOG_WARN(Engine, "can not create file because of path not is existed: {}", physicalPath);
			return false;
		}

		std::ofstream fout = std::ofstream(physicalPath, std::ios::binary | std::ios::trunc);
		if (!fout.is_open())
		{
			return false;
		}

		if (initialData && size > 0)
		{
			fout.write((const char*)initialData, size);
		}

		fout.close();
		return true;
	}

	bool FileSystem::deleteFile(const std::string& virtualPath)
	{
		auto physicalPath = virtualToPhysical(virtualPath);
		if (physicalPath.empty())
		{
			return false;
		}

		fs::path path = physicalPath;
		if (!fs::exists(path) || !fs::is_regular_file(path))
		{
			return false;
		}

		return fs::remove(path);
	}

	bool FileSystem::deleteAsset(const std::string& virtualPath)
	{
		auto physicalPath = virtualToPhysical(virtualPath);
		if (physicalPath.empty())
		{
			return false;
		}

		fs::path path = fmt::format("{}.asset", physicalPath);
		if (!fs::exists(path) || fs::is_directory(path))
		{
			return false;
		}

		return fs::remove(path);
	}

	bool FileSystem::createDirectory(const std::string& virtualPath)
	{
		auto physicalPath = virtualToPhysical(virtualPath);
		if (physicalPath.empty())
		{
			return false;
		}

		fs::path path = physicalPath;
		if (fs::exists(path))
		{
			if (!fs::is_directory(path))
			{
				V_LOG_WARN(Engine, "create directory {} failed.", path.parent_path().string());
				return false;
			}
			return true;
		}
		return fs::create_directories(path);
	}

	bool FileSystem::deleteDirectory(const std::string& virtualPath)
	{
		auto physicalPath = virtualToPhysical(virtualPath);
		if (physicalPath.empty())
		{
			return false;
		}

		fs::path path = physicalPath;
		if (fs::exists(path))
		{
			if (!fs::is_directory(path))
			{
				return false;
			}
			return fs::remove_all(physicalPath);
		}
		return true;
	}

	FileNode FileSystem::parentNode(const std::string& virtualPath)
	{
		auto path = fs::path(virtualPath).parent_path().generic_u8string();
		if (auto point = findMountPoint(path))
		{
			FileNode node;
			node.type = EFileType::Directory;
			node.path = path;
			return node;
		}
		return FileNode();
	}

	MountPoint* FileSystem::findMountPoint(const std::string& virtualPath)
	{
		MountPoint* point = nullptr;
		size_t prefixLength = 0;
		for (auto& mp : m_impl->mountPoints)
		{
			if (mp.path.length() <= virtualPath.length() && virtualPath.compare(0, mp.path.length(), mp.path) == 0)
			{
				if (prefixLength < mp.path.length() || !point || point->priority < mp.priority)
				{
					point = &mp;
				}
			}
		}
		return point;
	}
}
