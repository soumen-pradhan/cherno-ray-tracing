#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <vector>

/// @brief Controls camera position and movement. Pass an instance of it to renderer.
class Camera {
public:
    Camera(float verticalFOV, float nearClip, float farClip);

    /**
     * @brief Check mouse for rotation and keys for translation.
     * @param ts timestep, also called delta-time.
     * @return true if camera moved.
     */
    bool OnUpdate(float ts);
    void OnResize(uint32_t width, uint32_t height);

    const glm::mat4& GetProjection() const { return m_Projection; }
    const glm::mat4& GetInverseProjection() const { return m_InverseProjection; }
    const glm::mat4& GetView() const { return m_View; }
    const glm::mat4& GetInverseView() const { return m_InverseView; }

    const glm::vec3& GetPosition() const { return m_Position; }
    const glm::vec3& GetDirection() const { return m_ForwardDirection; }

    /// @brief A ray for every viewport fragment is calculated.
    const std::vector<glm::vec3>& GetRayDirections() const { return m_RayDirections; }

    /// @brief Used to adjust mouse sensitivity.
    float GetRotationSpeed();

private:
    void RecalculateProjection();
    void RecalculateView();
    void RecalculateRayDirections();

private:
    glm::mat4 m_Projection { 1.0f };
    glm::mat4 m_View { 1.0f };
    glm::mat4 m_InverseProjection { 1.0f };
    glm::mat4 m_InverseView { 1.0f };

    float m_VerticalFOV = 45.0f;
    float m_NearClip = 0.1f;
    float m_FarClip = 100.0f;

    glm::vec3 m_Position { 0.0f, 0.0f, 0.0f };
    glm::vec3 m_ForwardDirection { 0.0f, 0.0f, 0.0f };

    /// @brief Cached ray directions calculated by @ref `RecalculateRayDirections`
    std::vector<glm::vec3> m_RayDirections;

    glm::vec2 m_LastMousePosition { 0.0f, 0.0f };

    uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
};

#endif // CAMERA_H
