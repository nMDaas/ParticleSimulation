#include "Particle.hpp"

Particle::Particle() {}

Particle::Particle(glm::vec3 pos, float r) {
    position = pos;
    position_last = pos;
    radius = r;
    acceleration = glm::vec3(10.0f,10.0f, 0.0f);
    activated = false;
}

Particle::Particle(glm::vec3 pos, float r, bool i_activated){
    position = pos;
    position_last = pos;
    radius = r;
    acceleration = glm::vec3(10.0f,10.0f, 0.0f);
    activated = i_activated;
}

float Particle::getRadius() {
    return radius;
}

glm::vec3 Particle::getPosition() {
    return position;
}

void Particle::activateParticle() {
    activated = true;
}

bool Particle::getActivated(){
    return activated;
}

void Particle::printParticleInfo(){
    std::cout << "Displacement: " << glm::to_string(position - position_last) << std::endl;
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

void Particle::setPosition(glm::vec3 pos){
    position = pos;
}

glm::vec3 Particle::getVelocity(){
    return position - position_last;
}

void Particle::setVelocity(glm::vec3 v, float dt){
    v = 0.7f * v;
    position_last = position - (v * dt);
}

glm::vec3 Particle::getAcceleration(){
    return acceleration;
}