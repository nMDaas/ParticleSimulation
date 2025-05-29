#version 410 core

in vec2 vUV;
out vec4 fragColor;

uniform vec2 iResolution;
uniform vec3 cameraPosition;
uniform mat3 cameraRotation;

void main()
{
    vec2 uv = vUV;
    vec2 screenPos = uv * 2.0 - 1.0;
    screenPos.x *= iResolution.x / iResolution.y;

    vec3 rayOrigin = cameraPosition;
    vec3 rayDir = normalize(cameraRotation * vec3(screenPos, 1.0));

    // Raymarch here...
	fragColor = vec4(vUV.x, 1.0 - vUV.y, 0.0, 1.0);

}
