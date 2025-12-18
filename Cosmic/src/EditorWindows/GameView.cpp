#include "GameView.h"

namespace Cosmic {
    // Define static members
    glm::vec2 GameView::s_GameViewBounds[2] = { {0, 0}, {0, 0} };
    bool GameView::s_GameViewHovered = false;
    bool GameView::s_GameViewFocused = false;
}
