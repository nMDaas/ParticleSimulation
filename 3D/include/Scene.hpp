#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>

// Reusing the Particle class - think of it as a "Particle of Light"
#include "Particle.hpp"

class Scene{
public:
    Scene();
    void addLight(glm::vec3 position, float radius);
    std::vector<Particle*> getLights();

private:
    std::vector<Particle*> lights; 
};

#endif