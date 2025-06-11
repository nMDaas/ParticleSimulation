// Third Party Libraries
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <iostream>

#ifndef PARTICLE_HPP
#define PARTICLE_HPP

class Particle{
public:
    Particle();
    Particle(glm::vec3 pos, float r);
    Particle(glm::vec3 pos, float r, bool i_activated);

    void activateParticle();

    float getRadius();
    glm::vec3 getPosition();
    glm::vec3 getVelocity();
    glm::vec3 getAcceleration();
    bool getActivated(); // get value of activated variable

    void accelerate (glm::vec3  a);
    void update (float dt);

    void setPosition(glm::vec3 pos);
    void setVelocity(glm::vec3 v, float dt);
    void printParticleInfo();

private:
    glm::vec3 position;
    glm::vec3 position_last;
    float radius;
    float speed;
    glm::vec3 acceleration;
    bool activated;

};



#endif