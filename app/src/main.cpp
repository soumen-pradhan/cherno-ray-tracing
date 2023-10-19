#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include <fmt/format.h>

#include "Renderer.h"

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

            auto image = m_Renderer.GetFinalImage();
            if (image) {
                ImGui::Image(image->GetDescriptorSet(),
                    { (float)image->GetWidth(), (float)image->GetHeight() },
                    { 0, 1 }, { 1, 0 });
            }

            ImGui::End();
            ImGui::PopStyleVar();
        }

        Render();
    }

    void Render()
    {
        Walnut::Timer timer;

        m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
        m_Renderer.Render();

        m_LastRenderTime = timer.ElapsedMillis();
    }

private:
    Renderer m_Renderer;
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
