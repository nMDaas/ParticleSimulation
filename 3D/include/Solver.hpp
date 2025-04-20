#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <vector>
#include <iostream>

#include "Particle.hpp"

#ifndef SOLVER_HPP
#define SOLVER_HPP

class Solver{
public:
    Solver();
    ~Solver();
    void addParticle(glm::vec3 position, float radius);
    void applyGravity();
    void update();
    std::vector<Particle*> getParticles();
    

private:
    std::vector<Particle*> particles;
    glm::vec3 gravity;
    float step_dt;

    void updateObjects(float dt);

};



#endif