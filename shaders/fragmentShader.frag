#version 400 core
out vec4 FragColor;
uniform vec3 rgb;
uniform float isColored;
in vec4 vertexColor;
in vec3 normal;
in vec3 FragPos;
uniform mat4 matriceFinal;
vec3 normalizedNormal = normalize(vec3((matriceFinal) * vec4(normal, 0)));
float specularStrength = 1;
vec3 specularColor = vec3(1.0, 0.0, 0.0);
float specularShininess = 32;

void main() {
    float ambientStrength = 0.75;
    vec3 lightColor = rgb;
    vec3 ambient = ambientStrength * lightColor;
    vec3 objectColor = vec3(vertexColor.x, vertexColor.y, vertexColor.z);
    vec3 selectedColor;
    if (isColored == 0.0) {
        selectedColor = vec3(1.0, 1.0, 1.0);
    } else {
        selectedColor = objectColor;
    }
    vec3 lightDirection = vec3(1, -1, -2);

	vec3 view_dir = normalize(vec3(0.0, 0.0, 0.0) - FragPos.xyz);
	vec3 reflect_dir = reflect(normalize(lightDirection), normalizedNormal); 
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), specularShininess);
	vec3 specular = specularStrength * spec * specularColor; 

    float directionalLightIntensity = max(0.0, dot(normalizedNormal, normalize(lightDirection.xyz)));
	vec3 result = (ambient + directionalLightIntensity + specular) * selectedColor;
	FragColor = vec4(result, 1.0);
}