#include "Container.hpp"

Container::Container(){}

Container::Container(glm::vec3 pos, glm::vec3 i_proportions){
    position = pos;
    proportions = i_proportions;
    rotationZ = 0.0f;
}

void Container::updateRotationZ(float val) {
    rotationZ = rotationZ + val;
}  

glm::vec3 Container::getProportions(){
    return proportions;
}

glm::vec3 Container::getPosition(){
    return position;
}

// Gets local boundaries (according to cube obj file)
glm::vec3 Container::getLowerBoundaries(){
    return glm::vec3(-1.0f, -1.0f, -1.0f);
}

// Gets local boundaries (according to cube obj file)
glm::vec3 Container::getUpperBoundaries(){
    return glm::vec3(1.0f, 1.0f, 1.0f);
}

glm::mat4 Container::getTransform() {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::rotate(model, glm::radians(rotationZ), glm::vec3(0, 0, 1));
    model = glm::scale(model, proportions);
    return model;
}