#pragma once

#include "Nebula/Core.h"
#include <glm/glm.hpp>
#include <string>

namespace Nebula {

	class NEBULA_API NebulaGui {
	public:
		static void Begin(const char* name, bool* p_open = nullptr);
		static void End();
		
		static bool DragFloat(const char* label, float* v, float speed = 1.0f, float min = 0.0f, float max = 0.0f);
		static bool DragFloat2(const char* label, float* v, float speed = 1.0f, float min = 0.0f, float max = 0.0f);
		static bool DragFloat3(const char* label, float* v, float speed = 1.0f, float min = 0.0f, float max = 0.0f);
		static bool DragFloat4(const char* label, float* v, float speed = 1.0f, float min = 0.0f, float max = 0.0f);
		
		static void Text(const char* fmt, ...);
		static bool Button(const char* label);
		static bool Checkbox(const char* label, bool* v);
		static bool SliderFloat(const char* label, float* v, float min, float max);

		static bool Image(uint32_t textureID, const glm::vec2& size);
		static bool Image(uint32_t textureID, const glm::vec2& size, const glm::vec2& uv0 = { 0,0 }, const glm::vec2& uv1 = { 1,1 });
		static bool Image(void* textureID, const glm::vec2& size, const glm::vec2& uv0 = { 0, 0 }, const glm::vec2& uv1 = { 1, 1 });

	
		static glm::vec2 GetContentRegionAvail();
	};

}