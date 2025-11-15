#pragma once

#include "Nebula/ImGui/NebulaGui.h"
#include <glm/glm.hpp>

namespace Cosmic {

    class Viewport {
    public:
        static glm::vec2 OnImGuiRender(void* textureID, const glm::vec2& framebufferSize, const glm::vec2& uv0 = { 0,1 }, const glm::vec2& uv1 = { 1,0 })
        {
            Nebula::NebulaGui::Begin("Viewport");

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

            Nebula::NebulaGui::Image(textureID, { imageWidth, imageHeight }, uv0, uv1);

            Nebula::NebulaGui::End();
            
            return { imageWidth, imageHeight };
        }


    private:
        Viewport() = delete;
    };

}
