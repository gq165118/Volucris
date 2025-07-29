#ifndef __volucris_soft_object__
#define __volucris_soft_object__

#include <Engine/Asset/AssetManager.h>

namespace volucris
{
	template <typename T>
	class SoftObject
	{
	public:
		SoftObject(std::string path = "")
			: m_path(std::move(path))
			, m_object(nullptr)
		{

		}

		template<typename U,
			typename = std::enable_if_t<std::is_base_of_v<T, U>>>
		SoftObject(const std::shared_ptr<U> object)
			: m_path()
			, m_object(object)
		{

		}

		template<typename U,
			typename = std::enable_if_t<std::is_base_of_v<T, U>>>
		SoftObject(const SoftObject<U>& other)
			: m_path(other.getPath())
			, m_object(std::dynamic_pointer_cast<T>(other.object()))
		{
		}

		std::shared_ptr<T> tryLoad()
		{
			if (!m_object && isValid())
			{
				m_object = AssetManager::getInstance().loadAsset<T>(m_path);
			}
			return m_object;
		}

		std::shared_ptr<T> object() const
		{
			return m_object;
		}

		T* get() const
		{
			return m_object.get();
		}

		T* operator->() const
		{
			return m_object.get();
		}

		bool operator==(T* obj) const
		{
			return m_object.get() == obj;
		}

		bool operator!=(T* obj) const
		{
			return !operator==(obj);
		}

		bool operator==(const SoftObject<T>& obj) const
		{
			return m_path == obj.m_path;
		}

		bool operator!=(const SoftObject<T>& obj) const
		{
			return !operator==(obj);
		}

		explicit operator bool() const
		{
			return m_object != nullptr;
		}

		template <class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& m_path;
		}

		bool isValid() const { return !m_path.empty(); }

		const std::string& getPath() const { return m_path; }

	private:
		std::string m_path;
		std::shared_ptr<T> m_object;
	};
}

#endif // !__volucris_soft_object_path__
