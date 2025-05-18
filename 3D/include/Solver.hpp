#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <vector>
#include <iostream>

#include <cstdlib>

#include "Particle.hpp"
#include "Container.hpp"

#ifndef SOLVER_HPP
#define SOLVER_HPP

class Solver{
public:
    Solver();
    ~Solver();
    void addParticle(glm::vec3 position, float radius);
    void update(Container* gBox);
    std::vector<Particle*> getParticles();
    void activateNewParticle(int index); // activate particles[index]
    

private:
    std::vector<Particle*> particles;
    glm::vec3 gravity;
    float step_dt;

    void applyGravity();
    void applyContainer(Container* gBox);
    void checkCollisions();
    void updateObjects(float dt);

};



#endif