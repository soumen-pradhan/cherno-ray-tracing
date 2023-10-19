#include "Renderer.h"
#include "Walnut/Random.h"

#include <glm/glm.hpp>


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
            m_ImageData[x + y * wt] = PerPixel(coord);
        }
    }

    m_FinalImage->SetData(m_ImageData);
}

uint32_t Renderer::PerPixel(glm::vec2 coord)
{
    uint8_t r = (uint8_t)(coord.x * 255.0f);
    uint8_t g = (uint8_t)(coord.y * 255.0f);
    return 0xff'00'00'00 | (g << 8) | r;
}
