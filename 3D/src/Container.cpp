#include "Container.hpp"

Container::Container(){}

Container::Container(glm::vec3 pos, glm::vec3 i_proportions){
    position = pos;
    proportions = i_proportions;
}

glm::vec3 Container::getProportions(){
    return proportions;
}

glm::vec3 Container::getPosition(){
    return position;
}

glm::vec3 Container::getLowerBoundaries(){
    return glm::vec3(position.x - proportions.x, position.y - proportions.y, position.z - proportions.z);
}

glm::vec3 Container::getUpperBoundaries(){
    return glm::vec3(position.x + proportions.x, position.y + proportions.y, position.z + proportions.z);
}