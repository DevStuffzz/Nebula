#include "nbpch.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Nebula {

	// Orthographic Camera
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_ViewMatrix(1.0f)
	{
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
			glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	// Perspective Camera
	PerspectiveCamera::PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip)
		: m_ProjectionMatrix(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip)), m_ViewMatrix(1.0f)
	{
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void PerspectiveCamera::SetProjection(float fov, float aspectRatio, float nearClip, float farClip)
	{
		m_ProjectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void PerspectiveCamera::RecalculateViewMatrix()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position);
		transform = glm::rotate(transform, glm::radians(m_Rotation.y), glm::vec3(0, 1, 0)); // Yaw
		transform = glm::rotate(transform, glm::radians(m_Rotation.x), glm::vec3(1, 0, 0)); // Pitch
		transform = glm::rotate(transform, glm::radians(m_Rotation.z), glm::vec3(0, 0, 1)); // Roll

		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

}
