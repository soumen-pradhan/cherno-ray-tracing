#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <execution> // execution::par
#include <numeric> // iota

#include "Color.h"
#include "Renderer.h"
#include "Walnut/Random.h"

namespace Utils {

///@brief Convert RGBA to ABGR. `color` values must be clamped to `[0,1]`.
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

} // namespace Utils

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

    delete[] m_AccumData;
    m_AccumData = new glm::vec4[imgBufferLen];

    m_ImgHori.resize(width);
    m_ImgVert.resize(height);
    std::iota(std::begin(m_ImgHori), std::end(m_ImgHori), 0);
    std::iota(std::begin(m_ImgVert), std::end(m_ImgVert), 0);
}

void Renderer::Render(const Scene& scene, const Camera& camera)
{
    uint32_t wt = m_FinalImage->GetWidth(), ht = m_FinalImage->GetHeight();

    m_ActiveScene = &scene;
    m_ActiveCamera = &camera;

    if (m_FrameIdx == 1) {
        std::memset(m_AccumData, 0, wt * ht * sizeof(glm::vec4));
    }

    std::for_each(std::execution::par, std::begin(m_ImgVert), std::end(m_ImgVert),
        [this, wt](uint32_t y) {
            std::for_each(std::execution::par, std::begin(m_ImgHori), std::end(m_ImgHori),
                [this, y, wt](uint32_t x) {
                    auto color = PerPixel(x, y);

                    auto& accumColor = m_AccumData[x + y * wt];
                    accumColor += color;

                    color = glm::clamp(accumColor / (float)m_FrameIdx, { 0 }, { 1 });

                    m_ImageData[x + y * wt] = Utils::Vec2Rgba(color);
                });
        });

    m_FinalImage->SetData(m_ImageData);

    m_FrameIdx = m_Settings.Accum ? m_FrameIdx + 1 : 1;
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
    auto imgWt = m_FinalImage->GetWidth();
    Ray ray = {
        .Origin = m_ActiveCamera->GetPosition(),
        .Direction = m_ActiveCamera->GetRayDirections()[x + y * imgWt]
    };

    glm::vec3 skyColor = Color::Sky_300;

    // Change the contribution of `light` for each bounce.
    glm::vec3 contribution { 1.0f };
    glm::vec3 light = Color::Black;

    const int bounces = 8;

    for (int i = 0; i < bounces; i++) {
        auto payload = TraceRay(ray);

        if (payload.HitDist < 0.0f) {
            light += skyColor * contribution;
            break;
        }

        auto& sphere = m_ActiveScene->Spheres[payload.ObjectIdx];
        auto& material = m_ActiveScene->Materials[sphere.MatIdx];

        contribution *= material.Albedo;
        light += material.GetEmission();

        // Reflect about the normal, then jitter the reflected ray a bit (simulate roughness).
        ray.Origin = payload.WorldPos + payload.WorldNormal * 0.0001f;
        ray.Direction = glm::normalize(payload.WorldNormal + Walnut::Random::InUnitSphere());
    }

    return glm::vec4(light, 1.0f);
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{
    // (bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0
    // where
    // a = ray origin
    // b = ray direction
    // r = radius
    // t = hit distance

    int closestSphereIdx = -1;
    float hitDist = Utils::Inf;

    for (int idx = 0; idx < m_ActiveScene->Spheres.size(); idx++) {
        auto& sphere = m_ActiveScene->Spheres[idx];
        auto origin = ray.Origin - sphere.Pos;

        float A = glm::dot(ray.Direction, ray.Direction);
        float B = 2.0f * glm::dot(origin, ray.Direction);
        float C = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

        // B^2 - 4AC
        float discriminant = B * B - 4.0f * A * C;

        if (discriminant < 0.0f) {
            continue;
        }

        // 2 Quadratic Roots: (-B ± √D) / 2A

        // float t0 = (-B + glm::sqrt(discriminant)) / (2.0f * A);
        float closestHit = (-B - glm::sqrt(discriminant)) / (2.0f * A);

        if (closestHit > 0.0f && closestHit < hitDist) {
            hitDist = closestHit;
            closestSphereIdx = idx;
        }
    }

    if (closestSphereIdx < 0.0f) {
        return Miss(ray);
    }

    return ClosestHit(ray, hitDist, closestSphereIdx);
}

Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDist, int objectIdx)
{
    auto& closestSphere = m_ActiveScene->Spheres[objectIdx];
    auto shiftedOrigin = ray.Origin - closestSphere.Pos;
    auto shiftedWorldPos = shiftedOrigin + ray.Direction * hitDist;

    return HitPayload {
        .HitDist = hitDist,
        .WorldPos = shiftedWorldPos + closestSphere.Pos,
        .WorldNormal = glm::normalize(shiftedWorldPos),
        .ObjectIdx = objectIdx
    };
}

Renderer::HitPayload Renderer::Miss(const Ray& ray)
{
    return HitPayload {
        .HitDist = -1.0f
    };
}
