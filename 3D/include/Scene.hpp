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

class Scene{
public:
    Scene();
    
    void addLight(glm::vec3 position, float radius);
    std::vector<Particle*> getLights();
    void setLightGLuints(GLuint* i_lightVertexArrayObject, GLuint* i_lightVertexBufferObject);
    GLuint* getLightVertexArrayObject();
    GLuint* getLightVertexBufferObject();

private:
    std::vector<Particle*> lights; 
    GLuint* lightVertexArrayObject;
    GLuint* lightVertexBufferObject;
};

#endif