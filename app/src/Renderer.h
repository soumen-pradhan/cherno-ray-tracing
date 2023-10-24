#ifndef RENDERER_H
#define RENDERER_H

#include "Walnut/Image.h"
#include <glm/glm.hpp>

#include <memory>

#include "Camera.h"
#include "Ray.h"
#include "Scene.h"

/// @brief Owns Final Image and its data. Handles creating and resizing image.
class Renderer {
public:
    struct Settings {
        bool Accum = true;
    };

public:
    Renderer() = default;

    /// @brief Creates image if needed, then resizes it.
    void OnResize(uint32_t width, uint32_t height);

    /// @brief Call this in main loop, to create the final image.
    void Render(const Scene& scene, const Camera& camera);

    auto GetFinalImage() const { return m_FinalImage; }

    Settings& GetSettings() { return m_Settings; }
    void ResetFrameIdx() { m_FrameIdx = 1; }

    bool Sky = true;

private:
    /// @brief Description of Hit Point and Object.
    struct HitPayload {
        float HitDist;
        glm::vec3 WorldPos, WorldNormal;
        int ObjectIdx;
    };

    glm::vec4 PerPixel(uint32_t x, uint32_t y);

    /**
     * @brief Converts camera ray to a RGBA color. Calls `ClosestHit` or `Miss`.
     * @param ray Origin and Direction of camera
     */
    HitPayload TraceRay(const Ray& ray);
    HitPayload ClosestHit(const Ray& ray, float hitDist, int objectIdx);
    HitPayload Miss(const Ray& ray);

private:
    std::shared_ptr<Walnut::Image> m_FinalImage;
    uint32_t* m_ImageData = nullptr;

    Settings m_Settings;
    glm::vec4* m_AccumData = nullptr;
    uint32_t m_FrameIdx = 1;

    const Scene* m_ActiveScene = nullptr;
    const Camera* m_ActiveCamera = nullptr;

    /// @brief Hold image buffer indices for parallel CPU execution.
    std::vector<uint32_t> m_ImgHori, m_ImgVert;
};

#endif // RENDERER_H
