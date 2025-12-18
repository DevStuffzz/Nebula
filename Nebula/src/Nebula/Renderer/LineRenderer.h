#pragma once

#include "Nebula/Core.h"
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace Nebula {

	class NEBULA_API LineRenderer {
	public:
		virtual ~LineRenderer() = default;

		virtual void Begin() = 0;
		virtual void End() = 0;

		virtual void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color) = 0;
		virtual void DrawLines(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& colors) = 0;

		virtual void SetViewProjection(const glm::mat4& viewProjection) = 0;

		static std::shared_ptr<LineRenderer> Create();
	};

}
