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

        // Gizmo mode toolbar - simple buttons without styling for now
        Nebula::NebulaGui::PushID("GizmoToolbar");
        
        bool btnQ = Nebula::NebulaGui::Button("Q", { 30.0f, 30.0f });
        
        Nebula::NebulaGui::SameLine();
        bool btnW = Nebula::NebulaGui::Button("W", { 30.0f, 30.0f });

        Nebula::NebulaGui::SameLine();
        bool btnE = Nebula::NebulaGui::Button("E", { 30.0f, 30.0f });

        Nebula::NebulaGui::SameLine();
        bool btnR = Nebula::NebulaGui::Button("R", { 30.0f, 30.0f });
        
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

        // Store viewport bounds for picking - use actual image bounds, not content region
        auto windowPos = Nebula::NebulaGui::GetWindowPos();
        auto minBound = Nebula::NebulaGui::GetContentRegionMin();
        
        s_ViewportBounds[0] = { windowPos.x + minBound.x + offsetX, windowPos.y + minBound.y + offsetY };
        s_ViewportBounds[1] = { windowPos.x + minBound.x + offsetX + imageWidth, windowPos.y + minBound.y + offsetY + imageHeight };
        s_ViewportHovered = Nebula::NebulaGui::IsWindowHovered();
        s_ViewportFocused = Nebula::NebulaGui::IsWindowFocused();

            // Vertical dummy space for offsetY (top margin)
            Nebula::NebulaGui::Dummy({ 0.0f, offsetY });
            // Horizontal offset before image
            Nebula::NebulaGui::SameLine(0.0f, offsetX);

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
