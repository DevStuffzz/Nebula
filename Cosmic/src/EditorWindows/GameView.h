#pragma once

#include "Nebula/ImGui/NebulaGui.h"
#include <glm/glm.hpp>

namespace Cosmic {

    class GameView {
    public:
        static glm::vec2 OnImGuiRender(void* textureID, const glm::vec2& framebufferSize, 
                                      const glm::vec2& uv0 = { 0,1 }, const glm::vec2& uv1 = { 1,0 })
        {
            Nebula::NebulaGui::Begin("Game View");

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

            // Store viewport bounds for reference
            auto windowPos = Nebula::NebulaGui::GetWindowPos();
            auto minBound = Nebula::NebulaGui::GetContentRegionMin();
            
            s_GameViewBounds[0] = { windowPos.x + minBound.x + offsetX, windowPos.y + minBound.y + offsetY };
            s_GameViewBounds[1] = { windowPos.x + minBound.x + offsetX + imageWidth, windowPos.y + minBound.y + offsetY + imageHeight };
            s_GameViewHovered = Nebula::NebulaGui::IsWindowHovered();
            s_GameViewFocused = Nebula::NebulaGui::IsWindowFocused();

            // Vertical dummy space for offsetY (top margin)
            Nebula::NebulaGui::Dummy({ 0.0f, offsetY });
            // Horizontal offset before image
            Nebula::NebulaGui::SameLine(0.0f, offsetX);

            // Display the game view image
            Nebula::NebulaGui::Image(textureID, { imageWidth, imageHeight }, uv0, uv1);

            // Store viewport state
            s_GameViewHovered = Nebula::NebulaGui::IsWindowHovered();
            s_GameViewFocused = Nebula::NebulaGui::IsWindowFocused();

            Nebula::NebulaGui::End();

            return { imageWidth, imageHeight };
        }

        static bool IsGameViewHovered() { return s_GameViewHovered; }
        static bool IsGameViewFocused() { return s_GameViewFocused; }

    private:
        GameView() = delete;
        
        static glm::vec2 s_GameViewBounds[2];
        static bool s_GameViewHovered;
        static bool s_GameViewFocused;
    };

} // namespace Cosmic
