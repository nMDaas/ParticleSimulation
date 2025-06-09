#version 410 core

in vec2 vUV;
out vec4 fragColor;

uniform vec2 iResolution;
uniform vec3 cameraPosition;
uniform mat3 cameraRotation;
uniform float iTime;

uniform int particleCount;
uniform vec3 particlePositions[150];

// Signed distance
// --> If the distance is positive, the point is outside the sphere.
// --> If it’s zero, the point is exactly on the sphere’s surface.
// --> If it’s negative, the point is inside the sphere.

// Signed distance to a sphere
float sdSphere(vec3 spherePosToPosVec, float r) {
    return length(spherePosToPosVec) - r;
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
    if (particleCount == 0) return 10000.0; // no particles, return large distance

    // pos - spherePos is a vector from spherePos --> pos (spherePos --> currentPosition)
	// 0.4 = radius of sphere
    float dist = sdSphere(pos - particlePositions[0], 0.4); // TODO this should be passed in

    for (int i = 1; i < particleCount; i++) {
        float d = sdSphere(pos - particlePositions[i], 0.4);
        dist = smoothMinimum(dist, d, 0.8); // Blend factor
    }
    return dist;
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
    
    float fov = radians(45.0);
    float focalLength = 1.0 / tan(fov * 0.5);
    vec3 rayDir = normalize(cameraRotation * vec3(screenPos.x, screenPos.y, -focalLength));


    float t = raymarch(rayOrigin, rayDir);

    if (t < 100.0) {
        // Hit something — use gradient color
        vec3 baseColor = vec3(0.0, 0.5, 0.8); // blue-green
        vec3 white = vec3(1.0);
        vec3 finalColor = mix(baseColor, white, vUV.y);
        fragColor = vec4(finalColor, 1.0);
    } else {
        // Background
        fragColor = vec4(0.0);
    }
}
