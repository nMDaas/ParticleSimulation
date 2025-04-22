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
#include "Sphere.hpp"

class Scene{
public:
    Scene();
    Scene(Solver* i_gSolver, Camera* i_gCamera, Sphere* i_gSphere);
    ~Scene();
    
    void SetupSolverAndLights(); // Calls SetUpSolver() and SetUpLights()
    void addLight(glm::vec3 position, float radius);
    
    // vvvvvvvvvvvvvvvvvvvvvvvvvv Set Functions vvvvvvvvvvvvvvvvvvvvvvvvvv
    void setLightGLuints(GLuint* i_lightVertexArrayObject, GLuint* i_lightVertexBufferObject);
    void setParticleGLuints(std::vector<GLuint> i_gVertexArrayObjects, std::vector<GLuint> i_gVertexBufferObjects);
    void InitializeParticleGLuints();
    void InitializeLightGLuints();
    void InitializeBoxGLuints();

    // vvvvvvvvvvvvvvvvvvvvvvvvvv Get Functions vvvvvvvvvvvvvvvvvvvvvvvvvv
    std::vector<Particle*> getLights();
    GLuint* getLightVertexArrayObject();
    GLuint* getLightVertexBufferObject();
    GLuint* getBoxVertexArrayObject();
    GLuint* getBoxVertexBufferObject();
    Solver* getSolver();
    Camera* getCamera();
    std::vector<GLuint> getGVertexArrayObjects();
    std::vector<GLuint> getGVertexBufferObjects();

private:
    std::vector<Particle*> lights; 

    GLuint* lightVertexArrayObject;
    GLuint* lightVertexBufferObject;
    std::vector<GLuint> gVertexArrayObjects;
    std::vector<GLuint> gVertexBufferObjects;
    GLuint* boxVertexArrayObject;
    GLuint* boxVertexBufferObject;

    Solver* gSolver;
    Camera* gCamera;
    Sphere *gSphere;

    void SetUpSolver();
    void SetUpLights();
};

#endif