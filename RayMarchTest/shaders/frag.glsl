#version 410 core

in vec2 vUV;
out vec4 fragColor;

uniform vec2 iResolution;
uniform vec3 cameraPosition;
uniform mat3 cameraRotation;
uniform float iTime;

// Signed distance
// --> If the distance is positive, the point is outside the sphere.
// --> If it’s zero, the point is exactly on the sphere’s surface.
// --> If it’s negative, the point is inside the sphere.

// Signed distance to a sphere
float sdSphere(vec3 spherePosToPosVec, float r) {
    return length(spherePosToPosVec) - r;
}

// Signed distance to a box
float sdBox(vec3 boxPosToPosVec, vec3 boxDims) {
    vec3 d = abs(boxPosToPosVec) - boxDims;

	//| Point Location | `length(max(d, 0.0))` | `min(max(d.x, d.y, d.z), 0.0)` | Final SDF                         |
	//| -------------- | --------------------- | ------------------------------ | --------------------------------- |
	//| Outside box    | positive              | 0                              | distance to surface (positive)    |
	//| Inside box     | 0                     | negative                       | negative distance to closest face |
	//| On surface     | 0                     | 0                              | 0 (exact surface)                 |

	// TODO work out this math
	// min(max(d.x, max(d.y, d.z)), 0.0) - handles the case when the point is inside the box
	// --> positive values when the point is outside the box (returns actual distance to the surface)
	// --> negative values when the point is inside the box (returns how deep inside)
	// --> Return zero when the point is exactly on the surface

	// A: length(max(d, 0.0)) - how far outside the box (positive part)
	// B: min(max(d.x, max(d.y, d.z)), 0.0) - how far inside box
	// B positive: 
    return length(max(d, 0.0)) + min(max(d.x, max(d.y, d.z)), 0.0);
}

// Smooth minimum for blending
float smoothMinimum(float dSDSphere, float dSDBox, float blendFactor) {
	// get weight to blend two distances
	
	// --> 0.5 + 0.5 * (dSDBox - dSDSphere) / blendFactor
	// 		--> dSDBox < dSDSphere - point closer to dSDBox
	// 		--> dSDBox > dSDSphere - point closer to dSDSphere
	// 		--> (dSDBox - dSDSphere) / blendFactor - normalize by a blending factor (the larger this is, smoother the transition)
	// 		--> d1 == d2 --> weight = 0.5
	// 		--> dSDBox < dSDSphere --> weight approaches 1.0 
	// 		--> dSDBox > dSDSphere --> weight approaches 0.0
	// --> clamp(0.5 + 0.5 * (dSDBox - dSDSphere) / blendFactor, 0.0, 1.0) - clamps weight between 0 and 1
    float weight = clamp(0.5 + 0.5 * (dSDBox - dSDSphere) / blendFactor, 0.0, 1.0);

	// mix(a, b, t) linearly interpolates between two values a and b based on the parameter t
	// TODO this math and I don't get this relation
    return mix(dSDBox, dSDSphere, weight) - blendFactor * weight * (1.0 - weight);
}

// Scene SDF — blend a moving sphere and box
// TODO idk if I like this function name
float map(vec3 pos) {
    // Animate positions over time using sine waves
    vec3 spherePos = vec3(-0.5 + sin(iTime) * 0.3, 0.0, 0.0);
    vec3 boxPos = vec3(0.5 + cos(iTime * 0.8) * 0.3, 0.0, 0.0);

	// pos - spherePos is a vector from spherePos --> pos (spherePos --> currentPosition)
	// 0.5 = radius of sphere
    float dSDSphere = sdSphere(pos - spherePos, 0.5); 

	// pos - boxPos is a vector from boxPos --> pos (boxPos --> currentPosition)
	// vec(0.3) is the dimensions of the box
    float dSDBox = sdBox(pos - boxPos, vec3(0.3));

    return smoothMinimum(dSDSphere, dSDBox, 0.3); // Blend factor
}

// Raymarching loop
float raymarch(vec3 rayOrigin, vec3 rayDir) {
    float t = 0.0;

	// 100 is the maximum number of steps the ray will march
    for (int i = 0; i < 100; i++) {
        vec3 pos = rayOrigin + t * rayDir;
        float dist = map(pos);
        if (dist < 0.001) break;
        t += dist;
        if (t > 100.0) break;
    }
    return t;
}

void main()
{
    // Normalized screen coords
    vec2 screenPos = vUV * 2.0 - 1.0; // converting the vertex position coordinates from a range of [0,1] to [-1, 1] - TODO possibly unecessary
    screenPos.x *= iResolution.x / iResolution.y; // fixes distortion 

    vec3 rayOrigin = cameraPosition;
    vec3 rayDir = normalize(cameraRotation * vec3(screenPos, 1.0)); // applies rotation to the screenPos

    float t = raymarch(rayOrigin, rayDir);

    if (t < 100.0) {
        // Hit something — use gradient color
        fragColor = vec4(vUV.x, 1.0 - vUV.y, 0.0, 1.0);
    } else {
        // Background
        fragColor = vec4(0.0);
    }
}
