#pragma once
#pragma warning(disable: 4251)

#define _USE_MATH_DEFINES
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace Nebula {

	class Math
	{
	public:
		static bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
		{
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::quat orientation;
			glm::decompose(transform, scale, orientation, translation, skew, perspective);

			rotation = glm::eulerAngles(orientation);
			rotation = glm::degrees(rotation);

			return true;
		}
	};

}
