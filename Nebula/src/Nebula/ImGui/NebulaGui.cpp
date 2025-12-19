#include "nbpch.h"
#include "NebulaGui.h"
#include <imgui.h>

namespace Nebula {

	// Window
	bool NebulaGui::Begin(const char* name, bool* p_open) { return ImGui::Begin(name, p_open); }
	void NebulaGui::End() { ImGui::End(); }

	// Drag/Slider
	bool NebulaGui::DragFloat(const char* label, float* v, float speed, float min, float max) { return ImGui::DragFloat(label, v, speed, min, max); }
	bool NebulaGui::DragFloat2(const char* label, float* v, float speed, float min, float max) { return ImGui::DragFloat2(label, v, speed, min, max); }
	bool NebulaGui::DragFloat3(const char* label, float* v, float speed, float min, float max) { return ImGui::DragFloat3(label, v, speed, min, max); }
	bool NebulaGui::DragFloat4(const char* label, float* v, float speed, float min, float max) { return ImGui::DragFloat4(label, v, speed, min, max); }
	bool NebulaGui::SliderFloat(const char* label, float* v, float min, float max) { return ImGui::SliderFloat(label, v, min, max); }

	// Text/Label
	void NebulaGui::Text(const char* fmt, ...) { va_list args; va_start(args, fmt); ImGui::TextV(fmt, args); va_end(args); }
	void NebulaGui::TextWrapped(const char* fmt, ...) { va_list args; va_start(args, fmt); ImGui::TextWrappedV(fmt, args); va_end(args); }
	void NebulaGui::TextColored(const glm::vec4& col, const char* fmt, ...) { va_list args; va_start(args, fmt); ImGui::TextColoredV(ImVec4(col.x, col.y, col.z, col.w), fmt, args); va_end(args); }

	// Widgets
	bool NebulaGui::Button(const char* label) { return ImGui::Button(label); }
	bool NebulaGui::Button(const char* label, const glm::vec2& size) { return ImGui::Button(label, ImVec2(size.x, size.y)); }
	bool NebulaGui::Checkbox(const char* label, bool* v) { return ImGui::Checkbox(label, v); }
	bool NebulaGui::Selectable(const char* label, bool selected) { return ImGui::Selectable(label, selected); }

	// Input
	bool NebulaGui::InputText(const char* label, char* buf, size_t buf_size) { return ImGui::InputText(label, buf, buf_size); }

	// Combo
	bool NebulaGui::BeginCombo(const char* label, const char* preview_value) { return ImGui::BeginCombo(label, preview_value); }
	void NebulaGui::EndCombo() { ImGui::EndCombo(); }

	// Color
	bool NebulaGui::ColorEdit4(const char* label, float* col) { return ImGui::ColorEdit4(label, col); }
	bool NebulaGui::ColorEdit3(const char* label, float* col) { return ImGui::ColorEdit3(label, col); }

	// Tree/Collapsing
	bool NebulaGui::CollapsingHeader(const char* label, bool defaultOpen) { return ImGui::CollapsingHeader(label, defaultOpen ? ImGuiTreeNodeFlags_DefaultOpen : 0); }

	// Separator/Spacing
	void NebulaGui::Separator() { ImGui::Separator(); }
	void NebulaGui::Dummy(const glm::vec2& size) { ImGui::Dummy(ImVec2(size.x, size.y)); }
	void NebulaGui::SameLine(float posX, float spacingW) { ImGui::SameLine(posX, spacingW); }

	// Menu
	bool NebulaGui::BeginMenuBar() { return ImGui::BeginMenuBar(); }
	void NebulaGui::EndMenuBar() { ImGui::EndMenuBar(); }
	bool NebulaGui::BeginMenu(const char* label) { return ImGui::BeginMenu(label); }
	void NebulaGui::EndMenu() { ImGui::EndMenu(); }
	bool NebulaGui::MenuItem(const char* label, const char* shortcut) { return ImGui::MenuItem(label, shortcut); }

	// Popup
	void NebulaGui::OpenPopup(const char* str_id) { ImGui::OpenPopup(str_id); }
	bool NebulaGui::BeginPopup(const char* str_id) { return ImGui::BeginPopup(str_id); }
	bool NebulaGui::BeginPopupContextItem(const char* str_id) { return ImGui::BeginPopupContextItem(str_id); }
	bool NebulaGui::BeginPopupContextWindow(const char* str_id, ImGuiPopupFlags flags, bool also_over_items) { return ImGui::BeginPopupContextWindow(str_id, flags | (also_over_items ? 0 : ImGuiPopupFlags_NoOpenOverItems)); }
	void NebulaGui::EndPopup() { ImGui::EndPopup(); }
	void NebulaGui::CloseCurrentPopup() { ImGui::CloseCurrentPopup(); }

	// Columns/Layout
	void NebulaGui::Columns(int count, const char* id, bool border) { ImGui::Columns(count, id, border); }
	void NebulaGui::NextColumn() { ImGui::NextColumn(); }

