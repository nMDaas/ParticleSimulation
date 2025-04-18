#include "Scene.hpp"

Scene::Scene(){}

void Scene::addLight(glm::vec3 position, float radius) {
    lights.push_back(new Particle(position, radius));
}

std::vector<Particle*> Scene::getLights(){
    return lights;
}

void Scene::setLightGLuints(GLuint* i_lightVertexArrayObject, GLuint* i_lightVertexBufferObject) {
    lightVertexArrayObject = i_lightVertexArrayObject;
    lightVertexBufferObject = i_lightVertexBufferObject;
}

GLuint* Scene::getLightVertexArrayObject(){
    return lightVertexArrayObject;
}

GLuint* Scene::getLightVertexBufferObject(){
    return lightVertexBufferObject;
}