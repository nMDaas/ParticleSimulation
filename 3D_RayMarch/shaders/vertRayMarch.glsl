#version 410 core

layout(location = 0) in vec2 aPos; // passing in 2D position (x, y)
out vec2 vUV;

void main()
{
    vUV = aPos * 0.5 + 0.5; // converting the vertex position coordinates from a range of [-1, 1] to [0, 1]
    gl_Position = vec4(aPos, 0.0, 1.0); // converting to a glm::vec4 position
}
