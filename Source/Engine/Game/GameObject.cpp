#include "Engine/Game/GameObject.h"
#include <Engine/Core/VectorHelp.h>
#include <Core/Assert.h>
#include <Asset/AssetManager.h>
#include <Core/Volucris.h>

namespace volucris
{
	GameObject::GameObject()
		: Object()
		, m_dirty(false)
		, m_pathName()
	{
	}

	GameObject::~GameObject()
	{
		
	}

	RTTR_REGISTRATION
	{
		rttr::registration::class_<GameObject>("GameObject")
			 .constructor<>()
			 .property("Name", &GameObject::getDisplayName, &GameObject::setDisplayName)
			(
				rttr::metadata("Group", "Common"),
				rttr::metadata("Description", "object name")
			);
	}
}
