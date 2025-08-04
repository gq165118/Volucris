#ifndef __volucris_soft_object__
#define __volucris_soft_object__

#include <Engine/Asset/AssetManager.h>

namespace volucris
{

	class GameObject;

	template <typename T, typename=void>
	class SoftObject;

	class DependentObject
	{
	public:
		DependentObject(GameObject* reference);

		virtual ~DependentObject();

		virtual const std::string& getPackageName() const = 0;

		virtual bool isValid() const = 0;

		GameObject* getReference() const { return m_reference; }

	protected:
		GameObject* m_reference;
	};

	template <typename T>
	class SoftObject<T, std::enable_if_t<std::is_base_of_v<GameObject, T>>>
	{
		class DependentBridge : public DependentObject
		{
		public:
			DependentBridge(SoftObject<T>* object)
				: DependentObject(nullptr)
				, m_object(object)
			{

			}

			const std::string& getPackageName() const override
			{
				return m_object->getPath();
			}

			bool isValid() const override
			{
				return !m_object->getPath().empty();
			}

		private:
			SoftObject<T>* m_object;
		};

	public:
		SoftObject(std::string path = "", GameObject* reference=nullptr)
			: m_object(nullptr)
			, m_packageName(path)
			, m_bridge(this)
		{
		}

		template<typename U,
			typename = std::enable_if_t<std::is_base_of_v<T, U>>>
		SoftObject(const std::shared_ptr<U> object, GameObject* reference = nullptr)
			: m_object(object)
			, m_packageName()
			, m_bridge(this)
		{
			m_packageName = m_object->getPathName().fullpath;
		}

		template<typename U,
			typename = std::enable_if_t<std::is_base_of_v<T, U>>>
		SoftObject(const SoftObject<U>& other)
			: m_object(std::dynamic_pointer_cast<T>(other.object()))
			, m_packageName(other.getPath())
			, m_bridge(this)
		{
		}

		~SoftObject()
		{
			if (auto reference = m_bridge.getReference())
			{
				//reference->remove
			}
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
		DependentBridge m_bridge;
	};
}

#endif // !__volucris_soft_object_path__
