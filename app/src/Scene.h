#ifndef SCENE_H
#define SCENE_H

#include <glm/glm.hpp>
#include <vector>

#include "Color.h"

struct Sphere {
    glm::vec3 Pos { 0.0f };
    float Radius = 0.5f;
    glm::vec3 Albedo = Color::White;
};

struct Scene {
    std::vector<Sphere> Spheres;
};

#endif // SCENE_H
