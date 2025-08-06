#include "Game/SceneComponent.h"
#include <Core/VectorHelp.h>
#include <glm/ext.hpp>
#include "Game/Entity.h"

namespace volucris
{
	SceneComponent::SceneComponent()
		: Component()
		, m_components()
		, m_position(glm::vec3(0.0))
		, m_rotation(glm::vec3(0.0))
		, m_scale(glm::vec3(1.0))
		, m_rotationTransform()
		, m_relativeTransform()
		, m_worldTransform()
	{

	}

	SceneComponent::~SceneComponent()
	{
		
	}

	void SceneComponent::attach(std::shared_ptr<Component> comp)
	{
		const auto& entity = getEntity();
		if (!entity)
		{
			return;
		}

		if (comp->getEntity() != entity)
		{
			entity->attach(this, comp);
			return;
		}

		m_components.push_back(comp);
	}

	void SceneComponent::disattach(std::shared_ptr<Component> comp)
	{
		if (comp->getParentComponent() != this)
		{
			return;
		}

		if (auto entity = getEntity())
		{
			entity->disattach(comp);
			return;
		}

		VectorHelp::quickRemove(m_components, comp);
	}

	void SceneComponent::update()
	{
		Component::update();
		for (const auto& comp : m_components)
		{
			comp->update();
		}
	}

	void SceneComponent::updateTransform()
	{
		const glm::vec3 xasix = { 1.0, 0.0, 0.0 };
		const glm::vec3 yasix = { 0.0, 1.0, 0.0 };
		const glm::vec3 zasix = { 0.0, 0.0, 1.0 };

		m_relativeTransform = glm::translate(glm::mat4(1.0), m_position);
		m_rotationTransform = glm::rotate(glm::mat4(1.0), glm::radians(m_rotation.y), yasix);
		m_rotationTransform = glm::rotate(m_rotationTransform, glm::radians(m_rotation.x), xasix);
		m_rotationTransform = glm::rotate(m_rotationTransform, glm::radians(m_rotation.z), zasix);
		m_relativeTransform = m_relativeTransform * m_rotationTransform;
		m_relativeTransform = glm::scale(m_relativeTransform, m_scale);

		if (auto parent = getParentComponent())
		{
			m_worldTransform = parent->getWorldTransform() * m_relativeTransform;
		}
		else
		{
			m_worldTransform = m_relativeTransform;
		}

		onTransformChanged();

		for (const auto& comp : m_components)
		{
			if (const auto& sceneComp = dynamic_cast<SceneComponent*>(comp.get()))
			{
				sceneComp->updateTransform();
			}
		}

		markTransformDirty();
	}

	RTTR_REGISTRATION
	{
		rttr::registration::class_<SceneComponent>("SceneComponent")
			 .constructor<>()
			 .property("Position", &SceneComponent::getPosition, static_cast<void (SceneComponent::*)(const glm::vec3&)>(&SceneComponent::setPosition))
			(
				rttr::metadata("Group", "Transform"),
				rttr::metadata("Description", "position")
			)
			.property("Rotation", &SceneComponent::getRotation, static_cast<void (SceneComponent::*)(const glm::vec3&)>(&SceneComponent::setRotation))
				(
					rttr::metadata("Group", "Transform"),
					rttr::metadata("Description", "Rotation")
				)
			.property("Scale", &SceneComponent::getScale, static_cast<void (SceneComponent::*)(const glm::vec3&)>(&SceneComponent::setScale))
				(
					rttr::metadata("Group", "Transform"),
					rttr::metadata("Description", "Scale")
				);
	}

} // namespace volucris

BOOST_CLASS_EXPORT(volucris::SceneComponent)