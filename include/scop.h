#ifndef STRUCT_H
# define STRUCT_H
# define BUFFER_LENGTH 16
# define TIME_TRANSITION_S 2
# include <GL/gl3w.h>
# include <GLFW/glfw3.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <unistd.h>

typedef struct			s_vec3 {
	float				x;
	float				y;
	float				z;
}						t_vec3;

typedef struct			s_vec2 {
	float				x;
	float				y;
}						t_vec2;

typedef struct			s_rgb {
	float				r;
	float				g;
	float				b;
}						t_rgb;

typedef struct			s_materials {
	char				*name;
	t_rgb				rgb;
}						t_materials;

typedef struct			s_keys_hook {
    int					escape;
	int					r;
    int					t;
	int					one;
	int					two;
	int					three;
	int					four;
	int					kp_plus;
	int					kp_minus;
	int					plus;
	int					minus;
	int					w;
	int					a;
	int					s;
	int					d;
	int					space;
	int					ctrl;
	int					kp_9;
	int					kp_8;
	int					kp_7;
	int					kp_6;
	int					kp_5;
	int					kp_4;
	int					kp_3;
	int					kp_2;
	int					kp_1;
	int					kp_multiply;
	int					shift;
	int					n;
	int					b;
	int					m;
	int					c;
}						t_keys_hook;

typedef struct			s_array_mat {
	float				res[4][4];
}						t_array_mat;

typedef struct			s_hook_params {
	float				pointSize;
	float				translationX;
	float				translationY;
	float				translationZ;
	float				rotationX;
	float				rotationY;
	float				rotationZ;
	float				speed;
	int					isColored;
	int					isMtlColored;
	int					isNoise;
	int					isBnW;
	int					isTextured;
	int					currTexture;
	int					totalTexture;
	int					textureVType;
	t_rgb				rgb;
}						t_hook_params;

typedef struct			s_obj_spec {
	float				x_min;
	float				x_max;
	float				x_length;
	float				x_center;
	float				y_min;
	float				y_max;
	float				y_length;
	float				y_center;
	float				z_min;
	float				z_max;
	float				z_length;
	float				z_center;
}						t_obj_spec;

#endif

void parser(GLfloat **buffer, int *sizeMallocFaces, char *filename, t_obj_spec *mm, int *hasVT);
char *stringCopy(char *str);
void mallocFailed();
int EndsWith(const char *str, const char *suffix);
void print_vertex(t_vec3 *vertex, int size);
void print_buffer(GLfloat *buffer, int size);
void getMinMax(GLfloat *buffer, int size, t_obj_spec *mm);
t_vec3 getNormal(t_vec3 p1, t_vec3 p2, t_vec3 p3);
void getLengthMiddle(t_obj_spec *mm);
void fill_vertex3d(t_vec3 *vertex, float x, float y, float z);
void fill_vertex2d(t_vec2 *vertex, float x, float y);
t_rgb fill_rgb(float r, float g, float b);
float getRC();
char* concat(const char *s1, const char *s2);
t_vec3 fill_vec(float x, float y, float z);
void getPrePath(char *file, char path[256]);
t_array_mat m4_mult(t_array_mat a, t_array_mat b);
int shaderCreateProgram();
void checkKey(GLFWwindow *window, int key, int *stored_value);
void generateTextureMapping(const char *path);
void _show_fps_counter(GLFWwindow* window, int frames);
t_array_mat matriceIdentity();
t_array_mat matrice_perspective(float wow_near, float wow_far, float fov, float ratio);
t_array_mat matriceScale(float x, float y, float z);
t_array_mat matriceTranslation(float x, float y, float z);
t_array_mat matriceRotation(float x, float y, float z);
void keyCallback(GLFWwindow *window, t_keys_hook *keys_hook, t_hook_params *hook_params, const char **textures);
