#version 400 core
out vec4 FragColor;
uniform vec3 rgb;
uniform float isColored;
uniform float isMtlColored;
uniform float isNoise;
uniform float isBnW;
uniform float isTextured;
uniform float transitionTexture;
uniform float transitionNoise;
uniform float transitionBnW;
uniform float transitionMtlColor;
uniform float transitionRColor;
uniform float textureVType;
in vec4 rColor;
in vec4 mtlColor;
in vec3 normal;
in vec3 FragPos;
in vec2 TexCoord;
in vec2 TexCoordMtl;
uniform mat4 matriceFinal;
vec3 normalizedNormal = normalize(vec3((matriceFinal) * vec4(normal, 0)));
float specularStrength = 0.5;
vec3 specularColor = vec3(1.0, 1.0, 1.0);
float specularShininess = 32;

uniform sampler2D ourTexture;

float noise( vec2 p ) {
   	// 2^sqrt(2) (Gelfond–Schneider constant)
    vec2 K1 = vec2(23.14069263277926, 2.665144142690225 );

    return fract(cos(dot(p,K1)) * 12345.6789);
}

void main() {
    float ambientStrength = 0.75;
    vec3 lightColor = rgb;
    vec3 ambient = ambientStrength * lightColor;
    vec3 objectColorRand = vec3(rColor.x, rColor.y, rColor.z);
	vec3 objectMtlColor = vec3(mtlColor.x, mtlColor.y, mtlColor.z);
    vec4 startColor = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 baseColor = vec4(1.0, 1.0, 1.0, 1.0);

    vec3 lightDirection = vec3(1, -1, -2);

	vec3 view_dir = normalize(vec3(0.0, 0.0, 0.0) - FragPos.xyz);
	vec3 reflect_dir = reflect(normalize(lightDirection), normalizedNormal); 
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), specularShininess);
	vec3 specular = specularStrength * spec * specularColor; 

    float directionalLightIntensity = min(max(0.0, dot(normalizedNormal, normalize(lightDirection.xyz))), 0.75);
	FragColor = startColor;
	
	// apply texture
	if (textureVType == 1)
		FragColor *= mix(baseColor, texture(ourTexture, TexCoordMtl), transitionTexture);
	else
		FragColor *= mix(baseColor, texture(ourTexture, TexCoord), transitionTexture);
	// apply noise
	FragColor *= mix(baseColor, FragColor*noise(FragPos.xy), transitionNoise);
	// apply Black n White
	float bnwColor = (objectColorRand.x + objectColorRand.y + objectColorRand.z) / 3;
	FragColor *= mix(baseColor, vec4(vec3(bnwColor, bnwColor, bnwColor), 1.0), transitionBnW);
	// apply Mtl Color
	FragColor *= mix(baseColor, vec4(objectMtlColor, 1.0), transitionMtlColor);
	// apply Random Color
	FragColor *= mix(baseColor, vec4(objectColorRand, 1.0), transitionRColor);
	// apply lights
	FragColor *= vec4((ambient + directionalLightIntensity + specular), 1.0);
}