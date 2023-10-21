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
    Renderer() = default;

    /// @brief Creates image if needed, then resizes it.
    void OnResize(uint32_t width, uint32_t height);
    void Render(const Scene& scene, const Camera& camera);

    auto GetFinalImage() const { return m_FinalImage; }

    glm::vec3 LightDir { -1, -1, -1 };

private:
    struct HitPayload {
        float HitDist;
        glm::vec3 WorldPos, WorldNormal;
        int ObjectIdx;
    };

    glm::vec4 PerPixel(uint32_t x, uint32_t y);

    /**
     * @brief Converts camera ray to a RGBA color.
     * @param ray Origin and Direction of camera
     */
    HitPayload TraceRay(const Ray& ray);
    HitPayload ClosestHit(const Ray& ray, float hitDist, int objectIdx);
    HitPayload Miss(const Ray& ray);

private:
    std::shared_ptr<Walnut::Image> m_FinalImage;
    uint32_t* m_ImageData = nullptr;

    const Scene* m_ActiveScene = nullptr;
    const Camera* m_ActiveCamera = nullptr;
};

#endif // RENDERER_H
