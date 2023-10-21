#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Color.h"
#include "Renderer.h"
#include "Walnut/Random.h"

namespace Utils {

/**
 * @brief Convert `vec4` to `uint32_t` ABGR.
 * @param color Values must be clamped to [0,1]
 */
static uint32_t Vec2Rgba(const glm::vec4& color)
{
    auto r = (uint8_t)(color.r * 255.0f);
    auto g = (uint8_t)(color.g * 255.0f);
    auto b = (uint8_t)(color.b * 255.0f);
    auto a = (uint8_t)(color.a * 255.0f);

    // aa | bb | gg | rr -> 8 bytes
    return (a << 24) | (b << 16) | (g << 8) | r;
}

const float Inf = std::numeric_limits<float>::max();

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

void Renderer::Render(const Scene& scene, const Camera& camera)
{
    uint32_t wt = m_FinalImage->GetWidth(), ht = m_FinalImage->GetHeight();

    auto& rayDirs = camera.GetRayDirections();

    Ray ray = { .Origin = camera.GetPosition() };

    for (uint32_t y = 0; y < ht; y++) {
        for (uint32_t x = 0; x < wt; x++) {
            ray.Direction = rayDirs[x + y * wt];

            auto color = TraceRay(scene, ray);
            color = glm::clamp(color, { 0 }, { 1 });
            m_ImageData[x + y * wt] = Utils::Vec2Rgba(color);
        }
    }

    m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::TraceRay(const Scene& scene, const Ray& ray)
{
    // (bx^2 + by^2) t^2 + (2 (axbx + ayby)) t + (ax^2 + ay^2 - r^2) = 0
    // At^2 + Bt + C = 0
    // where
    // a = ray origin
    // b = ray dir
    // r = radius
    // t = hit dist

    if (scene.Spheres.empty()) {
        return glm::vec4(Color::Black, 1.0f);
    }

    const Sphere* closestSphere = nullptr;
    float hitDist = Utils::Inf;

    for (auto& sphere : scene.Spheres) {
        glm::vec3 origin = ray.Origin - sphere.Pos;

        float A = glm::dot(ray.Direction, ray.Direction);
        float B = 2.0f * glm::dot(origin, ray.Direction);
        float C = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

        // B^2 - 4AC
        float discriminant = B * B - 4.0F * A * C;

        if (discriminant < 0.0f) {
            continue;
        }

        // 2 roots: (-B ± √D) / 2A
        // float t0 = (-B + glm::sqrt(discriminant)) / (2.0f * A);
        float closestHit = (-B - glm::sqrt(discriminant)) / (2.0f * A);

        if (closestHit < hitDist) {
            hitDist = closestHit;
            closestSphere = &sphere;
        }
    }

    if (!closestSphere) {
        return glm::vec4(Color::Black, 1.0f);
    }

    // hit pos
    glm::vec3 origin = ray.Origin - closestSphere->Pos;
    glm::vec3 hitPoint = ray.Origin + ray.Direction * hitDist;
    glm::vec3 normal = glm::normalize(hitPoint);

    // glm::vec3 light = glm::normalize(glm::vec3(-1, -1, -1));
    auto light = glm::normalize(LightDir);
    float lightComp = glm::max(glm::dot(normal, -light), 0.0f);

    glm::vec3 sphereColor = closestSphere->Albedo;
    sphereColor *= lightComp;

    return glm::vec4(sphereColor, 1.0f);
}
