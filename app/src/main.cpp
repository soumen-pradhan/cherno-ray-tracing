#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

#include <fmt/format.h>

class ExampleLayer : public Walnut::Layer {
public:
    virtual void OnUIRender() override
    {
        {
            ImGui::Begin("Settings");

            ImGui::Text("Last render: %.3fms", m_LastRenderTime);

            if (ImGui::Button("Render")) {
                Render();
            }

            ImGui::End();
        }
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
            ImGui::Begin("Viewport");

            m_ViewportWidth = static_cast<uint32_t>(ImGui::GetContentRegionAvail().x);
            m_ViewportHeight = static_cast<uint32_t>(ImGui::GetContentRegionAvail().y);

            if (m_Image) {
                ImGui::Image(m_Image->GetDescriptorSet(),
                    { (float)m_Image->GetWidth(), (float)m_Image->GetHeight() });
            }

            ImGui::End();
            ImGui::PopStyleVar();
        }

        Render();
    }

    void Render()
    {
        Walnut::Timer timer;

        bool imgNotExists = !m_Image;
        bool sizeChanged = m_Image
            && (m_ViewportWidth != m_Image->GetWidth()
                || m_ViewportHeight != m_Image->GetHeight());

        uint32_t imgBufferLen = m_ViewportWidth * m_ViewportHeight;

        if (imgNotExists || sizeChanged) {
            m_Image = std::make_shared<Walnut::Image>(m_ViewportWidth, m_ViewportHeight,
                Walnut::ImageFormat::RGBA);

            delete[] m_ImageData;
            m_ImageData = new uint32_t[imgBufferLen];
        }

        for (uint32_t i = 0; i < imgBufferLen; i++) {
            m_ImageData[i] = Walnut::Random::UInt() | 0xff'ff'00'ff;
        }

        m_Image->SetData(m_ImageData);

        m_LastRenderTime = timer.ElapsedMillis();
    }

private:
    std::shared_ptr<Walnut::Image> m_Image;
    uint32_t* m_ImageData = nullptr;
    uint32_t m_ViewportWidth, m_ViewportHeight;

    float m_LastRenderTime = 0;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
    Walnut::ApplicationSpecification spec;
    spec.Name = "Walnut Example";

    Walnut::Application* app = new Walnut::Application(spec);
    app->PushLayer<ExampleLayer>();
    app->SetMenubarCallback([app]() {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit")) {
                app->Close();
            }
            ImGui::EndMenu();
        }
    });
    return app;
}
