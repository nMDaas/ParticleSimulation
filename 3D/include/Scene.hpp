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

class Scene{
public:
    Scene();
    Scene(Solver* i_gSolver, Camera* i_gCamera, ModelProcessor* i_gModelProcessor);
    ~Scene();
    
    void SetupScene(); // Calls SetupSolverAndLights()
    void addLight(glm::vec3 position, float radius);
    
    // vvvvvvvvvvvvvvvvvvvvvvvvvv Set Functions vvvvvvvvvvvvvvvvvvvvvvvvvv
    void InitializeGLuints();

    // vvvvvvvvvvvvvvvvvvvvvvvvvv Get Functions vvvvvvvvvvvvvvvvvvvvvvvvvv
    std::vector<Particle*> getLights();
    Solver* getSolver();
    Camera* getCamera();
    std::unordered_map<std::string, std::vector<GLuint>> getGVertexArrayObjects_map();
    std::unordered_map<std::string, std::vector<GLuint>> getGVertexBufferObjects_map();
    int getObjTotalIndices(std::string objName);

private:
    std::vector<Particle*> lights; 

    GLuint* lightVertexArrayObject;
    GLuint* lightVertexBufferObject;
    std::vector<GLuint> gVertexArrayObjects;
    std::vector<GLuint> gVertexBufferObjects;
    std::unordered_map<std::string, std::vector<GLuint>> gVertexArrayObjects_map; 
    std::unordered_map<std::string, std::vector<GLuint>> gVertexBufferObjects_map;
    GLuint* boxVertexArrayObject;
    GLuint* boxVertexBufferObject;

    Solver* gSolver;
    Camera* gCamera;
    ModelProcessor *gModelProcessor;

    void SetupSolverAndLights(); // Calls SetUpSolver() and SetUpLights()
    void SetUpSolver();
    void SetUpLights();
};

#endif