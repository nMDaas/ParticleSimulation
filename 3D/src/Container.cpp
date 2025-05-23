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

glm::vec3 Container::getLowerBoundaries(){
    glm::quat rotation = glm::angleAxis(glm::radians(rotationZ), glm::vec3(0.0f, 0.0f, 1.0f));

    // Rotate local axes scaled by extents
    glm::vec3 xAxis = rotation * glm::vec3(proportions.x, 0.0f, 0.0f);
    glm::vec3 yAxis = rotation * glm::vec3(0.0f, proportions.y, 0.0f);
    glm::vec3 zAxis = rotation * glm::vec3(0.0f, 0.0f, proportions.z);

    // Absolute contributions to axis-aligned bounding box
    glm::vec3 absExtents = glm::abs(xAxis) + glm::abs(yAxis) + glm::abs(zAxis);

    glm::vec3 lowerBounds = position - absExtents;

    return lowerBounds;
}

glm::vec3 Container::getUpperBoundaries(){
    glm::quat rotation = glm::angleAxis(glm::radians(rotationZ), glm::vec3(0.0f, 0.0f, 1.0f));

    // Rotate local axes scaled by extents
    glm::vec3 xAxis = rotation * glm::vec3(proportions.x, 0.0f, 0.0f);
    glm::vec3 yAxis = rotation * glm::vec3(0.0f, proportions.y, 0.0f);
    glm::vec3 zAxis = rotation * glm::vec3(0.0f, 0.0f, proportions.z);

    // Absolute contributions to axis-aligned bounding box
    glm::vec3 absExtents = glm::abs(xAxis) + glm::abs(yAxis) + glm::abs(zAxis);

    glm::vec3 upperBounds = position + absExtents;

    return upperBounds;
}