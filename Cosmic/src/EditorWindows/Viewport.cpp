#include "Viewport.h"

namespace Cosmic {
    // Define static members
    glm::vec2 Viewport::s_ViewportBounds[2] = { {0, 0}, {0, 0} };
    bool Viewport::s_ViewportHovered = false;
    bool Viewport::s_ViewportFocused = false;
    Viewport::GizmoMode Viewport::s_CurrentGizmoMode = Viewport::GizmoMode::None;
}
