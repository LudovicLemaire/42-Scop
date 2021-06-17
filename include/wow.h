#ifndef STRUCT_H
# define STRUCT_H

typedef struct			s_keys_hook
{
    int					escape;
	int					r;
    int					t;
	int					one;
	int					two;
	int					three;
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
}						t_keys_hook;

typedef struct			s_array_mat
{
	float				res[4][4];
}						t_array_mat;

typedef struct			s_hook_params
{
	float				pointSize;
	float				translationX;
	float				translationY;
	float				translationZ;
	float				rotationX;
	float				rotationY;
	float				rotationZ;
	float				speed;
}						t_hook_params;

typedef struct			s_vec
{
	float				x;
	float				y;
	float				z;
}						t_vec;


typedef struct			s_global_params
{
	t_vec				*vertex;
	GLfloat				*gl_buffer;
}						t_global_params;

#endif

void parser(GLfloat **buffer, int *sizeMallocFaces, char *filename);