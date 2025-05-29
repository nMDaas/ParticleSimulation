#version 410 core

in vec2 vUV;
out vec4 fragColor;

uniform vec2 iResolution;
uniform vec3 cameraPosition;
uniform mat3 cameraRotation;
uniform float iTime;

// Signed distance to a sphere
float sdSphere(vec3 p, float r) {
    return length(p) - r;
}

// Signed distance to a box
float sdBox(vec3 p, vec3 b) {
    vec3 d = abs(p) - b;
    return length(max(d, 0.0)) + min(max(d.x, max(d.y, d.z)), 0.0);
}

// Smooth minimum for blending
float smin(float a, float b, float k) {
    float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
    return mix(b, a, h) - k * h * (1.0 - h);
}

// Scene SDF — blend a moving sphere and box
float map(vec3 p) {
    // Animate positions over time using sine waves
    vec3 spherePos = vec3(-0.5 + sin(iTime) * 0.3, 0.0, 0.0);
    vec3 boxPos = vec3(0.5 + cos(iTime * 0.8) * 0.3, 0.0, 0.0);

    float d1 = sdSphere(p - spherePos, 0.5);
    float d2 = sdBox(p - boxPos, vec3(0.3));

    return smin(d1, d2, 0.3); // Blend factor
}

// Raymarching loop
float raymarch(vec3 ro, vec3 rd) {
    float t = 0.0;
    for (int i = 0; i < 100; i++) {
        vec3 p = ro + t * rd;
        float dist = map(p);
        if (dist < 0.001) break;
        t += dist;
        if (t > 100.0) break;
    }
    return t;
}

void main()
{
    // Normalized screen coords
    vec2 screenPos = vUV * 2.0 - 1.0;
    screenPos.x *= iResolution.x / iResolution.y;

    vec3 rayOrigin = cameraPosition;
    vec3 rayDir = normalize(cameraRotation * vec3(screenPos, 1.0));

    float t = raymarch(rayOrigin, rayDir);

    if (t < 100.0) {
        // Hit something — use gradient color
        fragColor = vec4(vUV.x, 1.0 - vUV.y, 0.0, 1.0);
    } else {
        // Background
        fragColor = vec4(0.0);
    }
}
