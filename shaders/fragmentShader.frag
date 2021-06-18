#version 330 core
out vec4 FragColor;
uniform vec3 rgb;
uniform float isColored;
in vec4 vertexColor;
in vec3 normal;
uniform mat4 matriceFinal;
vec3 normalizedNormal = normalize(vec3((matriceFinal) * vec4(normal, 0)));
void main() {
    float ambientStrength = 0.15;
    vec3 lightColor = rgb;
    vec3 ambient = ambientStrength * lightColor;
    vec3 objectColor = vec3(vertexColor.x, vertexColor.y, vertexColor.z);
    vec3 result;
    if (isColored == 0.0) {
        result = vec3(1.0, 1.0, 1.0);
    } else {
        result = objectColor;
    }
    vec3 lightDirection = vec3(1, -1, -2);
    float directionalLightIntensity = max(0.0, dot(normalizedNormal, normalize(-lightDirection.xyz)));
    FragColor = vec4(result * (directionalLightIntensity + ambient), 1.0);
}