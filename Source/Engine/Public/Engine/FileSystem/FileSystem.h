#ifndef __volucris_file_system_h__
#define __volucris_file_system_h__

#include <string>
#include <vector>

namespace volucris
{
	enum class EFileType
	{
		Unknown,
		Directory = 0x01,
		Asset = 0x02,
		NotAsset = 0x04,
		AllFile = Asset | NotAsset,
	};

	struct FileNode
	{
		EFileType type = EFileType::Unknown;
		std::string path;
	};

	struct MountPoint 
	{
		std::string path;
		std::string physicalPath;
		// 覆盖顺序
		int priority;
		// 是否可写
		bool writable;
	};

	class FileSystem
	{
	public:
		~FileSystem();

		static FileSystem& getInstance()
		{
			static FileSystem instance;
			return instance;
		}

		bool mount(MountPoint point);

		bool mount(const std::string& virtualPath, const std::string& physicalRoot, bool writable=false)
		{
			return mount(std::move(MountPoint({ virtualPath, physicalRoot, 0, writable })));
		}

		bool unmount(const std::string& virtualPath);

		bool isWritable(const std::string& virtualPath);

		bool fileExists(const std::string& virtualPath);

		bool directoryExists(const std::string& virtualPath);

		void renameDirectory(const std::string& oldPath, const std::string& newPath);

		std::string virtualToPhysical(const std::string& virtualPath);

		std::string physicalToVirtual (const std::string& physicalPath);

		// 查找某个路径下的所有节点
		std::vector<FileNode> getFileNodes(const std::string& virtualPath, int filters=(int)EFileType::Directory | (int)EFileType::Asset);

		// 创建文件
		bool createFile(const std::string& virtualPath, const void* initialData = nullptr, size_t size = 0);

		// 删除文件
		bool deleteFile(const std::string& virtualPath);

		bool deleteAsset(const std::string& virtualPath);

		// 创建文件夹
		bool createDirectory(const std::string& virtualPath);

		// 删除文件夹
		bool deleteDirectory(const std::string& virtualPath);

		// 获取父目录
		FileNode parentNode(const std::string& virtualPath);

	protected:
		// 找到优先级最高的节点
		MountPoint* findMountPoint(const std::string& virtualPath);

	private:
		struct Impl;
		Impl* m_impl;

	protected:
		FileSystem();
		FileSystem(const FileSystem&) = delete;
		FileSystem(FileSystem&&) = delete;
		FileSystem& operator=(const FileSystem&) = delete;
		FileSystem& operator=(FileSystem&&) = delete;
	};
}

#define gFileSystem volucris::FileSystem::getInstance()

#endif // !__volucris_file_system_h__
