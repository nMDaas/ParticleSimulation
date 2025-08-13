#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>

#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include <cstdlib>
#include <set>
#include <map>
#include <chrono>
#include <thread>
#include <mutex>

#include "Particle.hpp"
#include "Container.hpp"
#include "SpatialMapUtils.hpp"

#ifndef SOLVER_HPP
#define SOLVER_HPP

class Solver{
public:
    Solver();
    Solver(float particleSize, int i_numThreads); // particleSize refers to the radius of the particles
    ~Solver();
    void addParticle(glm::vec3 position, float radius, bool i_activated);
    void setupParticleLocks();
    void update(Container* gBox, int counter);
    std::vector<Particle*> getParticles();
    void activateNewParticle(int index); // activate particles[index]
    void printSolverInfo();

    Solver(const Solver&) = delete;
    Solver& operator=(const Solver&) = delete;
    Solver(Solver&&) = delete;
    Solver& operator=(Solver&&) = delete;

private:
    std::vector<Particle*> particles;
    std::vector<std::unique_ptr<std::mutex>> particle_locks;
    glm::vec3 gravity;
    float step_dt;
    float substep_dt;
    int substeps;
    float fluid_restitution;
    float wall_restitution;
    float threshold;
    std::unordered_map<Vec3i, std::vector<int>> spatialMap;
    float cell_size; // size of each cell in the spatial map
    int numThreads;

    std::ofstream outFile;

    std::vector<glm::vec3> cached_positions; // cache for particle positions
    std::vector<float> cached_masses; // cache for particle masses
    std::vector<float> cached_radii; // cache for particle radii
    std::vector<glm::vec3> cached_container_info; // box lower, upper boundaries, and proportions
    std::vector<bool> cached_activation_status; // cache for particle activation status

    std::unordered_map<int, std::set<int>> particlePairCollisionRecorded_map;
    std::unordered_map<int, std::unique_lock<std::mutex>> potentialColliders_locks;

    // Cache functions
    void cacheParticlePositions();
    void cacheParticleMasses();
    void cacheParticleActivationStatus();
    void cacheParticleRadii();
    void cacheContainerInfo(Container* gBox);
    
    void applyGravity();
    Vec3i getCellIndex(glm::vec3 pos, float cellSize); // Get the cell index for a given position in the spatial map
    void applyContainer(Container* gBox);
    void applyContainerThread(Container* gBox, int startIdx, int endIdx);
    void BuildSpatialMap(); // Build a spatial map for particles to optimize collision detection
    std::vector<int> GetPotentialCollisions(glm::vec3 pos, float radius, int i);
    void checkCollisions();
    void threadUpdateRange(int start, int end, int thread_id);
    void checkCollisionsWithSpatialHashing();
    void checkCollisionsWithSpatialHashing(int i_low, int i_high, int thread_id);
    void updateObjects(float dt);

    void updateParticle(int index);

    std::vector<Particle*> getCloseParticles(Particle* p, float range); // Get particles that are close to particle p
    bool getParticleInRange(Particle* particle, glm::vec3 upperBoundCoords, glm::vec3 lowerBoundCoords);

};



#endif