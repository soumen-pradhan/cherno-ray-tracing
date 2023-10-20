#include "Renderer.h"
#include "Walnut/Random.h"

#include <glm/glm.hpp>

namespace Utils {

/**
 * @brief Convert `vec4` to `uint32`
 * @param color Values must be clamped to 0..1
 * @return Color will be in reverse order (ABGR)
 */
static uint32_t Vec2Rgba(const glm::vec4& color)
{
    auto r = static_cast<uint8_t>(color.r * 255.0f);
    auto g = static_cast<uint8_t>(color.g * 255.0f);
    auto b = static_cast<uint8_t>(color.b * 255.0f);
    auto a = static_cast<uint8_t>(color.a * 255.0f);

    // aa | bb | gg | rr -> 8 bytes
    return (a << 24) | (b << 16) | (g << 8) | r;
}

}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
    if (m_FinalImage) {
        bool resizeNotNeeded = m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height;
        if (resizeNotNeeded) {
            return;
        }

        m_FinalImage->Resize(width, height);
    } else {
        m_FinalImage = std::make_shared<Walnut::Image>(width, height,
            Walnut::ImageFormat::RGBA);
    }

    uint32_t imgBufferLen = width * height;

    delete[] m_ImageData;
    m_ImageData = new uint32_t[imgBufferLen];
}

void Renderer::Render()
{
    uint32_t imgBufferLen = m_FinalImage->GetWidth() * m_FinalImage->GetHeight();

    uint32_t wt = m_FinalImage->GetWidth(), ht = m_FinalImage->GetHeight();

    for (uint32_t y = 0; y < ht; y++) {
        for (uint32_t x = 0; x < wt; x++) {
            glm::vec2 coord { (float)x / (float)wt, (float)y / (float)ht };
            coord = coord * 2.0f - 1.0f; // clip-space 0..1 -> -1..1

            auto color = PerPixel(coord);
            color = glm::clamp(color, { 0 }, { 1 });
            m_ImageData[x + y * wt] = Utils::Vec2Rgba(color);
        }
    }

    m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::PerPixel(glm::vec2 coord)
{
    uint8_t r = (uint8_t)(coord.x * 255.0f);
    uint8_t g = (uint8_t)(coord.y * 255.0f);

    glm::vec3 rayOrigin { 0.0f, 0.0f, 1.0f };
    glm::vec3 rayDir { coord.x, coord.y, -1.0f }; // xyz
    // rayDir = glm::normalize(rayDir);

    float radius = 0.5f; // clip-space -1..1

    /**
     * (bx^2 + by^2) t^2 + (2 (axbx + ayby)) t + (ax^2 + ay^2 - r^2) = 0
     * At^2 + Bt + C = 0
     * where
     * a = ray origin
     * b = ray dir
     * r = radius
     * t = hit dist
     */

    float A = glm::dot(rayDir, rayDir);
    float B = 2.0f * glm::dot(rayOrigin, rayDir);
    float C = glm::dot(rayOrigin, rayOrigin) - radius * radius;

    // B^2 - 4AC
    float discriminant = B * B - 4.0F * A * C;

    if (discriminant < 0.0f) {
        return glm::vec4(0, 0, 0, 1); // black
    }

    // 2 roots: (-B ± √D) / 2A
    float t0 = (-B + glm::sqrt(discriminant)) / (2.0f * A);
    float t1 = (-B - glm::sqrt(discriminant)) / (2.0f * A);

    float closestHit = t1;

    // hit pos
    glm::vec3 hitPoint = rayOrigin + rayDir * closestHit;
    glm::vec3 normal = glm::normalize(hitPoint);

    glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));

    float lightComp = glm::max(glm::dot(normal, -lightDir), 0.0f);

    glm::vec3 sphereColor(1, 0, 1);
    sphereColor *= lightComp;

    return glm::vec4(sphereColor, 1.0f);
}
