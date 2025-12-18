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

        static glm::vec2 OnImGuiRender(void* textureID, const glm::vec2& framebufferSize, 
                                      const glm::vec2& uv0 = { 0,1 }, const glm::vec2& uv1 = { 1,0 },
                                      std::function<void()> gizmoCallback = nullptr)
        {
        Nebula::NebulaGui::Begin("Viewport");

        // Gizmo mode toolbar with visual feedback for selected mode
        Nebula::NebulaGui::PushID("GizmoToolbar");
        
        // Helper to push button colors based on selection
        auto pushButtonStyle = [](bool isSelected) {
            if (isSelected) {
                Nebula::NebulaGui::PushStyleColor(21, { 0.3f, 0.5f, 0.8f, 1.0f }); // Button
                Nebula::NebulaGui::PushStyleColor(22, { 0.4f, 0.6f, 0.9f, 1.0f }); // ButtonHovered
                Nebula::NebulaGui::PushStyleColor(23, { 0.2f, 0.4f, 0.7f, 1.0f }); // ButtonActive
            }
        };
        
        // None/Exit mode (X)
        pushButtonStyle(s_CurrentGizmoMode == GizmoMode::None);
        bool btnQ = Nebula::NebulaGui::Button("X", { 30.0f, 30.0f });
        if (s_CurrentGizmoMode == GizmoMode::None) Nebula::NebulaGui::PopStyleColor(3);
        
        Nebula::NebulaGui::SameLine();
        
        // Translate mode (T)
        pushButtonStyle(s_CurrentGizmoMode == GizmoMode::Translate);
        bool btnW = Nebula::NebulaGui::Button("T", { 30.0f, 30.0f });
        if (s_CurrentGizmoMode == GizmoMode::Translate) Nebula::NebulaGui::PopStyleColor(3);

        Nebula::NebulaGui::SameLine();
        
        // Rotate mode (R)
        pushButtonStyle(s_CurrentGizmoMode == GizmoMode::Rotate);
        bool btnE = Nebula::NebulaGui::Button("R", { 30.0f, 30.0f });
        if (s_CurrentGizmoMode == GizmoMode::Rotate) Nebula::NebulaGui::PopStyleColor(3);

        Nebula::NebulaGui::SameLine();
        
        // Scale mode (S)
        pushButtonStyle(s_CurrentGizmoMode == GizmoMode::Scale);
        bool btnR = Nebula::NebulaGui::Button("S", { 30.0f, 30.0f });
        if (s_CurrentGizmoMode == GizmoMode::Scale) Nebula::NebulaGui::PopStyleColor(3);
        
        Nebula::NebulaGui::PopID();

        Nebula::NebulaGui::Separator();
        
        if (btnQ) s_CurrentGizmoMode = GizmoMode::None;
        if (btnW) s_CurrentGizmoMode = GizmoMode::Translate;
        if (btnE) s_CurrentGizmoMode = GizmoMode::Rotate;
        if (btnR) s_CurrentGizmoMode = GizmoMode::Scale;

        glm::vec2 availSize = Nebula::NebulaGui::GetContentRegionAvail();

        constexpr float desiredAspectRatio = 16.0f / 9.0f;

        float availWidth = availSize.x;
        float availHeight = availSize.y;

        float imageWidth = availWidth;
        float imageHeight = availWidth / desiredAspectRatio;

        if (imageHeight > availHeight)
        {
            imageHeight = availHeight;
            imageWidth = imageHeight * desiredAspectRatio;
        }

        // Center offsets
        float offsetX = (availWidth - imageWidth) * 0.5f;
        float offsetY = (availHeight - imageHeight) * 0.5f;

        // Vertical dummy space for offsetY (top margin)
        Nebula::NebulaGui::Dummy({ 0.0f, offsetY });
        // Horizontal offset before image
        Nebula::NebulaGui::SameLine(0.0f, offsetX);

        // Store viewport bounds for picking - get cursor position right before rendering image
        glm::vec2 imageTopLeft = Nebula::NebulaGui::GetCursorScreenPos();
        s_ViewportBounds[0] = imageTopLeft;
        s_ViewportBounds[1] = { imageTopLeft.x + imageWidth, imageTopLeft.y + imageHeight };
        s_ViewportHovered = Nebula::NebulaGui::IsWindowHovered();
        s_ViewportFocused = Nebula::NebulaGui::IsWindowFocused();

            Nebula::NebulaGui::Image(textureID, { imageWidth, imageHeight }, uv0, uv1);

            // Render gizmo overlay if callback provided - BEFORE End() to have window context
            if (gizmoCallback)
            {
                gizmoCallback();
            }

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
