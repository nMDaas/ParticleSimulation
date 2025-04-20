#include "Scene.hpp"

Scene::Scene(){}

Scene::Scene(Solver* i_gSolver, Camera* i_gCamera){
    gSolver = i_gSolver;
    gCamera = i_gCamera;
}

void Scene::addLight(glm::vec3 position, float radius) {
    lights.push_back(new Particle(position, radius));
}

void Scene::setLightGLuints(GLuint* i_lightVertexArrayObject, GLuint* i_lightVertexBufferObject) {
    lightVertexArrayObject = i_lightVertexArrayObject;
    lightVertexBufferObject = i_lightVertexBufferObject;
}

// vvvvvvvvvvvvvvvvvvvvvvvvvv Get Functions vvvvvvvvvvvvvvvvvvvvvvvvvv

std::vector<Particle*> Scene::getLights(){
    return lights;
}

GLuint* Scene::getLightVertexArrayObject(){
    return lightVertexArrayObject;
}

GLuint* Scene::getLightVertexBufferObject(){
    return lightVertexBufferObject;
}

Solver* Scene::getSolver(){
    return gSolver;
}

Camera* Scene::getCamera(){
    return gCamera;
}