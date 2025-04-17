#include "Solver.hpp"

Solver::Solver() {
    gravity = glm::vec3(0.0f, 1000.0f, 0.0f);
    step_dt = 1.0/60;
}

void Solver::addParticle(glm::vec3 position, float radius){
    particles.push_back(new Particle(position, radius));
}

std::vector<Particle*> Solver::getParticles(){
    return particles;
}

void Solver::applyGravity(){

}

void Solver::update(){
    
}

void Solver::updateObjects(float dt){

}