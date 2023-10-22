#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include <fmt/format.h>

#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Color.h"
#include "Renderer.h"

class ExampleLayer : public Walnut::Layer {
public:
    ExampleLayer()
        : m_Camera(45.0f, 0.1f, 100.0f)
    {
        // materials
        {
            m_Scene.Materials.emplace_back(Material {
                .Albedo = Color::Magenta,
                .Roughness = 0.0f,
            });

            m_Scene.Materials.emplace_back(Material {
                .Albedo = Color::Blue_800,
                .Roughness = 0.1f,
            });
        }

        m_Scene.Spheres.emplace_back(Sphere {
            .Pos = { 0.0f, -0.2f, -3.0f },
            .Radius = 0.8f,
            .MatIdx = 0,
        });

        m_Scene.Spheres.emplace_back(Sphere {
            .Pos = { 0.0f, -101.0f, -3.0f },
            .Radius = 100.0f,
            .MatIdx = 1,
        });
    }

    virtual void OnUpdate(float ts) override
    {
        if (m_Camera.OnUpdate(ts)) {
            m_Renderer.ResetFrameIdx();
        }
    }

    virtual void OnUIRender() override
    {
        {
            ImGui::Begin("Settings");

            ImGui::Text("Last render: %.3fms", m_LastRenderTime);

            if (ImGui::Button("Render")) {
                Render();
            }

            ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accum);

            if (ImGui::Button("Reset")) {
                m_Renderer.ResetFrameIdx();
            }

            ImGui::End();
        }
        {
            ImGui::Begin("Scene");

            ImGui::SliderFloat3("Light Dir", glm::value_ptr(m_Renderer.LightDir), -2, 2);
            ImGui::Separator();

            for (int i = 0; auto& sphere : m_Scene.Spheres) {
                ImGui::PushID(i);

                ImGui::DragFloat3("Position", glm::value_ptr(sphere.Pos), 0.1f);
                ImGui::DragFloat("Radius", &sphere.Radius, 0.1f);
                ImGui::DragInt("Material", &sphere.MatIdx,
                    1.0f, 0, (int)m_Scene.Materials.size() - 1);

                ImGui::Separator();

                ImGui::PopID();
                i++;
            }

            for (int i = 0; auto& material : m_Scene.Materials) {
                ImGui::PushID(i);

                ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo));
                ImGui::DragFloat("Roughness", &material.Roughness, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Metallic", &material.Metal, 0.01f, 0.0f, 1.0f);

                ImGui::Separator();

                ImGui::PopID();
                i++;
            }

            ImGui::End();
        }
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
            ImGui::Begin("Viewport");

            m_ViewportWidth = (uint32_t)ImGui::GetContentRegionAvail().x;
            m_ViewportHeight = (uint32_t)ImGui::GetContentRegionAvail().y;

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
        m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
        m_Renderer.Render(m_Scene, m_Camera);

        m_LastRenderTime = timer.ElapsedMillis();
    }

private:
    Renderer m_Renderer;
    Camera m_Camera;
    Scene m_Scene;
    uint32_t m_ViewportWidth, m_ViewportHeight;

    float m_LastRenderTime = 0;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
    Walnut::ApplicationSpecification spec;
    spec.Name = "Walnut Example";

    Walnut::Application* app = new Walnut::Application(spec);
    app->PushLayer<ExampleLayer>();
    // app->SetMenubarCallback([app]() {
    //     if (ImGui::BeginMenu("File")) {
    //         if (ImGui::MenuItem("Exit")) {
    //             app->Close();
    //         }
    //         ImGui::EndMenu();
    //     }
    // });
    return app;
}
