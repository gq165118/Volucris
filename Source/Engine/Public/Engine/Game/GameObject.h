#ifndef __volucris_game_object_h__
#define __volucris_game_object_h__

#include <Engine/Core/Object.h>
#include <vector>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <rttr/registration.h>
#include <rttr/rttr_enable.h>
#include <Engine/Core/Delegate.h>
#include <Engine/Core/TypesHelp.h>
#include <Engine/Asset/AssetPath.h>
#include <Engine/Game/SoftObject.h>

namespace volucris
{
	class Package;

	class GameObject;

	DECLARE_EVENT_MUTI_DELEGATE(ObjectDirtyEvent, void, GameObject*)

	class GameObject : public Object
	{
		RTTR_ENABLE()

	public:
		ObjectDirtyEvent DirtyStateChanged;


	public:
		GameObject();

		virtual ~GameObject() override;

		template <class Archive>
		void serialize(Archive& ar, const unsigned int version) 
		{
			ar& m_pathName;
		}

		void setPathName(AssetPath pathName)
		{
			m_pathName = std::move(pathName);
		}

		void setDisplayName(const std::string& name)
		{
			m_pathName.setName(name);
		}

		const AssetPath& getPathName() const
		{
			return m_pathName;
		}

		const std::string& getDisplayName() const
		{
			return m_pathName.name;
		}

		virtual std::string getClassName() const { return ""; }

		void markDirty(bool dirty)
		{
			if (m_dirty != dirty)
			{
				m_dirty = dirty;
				DirtyStateChanged.invoke(this);
			}
		}

		bool isDirty() const
		{
			return m_dirty;
		}


	private:
		bool m_dirty;
		AssetPath m_pathName;
	};
}

#endif // !__volucris_game_object_h__
