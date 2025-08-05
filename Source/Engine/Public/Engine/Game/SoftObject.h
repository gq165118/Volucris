#ifndef __volucris_soft_object__
#define __volucris_soft_object__

#include <Engine/Asset/AssetManager.h>

namespace volucris
{

	class GameObject;

	template <typename T, typename=void>
	class SoftObject;
	template <typename T>
	class SoftObject<T, std::enable_if_t<std::is_base_of_v<GameObject, T>>>
	{

	public:
		SoftObject(std::string path = "")
			: m_object(nullptr)
			, m_packageName(path)
		{
		}

		template<typename U,
			typename = std::enable_if_t<std::is_base_of_v<T, U>>>
		SoftObject(const std::shared_ptr<U> object)
			: m_object(object)
			, m_packageName()
		{
			m_packageName = m_object->getPathName().fullpath;
		}

		template<typename U,
			typename = std::enable_if_t<std::is_base_of_v<T, U>>>
		SoftObject(const SoftObject<U>& other)
			: m_object(std::dynamic_pointer_cast<T>(other.object()))
			, m_packageName(other.getPath())
		{
		}

		~SoftObject()
		{
		}

		std::shared_ptr<T> tryLoad()
		{
			if (!m_object && isValid())
			{
				m_object = AssetManager::getInstance().loadAsset<T>(m_packageName);
			}
			return m_object;
		}

		bool isValid() const { return !m_packageName.empty(); }

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
			return m_packageName == obj.m_packageName;
		}

		bool operator!=(const SoftObject<T>& obj) const
		{
			return !operator==(obj);
		}

		explicit operator bool() const
		{
			return m_object != nullptr;
		}

		const std::string& getPath() const { return m_packageName; }

		template <class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& m_packageName;
		}

		SoftObject<T>& operator=(const SoftObject& other)
		{
			m_packageName = other.m_packageName;
			m_object = other.object();
			return *this;
		}

	private:
		std::shared_ptr<T> m_object;
		std::string m_packageName;
	};
}

#endif // !__volucris_soft_object_path__
