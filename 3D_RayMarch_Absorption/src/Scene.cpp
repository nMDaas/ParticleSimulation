#include "Scene.hpp"

Scene::Scene(){}

Scene::Scene(Solver* i_gSolver, Camera* i_gCamera, ModelProcessor* i_gModelProcessor){
    gSolver = i_gSolver;
    gCamera = i_gCamera;
    gModelProcessor = i_gModelProcessor;
    cuboidSolverSetup = false;
}

Scene::~Scene(){
    for (int i = 0; i < lights.size(); i++) {
        delete lights[i];
    }
}

bool Scene::getIfCuboidSolverSetup(){
    return cuboidSolverSetup;
}

void Scene::updateBoxRotationZ(float val) {
    gBox.updateRotationZ(val);
}  

void Scene::SetupScene(int numParticles, float size){
    SetupSolverLightsAndContainer(numParticles, size);
    gModelProcessor->VertexSpecification(gSolver->getParticles().size());
}

void Scene::SetupSceneWithCuboidSetup(int w, int b, int h, float r){
    SetupCuboidSolverLightsAndContainer(w, b, h, r);
    gModelProcessor->VertexSpecification(gSolver->getParticles().size());
    cuboidSolverSetup = true;
}

// Setup geometry for particles and lights
void Scene::SetupSolverLightsAndContainer(int numParticles, float size){
    SetUpSolver(numParticles, size);
    SetUpLights();
    gBox = Container(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f, 2.0f, 2.0f));
}

void Scene::SetupCuboidSolverLightsAndContainer(int w, int b, int h, float r){
    SetUpCuboidSolver(w, b, h, r);
    SetUpLights();
    gBox = Container(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f, 2.0f, 2.0f));
}

void Scene::SetUpSolver(int numParticles, float size){
    for (int i = 0; i < numParticles; i++) {
        gSolver->addParticle(glm::vec3(0.0f,6.0f,0.0f), size, false);
    }
}

void Scene::SetUpCuboidSolver(int w, int b, int h, float r) {
    float spacing = r * 2.05f; // distance between particle centers

    // Total size of the cuboid
    float totalWidth  = (w - 1) * spacing;
    float totalBreadth = (b - 1) * spacing;
    float totalHeight = (h - 1) * spacing;

    // Origin offset so the cuboid is centered at (0, centerY, 0)
    float centerY = 5.0f; // height where the cuboid is centered
    glm::vec3 origin(
        -totalWidth / 2.0f,
        centerY - totalHeight / 2.0f,
        -totalBreadth / 2.0f
    );

    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            for (int k = 0; k < b; ++k) {
                glm::vec3 pos = origin + glm::vec3(i * spacing, j * spacing, k * spacing);
                gSolver->addParticle(pos, r, true);
            }
        }
    }
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

Container* Scene::getBox(){
    return &gBox;
}