#include "Solver.hpp"

Solver::Solver() {
    gravity = glm::vec3(0.0f, -100.0f, 0.0f);
    step_dt = 1.0f/60.0f;
}

Solver::~Solver(){
    for (int i = 0; i < particles.size(); i++) {
        delete particles[i];
    }
}

void Solver::addParticle(glm::vec3 position, float radius){
    particles.push_back(new Particle(position, radius));
}

std::vector<Particle*> Solver::getParticles(){
    return particles;
}

void Solver::update(){
    applyGravity();
    updateObjects(step_dt);
}

void Solver::applyGravity(){
    for (int i = 0; i < particles.size(); i++) {
        particles[i]->accelerate(gravity);
    }
}

void Solver::updateObjects(float dt){
    for (int i = 0; i < particles.size(); i++) {
        particles[i]->update(dt);
    }
}