	// ID stack
	void NebulaGui::PushID(const char* str_id) { ImGui::PushID(str_id); }
	void NebulaGui::PushID(int str_id) { ImGui::PushID(str_id); }
	void NebulaGui::PopID() { ImGui::PopID(); }

	// Drag and Drop
	bool NebulaGui::BeginDragDropSource() { return ImGui::BeginDragDropSource(); }
	bool NebulaGui::SetDragDropPayload(const char* type, const void* data, size_t size) { return ImGui::SetDragDropPayload(type, data, size); }
	void NebulaGui::EndDragDropSource() { ImGui::EndDragDropSource(); }
	bool NebulaGui::BeginDragDropTarget() { return ImGui::BeginDragDropTarget(); }
	const void* NebulaGui::AcceptDragDropPayload(const char* type) {
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(type);
		return payload ? payload->Data : nullptr;
	}
	void NebulaGui::EndDragDropTarget() { ImGui::EndDragDropTarget(); }

	// Images
	bool NebulaGui::Image(uint32_t textureID, const glm::vec2& size) { ImGui::Image((void*)(intptr_t)textureID, ImVec2(size.x, size.y)); return false; }
	bool NebulaGui::Image(uint32_t textureID, const glm::vec2& size, const glm::vec2& uv0, const glm::vec2& uv1) { ImGui::Image((void*)(intptr_t)textureID, ImVec2(size.x, size.y), ImVec2(uv0.x, uv0.y), ImVec2(uv1.x, uv1.y)); return false; }
	bool NebulaGui::Image(void* textureID, const glm::vec2& size, const glm::vec2& uv0, const glm::vec2& uv1) { ImGui::Image(textureID, ImVec2(size.x, size.y), ImVec2(uv0.x, uv0.y), ImVec2(uv1.x, uv1.y)); return false; }
	bool NebulaGui::ImageButton(const char* str_id, uint32_t textureID, const glm::vec2& size) { return ImGui::ImageButton(str_id, (void*)(intptr_t)textureID, ImVec2(size.x, size.y)); }
	bool NebulaGui::ImageButton(const char* str_id, void* textureID, const glm::vec2& size) { return ImGui::ImageButton(str_id, textureID, ImVec2(size.x, size.y)); }

	// Docking
	void NebulaGui::DockSpaceOverViewport() { ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport()); }

	// Utility
	glm::vec2 NebulaGui::GetContentRegionAvail() { auto avail = ImGui::GetContentRegionAvail(); return glm::vec2(avail.x, avail.y); }
	glm::vec2 NebulaGui::GetWindowPos() { auto pos = ImGui::GetWindowPos(); return glm::vec2(pos.x, pos.y); }
	glm::vec2 NebulaGui::GetContentRegionMin() { auto min = ImGui::GetWindowContentRegionMin(); return glm::vec2(min.x, min.y); }
	glm::vec2 NebulaGui::GetContentRegionMax() { auto max = ImGui::GetWindowContentRegionMax(); return glm::vec2(max.x, max.y); }
	bool NebulaGui::IsWindowHovered() { return ImGui::IsWindowHovered(); }
	bool NebulaGui::IsWindowFocused() { return ImGui::IsWindowFocused(); }
	glm::vec2 NebulaGui::GetItemRectMin() { auto min = ImGui::GetItemRectMin(); return glm::vec2(min.x, min.y); }
	glm::vec2 NebulaGui::GetItemRectMax() { auto max = ImGui::GetItemRectMax(); return glm::vec2(max.x, max.y); }
	void NebulaGui::SetItemDefaultFocus() { ImGui::SetItemDefaultFocus(); }
	glm::vec2 NebulaGui::GetCursorScreenPos() { auto pos = ImGui::GetCursorScreenPos(); return glm::vec2(pos.x, pos.y); }
	void* NebulaGui::GetWindowDrawList() { return ImGui::GetWindowDrawList(); }
	void* NebulaGui::GetForegroundDrawList() { return ImGui::GetForegroundDrawList(); }
	glm::vec2 NebulaGui::GetMousePos() { auto pos = ImGui::GetMousePos(); return glm::vec2(pos.x, pos.y); }

	// Scrolling
	float NebulaGui::GetScrollY() { return ImGui::GetScrollY(); }
	float NebulaGui::GetScrollMaxY() { return ImGui::GetScrollMaxY(); }
	void NebulaGui::SetScrollHereY(float center_y_ratio) { ImGui::SetScrollHereY(center_y_ratio); }

	// Style
	void NebulaGui::PushStyleVar(int idx, const glm::vec2& val) { ImGui::PushStyleVar(idx, ImVec2(val.x, val.y)); }
	void NebulaGui::PopStyleVar(int count) { ImGui::PopStyleVar(count); }
	void NebulaGui::PushStyleColor(int idx, const glm::vec4& col) { ImGui::PushStyleColor(idx, ImVec4(col.x, col.y, col.z, col.w)); }
	void NebulaGui::PopStyleColor(int count) { ImGui::PopStyleColor(count); }

}
