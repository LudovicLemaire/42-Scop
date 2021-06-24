#ifndef STRUCT_H
# define STRUCT_H
# define BUFFER_LENGTH 14
# define TIME_TRANSITION_S 2

typedef struct			s_vec {
	float				x;
	float				y;
	float				z;
}						t_vec;

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

void parser(GLfloat **buffer, int *sizeMallocFaces, char *filename, t_obj_spec *mm);