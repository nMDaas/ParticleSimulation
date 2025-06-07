#include "Solver.hpp"

Solver::Solver() : outFile("debug.txt"){
    gravity = glm::vec3(0.0f, -100.0f, 0.0f);
    step_dt = 1.0f/60.0f;
    substeps = 8;
    substep_dt = step_dt / substeps;

    // Seed the random number generator
    srand(static_cast<unsigned int>(time(0)));

    fluid_restitution = 0.1f;
    wall_restitution = 1.2f;
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

void Solver::addParticle(glm::vec3 position, float radius){
    particles.push_back(new Particle(position, radius));

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

void Solver::update(Container* gBox){
    outFile << "//////////////////////////////////////////////////////////" << std::endl;
    for (int i = 0; i < substeps; i++) {
        outFile << "------------- substep " << i << "-------------" << std::endl;
        
        outFile << "+++ applyGravity() +++" << std::endl;
        applyGravity();
        printSolverInfo();
        outFile << "+++++++++++++++++++++" << std::endl;

        outFile << "+++ updateObjects() +++" << std::endl;
        updateObjects(substep_dt);
        printSolverInfo();
        outFile << "+++++++++++++++++++++" << std::endl;

        outFile << "+++ applyContainer() +++" << std::endl;
        applyContainer(gBox);
        printSolverInfo();
        outFile << "+++++++++++++++++++++" << std::endl;

        outFile << "+++ checkCollisions +++" << std::endl;
        checkCollisions();
        printSolverInfo();
        outFile << "+++++++++++++++++++++" << std::endl;

        outFile << "+++ applyContainer() +++" << std::endl;
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

            bool collided = false;

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
            if (localPos.x - r_local_x < boxLowerBoundaries.x) {
                localPos.x = boxLowerBoundaries.x + r_local_x;
                localVel.x *= -wall_restitution;
                collided = true;
            }
            if (localPos.x + r_local_x > boxUpperBoundaries.x) {
                localPos.x = boxUpperBoundaries.x - r_local_x;
                localVel.x *= -wall_restitution;
                collided = true;
            }

            // Z axis
            if (localPos.z - r_local_z < boxLowerBoundaries.z) {
                localPos.z = boxLowerBoundaries.z + r_local_z;
                localVel.z *= -wall_restitution;
                collided = true;
            }
            if (localPos.z + r_local_z > boxUpperBoundaries.z) {
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
    for (int i = 0; i < particles.size(); i++) {
        if (particles[i]->getActivated()) {
            Particle* particle_i = particles[i];
            for (int j = 0; j < particles.size(); j++) {
                if (particles[j]->getActivated()){
                    if (i == j) {
                        continue;
                    }
                    else {
                        Particle* particle_j = particles[j];
                        glm::vec3 v = particle_i->getPosition() - particle_j->getPosition();
                        float dist = glm::distance(particle_i->getPosition(), particle_j->getPosition());
                        float min_dist = particle_i->getRadius() + particle_j->getRadius();

                        float dist_diff = min_dist - dist;
                        if (dist < min_dist && dist_diff > threshold) {
                            outFile << "COLLISION! Particle " << i << " <---> Particle " << j << std::endl; 
                            glm::vec3 n = v / dist; // normalize
                            float total_mass = particle_i->getRadius() * particle_i->getRadius() + particle_j->getRadius() * particle_j->getRadius();
                            float mass_ratio = (particle_i->getRadius() * particle_i->getRadius())/ total_mass;
                            float delta = 0.5 * (min_dist - dist);

                            // Larger particles move less
                            glm::vec3 particle_i_new_pos = particle_i->getPosition() + ((n * (1 - mass_ratio) * delta)/ static_cast<float>(substeps));
                            glm::vec3 particle_j_new_pos = particle_j->getPosition() - ((n * mass_ratio * delta)/ static_cast<float>(substeps));
                            particle_i->setPosition(particle_i_new_pos);
                            particle_j->setPosition(particle_j_new_pos);

                            glm::vec3 v_i = particle_i->getVelocity();
                            glm::vec3 v_j = particle_j->getVelocity();

                            glm::vec3 relativeVelocity = v_i - v_j;
                            glm::vec3 diff = particle_i->getPosition() - particle_j->getPosition();
                            glm::vec3 normal = diff / dist;
                            float velocityAlongNormal = glm::dot(relativeVelocity, normal);

                            if (velocityAlongNormal < 0.0f) { // only resolve if moving toward each other
                                float impulseMag = -(1.0f + fluid_restitution) * velocityAlongNormal / total_mass; // TODO work out math for this
                                glm::vec3 impulse = impulseMag * normal;

                                particle_i->setVelocity(v_i + impulse * (1.0f - mass_ratio), 1.0f);
                                particle_j->setVelocity(v_j - impulse * mass_ratio, 1.0f);
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