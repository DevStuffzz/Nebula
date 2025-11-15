#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Nebula {

	class Transform {
	public:
		Transform()
			: m_Position(0.0f), m_Rotation(0.0f), m_Scale(1.0f)
		{
		}

		Transform(const glm::vec3& position, const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f))
			: m_Position(position), m_Rotation(rotation), m_Scale(scale)
		{
		}

		// Getters
		const glm::vec3& GetPosition() const { return m_Position; }
		const glm::vec3& GetRotation() const { return m_Rotation; }
		const glm::vec3& GetScale() const { return m_Scale; }

		// Setters
		void SetPosition(const glm::vec3& position) { m_Position = position; }
		void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; }
		void SetScale(const glm::vec3& scale) { m_Scale = scale; }

		// Calculate transformation matrix
		glm::mat4 GetTransform() const
		{
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position);
			transform = glm::rotate(transform, glm::radians(m_Rotation.x), glm::vec3(1, 0, 0));
			transform = glm::rotate(transform, glm::radians(m_Rotation.y), glm::vec3(0, 1, 0));
			transform = glm::rotate(transform, glm::radians(m_Rotation.z), glm::vec3(0, 0, 1));
			transform = glm::scale(transform, m_Scale);
			return transform;
		}

	private:
		glm::vec3 m_Position;
		glm::vec3 m_Rotation; // Euler angles in degrees
		glm::vec3 m_Scale;
	};

}
