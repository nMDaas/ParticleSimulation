#include "Solver.hpp"

Solver::Solver() : outFile("debug.txt"){
    gravity = glm::vec3(0.0f, -300.0f, 0.0f);
    step_dt = 1.0f/60.0f;
    substeps = 8;
    substep_dt = step_dt / substeps;

    // Seed the random number generator
    srand(static_cast<unsigned int>(time(0)));

    fluid_restitution = 1.0f;
    wall_restitution = 0.8f;
    threshold = 0.01; 
}

Solver::~Solver(){
    for (int i = 0; i < particles.size(); i++) {
        delete particles[i];
    }
}

void Solver::printSolverInfo(){
    outFile << "-------- particles info ---------" << std::endl;
    for (int i = 0; i < particles.size(); i++) {
        outFile << "Particle " << i << std::endl;
        outFile << "\t position: " << glm::to_string(particles[i]->getPosition()) << std::endl;
        outFile << "\t velocity: " << glm::to_string(particles[i]->getVelocity()) << std::endl;
        outFile << "\t acceleration: " << glm::to_string(particles[i]->getAcceleration()) << std::endl;
    }
    outFile << "------------------------------" << std::endl;
}

void Solver::addParticle(glm::vec3 position, float radius, bool i_activated){
    particles.push_back(new Particle(position, radius, i_activated));

    // Generate a float between a range
    float min = 45.0f;
    float max = 180.0f;

    float speed = 7.0f;

    // Random angles for spherical coordinates
    float theta = ((float)rand() / RAND_MAX) * 2.0f * M_PI; // azimuthal angle (around Y axis)
    float phi = ((float)rand() / RAND_MAX) * (M_PI / 4.0f); 

    // Convert spherical to Cartesian velocity
    float vx = speed * sin(phi) * cos(theta);
    float vy = -speed * cos(phi);    // downward
    float vz = speed * sin(phi) * sin(theta);

    glm::vec3 initialVelocity = glm::vec3(vx, vy, vz);
    particles[particles.size() - 1]->setVelocity(initialVelocity, substep_dt);

}

std::vector<Particle*> Solver::getParticles(){
    return particles;
}

void Solver::activateNewParticle(int index){
    particles[index]->activateParticle();
}

void Solver::update(Container* gBox, int counter){
    outFile << "//////////////////////////////////////////////////////////" << std::endl;
    for (int i = 0; i < substeps; i++) {
        outFile << "------------- count " << counter << " substep " << i << "-------------" << std::endl;
        
        outFile << "+++ applyGravity() count " << counter << " substep " << i << "+++" << std::endl;
        applyGravity();
        printSolverInfo();
        outFile << "+++++++++++++++++++++" << std::endl;

        outFile << "+++ updateObjects() count " << counter << " substep " << i << "+++" << std::endl;
        updateObjects(substep_dt);
        printSolverInfo();
        outFile << "+++++++++++++++++++++" << std::endl;

        outFile << "+++ applyContainer() count " << counter << " substep " << i << "+++" << std::endl;
        applyContainer(gBox);
        printSolverInfo();
        outFile << "+++++++++++++++++++++" << std::endl;

        outFile << "+++ checkCollisions count " << counter << " substep " << i << "+++" << std::endl;
        checkCollisions();
        printSolverInfo();
        outFile << "+++++++++++++++++++++" << std::endl;

        outFile << "+++ applyContainer() count " << counter << " substep " << i << "+++" << std::endl;
        applyContainer(gBox);
        printSolverInfo();
        outFile << "+++++++++++++++++++++" << std::endl;

        outFile << "--------------------------" << std::endl;
    }
    outFile << "//////////////////////////////////////////////////////////" << std::endl;
}

void Solver::applyGravity(){
    for (int i = 0; i < particles.size(); i++) {
        if (particles[i]->getActivated()){
            particles[i]->accelerate(gravity);
        }
    }
}

