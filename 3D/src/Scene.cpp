#include "Scene.hpp"

Scene::Scene(){}

Scene::Scene(Solver* i_gSolver, Camera* i_gCamera, ModelProcessor* i_gModelProcessor){
    gSolver = i_gSolver;
    gCamera = i_gCamera;
    gModelProcessor = i_gModelProcessor;
}

Scene::~Scene(){
    for (int i = 0; i < lights.size(); i++) {
        delete lights[i];
    }
}

void Scene::SetupScene(){
    SetupSolverAndLights();
    gModelProcessor->VertexSpecification(gSolver->getParticles().size());
}

// Setup geometry for particles and lights
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

void Scene::InitializeGLuints(){
    gVertexArrayObjects_map = gModelProcessor->getGVertexArrayObjects_map();
    gVertexBufferObjects_map = gModelProcessor->getGVertexBufferObjects_map();
}

// vvvvvvvvvvvvvvvvvvvvvvvvvv Get Functions vvvvvvvvvvvvvvvvvvvvvvvvvv

std::vector<Particle*> Scene::getLights(){
    return lights;
}

Solver* Scene::getSolver(){
    return gSolver;
}

Camera* Scene::getCamera(){
    return gCamera;
}

std::unordered_map<std::string, std::vector<GLuint>> Scene::getGVertexArrayObjects_map(){
    return gVertexArrayObjects_map;
}

std::unordered_map<std::string, std::vector<GLuint>> Scene::getGVertexBufferObjects_map(){
    return gVertexBufferObjects_map;
}

int Scene::getObjTotalIndices(std::string objName){
    return gModelProcessor->getObjTotalIndices(objName);
}