#ifndef COLOR_H
#define COLOR_H

#include <glm/glm.hpp>

namespace Color {

constexpr glm::vec3
    White { 1.0f },
    Black { 0.0f },

    Red { 1.0f, 0.0f, 0.0f },
    Blue { 0.0f, 1.0f, 0.0f },
    Green { 0.0f, 0.0f, 1.0f },

    Teal { 0.0f, 0.5f, 0.51f },
    Orange { 0.96f, 0.5f, 0.015f };

} // namespace Color

#endif // COLOR_H
