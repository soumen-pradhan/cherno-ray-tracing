#ifndef COLOR_H
#define COLOR_H

#include <glm/glm.hpp>

consteval static glm::vec3 FromHex(uint32_t hex)
{
    uint8_t r = (hex & 0xff'00'00) >> 16;
    uint8_t g = (hex & 0x00'ff'00) >> 8;
    uint8_t b = (hex & 0x00'00'ff);

    return { r / 255.0f, g / 255.0f, b / 255.0f };
}

/// @brief Tailwind-css color classes
/// @link https://tailwindcss.com/docs/customizing-colors
namespace Color {

// clang-format off

constexpr glm::vec3
    White { 1.0f },
    Black { 0.0f },

    Slate_800  = FromHex(0x1e293b),
    Slate_900  = FromHex(0x0f172a),
    Slate_950  = FromHex(0x020617),
    Red_600    = FromHex(0xdc2626),
    Orange_600 = FromHex(0xea580c),
    Green_600  = FromHex(0x16a34a),
    Green_900  = FromHex(0x14532d),
    Teal_700   = FromHex(0x0f766e),
    Sky_200    = FromHex(0xbae6fd),
    Sky_300    = FromHex(0x7dd3fc),
    Sky_950    = FromHex(0x082f49),
    Sky_400    = FromHex(0x38bdf8),
    Blue_600   = FromHex(0x2563eb),
    Blue_700   = FromHex(0x1d4ed8),
    Blue_800   = FromHex(0x1e40af),
    Pink_600   = FromHex(0xdb2777),

    Magenta {1.0f, 0.0f, 1.0f};

} // namespace Color

#endif // COLOR_H
