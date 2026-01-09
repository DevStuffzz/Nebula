#pragma once
#pragma warning(disable: 4251)

#include <glm/glm.hpp>
#include <string>
#include "Nebula/Core.h"

// Forward declare ImGui types to avoid exposing imgui.h
typedef int ImGuiPopupFlags;

namespace Nebula {

	// ImGui Style Variables
	enum NebulaGuiStyleVar_
	{
		NebulaGuiStyleVar_FramePadding = 11,    // ImVec2    FramePadding
		NebulaGuiStyleVar_ItemSpacing = 14,     // ImVec2    ItemSpacing
	};

	class NEBULA_API NebulaGui {
	public:
		// Window
		static bool Begin(const char* name, bool* p_open = nullptr);
		static void End();

		// Drag/Slider
		static bool DragFloat(const char* label, float* v, float speed = 1.0f, float min = 0.0f, float max = 0.0f);
		static bool DragFloat2(const char* label, float* v, float speed = 1.0f, float min = 0.0f, float max = 0.0f);
		static bool DragFloat3(const char* label, float* v, float speed = 1.0f, float min = 0.0f, float max = 0.0f);
		static bool DragFloat4(const char* label, float* v, float speed = 1.0f, float min = 0.0f, float max = 0.0f);
		static bool DragInt(const char* label, int* v, float speed = 1.0f, int min = 0, int max = 0);
		static bool SliderFloat(const char* label, float* v, float min, float max);

		// Text/Label
		static void Text(const char* fmt, ...);
		static void TextWrapped(const char* fmt, ...);
		static void TextColored(const glm::vec4& col, const char* fmt, ...);

		// Widgets
		static bool Button(const char* label);
		static bool Button(const char* label, const glm::vec2& size);
		static bool Checkbox(const char* label, bool* v);
		static bool Selectable(const char* label, bool selected = false);

		// Input
		static bool InputText(const char* label, char* buf, size_t buf_size);
		static bool InputTextMultiline(const char* label, char* buf, size_t buf_size, const glm::vec2& size = glm::vec2(0, 0), int flags = 0);

		// Combo
		static bool BeginCombo(const char* label, const char* preview_value);
		static void EndCombo();

		// Color
		static bool ColorEdit4(const char* label, float* col);
		static bool ColorEdit3(const char* label, float* col);

		// Tree/Collapsing
		static bool CollapsingHeader(const char* label, bool defaultOpen = false);

		// Separator/Spacing
		static void Separator();
		static void Dummy(const glm::vec2& size);
		static void SameLine(float posX = 0.0f, float spacingW = -1.0f);

		// Menu
		static bool BeginMenuBar();
		static void EndMenuBar();
		static bool BeginMenu(const char* label);
		static void EndMenu();
		static bool MenuItem(const char* label, const char* shortcut = nullptr);

		// Popup
		static void OpenPopup(const char* str_id);
		static bool BeginPopup(const char* str_id);
		static bool BeginPopupContextItem(const char* str_id = nullptr);
		static bool BeginPopupContextWindow(const char* str_id = nullptr, ImGuiPopupFlags flags = 1, bool also_over_items = false);
		static void EndPopup();
		static void CloseCurrentPopup();

		// Tab Bar
		static bool BeginTabBar(const char* label);
		static void EndTabBar();

		static bool BeginTabItem(const char* label, bool open);
		static void EndTabItem();

		// Columns/Layout
		static void Columns(int count, const char* id = nullptr, bool border = true);
		static void NextColumn();

		// ID stack
		static void PushID(const char* str_id);
		static void PushID(int str_id);
		static void PopID();

		// Drag and Drop
		static bool BeginDragDropSource();
		static bool SetDragDropPayload(const char* type, const void* data, size_t size);
		static void EndDragDropSource();
		static bool BeginDragDropTarget();
		static const void* AcceptDragDropPayload(const char* type);
		static void EndDragDropTarget();

		// Images
		static bool Image(uint32_t textureID, const glm::vec2& size);
		static bool Image(uint32_t textureID, const glm::vec2& size, const glm::vec2& uv0, const glm::vec2& uv1);
		static bool Image(void* textureID, const glm::vec2& size, const glm::vec2& uv0 = { 0, 0 }, const glm::vec2& uv1 = { 1, 1 });
		static bool ImageButton(const char* str_id, uint32_t textureID, const glm::vec2& size);
		static bool ImageButton(const char* str_id, void* textureID, const glm::vec2& size);

		// Docking
		static void DockSpaceOverViewport();

		// Utility
		static glm::vec2 GetContentRegionAvail();
		static glm::vec2 GetWindowPos();
		static glm::vec2 GetContentRegionMin();
		static glm::vec2 GetContentRegionMax();
		static bool IsWindowHovered();
		static bool IsWindowFocused();
		static glm::vec2 GetItemRectMin();
		static glm::vec2 GetItemRectMax();
		static void SetItemDefaultFocus();
		static glm::vec2 GetCursorScreenPos();
		static void* GetWindowDrawList();
		static void* GetForegroundDrawList();
		static glm::vec2 GetMousePos();
		
		// Scrolling
		static float GetScrollY();
		static float GetScrollMaxY();
		static void SetScrollHereY(float center_y_ratio = 0.5f);
		
		// Clipboard
		static void SetClipboardText(const char* text);
		static const char* GetClipboardText();
		
		// Style
		static void PushStyleVar(int idx, const glm::vec2& val);
		static void PopStyleVar(int count = 1);
		static void PushStyleColor(int idx, const glm::vec4& col);
		static void PopStyleColor(int count = 1);
	};

}