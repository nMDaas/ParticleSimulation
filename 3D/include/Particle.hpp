#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#ifndef PARTICLE_HPP
#define PARTICLE_HPP

class Particle{
public:
    Particle();
    Particle(glm::vec3 pos, float r);
    float getRadius();
    glm::vec3 getPosition();

private:
    glm::vec3 position;
    glm::vec3 position_last;
    float radius;
    float speed;

};



#endif