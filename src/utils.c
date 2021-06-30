#include <scop.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void mallocFailed(char *text) {
	printf("\x1b[91mMalloc failed\x1b[0m: %s\n", text);
	exit(0);
}

int EndsWith(const char *str, const char *suffix) {
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix >  lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

// Debug prints
void print_vertex(t_vec3 *vertex, int size) {
    int i = 0;
    while (i < size) {
        printf("vertex[%d]: %f %f %f\n", i, vertex[i].x, vertex[i].y, vertex[i].z);
        ++i;
    }
}
void print_buffer(GLfloat *buffer, int size) {
    int i = 0;
    while (size--) {
        printf("%f, %f, %f,   %f, %f, %f,   %f, %f, %f\n", buffer[i], buffer[i+1], buffer[i+2], buffer[i+3], buffer[i+4], buffer[i+5], buffer[i+6], buffer[i+7], buffer[i+8]);
        i += BUFFER_LENGTH;
    }
}

void getMinMax(GLfloat *buffer, int size, t_obj_spec *mm) {
    int i = 0;

    while (size--) {
        mm->x_max = fmax(mm->x_max, buffer[i]);
        mm->x_min = fmin(mm->x_min, buffer[i]);
        mm->y_max = fmax(mm->y_max, buffer[i+1]);
        mm->y_min = fmin(mm->y_min, buffer[i+1]);
        mm->z_max = fmax(mm->z_max, buffer[i+2]);
        mm->z_min = fmin(mm->z_min, buffer[i+2]);
        i += BUFFER_LENGTH;
    }
}

t_vec3 getNormal(t_vec3 p1, t_vec3 p2, t_vec3 p3) {
    t_vec3 u;
    t_vec3 v;
    t_vec3 n;
    
    u.x = p2.x - p1.x;
    u.y = p2.y - p1.y;
    u.z = p2.z - p1.z;
    
    v.x = p3.x - p1.x;
    v.y = p3.y - p1.y;
    v.z = p3.z - p1.z;
    
    n.x = u.z * v.y - u.y * v.z;
    n.y = u.x * v.z - u.z * v.x;
    n.z = u.y * v.x - u.x * v.y;
    return n;
}

void getLengthMiddle(t_obj_spec *mm) {
    mm->x_length = fabs(mm->x_min - mm->x_max);
    mm->x_center = (mm->x_max + mm->x_min) / 2;
    mm->y_length = fabs(mm->y_min - mm->y_max);
    mm->y_center = (mm->y_max + mm->y_min) / 2;
    mm->z_length = fabs(mm->z_min - mm->z_max);
    mm->z_center = (mm->z_max + mm->z_min) / 2;
}

void fill_vertex3d(t_vec3 *vertex, float x, float y, float z) {
    vertex->x = x;
    vertex->y = y;
    vertex->z = z;
}

void fill_vertex2d(t_vec2 *vertex, float x, float y) {
    vertex->x = x;
    vertex->y = y;
}

t_vec3 fill_vec(float x, float y, float z) {
	t_vec3 vec;
	vec.x = x;
	vec.y = y;
	vec.z = z;

	return vec;
}

t_rgb fill_rgb(float r, float g, float b) {
	t_rgb rgb;
	rgb.r = r;
	rgb.g = g;
	rgb.b = b;

	return rgb;
}

// Create random color
float getRC() {
    float color = (float)rand() / RAND_MAX;
    return color;
}

char* concat(const char *s1, const char *s2) {
    char *result = calloc(strlen(s1) + strlen(s2) + 1, sizeof(char));
	if (!result)
		mallocFailed("Concat calloc");
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void getPrePath(char *file, char path[256]) {
    size_t i;
    size_t len = strlen(file);

    memset(path, 0, 256 * sizeof(char));
    for (i = len - 1 ; i > 0; i--)
        if (file[i] == '/')
            strncpy(path, file, i + 1);
}

t_array_mat m4_mult(t_array_mat a, t_array_mat b) {
    t_array_mat     res;
    int        i;
    int        j;
    int        k; 
    
    for (i = 0; i < 4; i++) 
    { 
        for (j = 0; j < 4; j++) 
        { 
            res.res[i][j] = 0; 
            for (k = 0; k < 4; k++) 
                res.res[i][j] += a.res[i][k] * b.res[k][j]; 
        } 
    }

    return (res);
}

void checkKey(GLFWwindow *window, int key, int *stored_value) {
    /*
     1: PRESS
     2: REPEAT
     0: nothing
    -1: RELEASE
    */
    if (glfwGetKey(window, key) == GLFW_PRESS) {
        if (*stored_value == 0 || *stored_value == -1) {
            *stored_value = 1;
        } else if (*stored_value == 1) {
            *stored_value = 2;
        }
    } else {
        if (*stored_value == 1 || *stored_value == 2) {
            *stored_value = -1;
        } else if (*stored_value == -1) {
            *stored_value = 0;
        }
    }
}

void generateTextureMapping(const char *path) {
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width2, height2, nrChannels2;
	unsigned char *data2 = stbi_load(path, &width2, &height2, &nrChannels2, 0);
	if (!data2) {
		printf("\x1b[91mTexture loading failed\x1b[0m\n");
		exit(0);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data2);
}

void _show_fps_counter(GLFWwindow* window, int frames) {
    char tmp[128];
    sprintf(tmp, "Tamer la fenetre: %d fps", frames);
    glfwSetWindowTitle(window, tmp);
}

t_array_mat matriceIdentity() {
    t_array_mat matrice;

    matrice.res[0][0] = 1;
    matrice.res[0][1] = 0;
    matrice.res[0][2] = 0;
    matrice.res[0][3] = 0;
    matrice.res[1][0] = 0;
    matrice.res[1][1] = 1;
    matrice.res[1][2] = 0;
    matrice.res[1][3] = 0;
    matrice.res[2][0] = 0;
    matrice.res[2][1] = 0;
    matrice.res[2][2] = 1;
    matrice.res[2][3] = 0;
    matrice.res[3][0] = 0;
    matrice.res[3][1] = 0;
    matrice.res[3][2] = 0;
    matrice.res[3][3] = 1;
    return matrice;
}

t_array_mat matrice_perspective(float wow_near, float wow_far, float fov, float ratio) {
    t_array_mat mat_perspective = matriceIdentity();
    float e = tan(fov / 2.0);

    mat_perspective.res[0][0] = 1.0f / (ratio * e);
    mat_perspective.res[1][1] = 1.0f / (e);
    mat_perspective.res[2][2] = (wow_far + wow_near) / (wow_far - wow_near) * -1;
    mat_perspective.res[2][3] = -1.0f;
    mat_perspective.res[3][2] = (2.0f * wow_far * wow_near) / (wow_far - wow_near) * -1;
    return mat_perspective;
}

t_array_mat matriceScale(float x, float y, float z) {
    t_array_mat mat_scale = matriceIdentity();

    mat_scale.res[0][0] = x;
    mat_scale.res[1][1] = y;
    mat_scale.res[2][2] = z;

    return mat_scale;
}

t_array_mat matriceTranslation(float x, float y, float z) {
    t_array_mat mat = matriceIdentity();

    mat.res[3][0] = x;
    mat.res[3][1] = y;
    mat.res[3][2] = z;

    return mat;
}

t_array_mat matriceRotation(float x, float y, float z) {
    t_array_mat mat_rotationX = matriceIdentity();
    t_array_mat mat_rotationY = matriceIdentity();
    t_array_mat mat_rotationZ = matriceIdentity();

    mat_rotationX.res[1][1] = cos(x);
    mat_rotationX.res[1][2] = -sin(x);
    mat_rotationX.res[2][1] = sin(x);
    mat_rotationX.res[2][2] = cos(x);

    mat_rotationY.res[0][0] = cos(y);
    mat_rotationY.res[0][2] = sin(y);
    mat_rotationY.res[2][0] = -sin(y);
    mat_rotationY.res[2][2] = cos(y);

    mat_rotationZ.res[0][0] = cos(z);
    mat_rotationZ.res[0][1] = -sin(z);
    mat_rotationZ.res[1][0] = sin(z);
    mat_rotationZ.res[1][1] = cos(z);

    return m4_mult(m4_mult(mat_rotationX, mat_rotationY), mat_rotationZ);
}