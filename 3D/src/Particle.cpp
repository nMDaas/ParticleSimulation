#include "Particle.hpp"

Particle::Particle() {}

Particle::Particle(glm::vec3 pos, float r) {
    position = pos;
    position_last = pos;
    radius = r;
    acceleration = glm::vec3(10.0f,10.0f, 0.0f);
}

float Particle::getRadius() {
    return radius;
}

glm::vec3 Particle::getPosition() {
    return position;
}

void Particle::accelerate(glm::vec3  a){
    acceleration += a;
}

void Particle::update (float dt){
    glm::vec3 displacement = position - position_last;
    position_last = position;
    position = position + displacement + acceleration * (dt * dt);
    acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
}