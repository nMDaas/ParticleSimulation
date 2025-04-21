#include "Scene.hpp"

Scene::Scene(){}

Scene::Scene(Solver* i_gSolver, Camera* i_gCamera){
    gSolver = i_gSolver;
    gCamera = i_gCamera;
}

Scene::~Scene(){
    for (int i = 0; i < lights.size(); i++) {
        delete lights[i];
    }
}

void Scene::SetupSolverAndLights(){
    SetUpSolver();
    SetUpLights();
}

void Scene::SetUpSolver(){
    gSolver->addParticle(glm::vec3(2.0f,1.0f,0), 1.0f);
    gSolver->addParticle(glm::vec3(-1.0f,0.0f,0), 0.5f);
}

void Scene::SetUpLights(){
    addLight(glm::vec3(3.0f,1.0f,5.0f), 1.0f);
}

void Scene::addLight(glm::vec3 position, float radius) {
    lights.push_back(new Particle(position, radius));
}

// vvvvvvvvvvvvvvvvvvvvvvvvvv Set Functions vvvvvvvvvvvvvvvvvvvvvvvvvv

void Scene::setLightGLuints(GLuint* i_lightVertexArrayObject, GLuint* i_lightVertexBufferObject) {
    lightVertexArrayObject = i_lightVertexArrayObject;
    lightVertexBufferObject = i_lightVertexBufferObject;
}

void Scene::setParticleGLuints(std::vector<GLuint> i_gVertexArrayObjects, std::vector<GLuint> i_gVertexBufferObjects){
    gVertexArrayObjects = i_gVertexArrayObjects;
    gVertexBufferObjects = i_gVertexBufferObjects;
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

std::vector<GLuint> Scene::getGVertexArrayObjects(){
    return gVertexArrayObjects;
}

std::vector<GLuint> Scene::getGVertexBufferObjects(){
    return gVertexArrayObjects;
}