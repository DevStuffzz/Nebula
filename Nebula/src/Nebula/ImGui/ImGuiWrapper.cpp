#include "nbpch.h"
#include "NebulaGui.h"
#include <imgui.h>
#include <cstdarg>

namespace Nebula {

	void NebulaGui::Begin(const char* name, bool* p_open)
	{
		ImGui::Begin(name, p_open);
	}

	void NebulaGui::End()
	{
		ImGui::End();
	}

	bool NebulaGui::DragFloat(const char* label, float* v, float speed, float min, float max)
	{
		return ImGui::DragFloat(label, v, speed, min, max);
	}

	bool NebulaGui::DragFloat2(const char* label, float* v, float speed, float min, float max)
	{
		return ImGui::DragFloat2(label, v, speed, min, max);
	}

	bool NebulaGui::DragFloat3(const char* label, float* v, float speed, float min, float max)
	{
		return ImGui::DragFloat3(label, v, speed, min, max);
	}

	bool NebulaGui::DragFloat4(const char* label, float* v, float speed, float min, float max)
	{
		return ImGui::DragFloat4(label, v, speed, min, max);
	}

	void NebulaGui::Text(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		ImGui::TextV(fmt, args);
		va_end(args);
	}

	bool NebulaGui::Button(const char* label)
	{
		return ImGui::Button(label);
	}

	bool NebulaGui::Checkbox(const char* label, bool* v)
	{
		return ImGui::Checkbox(label, v);
	}

	bool NebulaGui::SliderFloat(const char* label, float* v, float min, float max)
	{
		return ImGui::SliderFloat(label, v, min, max);
	}

	bool NebulaGui::Image(uint32_t textureID, const glm::vec2& size)
	{
		if (textureID == 0)
			return false; // No valid texture to display

		// ImGui::Image expects void* texture id, cast from uint32_t
		ImGui::Image((void*)(intptr_t)textureID, ImVec2(size.x, size.y));
		return true;
	}

	bool NebulaGui::Image(uint32_t textureID, const glm::vec2& size, const glm::vec2& uv0, const glm::vec2& uv1)
	{
		if (textureID == 0)
			return false;

		ImGui::Image((void*)(intptr_t)textureID, ImVec2(size.x, size.y), ImVec2(uv0.x, uv0.y), ImVec2(uv1.x, uv1.y));
		return true;
	}

	bool NebulaGui::Image(void* textureID, const glm::vec2& size, const glm::vec2& uv0, const glm::vec2& uv1)
	{
		if (textureID == nullptr)
			return false;

		ImGui::Image(
			textureID,
			ImVec2(size.x, size.y),
			ImVec2(uv0.x, uv0.y),
			ImVec2(uv1.x, uv1.y)
		);

		return true;
	}



	glm::vec2 NebulaGui::GetContentRegionAvail()
	{
		ImVec2 size = ImGui::GetContentRegionAvail();
		return glm::vec2(size.x, size.y);
	}

}
