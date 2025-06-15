#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include <cstdlib>
#include <set>
#include <map>
#include <chrono>
#include <thread>

#include "Particle.hpp"
#include "Container.hpp"

#ifndef SOLVER_HPP
#define SOLVER_HPP

class Solver{
public:
    Solver();
    ~Solver();
    void addParticle(glm::vec3 position, float radius, bool i_activated);
    void update(Container* gBox, int counter);
    std::vector<Particle*> getParticles();
    void activateNewParticle(int index); // activate particles[index]
    void printSolverInfo();

private:
    std::vector<Particle*> particles;
    glm::vec3 gravity;
    float step_dt;
    float substep_dt;
    int substeps;
    float fluid_restitution;
    float wall_restitution;
    float threshold;

    std::ofstream outFile;

    void applyGravity();
    void applyContainer(Container* gBox);
    void applyContainerThread(Container* gBox, int startIdx, int endIdx);
    void checkCollisions();
    void updateObjects(float dt);

    std::vector<Particle*> getCloseParticles(Particle* p, float range); // Get particles that are close to particle p
    bool getParticleInRange(Particle* particle, glm::vec3 upperBoundCoords, glm::vec3 lowerBoundCoords);

};



#endif