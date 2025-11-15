#pragma once

#include "Nebula/Core.h"
#include <string>
#include <glm/glm.hpp>

namespace Nebula {
	class NEBULA_API Shader {
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetMat4(const std::string& name, const glm::mat4& matrix) = 0;

		static Shader* Create(const std::string& filepath);
		static Shader* Create(const std::string& vertexSrc, const std::string& fragmentSrc);
	};
}