void Solver::applyContainer(Container* gBox){
    glm::mat4 modelWorld = gBox->getTransform(); 
    glm::mat4 modelLocal = glm::inverse(modelWorld); // Transform from world space to local box space

    // Local space bounds
    glm::vec3 boxLowerBoundaries = gBox->getLowerBoundaries(); 
    glm::vec3 boxUpperBoundaries = gBox->getUpperBoundaries();

    for (int i = 0; i < particles.size(); i++) {
        if (particles[i]->getActivated()){
            glm::vec3 worldPos = particles[i]->getPosition(); // Current world-space position
            glm::vec3 worldVel = particles[i]->getVelocity(); // Current world-space velocity

            glm::vec3 localPos = glm::vec3(modelLocal * glm::vec4(worldPos, 1.0f)); // local-space position
            glm::vec3 localVel = glm::vec3(modelLocal * glm::vec4(worldVel, 0.0f)); // local-space position velocity

            // Separated radius variable into 3 to account for gBox having different proportions
            float r_local_x = particles[i]->getRadius() / gBox->getProportions().x;
            float r_local_y = particles[i]->getRadius() / gBox->getProportions().y;
            float r_local_z = particles[i]->getRadius() / gBox->getProportions().z;

            bool withinYMax = localPos.y + r_local_y < boxUpperBoundaries.y;

            bool collided = false;
            float thresholdContainer = 1.05; // might vary depending on radius size

            // Y axis
            if (localPos.y - r_local_y < boxLowerBoundaries.y) {
                localPos.y = boxLowerBoundaries.y + r_local_y;
                localVel.y *= -wall_restitution;
                collided = true;
            }
            /*if (localPos.y + r_local_y > boxUpperBoundaries.y) {
                localPos.y = boxUpperBoundaries.y - r_local_y;
                localVel.y *= -wall_restitution;
                collided = true;
            }*/

            // X axis
            if (localPos.x - r_local_x < boxLowerBoundaries.x && withinYMax && std::abs(localPos.x - r_local_x) < thresholdContainer) {
                localPos.x = boxLowerBoundaries.x + r_local_x;
                localVel.x *= -wall_restitution;
                collided = true;
            }
            if (localPos.x + r_local_x > boxUpperBoundaries.x && withinYMax && std::abs(localPos.x + r_local_x) < thresholdContainer) {
                localPos.x = boxUpperBoundaries.x - r_local_x;
                localVel.x *= -wall_restitution;
                collided = true;
            }

            // Z axis
            if (localPos.z - r_local_z < boxLowerBoundaries.z && withinYMax && std::abs(localPos.z - r_local_z) < thresholdContainer) {
                localPos.z = boxLowerBoundaries.z + r_local_z;
                localVel.z *= -wall_restitution;
                collided = true;
            }
            if (localPos.z + r_local_z > boxUpperBoundaries.z && withinYMax && std::abs(localPos.z + r_local_z) < thresholdContainer) {
                localPos.z = boxUpperBoundaries.z - r_local_z;
                localVel.z *= -wall_restitution;
                collided = true;
            }

            if (collided) {
                // get position and velocity back to world coordinates
                glm::vec3 correctedWorldPos = glm::vec3(modelWorld * glm::vec4(localPos, 1.0f));
                glm::vec3 correctedWorldVel = glm::vec3(modelWorld * glm::vec4(localVel, 0.0f));
                particles[i]->setPosition(correctedWorldPos);
                particles[i]->setVelocity(correctedWorldVel, 1.0f);
            }
        }
    }
}

