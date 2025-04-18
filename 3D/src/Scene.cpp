#include "Scene.hpp"

Scene::Scene(){}

void Scene::addLight(glm::vec3 position, float radius) {
    lights.push_back(new Particle(position, radius));
}

std::vector<Particle*> Scene::getLights(){
    return lights;
}