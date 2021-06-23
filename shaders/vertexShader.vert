#version 400 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aRCol;
layout (location = 2) in vec3 aMtlCol;
layout (location = 3) in vec3 aNor;
layout (location = 4) in vec2 aTex;
out vec3 normal;
out vec4 rColor;
out vec4 mtlColor;
out vec3 FragPos;
out vec2 TexCoord;
uniform mat4 matriceFinal;
uniform mat4 matricePerspective;
uniform float pointSize;
void main() {
	normal = aNor;
	gl_PointSize = pointSize;
	gl_Position = (matricePerspective * matriceFinal ) * vec4(aPos, 1.0);
	FragPos = vec3(matriceFinal * vec4(aPos, 1.0));
	rColor = vec4(aRCol, 1.0);
	mtlColor = vec4(aMtlCol, 1.0);
	TexCoord = aTex;
}