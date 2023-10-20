#ifndef RENDERER_H
#define RENDERER_H

#include "Walnut/Image.h"
#include <glm/glm.hpp>
#include <memory>

#include "Camera.h"
#include "Ray.h"

/// @brief Owns Final Image and its data. Handles creating and resizing image.
class Renderer {
public:
    Renderer() = default;

    /// @brief Creates image if needed, then resizes it.
    void OnResize(uint32_t width, uint32_t height);
    void Render(const Camera& camera);

    auto GetFinalImage() const { return m_FinalImage; }

    float lightDir[3] = { -1, -1, -1 };

private:
    /**
     * @brief Converts camera ray to a RGBA color.
     * @param ray Origin and Direction of camera
     */
    glm::vec4 TraceRay(const Ray& ray);

private:
    std::shared_ptr<Walnut::Image> m_FinalImage;
    uint32_t* m_ImageData = nullptr;
};

#endif // RENDERER_H
