#ifndef __volucris_package_h__
#define __volucris_package_h__

#include <Engine/Game/GameObject.h>
#include <string>
#include <Engine/Asset/AssetData.h>
#include <boost/serialization/vector.hpp>

namespace volucris
{
	class Package : public GameObject
	{
	public:
		Package();

		Package(const std::string& path);

		~Package() override;

		void setAssetData(const AssetData& assetData)
		{
			m_assetData = assetData;
			m_assetData.className = m_object ? m_object->getClassName() : "";
		}

		void setAssetClassName(const std::string& name)
		{
			m_assetData.className = name;
		}

		void setObject(const std::shared_ptr<GameObject>& object)
		{
			m_object = object;
			m_assetData.className = object->getClassName();
		}

		std::shared_ptr<GameObject> getAssetObject() const { return m_object; }

		const AssetData& getAssetData() const { return m_assetData; }

		template <class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& m_object;
		}

	private:
		AssetData m_assetData; // 包含的资源数据
		std::shared_ptr<GameObject> m_object;
	};
}

BOOST_CLASS_EXPORT_KEY(volucris::Package)

#endif // !__volucris_package_h__
