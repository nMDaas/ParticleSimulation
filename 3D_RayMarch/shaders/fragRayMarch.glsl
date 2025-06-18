#version 410 core

in vec2 vUV;
out vec4 fragColor;

uniform vec2 iResolution;
uniform vec3 cameraPosition;
uniform mat3 cameraRotation;
uniform float iTime;

uniform int particleCount;
uniform vec3 particlePositions[600];

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
	// 0.2 = radius of sphere
    float dist = sdSphere(pos - particlePositions[0], 0.2); // TODO this should be passed in

    for (int i = 1; i < particleCount; i++) {
        float d = sdSphere(pos - particlePositions[i], 0.2);
        dist = smoothMinimum(dist, d, 0.3); // Blend factor
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

float accumulateDensity(vec3 rayOrigin, vec3 rayDir) {
    float t = 0.0;
    float density = 0.0;
    float densityStep = 0.1; // Step size along the ray
    const int maxSteps = 100;

    for (int i = 0; i < maxSteps; ++i) {
        vec3 pos = rayOrigin + rayDir * t;

        float d = map(pos); // Get SDF value at current position

        // Treat negative or small values as high density
        if (d < 0.2) {
            density += (0.2 - d); // You can tweak this to control brightness/density
        }

        t += densityStep;
        if (t > 100.0) break;
    }

    return density;
}

vec3 estimateNormal(vec3 p) {
    float eps = 0.001;
    vec2 h = vec2(eps, 0.0);
    return normalize(vec3(
        map(p + h.xyy) - map(p - h.xyy),
        map(p + h.yxy) - map(p - h.yxy),
        map(p + h.yyx) - map(p - h.yyx)
    ));
}

/*
TO VISUALIZE NORMALS: 
vec3 hitPos = rayOrigin + t * rayDir;
vec3 normal = estimateNormal(hitPos);
vec3 normalColor = normal * 0.5 + 0.5; // convert from [-1,1] to [0,1]
fragColor = vec4(normalColor, 1.0);
*/

vec3 getColor(vec3 rayOrigin, vec3 rayDir, float t) {
    // TODO should be customizable
    float ambient_strength = 0.2;
    float diffuse_strength = 0.8;

    vec3 u_lightColor = vec3(1.0f, 1.0f, 1.0f); // Should also be passed in

    // Calculations for ambient lighting
    vec3 ambient = ambient_strength * u_lightColor;

    // Calculations for diffuse lighting 
    vec3 hitPos = rayOrigin + t * rayDir;
    vec3 normal = estimateNormal(hitPos);

	vec3 normals  = normalize(normal); // Currently important to visualize normals too
    vec3 u_lightPosition = vec3(3.0f,4.0f,5.0f); // TODO this should be passed in
    vec3 lightDir = normalize(u_lightPosition - hitPos);  
    float diff = max(dot(normals, lightDir), 0.0);
    vec3 diffuse = diffuse_strength * diff * u_lightColor;

    vec3 baseColor = vec3(0.0, 0.5, 0.8);
    vec3 lightColor_x_objectColor = (ambient + diffuse) * baseColor;

    return lightColor_x_objectColor;
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

        vec3 hitPos = rayOrigin + t * rayDir;
        vec3 normal = estimateNormal(hitPos);

        vec3 combinedColor = getColor(rayOrigin, rayDir, t);

        float density = accumulateDensity(rayOrigin, rayDir);
        
        float absorption = 0.4; // You can try values like 0.5, 1.0, 2.0 to control how strong the absorption is
        float transmission = exp(-absorption * density);

        vec3 lightColor = vec3(1.0); // White light
        vec3 color = lightColor * transmission; // Apply Beer-Lambert attenuation

        fragColor = vec4(color, 1.0);

    } else {
        // Background
        discard;
    }
}
