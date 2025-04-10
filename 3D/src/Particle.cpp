#include "Particle.hpp"

Particle::Particle() {}

Particle::Particle(glm::vec3 pos, float r) {
    position = pos;
    position_last = pos;
    radius = r;
}

float Particle::getRadius() {
    return radius;
}

glm::vec3 Particle::getPosition() {
    return position;
}