#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec3 aNor;
out vec3 normal = aNor;
out vec4 vertexColor;
uniform mat4 matriceFinal;
uniform mat4 matricePerspective;
uniform float pointSize;
void main() {
   gl_PointSize = pointSize;
   gl_Position = (matricePerspective * matriceFinal ) * vec4(aPos, 1.0);
   vertexColor = vec4(aCol, 1.0);
};