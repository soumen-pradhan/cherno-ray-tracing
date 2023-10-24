#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include <fmt/format.h>

#include <glm/gtc/type_ptr.hpp>
#include <nfd.h>

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
                .Albedo = Color::Sky_950,
                .Roughness = 0.1f,
            });

            m_Scene.Materials.emplace_back(Material {
                .Albedo = Color::Orange_600,
                .Roughness = 0.1f,
                .EmissionColor = Color::Orange_600,
                .EmissionPower = 2.0f });
        }

        m_Scene.Spheres.emplace_back(Sphere {
            .Pos = { 0.0f, 0.0f, -3.0f },
            .Radius = 1.0f,
            .MatIdx = 0,
        });

        m_Scene.Spheres.emplace_back(Sphere {
            .Pos = { 2.0f, 0.0f, -3.0f },
            .Radius = 1.0f,
            .MatIdx = 2,
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

            if (ImGui::Button("Pause/Play")) {
                m_Pause = !m_Pause;
            }
            ImGui::SameLine();
            ImGui::Text("Last render: %.3fms", m_LastRenderTime);

            if (ImGui::Button("Reset")) {
                m_Renderer.ResetFrameIdx();
            }
            ImGui::SameLine();
            ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accum);

            if (ImGui::Button("Save")) {
                nfdchar_t* outPath = nullptr;
                nfdresult_t result = NFD_SaveDialog(nullptr, nullptr, &outPath);

                if (result == NFD_OKAY) {
                    m_Renderer.GetFinalImage()->SaveToBmp(outPath);
                    free(outPath);
                } else if (result == NFD_ERROR) {
                    fmt::println(stderr, "Error: {}", NFD_GetError());
                }
            }

            ImGui::End();
        }
        {
            ImGui::Begin("Scene");

            ImGui::Checkbox("Sky", &m_Renderer.Sky);
            ImGui::Separator();

            if (ImGui::CollapsingHeader("Objects")) {
                for (int i = 0; auto& sphere : m_Scene.Spheres) {
                    ImGui::PushID(i);

                    ImGui::DragFloat3("Position", glm::value_ptr(sphere.Pos), 0.1f);
                    ImGui::DragFloat("Radius", &sphere.Radius, 0.1f);
                    ImGui::DragInt("Material", &sphere.MatIdx,
                        1.0f, 0, (int)m_Scene.Materials.size() - 1);

                    ImGui::Separator();
                    ImGui::Spacing();

                    ImGui::PopID();
                    i++;
                }
            }

            for (int i = 0; auto& material : m_Scene.Materials) {
                auto materialLabel = fmt::format("Material {}", i);

                if (ImGui::CollapsingHeader(materialLabel.c_str())) {
                    ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo));
                    ImGui::DragFloat("Roughness", &material.Roughness, 0.01f, 0.0f, 1.0f);
                    ImGui::DragFloat("Metallic", &material.Metallic, 0.01f, 0.0f, 1.0f);

                    ImGui::ColorEdit3("EmissionColor", glm::value_ptr(material.EmissionColor));
                    ImGui::DragFloat("EmissionPower", &material.EmissionPower, 0.1f, 0.0f, FLT_MAX);

                    ImGui::Spacing();
                }

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

        if (!m_Pause) {
            Render();
        }
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
    bool m_Pause = false;
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
