#ifndef __volucris_game_object_h__
#define __volucris_game_object_h__

#include <Engine/Core/Object.h>
#include <vector>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/export.hpp>
#include <rttr/registration.h>
#include <rttr/rttr_enable.h>

namespace volucris
{
	class Package;

	class GameObject : public Object
	{
		RTTR_ENABLE()
	public:
		GameObject();

		virtual ~GameObject() override;

		template <class Archive>
		void serialize(Archive& ar, const unsigned int version) 
		{
		}

		void setDisplayName(const std::string& name)
		{
			m_displayName = name;
		}

		const std::string& getDisplayName() const
		{
			return m_displayName;
		}

		virtual std::string getClassName() const { return ""; }

	private:
		std::string m_displayName;
	};
}

#endif // !__volucris_game_object_h__
