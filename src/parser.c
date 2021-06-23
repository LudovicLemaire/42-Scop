#include <stdio.h>
#include <GL/gl3w.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <wow.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <unistd.h>

char *stringCopy(char *str) {
	int len = strlen(str);
	char *newStr = calloc(len, sizeof(char));
	strcpy(newStr, str);

	return newStr;
}

void mallocFailed() {
	printf("\x1b[91mMalloc failed\x1b[0m\n");
	exit(0);
}

int EndsWith(const char *str, const char *suffix)
{
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix >  lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

void print_vertex(t_vec *vertex, int size) {
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

t_vec getNormal(t_vec p1, t_vec p2, t_vec p3) {
    t_vec u;
    t_vec v;
    t_vec n;
    
    u.x = p2.x - p1.x;
    u.y = p2.y - p1.y;
    u.z = p2.z - p1.z;
    
    v.x = p3.x - p1.x;
    v.y = p3.y - p1.y;
    v.z = p3.z - p1.z;
    
    n.x = u.y * v.z - u.z * v.y;
    n.y = u.z * v.x - u.x * v.z;
    n.z = u.x * v.y - u.y * v.x;
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

void fill_vertex(t_vec *vertex, float x, float y, float z) {
    vertex->x = x;
    vertex->y = y;
    vertex->z = z;
}

t_rgb fill_rgb(float r, float g, float b) {
	t_rgb rgb;
	rgb.r = r;
	rgb.g = g;
	rgb.b = b;

	return rgb;
}

float copyGetRC() {
    float color = (float)rand() / (float)RAND_MAX;
    return color;
}

char* concat(const char *s1, const char *s2)
{
    char *result = calloc(strlen(s1) + strlen(s2) + 1, sizeof(char));
	if (!result) {
		printf("15'\n");
		mallocFailed();
	}
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void setFace(GLfloat **buffer, int *j, int *face_inc, GLfloat x, GLfloat y, GLfloat z, t_rgb rgbRand, t_rgb rgbMtl, t_vec normal, GLfloat x_texture, GLfloat y_texture) {
    (*buffer)[(*j)++] = x;
    (*buffer)[(*j)++] = y;
    (*buffer)[(*j)++] = z;
    (*buffer)[(*j)++] = rgbRand.r;
    (*buffer)[(*j)++] = rgbRand.g;
    (*buffer)[(*j)++] = rgbRand.b;
	(*buffer)[(*j)++] = rgbMtl.r;
	(*buffer)[(*j)++] = rgbMtl.g;
	(*buffer)[(*j)++] = rgbMtl.b;
    (*buffer)[(*j)++] = normal.x;
    (*buffer)[(*j)++] = normal.y;
    (*buffer)[(*j)++] = normal.z;
	(*buffer)[(*j)++] = x_texture;
	(*buffer)[(*j)++] = y_texture;
    ++(*face_inc);
}

t_vec fill_vec(float x, float y, float z) {
	t_vec vec;
	vec.x = x;
	vec.y = y;
	vec.z = z;

	return vec;
}

void getPrePath(char *file, char path[256]) {
    size_t i;
    size_t len = strlen(file);

    memset(path, 0, 256 * sizeof(char));
    for (i = len - 1 ; i > 0; i--)
        if (file[i] == '/')
            strncpy(path, file, i + 1);
}

void parser(GLfloat **buffer, int *sizeMallocFaces, char *filename, t_obj_spec *mm) {
    srand(time(NULL));
    if (access(filename, F_OK ) != 0 || !EndsWith(filename, ".obj")) {
        printf("\x1b[91mObj Path\x1b[0m: %s\n", filename);
        exit(-1);
    } else {
        printf("\x1b[92mObj Path\x1b[0m: %s\n", filename);
    }

    FILE *inputfile = fopen(filename, "r");

    struct stat file_info;
    stat(filename, &file_info);
    int sizeMallocVertex = file_info.st_size / 50;
	int sizeMallocNormal = file_info.st_size / 50;
	int sizeMallocMaterials = 10;
    *sizeMallocFaces = sizeMallocVertex * BUFFER_LENGTH;

    t_vec *vertex = calloc(sizeMallocVertex, sizeof(t_vec));
	if (!vertex){
		printf("1\n");
		mallocFailed();
	}
    *buffer = calloc(*sizeMallocFaces, sizeof(GLfloat));
	if (!*buffer) {
		printf("2\n");
		mallocFailed();
	}
	t_vec *normal_buff = calloc(sizeMallocNormal, sizeof(t_vec));
	if (!normal_buff) {
		printf("3\n");
		mallocFailed();
	}
	t_materials *materials = calloc(sizeMallocMaterials, sizeof(t_materials));
	if (!materials) {
		printf("4\n");
		mallocFailed();
	}
    size_t linesize = 0;
    char* linebuf = 0;
    ssize_t linelen = 0;
    float x, y, z;
    int a_point, a_texture, a_normal, b_point, b_texture, b_normal, c_point, c_texture, c_normal, d_point, d_normal, e_point;
    int vertex_inc = 0;
	int normal_inc = 0;
    int face_inc = 0;
	char mtl_reduced_path[256];
	char *mtl_path;
	float r, g, b;
	char materialName[256];
	int totalMaterials = 0;
	t_rgb rgbMtl = fill_rgb(0.0, 1.0, 0.0);
	int haveMtl = 0;

    while ((linelen = getline(&linebuf, &linesize, inputfile)) > 0) {
        // realloc
        if (vertex_inc + 1 >= sizeMallocVertex) {
            sizeMallocVertex *= 1.5;
            printf("sizeMallocVertex: %d\n", sizeMallocVertex);
            vertex = realloc(vertex, (sizeMallocVertex) * sizeof(t_vec));
			if (!vertex) {
				printf("10\n");
				mallocFailed();
			}
        }
        if ((face_inc + 1) * 18 >= *sizeMallocFaces) {
            *sizeMallocFaces *= 1.5;
            printf("\x1b[93msizeMallocFaces:\x1b[0m %d\n", *sizeMallocFaces);
            *buffer = realloc(*buffer, *sizeMallocFaces * sizeof(GLfloat));
			if (!*buffer) {
				printf("9\n");
				mallocFailed();
			}
        }

		if (sscanf(linebuf, "usemtl %s", materialName) == 1) {
			if (haveMtl) {
				int lenMaterials = totalMaterials;

				while (lenMaterials--) {
					if (strcmp(materialName, materials[lenMaterials].name) == 0) { 
						rgbMtl = fill_rgb(materials[lenMaterials].rgb.r, materials[lenMaterials].rgb.g, materials[lenMaterials].rgb.b);
					}
				}
			}
		}

		// mtl file
        else if (sscanf(linebuf, "mtllib %s", mtl_reduced_path) == 1) {
			char prePath[256];
			getPrePath(filename, prePath);
			mtl_path = concat(prePath, mtl_reduced_path);
			if (access(mtl_path, F_OK ) != 0 || !EndsWith(mtl_path, ".mtl")) {
				printf("\x1b[91mMtl Path\x1b[0m: %s\n", mtl_path);
				exit(-1);
			} else {
				printf("\x1b[92mMtl Path\x1b[0m: %s\n", mtl_path);
    		}

			FILE *mtlFile = fopen(mtl_path, "r");
			size_t linesizeMtl = 0;
    		char* linebufMtl = 0;
			ssize_t linelenMtl = 0;
			char nameMaterial[256];
			haveMtl = 1;

			while ((linelenMtl = getline(&linebufMtl, &linesizeMtl, mtlFile)) > 0) {
				if (sscanf(linebufMtl, "newmtl %s", nameMaterial) == 1) {
					if (totalMaterials+1 > sizeMallocMaterials) {
						sizeMallocMaterials += 10;
						materials = realloc(materials, (sizeMallocMaterials) * sizeof(t_materials));
						if (!materials) {
							printf("8\n");
							mallocFailed();
						}
					}
					materials[totalMaterials].name = strdup(nameMaterial);
					++totalMaterials;
				}
				if (sscanf(linebufMtl, "Kd %f %f %f", &r, &g, &b) == 3) {
					t_rgb rgb = fill_rgb(r, g, b);
					materials[totalMaterials-1].rgb = rgb;
				}
				////printf("FREE linebufMtl 1\n");
				free(linebufMtl);
        		linebufMtl = NULL;
			}
			//printf("FREE linebufMtl 2\n");
			free(linebufMtl);
        	linebufMtl = NULL;
			fclose(mtlFile);
        }



        // vertex
        else if (sscanf(linebuf, "v %f %f %f", &x, &y, &z) == 3) {
            fill_vertex(&vertex[vertex_inc], x, y, z);
            ++vertex_inc;
        }

		// normal
        else if (sscanf(linebuf, "vn %f %f %f", &x, &y, &z) == 3) {
            fill_vertex(&normal_buff[normal_inc], x, y, z);
            ++normal_inc;
        }

        // create random color
        t_rgb rgbRand = fill_rgb(copyGetRC(), copyGetRC(), copyGetRC());
        int j = face_inc * BUFFER_LENGTH;
        if (sscanf(linebuf, "f %d/%d/%d %d/%d/%d %d/%d/%d", &a_point, &a_texture, &a_normal, &b_point, &b_texture, &b_normal, &c_point, &c_texture, &c_normal) == 9) {
            t_vec normal = fill_vec(-normal_buff[a_normal-1].x, -normal_buff[a_normal-1].y, -normal_buff[a_normal-1].z);
            setFace(buffer, &j, &face_inc, vertex[a_point-1].x, vertex[a_point-1].y, vertex[a_point-1].z, rgbRand, rgbMtl, normal, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[b_point-1].x, vertex[b_point-1].y, vertex[b_point-1].z, rgbRand, rgbMtl, normal, 0.0, 1.0);
            setFace(buffer, &j, &face_inc, vertex[c_point-1].x, vertex[c_point-1].y, vertex[c_point-1].z, rgbRand, rgbMtl, normal, 1.0, 1.0);
        }
		else if (sscanf(linebuf, "f %d//%d %d//%d %d//%d %d//%d", &a_point, &a_normal, &b_point, &b_normal, &c_point, &c_normal, &d_point, &d_normal) == 8) {
			t_vec normal = fill_vec(-normal_buff[b_normal-1].x, -normal_buff[b_normal-1].y, -normal_buff[b_normal-1].z);
            setFace(buffer, &j, &face_inc, vertex[a_point-1].x, vertex[a_point-1].y, vertex[a_point-1].z, rgbRand, rgbMtl, normal, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[b_point-1].x, vertex[b_point-1].y, vertex[b_point-1].z, rgbRand, rgbMtl, normal, 0.0, 1.0);
            setFace(buffer, &j, &face_inc, vertex[c_point-1].x, vertex[c_point-1].y, vertex[c_point-1].z, rgbRand, rgbMtl, normal, 1.0, 1.0);

            normal = fill_vec(-normal_buff[c_normal-1].x, -normal_buff[c_normal-1].y, -normal_buff[c_normal-1].z);
            setFace(buffer, &j, &face_inc, vertex[a_point-1].x, vertex[a_point-1].y, vertex[a_point-1].z, rgbRand, rgbMtl, normal, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[c_point-1].x, vertex[c_point-1].y, vertex[c_point-1].z, rgbRand, rgbMtl, normal, 0.0, 1.0);
            setFace(buffer, &j, &face_inc, vertex[d_point-1].x, vertex[d_point-1].y, vertex[d_point-1].z, rgbRand, rgbMtl, normal, 1.0, 1.0);
        }
        else if (sscanf(linebuf, "f %d %d %d", &a_point, &b_point, &c_point) == 3) {
            t_vec normal = getNormal(vertex[a_point-1], vertex[b_point-1], vertex[c_point-1]);
            setFace(buffer, &j, &face_inc, vertex[a_point-1].x, vertex[a_point-1].y, vertex[a_point-1].z, rgbRand, rgbMtl, normal, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[b_point-1].x, vertex[b_point-1].y, vertex[b_point-1].z, rgbRand, rgbMtl, normal, 0.0, 1.0);
            setFace(buffer, &j, &face_inc, vertex[c_point-1].x, vertex[c_point-1].y, vertex[c_point-1].z, rgbRand, rgbMtl, normal, 1.0, 1.0);
        }
        else if (sscanf(linebuf, "f %d/%d %d/%d %d/%d", &a_point, &a_texture, &b_point, &b_texture, &c_point, &c_texture) == 6) {
            t_vec normal = getNormal(vertex[a_point-1], vertex[b_point-1], vertex[c_point-1]);
            setFace(buffer, &j, &face_inc, vertex[a_point-1].x, vertex[a_point-1].y, vertex[a_point-1].z, rgbRand, rgbMtl, normal, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[b_point-1].x, vertex[b_point-1].y, vertex[b_point-1].z, rgbRand, rgbMtl, normal, 0.0, 1.0);
            setFace(buffer, &j, &face_inc, vertex[c_point-1].x, vertex[c_point-1].y, vertex[c_point-1].z, rgbRand, rgbMtl, normal, 1.0, 1.0);
        }
        else if (sscanf(linebuf, "f %d//%d %d//%d %d//%d", &a_point, &a_normal, &b_point, &b_normal, &c_point, &c_normal) == 6) {
			t_vec normal = fill_vec(-normal_buff[a_normal-1].x, -normal_buff[a_normal-1].y, -normal_buff[a_normal-1].z);
            setFace(buffer, &j, &face_inc, vertex[a_point-1].x, vertex[a_point-1].y, vertex[a_point-1].z, rgbRand, rgbMtl, normal, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[b_point-1].x, vertex[b_point-1].y, vertex[b_point-1].z, rgbRand, rgbMtl, normal, 0.0, 1.0);
            setFace(buffer, &j, &face_inc, vertex[c_point-1].x, vertex[c_point-1].y, vertex[c_point-1].z, rgbRand, rgbMtl, normal, 1.0, 1.0);
        }
        else if (sscanf(linebuf, "f %d %d %d %d", &a_point, &b_point, &c_point, &d_point) == 4) {
            t_vec normal = getNormal(vertex[a_point-1], vertex[b_point-1], vertex[c_point-1]);
            setFace(buffer, &j, &face_inc, vertex[a_point-1].x, vertex[a_point-1].y, vertex[a_point-1].z, rgbRand, rgbMtl, normal, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[b_point-1].x, vertex[b_point-1].y, vertex[b_point-1].z, rgbRand, rgbMtl, normal, 0.0, 1.0);
            setFace(buffer, &j, &face_inc, vertex[c_point-1].x, vertex[c_point-1].y, vertex[c_point-1].z, rgbRand, rgbMtl, normal, 1.0, 1.0);

            normal = getNormal(vertex[a_point-1], vertex[c_point-1], vertex[d_point-1]);
            setFace(buffer, &j, &face_inc, vertex[a_point-1].x, vertex[a_point-1].y, vertex[a_point-1].z, rgbRand, rgbMtl, normal, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[c_point-1].x, vertex[c_point-1].y, vertex[c_point-1].z, rgbRand, rgbMtl, normal, 0.0, 1.0);
            setFace(buffer, &j, &face_inc, vertex[d_point-1].x, vertex[d_point-1].y, vertex[d_point-1].z, rgbRand, rgbMtl, normal, 1.0, 1.0);
        }
        else if (sscanf(linebuf, "f %d %d %d %d %d", &a_point, &b_point, &c_point, &d_point, &e_point) == 5) {
            t_vec normal = getNormal(vertex[a_point-1], vertex[b_point-1], vertex[c_point-1]);
            setFace(buffer, &j, &face_inc, vertex[a_point-1].x, vertex[a_point-1].y, vertex[a_point-1].z, rgbRand, rgbMtl, normal, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[b_point-1].x, vertex[b_point-1].y, vertex[b_point-1].z, rgbRand, rgbMtl, normal, 0.0, 1.0);
            setFace(buffer, &j, &face_inc, vertex[c_point-1].x, vertex[c_point-1].y, vertex[c_point-1].z, rgbRand, rgbMtl, normal, 1.0, 1.0);

            normal = getNormal(vertex[a_point-1], vertex[c_point-1], vertex[d_point-1]);
            setFace(buffer, &j, &face_inc, vertex[a_point-1].x, vertex[a_point-1].y, vertex[a_point-1].z, rgbRand, rgbMtl, normal, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[c_point-1].x, vertex[c_point-1].y, vertex[c_point-1].z, rgbRand, rgbMtl, normal, 0.0, 1.0);
            setFace(buffer, &j, &face_inc, vertex[d_point-1].x, vertex[d_point-1].y, vertex[d_point-1].z, rgbRand, rgbMtl, normal, 1.0, 1.0);

            normal = getNormal(vertex[a_point-1], vertex[d_point-1], vertex[e_point-1]);
            setFace(buffer, &j, &face_inc, vertex[a_point-1].x, vertex[a_point-1].y, vertex[a_point-1].z, rgbRand, rgbMtl, normal, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[d_point-1].x, vertex[d_point-1].y, vertex[d_point-1].z, rgbRand, rgbMtl, normal, 0.0, 1.0);
            setFace(buffer, &j, &face_inc, vertex[e_point-1].x, vertex[e_point-1].y, vertex[e_point-1].z, rgbRand, rgbMtl, normal, 1.0, 1.0);
        }
		////printf("FREE linebuf 1\n");
        free(linebuf);
        linebuf = NULL;
    }
	// free
	//printf("FREE linebuf 2\n");
    free(linebuf);
    linebuf = NULL;
	//printf("FREE vertex\n");
    free(vertex);
    vertex = NULL;
	//printf("FREE normalbuf\n");
	free(normal_buff);
    normal_buff = NULL;
	if (haveMtl) {
		free(mtl_path);
		mtl_path = NULL;
	}
	int lenMaterials = totalMaterials;
	while (lenMaterials--) {
		//printf("FREE material %d\n", lenMaterials);
		free(materials[lenMaterials].name);
		materials[lenMaterials].name = NULL;
	}
	//printf("FREE material struct\n");
	free(materials);
	materials = NULL;
	fclose(inputfile);
	//realloc buffer to correct size
    *buffer = realloc(*buffer, ((face_inc * 3 * BUFFER_LENGTH) + 1) * sizeof(GLfloat));
	if (!*buffer) {
		printf("7\n");
		mallocFailed();
	}

    *sizeMallocFaces = (face_inc * BUFFER_LENGTH);

    printf("\x1b[94mNombre de triangles:\x1b[0m %d\n", face_inc/3);

    mm->x_min = FLT_MAX;
    mm->y_min = FLT_MAX;
    mm->z_min = FLT_MAX;
    mm->x_max = -FLT_MAX;
    mm->y_max = -FLT_MAX;
    mm->z_max = -FLT_MAX;

    getMinMax(*buffer, face_inc, mm);
    getLengthMiddle(mm);
    //print_vertex(vertex, vertex_inc);
    // print_buffer(*buffer, face_inc);
}
