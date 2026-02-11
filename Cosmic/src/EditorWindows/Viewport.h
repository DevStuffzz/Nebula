#pragma once

#include "Nebula/ImGui/NebulaGui.h"
#include <glm/glm.hpp>
#include <functional>

namespace Cosmic {

	class Viewport {
	public:
		enum class GizmoMode {
			None = 0,
			Translate,
			Rotate,
			Scale
		};

		static glm::vec2 OnImGuiRender(
			void* textureID,
			const glm::vec2& framebufferSize,
			const glm::vec2& uv0 = { 0, 1 },
			const glm::vec2& uv1 = { 1, 0 },
			std::function<void()> gizmoCallback = nullptr)
		{
			Nebula::NebulaGui::Begin("Viewport");

			// ───────────────── Gizmo Toolbar ─────────────────
			Nebula::NebulaGui::PushID("GizmoToolbar");

			auto pushButtonStyle = [](bool selected) {
				if (selected) {
					Nebula::NebulaGui::PushStyleColor(21, { 0.3f, 0.5f, 0.8f, 1.0f });
					Nebula::NebulaGui::PushStyleColor(22, { 0.4f, 0.6f, 0.9f, 1.0f });
					Nebula::NebulaGui::PushStyleColor(23, { 0.2f, 0.4f, 0.7f, 1.0f });
				}
				};

			pushButtonStyle(s_CurrentGizmoMode == GizmoMode::None);
			bool btnNone = Nebula::NebulaGui::Button("X", { 30.0f, 30.0f });
			if (s_CurrentGizmoMode == GizmoMode::None) Nebula::NebulaGui::PopStyleColor(3);

			Nebula::NebulaGui::SameLine();

			pushButtonStyle(s_CurrentGizmoMode == GizmoMode::Translate);
			bool btnTranslate = Nebula::NebulaGui::Button("T", { 30.0f, 30.0f });
			if (s_CurrentGizmoMode == GizmoMode::Translate) Nebula::NebulaGui::PopStyleColor(3);

			Nebula::NebulaGui::SameLine();

			pushButtonStyle(s_CurrentGizmoMode == GizmoMode::Rotate);
			bool btnRotate = Nebula::NebulaGui::Button("R", { 30.0f, 30.0f });
			if (s_CurrentGizmoMode == GizmoMode::Rotate) Nebula::NebulaGui::PopStyleColor(3);

			Nebula::NebulaGui::SameLine();

			pushButtonStyle(s_CurrentGizmoMode == GizmoMode::Scale);
			bool btnScale = Nebula::NebulaGui::Button("S", { 30.0f, 30.0f });
			if (s_CurrentGizmoMode == GizmoMode::Scale) Nebula::NebulaGui::PopStyleColor(3);

			Nebula::NebulaGui::PopID();
			Nebula::NebulaGui::Separator();

			if (btnNone)      s_CurrentGizmoMode = GizmoMode::None;
			if (btnTranslate) s_CurrentGizmoMode = GizmoMode::Translate;
			if (btnRotate)    s_CurrentGizmoMode = GizmoMode::Rotate;
			if (btnScale)     s_CurrentGizmoMode = GizmoMode::Scale;

			// ───────────────── Viewport Image ─────────────────
			glm::vec2 availSize = Nebula::NebulaGui::GetContentRegionAvail();

			float imageWidth = availSize.x;
			float imageHeight = availSize.y;

			glm::vec2 imageTopLeft = Nebula::NebulaGui::GetCursorScreenPos();

			s_ViewportBounds[0] = imageTopLeft;
			s_ViewportBounds[1] = { imageTopLeft.x + imageWidth, imageTopLeft.y + imageHeight };

			s_ViewportHovered = Nebula::NebulaGui::IsWindowHovered();
			s_ViewportFocused = Nebula::NebulaGui::IsWindowFocused();

			Nebula::NebulaGui::Image(textureID, { imageWidth, imageHeight }, uv0, uv1);

			// Gizmo overlay
			if (gizmoCallback)
				gizmoCallback();

			Nebula::NebulaGui::End();

			return { imageWidth, imageHeight };
		}

		static const glm::vec2* GetViewportBounds() { return s_ViewportBounds; }
		static bool IsViewportHovered() { return s_ViewportHovered; }
		static bool IsViewportFocused() { return s_ViewportFocused; }
		static GizmoMode GetGizmoMode() { return s_CurrentGizmoMode; }

	private:
		Viewport() = delete;

		static glm::vec2 s_ViewportBounds[2];
		static bool s_ViewportHovered;
		static bool s_ViewportFocused;
		static GizmoMode s_CurrentGizmoMode;
	};

}
