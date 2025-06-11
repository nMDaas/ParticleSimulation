// Third Party Libraries
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>

// Reusing the Particle class - think of it as a "Particle of Light"
#include "Particle.hpp"
#include "Camera.hpp"
#include "Solver.hpp"
#include "ModelProcessor.hpp"
#include "Container.hpp"

class Scene{
public:
    Scene();
    Scene(Solver* i_gSolver, Camera* i_gCamera, ModelProcessor* i_gModelProcessor);
    ~Scene();
    
    void SetupScene(int numParticles, float size); // Calls SetupSolverAndLights()
    void SetupSceneWithCuboidSetup(int w, int b, int h, float r);  // Calls SetupCuboidSolverLightsAndContainer()
    void addLight(glm::vec3 position, float radius);
    void updateBoxRotationZ(float val);
    
    // vvvvvvvvvvvvvvvvvvvvvvvvvv Set Functions vvvvvvvvvvvvvvvvvvvvvvvvvv
    void InitializeGLuints();

    // vvvvvvvvvvvvvvvvvvvvvvvvvv Get Functions vvvvvvvvvvvvvvvvvvvvvvvvvv
    bool getIfCuboidSolverSetup();
    std::vector<Particle*> getLights();
    Solver* getSolver();
    Camera* getCamera();
    Container* getBox();
    std::unordered_map<std::string, std::vector<GLuint>> getGVertexArrayObjects_map();
    std::unordered_map<std::string, std::vector<GLuint>> getGVertexBufferObjects_map();
    int getObjTotalIndices(std::string objName);
    

private:
    std::vector<Particle*> lights; 

    std::unordered_map<std::string, std::vector<GLuint>> gVertexArrayObjects_map; 
    std::unordered_map<std::string, std::vector<GLuint>> gVertexBufferObjects_map;

    Solver* gSolver;
    Camera* gCamera;
    Container gBox;
    ModelProcessor *gModelProcessor;

    bool cuboidSolverSetup; // Cuboid particle setup or free drip setup

    void SetupSolverLightsAndContainer(int numParticles, float size); // Calls SetUpSolver() and SetUpLights()
    void SetUpSolver(int numParticles, float size);

    void SetupCuboidSolverLightsAndContainer(int w, int b, int h, float r);  // Calls SetUpCuboidSolver() and SetUpLights()
    void SetUpCuboidSolver(int w, int b, int h, float r);

    void SetUpLights();
};

#endif