void Solver::checkCollisions(){
    std::unordered_map<int, std::set<int>> particlePairCollisionRecorded_map; // stores i->j collisions for which calculations are already done

    for (int i = 0; i < particles.size(); i++) {
        if (particles[i]->getActivated()) {
            Particle* particle_i = particles[i];

            std::vector<Particle*> closeParticles = getCloseParticles(particle_i, 0.3);

            for (int j = 0; j < closeParticles.size(); j++) {
                if (closeParticles[j]->getActivated()){
                    if (i == j) {
                        continue;
                    }
                    else {
                        Particle* particle_j = closeParticles[j];
                        glm::vec3 v = particle_i->getPosition() - particle_j->getPosition();
                        float dist = glm::distance(particle_i->getPosition(), particle_j->getPosition());
                        float min_dist = particle_i->getRadius() + particle_j->getRadius();

                        float dist_diff = min_dist - dist;
                        if (dist < min_dist && dist_diff > threshold) {
                            
                            // First, must make sure that collision in the other way has not already seen and calculated!
                            auto it = particlePairCollisionRecorded_map.find(j); // Check if particle j has already been recorded
                            if (it != particlePairCollisionRecorded_map.end()) {
                                // Particle j has been recorded
                                if (particlePairCollisionRecorded_map[j].find(i) != particlePairCollisionRecorded_map[j].end()) { // Check if i in set for j
                                    // Particle i has been recorded in collision with j
                                    continue;
                                }
                            }

                            
                            outFile << "COLLISION! Particle " << i << " <---> Particle " << j << std::endl; 
                            
                            float total_mass = particle_i->getMass() + particle_j->getMass();
                            float mass_ratio = particle_i->getMass() / total_mass;
                            float delta = 0.5 * dist_diff; //  compute much overlap exists between i and j and then halves it

                            // Larger particles move less
                            glm::vec3 n = v / dist; // normalize
                            // Adding for particle_i and subtracting for particle_j because v is a vector from j to i
                            glm::vec3 particle_i_new_pos = particle_i->getPosition() + ((n * (1 - mass_ratio) * delta)/ static_cast<float>(substeps));
                            glm::vec3 particle_j_new_pos = particle_j->getPosition() - ((n * mass_ratio * delta)/ static_cast<float>(substeps));
                            
                            glm::vec3 particle_i_old_pos = particle_i->getPosition();
                            glm::vec3 particle_j_old_pos = particle_j->getPosition();

                            particle_i->setPosition(particle_i_new_pos);
                            particle_j->setPosition(particle_j_new_pos);

                            glm::vec3 v_i = particle_i->getVelocity();
                            glm::vec3 v_j = particle_j->getVelocity();

                            glm::vec3 relativeVelocity = v_i - v_j;
                            // velocityAlongNormal: relative velocity between the two particles projected onto the collision normal
                            // velocityAlongNormal: the direction in which they’re colliding
                            // If velocityAlongNormal > 0 → they're separating
                            // If velocityAlongNormal < 0 → they're moving toward each other (we need to resolve this)
                            float velocityAlongNormal = glm::dot(relativeVelocity, n);

                            // Compute inverse masses to determine how much each particle responds to impulse
                            // Lighter particles (smaller mass) get larger inverse mass and react more to collisions
                            float invMass_pi = 1.0f / particle_i->getMass();
                            float invMass_pj = 1.0f / particle_j->getMass();

                            // velocityAlongNormal < 0 means they are closing in along the collision normal
                            // only resolve if moving toward each other
                            if (velocityAlongNormal < 0.0f) { 
                                /* Compute scalar impulse magnitude using physics of elastic collision:
                                impulseMag = -(1 + e) * v / (invMass_pi + invMass_pj)
                                    - (1 + restitution): scales the bounce (e.g., 1.0 = perfectly elastic)
                                    - velocityAlongNormal: relative speed toward each other
                                    - (invMass1 + invMass2): distributes impulse based on how easily each particle can move
                                */
                                float impulseMag = -(1.0f + fluid_restitution) * velocityAlongNormal / (invMass_pi + invMass_pj);
                                
                                // Direction of the impulse is along the collision normal.
                                // Multiply the scalar impulse magnitude by the direction vector to get the vector form.
                                glm::vec3 impulse = impulseMag * n;

                                particle_i->setVelocity(v_i + (impulse * invMass_pi), 1.0f);
                                particle_j->setVelocity(v_j - (impulse * invMass_pj), 1.0f);
                            }

                            float particle_i_distance_diff = glm::distance(particle_i_old_pos, particle_i->getPosition());
                            float particle_j_distance_diff = glm::distance(particle_j_old_pos, particle_j->getPosition());
                            outFile << "particle_i_distance_diff: " << particle_i_distance_diff << std::endl;
                            outFile << "particle_j_distance_diff: " << particle_j_distance_diff << std::endl;

                            // Now, must record collision
                            if (it != particlePairCollisionRecorded_map.end()) {
                                // Key already exists, just need to insert j into the set for i in the map
                                std::set<int> i_set = particlePairCollisionRecorded_map[i];
                                i_set.insert(j);
                                particlePairCollisionRecorded_map[i] = i_set;
                            }
                            else {
                                // Key does not exist, must create a key with a fresh set
                                particlePairCollisionRecorded_map[i] = std::set<int>{j};
                            }
                        
                        }
                    }
                }
            }
        }
    }
}


void Solver::updateObjects(float dt){
    for (int i = 0; i < particles.size(); i++) {
        if (particles[i]->getActivated()){
            particles[i]->update(dt);
        }
    }
}

// Get particles that are close to particle p
std::vector<Particle*> Solver::getCloseParticles(Particle* p, float range){
    std::vector<Particle*> closeParticles;

    glm::vec3 p_position = p->getPosition();
    glm::vec3 lowerBoundCoords = glm::vec3(p_position.x - range, p_position.y - range, p_position.z - range);
    glm::vec3 upperBoundCoords = glm::vec3(p_position.x + range, p_position.y + range, p_position.z + range);

    for (int i = 0; i < particles.size(); i++) {
        if (particles[i] == p) {
            continue;
        }
        else {
            if (getParticleInRange(particles[i], upperBoundCoords, lowerBoundCoords)) {
                closeParticles.push_back(particles[i]);
            }
        }
    }

    return closeParticles;
} 

bool Solver::getParticleInRange(Particle* particle, glm::vec3 upperBoundCoords, glm::vec3 lowerBoundCoords){
    glm::vec3 particle_position = particle->getPosition();
    if (particle_position.x < lowerBoundCoords.x || particle_position.x > upperBoundCoords.x){
        return false;
    }
    if (particle_position.y < lowerBoundCoords.y || particle_position.y > upperBoundCoords.y){
        return false;
    }
    if (particle_position.z < lowerBoundCoords.z || particle_position.z > upperBoundCoords.z){
        return false;
    }
    return true;
}