// Third Party Libraries
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>


#ifndef CONTAINER_HPP
#define CONTAINER_HPP

class Container{
public:
    Container();
    Container(glm::vec3 pos, glm::vec3 i_proportions); 
    glm::vec3 getProportions();
    glm::vec3 getPosition();
    glm::vec3 getUpperBoundaries();
    glm::vec3 getLowerBoundaries();

private:
    glm::vec3 position;
    glm::vec3 proportions; // proportions will be used as a glm::vec3 for width, breadth, height
};

#endif