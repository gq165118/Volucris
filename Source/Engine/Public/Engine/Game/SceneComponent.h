#ifndef __volucris_scene_component_h__
#define __volucris_scene_component_h__

#include <Engine/Game/Component.h>
#include <glm/glm.hpp>

namespace volucris
{
	class SceneComponent : public Component
	{
		RTTR_ENABLE(Component)

	public:
		SceneComponent();

		~SceneComponent() override;

		void attach(std::shared_ptr<Component> comp);

		void disattach(std::shared_ptr<Component> comp);

		void setPosition(float x, float y, float z)
		{
			setPosition(glm::vec3(x, y, z));
		}

		void setPosition(const glm::vec3& pos)
		{
			m_position = pos;
			updateTransform();
		}

		void setRotation(float x, float y, float z)
		{
			setRotation(glm::vec3(x, y, z));
		}

		void setRotation(const glm::vec3& rot)
		{
			m_rotation = rot;
			updateTransform();
		}

		void setScale(float x, float y, float z)
		{
			setScale(glm::vec3(x, y, z));
		}

		void setScale(const glm::vec3& scale)
		{
			m_scale = scale;
			updateTransform();
		}

		const glm::vec3& getPosition() const { return m_position; }

		const glm::vec3& getRotation() const { return m_rotation; }

		const glm::vec3& getScale() const { return m_scale; }

		const glm::mat4& getWorldTransform() const { return m_worldTransform; }

		const glm::mat4& getRelativeTransform() const { return m_relativeTransform; }

		const glm::mat4& getRotationTransform() const { return m_rotationTransform; }

		void update() override;

		const std::vector<std::shared_ptr<Component>>& getAttachedComponents() const { return m_components; }

	protected:
		void updateTransform();

		virtual void onTransformChanged() {}

	private:
		// TODO: 预防循环嵌套
		friend class Entity;

		std::vector<std::shared_ptr<Component>> m_components;

		glm::vec3 m_position;
		glm::vec3 m_rotation;
		glm::vec3 m_scale;

		glm::mat4 m_rotationTransform;
		glm::mat4 m_relativeTransform;
		glm::mat4 m_worldTransform;
	};
}

#endif // !__volucris_scene_component_h__
