#version 400 core
out vec4 FragColor;
uniform vec3 rgb;
uniform float isColored;
uniform float isMtlColored;
uniform float isNoise;
uniform float isBnW;
in vec4 rColor;
in vec4 mtlColor;
in vec3 normal;
in vec3 FragPos;
uniform mat4 matriceFinal;
vec3 normalizedNormal = normalize(vec3((matriceFinal) * vec4(normal, 0)));
float specularStrength = 0.5;
vec3 specularColor = vec3(1.0, 1.0, 1.0);
float specularShininess = 32;

float noise( vec2 p ) {
   	// e^pi (Gelfond's constant)
   	// 2^sqrt(2) (Gelfond–Schneider constant)
    vec2 K1 = vec2( 23.14069263277926, 2.665144142690225 );

    return fract(cos(dot(p,K1)) * 12345.6789);
}

void main() {
    float ambientStrength = 0.75;
    vec3 lightColor = rgb;
    vec3 ambient = ambientStrength * lightColor;
    vec3 objectColorRand = vec3(rColor.x, rColor.y, rColor.z);
	vec3 objectMtlColor = vec3(mtlColor.x, mtlColor.y, mtlColor.z);
    vec3 selectedColor;

	if (isColored == 1.0) {
		selectedColor = objectColorRand;
	} else if (isMtlColored == 1.0) {
		selectedColor = objectMtlColor;
	} else {
		selectedColor = vec3(0.9, 0.9, 0.9);
	}
	if (isBnW == 1.0) {
		float bnwColor = (objectColorRand.x + objectColorRand.y + objectColorRand.z) / 3;
		selectedColor = vec3(bnwColor, bnwColor, bnwColor);
	}
	if (isNoise == 1.0) {
		selectedColor *= noise(FragPos.xy);
	}
    vec3 lightDirection = vec3(1, -1, -2);

	vec3 view_dir = normalize(vec3(0.0, 0.0, 0.0) - FragPos.xyz);
	vec3 reflect_dir = reflect(normalize(lightDirection), normalizedNormal); 
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), specularShininess);
	vec3 specular = specularStrength * spec * specularColor; 

    float directionalLightIntensity = min(max(0.0, dot(normalizedNormal, normalize(lightDirection.xyz))), 0.75);
	vec3 result = (ambient + directionalLightIntensity + specular) * selectedColor;
	FragColor = vec4(result, 1.0);
}