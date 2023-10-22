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

    Red_600    = FromHex(0xdc'26'26),
    Orange_600 = FromHex(0xea'58'0c),
    Green_600  = FromHex(0x16'a3'4a),
    Teal_700   = FromHex(0x0f'76'6e),
    Sky_200    = FromHex(0xba'e6'fd),
    Sky_300    = FromHex(0x7d'd3'fc),
    Sky_400    = FromHex(0x38'bd'f8),
    Blue_600   = FromHex(0x25'63'eb),
    Blue_700   = FromHex(0x1d'4e'd8),
    Blue_800   = FromHex(0x1e'40'af),
    Pink_600   = FromHex(0xdb'27'77),

    Magenta {1.0f, 0.0f, 1.0f};

} // namespace Color

#endif // COLOR_H
