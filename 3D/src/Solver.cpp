#include "Solver.hpp"

Solver::Solver() {
    gravity = glm::vec3(0.0f, -100.0f, 0.0f);
    step_dt = 1.0f/60.0f;

    // Seed the random number generator
    srand(static_cast<unsigned int>(time(0)));
}

Solver::~Solver(){
    for (int i = 0; i < particles.size(); i++) {
        delete particles[i];
    }
}

void Solver::addParticle(glm::vec3 position, float radius){
    particles.push_back(new Particle(position, radius));

    // Generate a float between a range
    float min = 45.0f;
    float max = 180.0f;

    float speed = 7.0f;
    float angle = min + (static_cast<float>(rand()) / RAND_MAX) * (max - min); // generate random angle

    glm::vec3 initialVelocity = glm::vec3(std::cos(glm::radians(angle)), std::sin(glm::radians(angle)), 0.0f) * speed;
    particles[particles.size() - 1]->setVelocity(initialVelocity, step_dt);
}

std::vector<Particle*> Solver::getParticles(){
    return particles;
}

void Solver::activateNewParticle(int index){
    particles[index]->activateParticle();
}

void Solver::update(Container* gBox){
    applyGravity();
    applyContainer(gBox);
    checkCollisions();
    updateObjects(step_dt);
}

void Solver::applyGravity(){
    for (int i = 0; i < particles.size(); i++) {
        if (particles[i]->getActivated()){
            particles[i]->accelerate(gravity);
        }
    }
}

void Solver::applyContainer(Container* gBox){
    glm::vec3 boxLowerBoundaries = gBox->getLowerBoundaries();
    glm::vec3 boxUpperBoundaries = gBox->getUpperBoundaries();

    for (int i = 0; i < particles.size(); i++) {
        if (particles[i]->getActivated()){
            glm::vec3 current_position = particles[i]->getPosition(); // current particle position
            if (current_position.y - particles[i]->getRadius() < boxLowerBoundaries.y) { // y lower boundary
                particles[i]->setPosition(glm::vec3(current_position.x, boxLowerBoundaries.y + particles[i]->getRadius(), current_position.z )); // reposition to be inside boundary
                glm::vec3 v = particles[i]->getVelocity();
                particles[i]->setVelocity(glm::vec3(v.x, v.y * -1.0f, v.z), 1.0f);
            }
            /*if (current_position.y + particles[i]->getRadius() > boxUpperBoundaries.y) { // y upper boundary
                particles[i]->setPosition(glm::vec3(current_position.x, boxUpperBoundaries.y - particles[i]->getRadius(), current_position.z )); // reposition to be inside boundary
                glm::vec3 v = particles[i]->getVelocity();
                particles[i]->setVelocity(glm::vec3(v.x, v.y * -1.0f, v.z), 1.0f);
            }*/
            if (current_position.x - particles[i]->getRadius() < boxLowerBoundaries.x) { // x lower boundary
                particles[i]->setPosition(glm::vec3(boxLowerBoundaries.x + particles[i]->getRadius(), current_position.y, current_position.z )); // reposition to be inside boundary
                glm::vec3 v = particles[i]->getVelocity();
                particles[i]->setVelocity(glm::vec3(v.x * -1.0f, v.y, v.z), 1.0f);
            }
            if (current_position.x + particles[i]->getRadius() > boxUpperBoundaries.x) { // x upper boundary
                particles[i]->setPosition(glm::vec3(boxUpperBoundaries.x - particles[i]->getRadius(), current_position.y, current_position.z )); // reposition to be inside boundary
                glm::vec3 v = particles[i]->getVelocity();
                particles[i]->setVelocity(glm::vec3(v.x * -1.0f, v.y, v.z), 1.0f);
            }
            if (current_position.z - particles[i]->getRadius() < boxLowerBoundaries.z) { // z lower boundary
                particles[i]->setPosition(glm::vec3(current_position.x, current_position.y, boxLowerBoundaries.z + particles[i]->getRadius())); // reposition to be inside boundary
                glm::vec3 v = particles[i]->getVelocity();
                particles[i]->setVelocity(glm::vec3(v.x, v.y, v.z * -1.0f), 1.0f);
            }
            if (current_position.z + particles[i]->getRadius() > boxUpperBoundaries.z) { // z upper boundary
                particles[i]->setPosition(glm::vec3(current_position.x, current_position.y, boxUpperBoundaries.z - particles[i]->getRadius())); // reposition to be inside boundary
                glm::vec3 v = particles[i]->getVelocity();
                particles[i]->setVelocity(glm::vec3(v.x, v.y, v.z * -1.0f), 1.0f);
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
                        if (dist < min_dist) {
                            glm::vec3 n = v / dist; // normalize
                            float total_mass = particle_i->getRadius() * particle_i->getRadius() + particle_j->getRadius() * particle_j->getRadius();
                            float mass_ratio = (particle_i->getRadius() * particle_i->getRadius())/ total_mass;
                            float delta = 0.5 * (min_dist - dist);

                            // Larger particles move less
                            glm::vec3 particle_i_new_pos = particle_i->getPosition() + (n * (1 - mass_ratio) * delta);
                            glm::vec3 particle_j_new_pos = particle_j->getPosition() - (n * mass_ratio * delta);
                            particle_i->setPosition(particle_i_new_pos);
                            particle_j->setPosition(particle_j_new_pos);
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