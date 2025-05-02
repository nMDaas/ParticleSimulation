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

void Solver::update(Container* gBox){
    applyGravity();
    applyContainer(gBox);
    updateObjects(step_dt);
}

void Solver::applyGravity(){
    for (int i = 0; i < particles.size(); i++) {
        particles[i]->accelerate(gravity);
    }
}

void Solver::applyContainer(Container* gBox){
    glm::vec3 boxLowerBoundaries = gBox->getLowerBoundaries();
    glm::vec3 boxUpperBoundaries = gBox->getUpperBoundaries();

    std::cout << "boxLowerBoundaries: " << glm::to_string(boxLowerBoundaries) << std::endl;

    for (int i = 0; i < particles.size(); i++) {
        glm::vec3 current_position = particles[i]->getPosition(); // current particle position
        std::cout << "current_position: " <<  glm::to_string(current_position) << std::endl;
        if (current_position.y - particles[i]->getRadius() <= boxLowerBoundaries.y) { // y lower boundary
            std::cout << "hit" << std::endl;
            particles[i]->setPosition(glm::vec3(current_position.x, boxLowerBoundaries.y + particles[i]->getRadius(), current_position.z )); // reposition to be inside boundary
            glm::vec3 v = particles[i]->getVelocity();
            particles[i]->setVelocity(glm::vec3(v.x, v.y * -1.0f, v.z), 1.0f);
        }
    }

    std::cout << std::endl;
}

void Solver::updateObjects(float dt){
    for (int i = 0; i < particles.size(); i++) {
        particles[i]->update(dt);
    }
}