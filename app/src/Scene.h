#ifndef SCENE_H
#define SCENE_H

#include <glm/glm.hpp>
#include <vector>

#include "Color.h"

struct Material {
    glm::vec3 Albedo = Color::White;
    float Roughness = 1.0f;
    float Metal = 0.0f;
};

struct Sphere {
    glm::vec3 Pos { 0.0f };
    float Radius = 0.5f;
    int MatIdx = 0;
};

struct Scene {
    std::vector<Sphere> Spheres;
    std::vector<Material> Materials;
};

#endif // SCENE_H
